/*****************************************************************************
*                                                                            *
* PROJECT_NAME             OPC UA C++ Server & Client SDK           *
*                                                                            *
* VERSION                  6.00.0                                            *
*                                                                            *
* DATE                     13.6.2022                                        *
*                                                                            *
*                                                                            *
* NOTICE:                                                                    *
*   This code and information is provided "as is" without warranty of        *
*   any kind, either expressed or implied, including but not limited         *
*   to the implied warranties of merchantability and/or fitness for a        *
*   particular purpose.                                                      *
*****************************************************************************/
#include "stdafx.h"
#if TB5_HISTORY
#include "HistorizingVariable.h"
#include "ServerWriteRequest.h"
#include "ServerHistoryReadRawTransaction.h"
#include "ServerHistoryReadRawRequest.h"
#include "ServerHistoryReadRawContinuationPoint.h"
#include "ServerHistoryReadProcessedTransaction.h"
#include "ServerHistoryReadProcessedRequest.h"
#include "OSCompat.h"
#include "Util_SingleLock.h"
#include "ServerSession.h"
#include "AggregateConfiguration.h"
#include "ServerAddressSpaceRoot.h"
#include "Constants.h"
#include "ServerObject.h"
#include "DiagnosticMessages.h"

#include <algorithm>
#include <cmath>
#include "ServerAuditWriteUpdateEvent.h"
#include "Application.h"
#include "ServerWriteTransaction.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

const OTUInt64 HistorizingVariable::cMaxNumberOfTimeSlicesPerRequest = 10;
const OTUInt32 HistorizingVariable::cMaxNumberOfHistoricalValuesDefault = 100;

struct compareValueBySourceTimestamp
{
	bool operator()(const DataValue& left, const DataValue& right) const
	{
		return left.getSourceTimestamp()->compare(right.getSourceTimestamp()) < 0;
	}
};

class TimeSlice
{
public:
	TimeSlice() {}
	TimeSlice(DateTime startTime, DateTime endTime, const std::list<const DataValue*>& dataValues)
		: m_startTime(startTime), m_endTime(endTime), m_dataValues(dataValues) {}

	DateTime getStartTime() const
	{
		return m_startTime;
	}

	DateTime getEndTime() const
	{
		return m_endTime;
	}

	const std::list<const DataValue*>& getDataValues() const
	{
		return m_dataValues;
	}

private:
	DateTime m_startTime, m_endTime;
	std::list<const DataValue*> m_dataValues;
};

class TimeSliceIterator
{
public:
	TimeSliceIterator(std::list<DataValue>* dataValueQueue, DateTime& startTime, DateTime& endTime, OTDouble processingInterval)
		: m_dataValueQueue(dataValueQueue), m_startTime(startTime), m_endTime(endTime), m_processingInterval(processingInterval), m_currentTimeSlice(0)
	{
		m_timeIsFlowingForward = m_startTime.compare(m_endTime) <= 0;

		OTDouble totalTimeSliceLength = m_timeIsFlowingForward ? (OTDouble)DateTime::getDurationMilliseconds(m_startTime, m_endTime) : (OTDouble)DateTime::getDurationMilliseconds(m_endTime, m_startTime);
		m_timeSliceLength = processingInterval > 0 ? processingInterval : (OTDouble)totalTimeSliceLength;
		m_numberOfTimeSlices = (OTUInt64)ceil(totalTimeSliceLength / m_timeSliceLength);
	}

	OTUInt64 getNumberOfTimeSlices() const
	{
		return m_numberOfTimeSlices;
	}

	EnumStatusCode getNextTimeSlice(TimeSlice& timeSlice)
	{
		if(m_currentTimeSlice >= m_numberOfTimeSlices)
		{
			return EnumStatusCode_BadInternalError;
		}

		// Determine begin and end of time slice
		DateTime beginOfSlice;
		DateTime endOfSlice;
		if(m_timeIsFlowingForward)
		{
			beginOfSlice = m_startTime;

			endOfSlice = beginOfSlice;
			endOfSlice.addMilliseconds((OTUInt64)m_timeSliceLength);
			if(endOfSlice.compare(m_endTime) > 0)
			{
				endOfSlice = m_endTime;
			}

			m_startTime = endOfSlice;
		}
		else
		{
			beginOfSlice = m_startTime;

			endOfSlice = beginOfSlice;
			endOfSlice.subtractMilliseconds((OTUInt64)m_timeSliceLength);
			if(endOfSlice.compare(m_endTime) < 0)
			{
				endOfSlice = m_endTime;
			}

			m_startTime = endOfSlice;
		}

		// Determine values that fall within the time slice.
		std::list<const DataValue*> dataValuesWithinTimeSlice;
		for(std::list<DataValue>::const_iterator it = (*m_dataValueQueue).begin();
			it != (*m_dataValueQueue).end(); it++)
		{
			const IDateTime* pDataValueTimeStamp = (*it).getSourceTimestamp();

			if(
				(
					m_timeIsFlowingForward &&
					pDataValueTimeStamp->compare(&beginOfSlice) >= 0 &&
					pDataValueTimeStamp->compare(&endOfSlice) < 0
				)
				||
				(
					!m_timeIsFlowingForward &&
					pDataValueTimeStamp->compare(&beginOfSlice) <= 0 &&
					pDataValueTimeStamp->compare(&endOfSlice) > 0
				)
				)
			{
				DateTime dataValueTimeStamp(pDataValueTimeStamp);
				dataValuesWithinTimeSlice.push_back(&(*it));
			}
		}

		timeSlice = TimeSlice(beginOfSlice, endOfSlice, dataValuesWithinTimeSlice);
		m_currentTimeSlice++;

		return EnumStatusCode_Good;
	}

private:
	// prevent assignments
	TimeSliceIterator& operator=(TimeSliceIterator&);

	std::list<DataValue>* m_dataValueQueue;
	DateTime& m_startTime;
	DateTime& m_endTime;
	OTDouble m_processingInterval;
	OTUInt64 m_currentTimeSlice;

