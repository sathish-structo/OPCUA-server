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
#include "HistorizingExclusiveLimitAlarm.h"
#include "Util_SingleLock.h"
#include "ServerEventFilterEvaluator.h"
#include "ServerHistoryReadEventContinuationPoint.h"
#include "DiagnosticMessages.h"

HistorizingExclusiveLimitAlarm::HistorizingExclusiveLimitAlarm()
	: Server::ExclusiveLimitAlarm()
{
}

EnumStatusCode HistorizingExclusiveLimitAlarm::handleHistoryReadEventRequest(Server::HistoryReadEventTransaction* transaction, Server::HistoryReadEventRequest* request)
{
	EnumStatusCode requestStatus = EnumStatusCode_Good;

	//////////////////////////////////////////////////////////////////////////
	// Collect request parameters (from request or continuation point)
	//////////////////////////////////////////////////////////////////////////
	DateTime startTime, endTime;
	EventFilter filter;
	OTUInt32 maxNumberOfValuesPerNode = 0;
	EnumTimestampsToReturn timestampsToReturn = EnumTimestampsToReturn_Neither;

	if(request->hasContinuationPoint())
	{
		Server::HistoryReadEventContinuationPointPtr continuationPoint;
		requestStatus = request->getContinuationPoint(continuationPoint);

		if(StatusCode::isGood(requestStatus))
		{
			startTime = continuationPoint->getContextAsTimestamp();
			endTime.set(continuationPoint->getEndTime());
			continuationPoint->getFilter(filter);
			maxNumberOfValuesPerNode = continuationPoint->getMaxNumberOfValuesPerNode();
			timestampsToReturn = continuationPoint->getTimestampsToReturn();
		}
	}
	else
	{
		startTime = transaction->getStartTime();
		endTime = transaction->getEndTime();
		transaction->getFilter(filter);
		maxNumberOfValuesPerNode = transaction->getMaxNumberOfValuesPerNode();
		timestampsToReturn = transaction->getTimestampToReturn();
	}

	//////////////////////////////////////////////////////////////////////////
	// Process request
	//////////////////////////////////////////////////////////////////////////

	if(StatusCode::isGood(requestStatus))
	{
		DateTime nextStartTime;
		std::vector<HistoryEventFieldList> historicalEvents;
		requestStatus = processRequest(startTime, endTime, filter, maxNumberOfValuesPerNode, timestampsToReturn, historicalEvents, nextStartTime);

		if(StatusCode::isGood(requestStatus))
		{
			request->setEvents(historicalEvents);
			if(!nextStartTime.isNull())
			{
				Server::HistoryReadEventContinuationPointPtr continuationPoint;
				if(request->hasContinuationPoint())
				{
					EnumStatusCode getContinuationPointResult = request->getContinuationPoint(continuationPoint);
					_ASSERTION(StatusCode::isGood(getContinuationPointResult), _T("An error occurred while retrieving a ContinuationPoint"));
					_UNUSED(getContinuationPointResult);
				}
				else
				{
					continuationPoint = Server::HistoryReadEventContinuationPoint::create();
					requestStatus = request->setContinuationPoint(continuationPoint);
				}
				_ASSERTION(continuationPoint, _T("The pointer to ContinuationPoint should always be set"));
				if(continuationPoint)
				{
					continuationPoint->setContextFromTimestamp(nextStartTime);
				}
			}
		}
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

	request->setStatusCode(requestStatus);
	request->complete();

	return EnumStatusCode_Good;
}

EnumStatusCode HistorizingExclusiveLimitAlarm::processRequest(
	DateTime startTime,
	DateTime endTime,
	const EventFilter& filter,
	OTUInt32 maxNumberOfValuesPerNode,
	EnumTimestampsToReturn timestampsToReturn,
	std::vector<HistoryEventFieldList>& historicalEvents,
	DateTime& nextStartTime)
{
	(void) timestampsToReturn;
	Util_SingleLock<Util_Sync> lock(m_sync);
	EnumStatusCode requestStatus = EnumStatusCode_Good;

	// Reverse sequence of events (temporarily) if events are to be returned in reverse order
	bool timeFlowsForwards =	(!startTime.isNull() && !endTime.isNull() && startTime.compare(endTime) <= 0) ||
								(!startTime.isNull() && endTime.isNull() && maxNumberOfValuesPerNode > 0);
	if(!timeFlowsForwards)
	{
		m_historicalEvents.reverse();
	}

	// Part 11 explicitly allows startTime and endTime to be the same and to return events
	// at the given startTime. To account for this case, we increment the upper bound by one millisecond.
	if(startTime.compare(endTime) == 0)
	{
		endTime.addMilliseconds(1);
	}

	OTUInt32 numberOfCollectedEvents = 0;
	for(std::list<HistoricalEvent>::iterator it = m_historicalEvents.begin();
		it != m_historicalEvents.end() && StatusCode::isGood(requestStatus); it++)
	{
		HistoricalEvent& historicalEvent = *it;

		// Skip event if it is not included in the requested time range
		DateTime eventTime = historicalEvent.getTime();
		if	(
				(timeFlowsForwards && !endTime.isNull() && (eventTime.compare(startTime) < 0 || eventTime.compare(endTime) >= 0)) ||
				(timeFlowsForwards && endTime.isNull() && (eventTime.compare(startTime) < 0)) ||
				(!timeFlowsForwards && !startTime.isNull() && (eventTime.compare(endTime) <= 0 || eventTime.compare(startTime) > 0)) ||
				(!timeFlowsForwards && startTime.isNull() && (eventTime.compare(endTime) >= 0))
			)
		{
			continue;
		}

		// Break loop if the requested maximum number of events has been collected
		if(maxNumberOfValuesPerNode > 0 && numberOfCollectedEvents == maxNumberOfValuesPerNode)
		{
			nextStartTime = eventTime;
			break;
		}

		// Evaluate where-clause
		bool historicalEventPassesFilter;
		EnumStatusCode evaluateFilterResult = SoftingOPCToolbox5::Server::EventFilterEvaluator::evaluate(
			filter.getWhereClause(), historicalEvent, historicalEventPassesFilter);
		if(StatusCode::isGood(evaluateFilterResult) && historicalEventPassesFilter)
		{
			// evaluate select-clause
			HistoryEventFieldList eventFieldList;

			OTUInt32 selectClauseCount = filter.getSelectClauseCount();
			for(OTUInt32 i = 0; i < selectClauseCount; i++)
			{
				SimpleAttributeOperand selectClause = filter.getSelectClause(i);
				Value attributeValue;

				// Validate TypeDefinitionId
				bool typeMatches = isInstanceOfUaType(selectClause.getTypeDefinitionId(), true);
				if(typeMatches)
				{
					EnumStatusCode getAttributeValueStatus = historicalEvent.getAttributeValue(selectClause.getBrowsePath(), selectClause.getAttributeId(), attributeValue);
					if(StatusCode::isGood(getAttributeValueStatus))
					{
						// Apply IndexRange
						const INumericRange* indexRange = selectClause.getIndexRange();
						if(indexRange->getDimensions() > 0)
						{
							Value attributeValueSubset;
							EnumStatusCode getSubsetResult = attributeValue.getSubset(attributeValueSubset, indexRange);
							if(StatusCode::isGood(getSubsetResult))
							{
								attributeValue = attributeValueSubset;
							}
							else
							{
								attributeValue.setStatusCode(getSubsetResult);
							}
						}
					}
					else
					{
						attributeValue.setStatusCode(getAttributeValueStatus);
					}
				}

				eventFieldList.addEventField(&attributeValue);
			}

			historicalEvents.push_back(eventFieldList);
			numberOfCollectedEvents++;
		}
	}

	// Restore event sequence
	if(!timeFlowsForwards)
	{
		m_historicalEvents.reverse();
	}

	if(StatusCode::isGood(requestStatus) && historicalEvents.empty())
	{
		requestStatus = EnumStatusCode_GoodNoData;
	}

	return requestStatus;
}

void HistorizingExclusiveLimitAlarm::addHistoricalEvent(const HistoricalEvent& historicalEvent)
{
	Util_SingleLock<Util_Sync> lock(m_sync);

	m_historicalEvents.push_back(historicalEvent);
	m_historicalEvents.sort(); // keep events sorted by their Time field
}
#endif