	OTBoolean m_timeIsFlowingForward;
	OTUInt64 m_numberOfTimeSlices;
	OTDouble m_timeSliceLength;
};

HistorizingVariable::HistorizingVariable()
	: m_historizingActive(true), m_maxNumberOfHistoricalValues(cMaxNumberOfHistoricalValuesDefault)
{
}

HistorizingVariable::HistorizingVariable(OTUInt32 maxNumberOfHistoricalValues)
	: m_historizingActive(true), m_maxNumberOfHistoricalValues(maxNumberOfHistoricalValues)
{
	if(maxNumberOfHistoricalValues == 0)
	{
		maxNumberOfHistoricalValues = cMaxNumberOfHistoricalValuesDefault;
	}
}

HistorizingVariable::HistorizingVariable(void* otbHandle)
	: Server::Variable(otbHandle), m_historizingActive(true), m_maxNumberOfHistoricalValues(cMaxNumberOfHistoricalValuesDefault)
{
}

HistorizingVariable::~HistorizingVariable()
{

}

EnumStatusCode HistorizingVariable::handleWriteRequest(Server::WriteTransaction* transaction, Server::WriteRequest* request)
{
	EnumStatusCode result = EnumStatusCode_Good;
	AuditWriteUpdateEventPtr auditEvent;
	if (Application::instance()->getEnableAuditEvents())
	{
		auditEvent = AuditWriteUpdateEvent::create();
		Value oldValue = getDataValue()->getValue();
		auditEvent->setAttributeId(request->getWriteValue()->getAttributeId());
		auditEvent->setIndexRange(request->getWriteValue()->getIndexRange());
		auditEvent->setOldValue(getDataValue()->getValue());
		auditEvent->setClientUserId(transaction->getSession()->getUserClientId());
	}

	result = Server::Variable::handleWriteRequest(transaction, request);
	if(auditEvent)
	{
		auditEvent->setNewValue(getDataValue()->getValue());
		auditEvent->setStatus(StatusCode::isGood(request->getStatusCode()));
		auditEvent->report(NULL); // report on server node
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		// the historizing bit may have changed
		enableHistorizing(getIsHistorizing());
		
		addHistoricalValue(request->getWriteValue()->getDataValue());
	}
	return result;
}

#ifdef MODULE_TEST_HISTORYREAD_TRANSACTION
EnumStatusCode HistorizingVariable::transactionModuleTest(Server::HistoryReadRawTransaction* transaction, Server::HistoryReadRawRequest* request)
{
	EnumStatusCode result = EnumStatusCode_Good;

	// transaction tests
	// just call, cannot test expected value
	vector < Server::HistoryReadRawRequestPtr > requests = transaction->getRequests();
	tstring audirEntryId = transaction->getAuditEntryId();
	DateTime endTime = transaction->getEndTime();
	DateTime startTime = transaction->getStartTime();
	Server::SessionPtr session = transaction->getSession();

	// ByteString tests
	{
		tstring strContext = _T("Test string context ÖÄÜ");
		tstring strContext2;
		ByteString strByteString(strContext); // constructur with tstring

		strByteString.setUtf8String(strContext); // set / get from utf8
		strContext2 = strByteString.getStringFromUtf8();
		_ASSERTION(strContext2.compare(strContext) == 0, _T("ByteString.getStringFromUtf8 returns different string than set"));

		strContext2 = strByteString.toString();	// just test calling - no validation possible

		BYTE * pBuffer = new BYTE[7];
		for (BYTE i=0; i<7; i++)
		{
			pBuffer[i] = i;
		}
		strByteString = ByteString(7, pBuffer);
		delete[] pBuffer; // constructor has copied the buffer
		const BYTE * pConstBuffer;
		unsigned int bsSize;
		bsSize = strByteString.getSize();
		_ASSERTION(bsSize == 7, _T("ByteString.getSize returned different size than set"));
		pConstBuffer = strByteString.getDataPointer();
		for (BYTE i=0; i<bsSize; i++)
		{
			_ASSERTION(pConstBuffer[i] == i, _T("ByteString.getDataPointer returned different data than set"));
		}
	}

	if (requests.size() > 0 && requests.at(0) == request)
	{
		HistoryReadRawContinuationPointPtr contPoint;
		ByteString orgContext;
		bool hasContPoint = requests[0]->hasContinuationPoint();
		if (hasContPoint)
		{
			// save the original continuation point
			void *pContext;
			requests[0]->getContinuationPoint(contPoint);
			OTUInt32 contSize = contPoint->getContext(&pContext);
			orgContext = ByteString(contSize, (BYTE*)pContext); // ByteString constructor with size and buffer
			requests[0]->deleteContinuationPoint();
		}

		// request tests
		{
			HistoryReadRawContinuationPointPtr testContPoint;
			_ASSERTION(requests[0]->hasContinuationPoint() == false, _T("hasContinuationPoint returned true while no continuation point should be there"));
#ifdef _DEBUG
			EnumStatusCode result =
#endif
				requests[0]->getContinuationPoint(testContPoint);
			_ASSERTION(StatusCode::isFAILED(result), _T("getContinuationPoint returned GOOD while no continuation point should be there"));
			_ASSERTION(testContPoint.isNull(), _T("getContinuationPoint returned continuation point while no continuation point should be there"));
		}

		// create a new continuation point
		requests[0]->createContinuationPoint(contPoint);

		{
			HistoryReadRawContinuationPointPtr testContPoint;
			_ASSERTION(requests[0]->hasContinuationPoint() == true, _T("hasContinuationPoint returned false while a continuation point should be there"));
#ifdef _DEBUG
			EnumStatusCode result =
#endif
				requests[0]->getContinuationPoint(testContPoint);
			_ASSERTION(StatusCode::isSUCCEEDED(result), _T("getContinuationPoint returned BAD while a continuation point should be there"));
			_ASSERTION(testContPoint.isNotNull(), _T("getContinuationPoint returned no continuation point while a continuation point should be there"));
		}

		// just call, cannot test expected value
		NumericRange range = requests[0]->getIndexRange();
		NodeId nodeId = requests[0]->getNodeId();

		vector <DataValue> historyValues;
		// set empty array
		requests[0]->setDataValues(historyValues);

		// set one value
		DataValue dataVal;
		dataVal.setValue(Value((OTInt32) 3));
		historyValues.push_back(dataVal);
		requests[0]->setDataValues(historyValues);

		// set again empty array
		historyValues.clear();
		requests[0]->setDataValues(historyValues);


		// ContinuationPoint tests
		{
			BYTE * pBuffer = new BYTE[7];
			void * pConstBuffer;
			for (BYTE i=0; i<7; i++)
			{
				pBuffer[i] = i;
			}
			contPoint->setContext(pBuffer, 7);
			delete[] pBuffer; // setContext has copied the buffer
			unsigned int contextSize = contPoint->getContext(&pConstBuffer);
			_ASSERTION(contextSize == 7, _T("ContinuationPoint.getContext returns different size than set"));
			for (BYTE i=0; i<contextSize; i++)
			{
				_ASSERTION(((BYTE*)(pConstBuffer))[i] == i, _T("ContinuationPoint.getContext returned different data than set"));
			}

			tstring strContext = _T("Test string context ÖÄÜ");
			ByteString strByteString(strContext); // constructor with tstring
			contPoint->setContextFromByteString(strByteString);
			ByteString strByteString2 = contPoint->getContextAsByteString();
			_ASSERTION(strByteString.compare(strByteString2) == true, _T("ContinuationPoint.getContextAsByteString returns different string than set"));

			HistoryReadRawContinuationPointPtr contPoint2;
			requests[0]->getContinuationPoint(contPoint2);

			DateTime now, now2;
			now.now();
			contPoint->setContextFromTimestamp(now);
			now2 = contPoint->getContextAsTimestamp();
			_ASSERTION(now.compare(now2) == 0, _T("ContinuationPoint.getContextAsTimestamp returns different string than set"));

			// startTime, EndTime, MaxNumberOfValuesPerNode, TimestampsToReturn, Type cannot be set
			DateTime testTime, testTime2;
			testTime = contPoint->getEndTime();
			testTime2 = transaction->getEndTime();
			if (testTime.compare(testTime2) != 0)
			{
				if (!hasContPoint)
				{
					// if already a continuation point was originally available, the values may differ
					_REPORT_ASSERTION(_T("ContinuationPoint.getEndTime returns different value than transaction"));
				}
			}
			testTime = contPoint->getStartTime();
			testTime2 = transaction->getStartTime();
			if (testTime.compare(testTime2) != 0)
			{
				if (!hasContPoint)
				{
					// if already a continuation point was originally available, the values may differ
					_REPORT_ASSERTION(_T("ContinuationPoint.getStartTime returns different value than transaction"));
				}
			}
			if (contPoint->getMaxNumberOfValuesPerNode() != transaction->getMaxNumberOfValuesPerNode())
			{
				if (!hasContPoint)
				{
					// if already a continuation point was originally available, the values may differ
					_REPORT_ASSERTION(_T("ContinuationPoint.getMaxNumberOfValuesPerNode returns different value than transaction"));
				}
			}
			if (contPoint->getTimestampsToReturn() != transaction->getTimestampToReturn())
			{
				if (!hasContPoint)
				{
					// if already a continuation point was originally available, the values may differ
					_REPORT_ASSERTION(_T("ContinuationPoint.getTimestampToReturn returns different value than transaction"));
				}
			}
			_ASSERTION(contPoint->getType() == EnumContinuationPointType_HistoryReadRaw, _T("ContinuationPoint.getType returns different than HistoryReadRaw"));
			if (contPoint->returnBoundingValues() != transaction->returnBoundingValues())
			{
				if (!hasContPoint)
				{
					// if already a continuation point was originally available, the values may differ
					_REPORT_ASSERTION(_T("ContinuationPoint.returnBoundingValues returns different value than transaction"));
				}
			}
		}
		requests[0]->deleteContinuationPoint();

		if (hasContPoint)
		{
			// restore the original continuation point
			requests[0]->createContinuationPoint(contPoint);
			contPoint->setContext(orgContext.getDataPointer(), (OTUInt32)orgContext.getSize());
			void *pContext;
			OTUInt32 contSize = contPoint->getContext(&pContext);
			requests[0]->getContinuationPoint(contPoint);
			orgContext = ByteString(contSize, (BYTE*)pContext); // ByteString constructor with size and buffer
		}
	}

	return result;
}
#endif // MODULE_TEST_HISTORYREAD_TRANSACTION

EnumStatusCode HistorizingVariable::handleHistoryReadRawRequest(Server::HistoryReadRawTransaction* transaction, Server::HistoryReadRawRequest* request)
{
#ifdef MODULE_TEST_HISTORYREAD_TRANSACTION
	transactionModuleTest(transaction, request);
#endif

	EnumStatusCode result = EnumStatusCode_Good;
	EnumStatusCode requestStatus = request->getStatusCode();

	// Validate the IndexRange argument, if provided
	NumericRange indexRange = request->getIndexRange();
	HistorizingVariableReadRawContinuationPointPtr continuationPoint;

	if(StatusCode::isSUCCEEDED(requestStatus))
	{
		// Determine the historical values to return
		DateTime totalStartTime, totalEndTime;
		bool returnBoundingValues = false;
		OTUInt32 maxNumberOfValuesPerNode = 0;
		EnumTimestampsToReturn timestampsToReturn = EnumTimestampsToReturn_Both;

		if(request->hasContinuationPoint())
		{
			HistoryReadRawContinuationPointPtr baseContinuationPoint;
			requestStatus = request->getContinuationPoint(baseContinuationPoint);
			if(StatusCode::isGood(requestStatus))
			{
				continuationPoint = static_pointer_cast<HistorizingVariableReadRawContinuationPoint>(baseContinuationPoint);

				totalStartTime = continuationPoint->getStartTime();
				totalEndTime = continuationPoint->getEndTime();
				returnBoundingValues = continuationPoint->returnBoundingValues();
				maxNumberOfValuesPerNode = continuationPoint->getMaxNumberOfValuesPerNode();
				timestampsToReturn = continuationPoint->getTimestampsToReturn();
			}
		}
		else
		{
			totalStartTime = transaction->getStartTime();
			totalEndTime = transaction->getEndTime();
			returnBoundingValues = transaction->returnBoundingValues();
			maxNumberOfValuesPerNode = transaction->getMaxNumberOfValuesPerNode();
			timestampsToReturn = transaction->getTimestampToReturn();

			// we create and attach a new continuation point to store the intermediate data,
			// if not needed it will be removed again
			continuationPoint = new HistorizingVariableReadRawContinuationPoint();
			continuationPoint->m_processingState = EnumReadRawProcessingState_LowerBound;
			// store the start time of the current continuation part
			continuationPoint->setContextFromTimestamp(totalStartTime);
			request->setContinuationPoint(continuationPoint);
		}


		// Fetch the requested historical values
		std::list<DataValue> historicalValuesToReturn;
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			requestStatus = getRawHistoricalValues(continuationPoint, totalStartTime, totalEndTime, returnBoundingValues, maxNumberOfValuesPerNode, historicalValuesToReturn);
			request->setStatusCode(requestStatus);
		}

		// Apply IndexRange, if provided
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			if(indexRange.getDimensions() > 0)
			{
				for(std::list<DataValue>::iterator it = historicalValuesToReturn.begin();
					it != historicalValuesToReturn.end(); it++)
				{
					DataValue& localDataValue = (*it);

					if(StatusCode::isSUCCEEDED(localDataValue.getStatusCode()))
					{
						Value localValueSubset;
						requestStatus = (*it).getValue()->getSubset(localValueSubset, &indexRange);
						if(StatusCode::isSUCCEEDED(requestStatus))
						{
							localDataValue.setValue(localValueSubset);
						}
					}
				}
			}
		}

		// Apply TimestampsToReturn
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			for(std::list<DataValue>::iterator it = historicalValuesToReturn.begin();
				it != historicalValuesToReturn.end(); it++)
			{
				DataValue& dataValue = (*it);

				if(StatusCode::isSUCCEEDED(dataValue.getStatusCode()))
				{
					if(timestampsToReturn == EnumTimestampsToReturn_Server)
					{
						dataValue.setSourceTimestamp(DateTime());
					}
					else if(timestampsToReturn == EnumTimestampsToReturn_Source)
					{
						dataValue.setServerTimestamp(DateTime());
					}
				}
			}
		}

		//  Copy the historical values to the response
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			std::vector<DataValue> historicalValuesToReturnVector;
			historicalValuesToReturnVector.reserve(historicalValuesToReturn.size());
			for(std::list<DataValue>::iterator it = historicalValuesToReturn.begin();
				it != historicalValuesToReturn.end(); it++)
			{
				EnumStatusCode statusCode = it->getStatusCode();
				historicalValuesToReturnVector.push_back(*it);
				// update status code to include correct bits for read raw
				StatusCode::setRawHistorianBit(statusCode);
				historicalValuesToReturnVector.back().setStatusCode(statusCode);
			}
			requestStatus = request->setDataValues(historicalValuesToReturnVector);
		}

		// Create or delete a continuation point if required
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			// we previously added the continuation point in any case (probably even just for temporary usage),
			// so we just have to remove it when we are done
			if(continuationPoint->m_processingState == EnumReadRawProcessingState_Finished)
			{
				request->deleteContinuationPoint();
			}
		}
	}

	// Only write Bad or Uncertain status codes to prevent overwriting Good sub-codes
	// returned by getHistoricalValues()
	if(!StatusCode::isGood(requestStatus))
	{
		request->setStatusCode(requestStatus);
	}

	// Supply diagnostic infos if requested
	if(DiagnosticInfo::returnOperationLevelDiagnostics(transaction->getDiagnosticsToReturn()))
	{
		DiagnosticMessages::EnumDiagnosticMessage messageType =
			StatusCode::isGood(request->getStatusCode()) ?
			DiagnosticMessages::EnumDiagnosticMessage_NoError :
		DiagnosticMessages::EnumDiagnosticMessage_GeneralError;

		DiagnosticInfo diagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
			DiagnosticMessages::EnumDiagnosticMessageLevel_Operation, messageType, transaction);

		if(transaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_ServiceLevel_AdditionalInfo)
		{
			diagnosticInfo.setAdditionalInfo(_T("No additional information available"));
		}
		if(transaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_OperationLevel_InnerDiagnostics)
		{
			DiagnosticInfo innerDiagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
				DiagnosticMessages::EnumDiagnosticMessageLevel_Operation, messageType, transaction);

			diagnosticInfo.setInnerDiagnosticInfo(&innerDiagnosticInfo);
		}

		EnumStatusCode setDiagnosticInfoResult = request->setDiagnosticInfo(diagnosticInfo);
		_UNUSED(setDiagnosticInfoResult);
		_ASSERTION(StatusCode::isGood(setDiagnosticInfoResult), _T("setDiagnosticInfo() failed"));
	}

	request->complete();

	return result;
}

EnumStatusCode HistorizingVariable::handleHistoryReadProcessedRequest(Server::HistoryReadProcessedTransaction* transaction, Server::HistoryReadProcessedRequest* request)
{
	EnumStatusCode result = EnumStatusCode_Good;
	EnumStatusCode requestStatus = request->getStatusCode();

	// Verify that StartTime and EndTime is defined
	if(transaction->getStartTime().isNull() || transaction->getEndTime().isNull())
	{
		requestStatus = EnumStatusCode_BadInvalidArgument;
	}

	// Verify that StartTime and EndTime are not the same
	if(StatusCode::isSUCCEEDED(requestStatus) && (transaction->getStartTime().compare(transaction->getEndTime()) == 0))
	{
		requestStatus = EnumStatusCode_BadInvalidArgument;
	}

	// Verify that the ProcessingInterval is not negative
	if(StatusCode::isSUCCEEDED(requestStatus) && transaction->getProcessingInterval() < 0.0)
	{
		requestStatus = EnumStatusCode_BadInvalidArgument;
	}

	// Validate the IndexRange argument, if provided
	NumericRange indexRange = request->getIndexRange();

	if(StatusCode::isSUCCEEDED(requestStatus))
	{
		// Determine the historical values to return
		DateTime startTime, endTime;
		double processingInterval = 0.0;
		SoftingOPCToolbox5::AggregateConfiguration aggregateConfiguration;
		NodeId aggregateType;
		EnumTimestampsToReturn timestampsToReturn = EnumTimestampsToReturn_Both;
		HistorizingVariableReadProcessedContinuationPointPtr continuationPoint;

		if(request->hasContinuationPoint())
		{
			HistoryReadProcessedContinuationPointPtr baseContinuationPoint;
			requestStatus = request->getContinuationPoint(baseContinuationPoint);
			if(StatusCode::isGood(requestStatus))
			{
				continuationPoint = static_pointer_cast<HistorizingVariableReadProcessedContinuationPoint>(baseContinuationPoint);

				processingInterval = continuationPoint->getProcessingInterval();
				aggregateConfiguration = continuationPoint->getAggregateConfiguration();
				aggregateType = continuationPoint->getAggregateType();
				timestampsToReturn = continuationPoint->getTimestampsToReturn();
			}
		}
		else
		{
			// we create and attach a new continuation point to store the intermediate data,
			// if not needed it will be removed again
			continuationPoint = new HistorizingVariableReadProcessedContinuationPoint();
			continuationPoint->m_startTime = transaction->getStartTime();
			continuationPoint->m_endTime = transaction->getEndTime();
			request->setContinuationPoint(continuationPoint);

			// Resolve aggregate configuration if default values are requested
			continuationPoint->m_aggregateConfiguration = transaction->getAggregateConfiguration();
			if(continuationPoint->m_aggregateConfiguration.getUseServerCapabilitiesDefaults())
			{
				bool useHardCodedDefaults = true;

				// use aggregate configuration of default "HA Configuration" if available
				ObjectPointer<Object> haConfiguration = getObject(BrowseName_HAConfiguration);
				if(haConfiguration.isNotNull())
				{
					ObjectPointer<Object> aggregateConfigurationVar = haConfiguration->getObject(BrowseName_AggregateConfiguration);
					if(aggregateConfigurationVar.isNotNull())
					{
						ObjectPointer<Variable> treatUncertainAsBadVar = aggregateConfigurationVar->getVariable(BrowseName_TreatUncertainAsBad);
						continuationPoint->m_aggregateConfiguration.setTreatUncertainAsBad(treatUncertainAsBadVar->getDataValue()->getValue()->getBoolean());

						ObjectPointer<Variable> percentDataBadVariable = aggregateConfigurationVar->getVariable(BrowseName_PercentDataBad);
						continuationPoint->m_aggregateConfiguration.setPercentDataBad(percentDataBadVariable->getDataValue()->getValue()->getUInt8());

						ObjectPointer<Variable> percentDataGoodVariable = aggregateConfigurationVar->getVariable(BrowseName_PercentDataGood);
						continuationPoint->m_aggregateConfiguration.setPercentDataGood(percentDataGoodVariable->getDataValue()->getValue()->getUInt8());

						ObjectPointer<Variable> useSlopedExtrapolationVar = aggregateConfigurationVar->getVariable(BrowseName_UseSlopedExtrapolation);
						continuationPoint->m_aggregateConfiguration.setUseSlopedExtrapolation(useSlopedExtrapolationVar->getDataValue()->getValue()->getBoolean());

						useHardCodedDefaults = false;
					}
				}

				if(useHardCodedDefaults)
				{
					// Use hard-coded defaults defined in OPC UA Spec. part 13
					continuationPoint->m_aggregateConfiguration.setTreatUncertainAsBad(true);
					continuationPoint->m_aggregateConfiguration.setPercentDataBad(100);
					continuationPoint->m_aggregateConfiguration.setPercentDataGood(100);
					continuationPoint->m_aggregateConfiguration.setUseSlopedExtrapolation(false);
				}
			}

			processingInterval = transaction->getProcessingInterval();
			aggregateConfiguration = transaction->getAggregateConfiguration();
			aggregateType = request->getAggregateType();
			timestampsToReturn = transaction->getTimestampToReturn();
		}

		// Fetch the requested historical values
		std::list<DataValue> historicalValuesToReturn;
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			requestStatus = getProcessedHistoricalValues(continuationPoint, indexRange, processingInterval, aggregateType, historicalValuesToReturn);
			request->setStatusCode(requestStatus);
		}

		// Apply TimestampsToReturn
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			for(std::list<DataValue>::iterator it = historicalValuesToReturn.begin();
				it != historicalValuesToReturn.end(); it++)
			{
				DataValue& dataValue = (*it);

				if(StatusCode::isSUCCEEDED(dataValue.getStatusCode()))
				{
					if(timestampsToReturn == EnumTimestampsToReturn_Server)
					{
						dataValue.setSourceTimestamp(DateTime());
					}
					else if(timestampsToReturn == EnumTimestampsToReturn_Source)
					{
						dataValue.setServerTimestamp(DateTime());
					}
				}
			}
		}

		//  Copy the historical values to the response
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			std::vector<DataValue> historicalValuesToReturnVector;
			historicalValuesToReturnVector.reserve(historicalValuesToReturn.size());
			for(std::list<DataValue>::iterator it = historicalValuesToReturn.begin();
				it != historicalValuesToReturn.end(); it++)
			{
				EnumStatusCode statusCode = it->getStatusCode();
				historicalValuesToReturnVector.push_back(*it);
				// update status code to include correct bits for read processed
				StatusCode::setCalculatedHistorianBit(statusCode);
				historicalValuesToReturnVector.back().setStatusCode(statusCode);
			}
			requestStatus = request->setDataValues(historicalValuesToReturnVector);
		}

		// Create or delete a continuation point if required
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			// we previously added the continuation point in any case (probably even just for temporary usage),
			// so we just have to remove it when we are done
			if(request->hasContinuationPoint() && continuationPoint->m_isFinished)
			{
				request->deleteContinuationPoint();
			}
		}
	}

	// Only write Bad or Uncertain status codes to prevent overwriting Good sub-codes
	// returned by getHistoricalValues()
	if(!StatusCode::isSUCCEEDED(requestStatus))
	{
		request->setStatusCode(requestStatus);
	}

	// Supply diagnostic infos if requested
	if(DiagnosticInfo::returnOperationLevelDiagnostics(transaction->getDiagnosticsToReturn()))
	{
		DiagnosticMessages::EnumDiagnosticMessage messageType =
			StatusCode::isGood(request->getStatusCode()) ?
			DiagnosticMessages::EnumDiagnosticMessage_NoError :
		DiagnosticMessages::EnumDiagnosticMessage_GeneralError;

		DiagnosticInfo diagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
			DiagnosticMessages::EnumDiagnosticMessageLevel_Operation, messageType, transaction);

		if(transaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_ServiceLevel_AdditionalInfo)
		{
			diagnosticInfo.setAdditionalInfo(_T("No additional information available"));
		}
		if(transaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_OperationLevel_InnerDiagnostics)
		{
			DiagnosticInfo innerDiagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
				DiagnosticMessages::EnumDiagnosticMessageLevel_Operation, messageType, transaction);

			diagnosticInfo.setInnerDiagnosticInfo(&innerDiagnosticInfo);
		}

		EnumStatusCode setDiagnosticInfoResult = request->setDiagnosticInfo(diagnosticInfo);
		_UNUSED(setDiagnosticInfoResult);
		_ASSERTION(StatusCode::isGood(setDiagnosticInfoResult), _T("setDiagnosticInfo() failed"));
	}

	request->complete();

	return result;
}

EnumStatusCode HistorizingVariable::getRawHistoricalValues(HistorizingVariableReadRawContinuationPointPtr continuationPoint, const DateTime& /*rawStartTime*/, DateTime endTime, bool returnBoundingValues,
	OTUInt32 maxNumberOfValuesPerNode, std::list<DataValue>& historicalValues)
{
	Util_SingleLock<Util_Sync> lock(sync);
	EnumStatusCode result = EnumStatusCode_Good;

	//////////////////////////////////////////////////////////////////////////
	// Determine whether values are to be returned in ascending or descending order
	//////////////////////////////////////////////////////////////////////////
	DateTime continuationStartTime = continuationPoint->getContextAsTimestamp();
	EnumReadRawProcessingState processingState = continuationPoint->m_processingState;

	if(continuationStartTime.isNull() && !endTime.isNull() && maxNumberOfValuesPerNode > 0)
	{
		continuationStartTime = endTime;
		endTime.clear();
	}
	else if(!continuationStartTime.isNull() && endTime.isNull() && maxNumberOfValuesPerNode > 0)
	{
		endTime.setHighDateTime(0xFFFFFFFF);
		endTime.setLowDateTime(0xFFFFFFFF);
	}
	// Part 11 explicitly allows startTime and endTime to be the same and to return events
	// at the given startTime. To account for this case, we increment the upper bound by one millisecond.
	if(continuationStartTime.compare(endTime) == 0)
	{
		endTime.addMilliseconds(1);
	}
	bool timeIsFlowingForward =	(continuationStartTime.compare(endTime) <= 0);

	//////////////////////////////////////////////////////////////////////////
	// Reverse sequence of values temporarily if values are to be returned in reverse order
	//////////////////////////////////////////////////////////////////////////
	if(!timeIsFlowingForward)
	{
		m_historicalValues.reverse();
	}

	//////////////////////////////////////////////////////////////////////////
	// Find the lower bound
	//////////////////////////////////////////////////////////////////////////
	bool moreDataToReturn = false;
	std::list<DataValue>::const_iterator it = m_historicalValues.begin();

	if(processingState == EnumReadRawProcessingState_LowerBound)
	{
		std::list<DataValue>::const_iterator lowerBoundIt = m_historicalValues.end();
		for(it = m_historicalValues.begin(); it != m_historicalValues.end(); it++)
		{
			const DataValue& dataValue = (*it);
			const IDateTime* sourceTimestamp = dataValue.getSourceTimestamp();

			if	(
					(timeIsFlowingForward && (sourceTimestamp->compare(continuationStartTime) <= 0)) ||
					(!timeIsFlowingForward && (sourceTimestamp->compare(continuationStartTime) >= 0))
				)
			{
				lowerBoundIt = it;
			}
			else
			{
				break;
			}
		}
		if(lowerBoundIt != m_historicalValues.end())
		{
			const DataValue& lowerBound = *(lowerBoundIt);
			if(returnBoundingValues || (lowerBound.getSourceTimestamp()->compare(continuationStartTime) == 0))
			{
				historicalValues.push_back(lowerBound);
			}
		}
		else
		{
			if(returnBoundingValues)
			{
				// enqueue BoundNotFound
				DataValue boundNotFoundDataValue;
				boundNotFoundDataValue.setStatusCode(EnumStatusCode_BadBoundNotFound);
				boundNotFoundDataValue.setSourceTimestamp(continuationStartTime);
				boundNotFoundDataValue.setServerTimestamp(continuationStartTime);
				historicalValues.push_back(boundNotFoundDataValue);
			}
		}
		processingState = EnumReadRawProcessingState_WithinBounds;
	}
	if(maxNumberOfValuesPerNode > 0 && (historicalValues.size() == maxNumberOfValuesPerNode))
	{
		moreDataToReturn = true;
	}

	//////////////////////////////////////////////////////////////////////////
	// Find values within bound
	//////////////////////////////////////////////////////////////////////////
	if(processingState == EnumReadRawProcessingState_WithinBounds && !moreDataToReturn)
	{
		for(it = m_historicalValues.begin(); it != m_historicalValues.end() && !moreDataToReturn ; it++)
		{
			const DataValue& dataValue = (*it);
			const IDateTime* sourceTimestamp = dataValue.getSourceTimestamp();

			if	(
					(timeIsFlowingForward && (sourceTimestamp->compare(continuationStartTime) <= 0 || sourceTimestamp->compare(endTime) >= 0)) ||
					(!timeIsFlowingForward && (sourceTimestamp->compare(continuationStartTime) >= 0 || sourceTimestamp->compare(endTime) <= 0))
				)
			{
				continue;
			}

			historicalValues.push_back(dataValue);
			continuationStartTime = sourceTimestamp;

			if(maxNumberOfValuesPerNode > 0 && (historicalValues.size() == maxNumberOfValuesPerNode))
			{
				moreDataToReturn = true;
				break;
			}
		}

		if(!moreDataToReturn)
		{
			processingState = EnumReadRawProcessingState_UpperBound;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Find the upper bound
	//////////////////////////////////////////////////////////////////////////
	if(processingState == EnumReadRawProcessingState_UpperBound && !moreDataToReturn)
	{
		if(returnBoundingValues)
		{
			for(it = m_historicalValues.begin(); it != m_historicalValues.end(); it++)
			{
				const DataValue& dataValue = (*it);
				const IDateTime* sourceTimestamp = dataValue.getSourceTimestamp();

				if	(
						(timeIsFlowingForward && (sourceTimestamp->compare(endTime) >= 0)) ||
						(!timeIsFlowingForward && (sourceTimestamp->compare(endTime) <= 0))
					)
				{
					break;
				}
			}

			if(it != m_historicalValues.end())
			{
				historicalValues.push_back(*(it));
			}
			else
			{
				// enqueue BoundNotFound
				DataValue boundNotFoundDataValue;
				boundNotFoundDataValue.setStatusCode(EnumStatusCode_BadBoundNotFound);

				boundNotFoundDataValue.setSourceTimestamp(endTime);
				boundNotFoundDataValue.setServerTimestamp(endTime);

				historicalValues.push_back(boundNotFoundDataValue);
			}
		}
		processingState = EnumReadRawProcessingState_Finished;
	}

	// Restore sequence of values if required
	if(!timeIsFlowingForward)
	{
		m_historicalValues.reverse();
	}

	// store the start time for the next continuation part
	continuationPoint->setContextFromTimestamp(continuationStartTime);
	continuationPoint->m_processingState = processingState;

	return result;
}

void HistorizingVariable::addHistoricalValue(const DataValue& dataValue)
{
	if(!m_historizingActive)
	{
		return;
	}
	Util_SingleLock<Util_Sync> lock(sync);

	while(m_historicalValues.size() >= m_maxNumberOfHistoricalValues)
	{
		m_historicalValues.pop_front();
	}

	DataValue valueToAdd(dataValue);

	const DateTime& sourceTimestamp = valueToAdd.getSourceTimestamp();
	const DateTime& serverTimestamp = valueToAdd.getServerTimestamp();

	bool sourceTimestampProvided = !(sourceTimestamp.isNull());
	bool serverTimestampProvided = !(serverTimestamp.isNull());

	if(!sourceTimestampProvided || !serverTimestampProvided)
	{
		DateTime currentTime;
		currentTime.utcNow();

		if(!sourceTimestampProvided)
		{
			valueToAdd.setSourceTimestamp(currentTime);
		}
		if(!serverTimestampProvided)
		{
			valueToAdd.setServerTimestamp(currentTime);
		}
	}

	m_historicalValues.push_back(valueToAdd);
	m_historicalValues.sort(compareValueBySourceTimestamp());
}

void HistorizingVariable::enableHistorizing(bool enable)
{
	m_historizingActive = enable;
	if(!m_historizingActive)
	{
		m_historicalValues.clear();
	}
}

EnumStatusCode HistorizingVariable::getProcessedHistoricalValues(HistorizingVariableReadProcessedContinuationPointPtr continuationPoint, const NumericRange& indexRange, OTDouble processingInterval, const NodeId& aggregateType, std::list<DataValue>& historicalValues)
{
	Util_SingleLock<Util_Sync> lock(sync);
	EnumStatusCode result = EnumStatusCode_Good;

	// Calculate the requested aggregate
	if(aggregateType.getIdentifierType() == EnumIdentifierType_Numeric &&
		aggregateType.getNamespaceIndex() == 0)
	{
		switch(aggregateType.getIdentifier().getUInt32())
		{
		case EnumNumericNodeId_AggregateFunction_Average:
			result = verifyAverageAggregate(indexRange);
			if(StatusCode::isSUCCEEDED(result))
			{
				result = calculateAverageAggregate(continuationPoint, processingInterval, historicalValues);
			}
			break;

		default:
			result = EnumStatusCode_BadAggregateNotSupported;
		}
	}
	else
	{
		result = EnumStatusCode_BadAggregateNotSupported;
	}

	return result;
}

EnumStatusCode HistorizingVariable::verifyAverageAggregate(const NumericRange &indexRange)
{
	EnumStatusCode result = EnumStatusCode_Good;

	// Verify that the variable is scalar
	if(getValueRank() != ValueRank_Scalar || indexRange.getDimensions() > 0)
	{
		result = EnumStatusCode_BadAggregateInvalidInputs;
	}

	// Verify that the variable's data type is numeric
	ObjectPointer<BaseNode> dataType;
	if(StatusCode::isSUCCEEDED(result))
	{
		const INodeId* pDataTypeId = getDataType();
		dataType = AddressSpaceRoot::instance()->getNode(pDataTypeId);
		if(dataType.isNull())
		{
			result = EnumStatusCode_BadAggregateInvalidInputs;
		}
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		NodeId numberId(EnumNumericNodeId_Number);
		if(!dataType->isOfUaType(&numberId, true))
		{
			result = EnumStatusCode_BadAggregateInvalidInputs;
		}
	}

	return result;
}

EnumStatusCode HistorizingVariable::calculateAverageAggregate(HistorizingVariableReadProcessedContinuationPointPtr continuationPoint, OTDouble processingInterval, std::list<DataValue> &aggregateValues)
{
	EnumStatusCode result = EnumStatusCode_Good;

	TimeSliceIterator timeSliceIterator(&m_historicalValues, continuationPoint->m_startTime, continuationPoint->m_endTime, processingInterval);
	OTUInt64 numberOfTimeSlices = timeSliceIterator.getNumberOfTimeSlices();

	// Iterate over all time slices. Each time slice has a length of at most "processingInterval".
	for(size_t i = 0; i < min(cMaxNumberOfTimeSlicesPerRequest, numberOfTimeSlices); i++)
	{
		TimeSlice timeSlice;
		result = timeSliceIterator.getNextTimeSlice(timeSlice);
		if(!StatusCode::isSUCCEEDED(result))
		{
			break;
		}

		const std::list<const DataValue*>& valuesInTimeSlice = timeSlice.getDataValues();
		double average = 0.0;
		size_t numberOfSamples = 0;
		if(valuesInTimeSlice.size() > 0)
		{
			result = calculateAverage(valuesInTimeSlice, average, numberOfSamples);
		}

		if(StatusCode::isSUCCEEDED(result) && numberOfSamples > 0)
		{
			Value doubleValue;
			doubleValue.setDouble(average);

			DataValue dataValue;
			dataValue.setValue(doubleValue);
			dataValue.setStatusCode(calculateAverageStatusCode(continuationPoint, valuesInTimeSlice));
			dataValue.setSourceTimestamp(timeSlice.getStartTime());
			dataValue.setServerTimestamp(timeSlice.getStartTime());

			aggregateValues.push_back(dataValue);
		}
		else
		{
			DataValue dataValue;
			dataValue.setStatusCode(EnumStatusCode_BadNoData);
			dataValue.setSourceTimestamp(timeSlice.getStartTime());
			dataValue.setServerTimestamp(timeSlice.getStartTime());

			aggregateValues.push_back(dataValue);
		}
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		continuationPoint->m_isFinished = numberOfTimeSlices <= cMaxNumberOfTimeSlicesPerRequest;
	}
	else
	{
		continuationPoint->m_isFinished = true;
	}

	return result;
}

EnumStatusCode HistorizingVariable::calculateAverage(const std::list<const DataValue*>& numericValues, double& average, size_t& numberOfSamples)
{
	EnumStatusCode result = EnumStatusCode_Good;

	double total = 0.0;
	for(std::list<const DataValue*>::const_iterator it = numericValues.begin();
		it != numericValues.end(); it++)
	{
		const DataValue* pDataValue = *it;

		Value castedValue;
		if(StatusCode::isSUCCEEDED(pDataValue->getValue()->castTo(EnumDataTypeId_Double, castedValue)))
		{
			if(StatusCode::isGood(pDataValue->getStatusCode()))
			{
				total += castedValue.getDouble();
				numberOfSamples++;
			}
		}
		else
		{
			result = EnumStatusCode_BadAggregateInvalidInputs;
			break;
		}
	}

	if(numberOfSamples > 0)
	{
		average = total / numberOfSamples;
	}

	return result;
}

EnumStatusCode HistorizingVariable::calculateAverageStatusCode(HistorizingVariableReadProcessedContinuationPointPtr continuationPoint, const std::list<const DataValue*>& numericValues)
{
	EnumStatusCode averageStatusCode = EnumStatusCode_Good;

	size_t totalNumberOfValues = 0;
	size_t numberOfGoodStatusCodes = 0;
	size_t numberOfBadStatusCodes = 0;

	for(std::list<const DataValue*>::const_iterator it = numericValues.begin();
		it != numericValues.end(); it++)
	{
		const DataValue* pDataValue = *it;

		totalNumberOfValues++;

		EnumStatusCode historicalStatusCode = pDataValue->getStatusCode();
		if(StatusCode::isGood(historicalStatusCode) || (StatusCode::isUncertain(historicalStatusCode) && !continuationPoint->m_aggregateConfiguration.getTreatUncertainAsBad()))
		{
			numberOfGoodStatusCodes++;
		}
		else if(StatusCode::isBad(historicalStatusCode) || (StatusCode::isUncertain(historicalStatusCode) && continuationPoint->m_aggregateConfiguration.getTreatUncertainAsBad()))
		{
			numberOfBadStatusCodes++;
		}
		else
		{
			_REPORT_ASSERTION(_T("Unhandled status code"));
		}
	}

	averageStatusCode = EnumStatusCode_Good;
	if (((double)numberOfGoodStatusCodes / totalNumberOfValues) * 100 < continuationPoint->m_aggregateConfiguration.getPercentDataGood())
	{
		averageStatusCode = EnumStatusCode_UncertainDataSubNormal;
	}
	if (continuationPoint->m_aggregateConfiguration.getPercentDataGood() != continuationPoint->m_aggregateConfiguration.getPercentDataBad() &&
		((double)numberOfBadStatusCodes / totalNumberOfValues) * 100 >= continuationPoint->m_aggregateConfiguration.getPercentDataBad())
	{
		averageStatusCode = EnumStatusCode_Bad;
	}

	return averageStatusCode;
}
#endif

HistorizingVariable::HistorizingVariableReadRawContinuationPoint::HistorizingVariableReadRawContinuationPoint()
{
	m_processingState = EnumReadRawProcessingState_LowerBound;
}

HistorizingVariable::HistorizingVariableReadProcessedContinuationPoint::HistorizingVariableReadProcessedContinuationPoint()
{
	m_isFinished = false;
	m_startTime.clear();
	m_endTime.clear();
	m_aggregateConfiguration.clear();
}
