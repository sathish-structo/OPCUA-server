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
#include "TestMethod.h"
#include "Util_Timer.h"
#include "Util_Sync.h"
#include "Util_Thread.h"
#include "ApplicationModule.h"
#include "DateTime.h"
#include "ServerAddressSpaceRoot.h"
#include "TestServerSession.h"
#include "DiagnosticMessages.h"
#include "ServerSubscription.h"
#include "ServerMonitoredItem.h"
#include "ServerBaseEventNotification.h"
#include "EventFieldList.h"
#include "Guid.h"
#include "ToolkitTrace.h"
#include "Application.h"
#include "ServerBaseEvent.h"
#include "Argument.h"
#include "EUInformation.h"
#include "Range.h"
#include "ServerAnalogItem.h"

class AsyncThread : public Util_Thread
{
	public:
		void *context;
		virtual long run();
};

TestMethod::TestMethod(unsigned int inputCount, unsigned int outputCount) : Method()
{
	m_executeAssync = false;
	m_inputCount=inputCount;
	m_outputCount=outputCount;
}

EnumStatusCode TestMethod::handleExecuteRequest
	(
	Server::MethodCallTransaction* transaction,
	Server::MethodCallRequest* request
	)
{
	EnumStatusCode result = EnumStatusCode_Good;

	//_tprintf(_T("TestMethod request %x: Execute %s\n"), (unsigned long)(&(*request)), m_executeAssync ? _T("asynchronously") : _T("synchronously"));
	if(m_executeAssync)
	{
		// allocate memory owned by the thread with a pointer to test method and specific request.
		// the request is a reference counted pointer, so core object will not be deleted!
		AsyncExecData * pAsyncData = (AsyncExecData *)malloc(sizeof(*pAsyncData));
		memset(pAsyncData, 0, sizeof(*pAsyncData));
		pAsyncData->pTestMethod = this;
		pAsyncData->transaction = transaction;
		pAsyncData->request = request;
		AsyncThread *pthread = new AsyncThread();
		pthread->context = pAsyncData;

		if(!getApplicationModule()->getThreadReaper()->addThread(pthread) || !pthread->start())
		{
			delete pthread;
			free(pAsyncData);
			request->setStatusCode(EnumStatusCode_BadInternalError);
			request->complete();
		}
	}
	else
	{
		// execute the request synchronously
		result = doRequest(transaction, request);
	}
	return result;
}
EnumStatusCode TestMethod::doRequest
	(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request
	)
{
	std::vector<Value> inputArgs = request->getInputArguments();

	//for(unsigned i = 0; i < inputArgs.size(); i++)
	//{
	//	_tprintf(_T("Input Value: %s\n"), inputArgs[i].toString().c_str());
	//}

	// we repeat the given input args as outputs
	std::vector<Value> outputArgs;
	for(unsigned i = 0; i < m_outputCount; i++)
	{
		if(i < inputArgs.size())
		{
			outputArgs.push_back(inputArgs[i]);
		}
		else
		{
			Value val;
			val.setInt32(0);
			outputArgs.push_back(val);
		}
	}

	// for the number of input params we return a good, for the rest a bad invalid
	vector<EnumStatusCode> inputStatusCodes;
	for(unsigned i = 0; i < inputArgs.size(); i++)
	{
		if(i < m_inputCount)
		{
			inputStatusCodes.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
		}
	}

	if(m_inputCount > inputArgs.size())
	{
		request->setStatusCode(EnumStatusCode_BadArgumentsMissing);
	}
	else if(m_inputCount < inputArgs.size())
	{
		request->setStatusCode(EnumStatusCode_BadInvalidArgument);
	}

	if(inputArgs.size() >= 1)
	{
		if(inputArgs[0].getDataType() != EnumDataTypeId_UInt32 || inputArgs[0].getArrayType() != EnumValueArrayType_Scalar)
		{
			inputStatusCodes[0] = EnumStatusCode_BadTypeMismatch;
			request->setStatusCode(EnumStatusCode_BadInvalidArgument);
		}
	}
	if(inputArgs.size() >= 2)
	{
		if(inputArgs[1].getDataType() != EnumDataTypeId_UInt32 || inputArgs[1].getArrayType() != EnumValueArrayType_Scalar)
		{
			inputStatusCodes[1] = EnumStatusCode_BadTypeMismatch;
			request->setStatusCode(EnumStatusCode_BadInvalidArgument);
		}
	}
	if(inputArgs.size() >= 3)
	{
		if(inputArgs[2].getDataType() != EnumDataTypeId_Double || inputArgs[2].getArrayType() != EnumValueArrayType_Scalar)
		{
			inputStatusCodes[2] = EnumStatusCode_BadTypeMismatch;
			request->setStatusCode(EnumStatusCode_BadInvalidArgument);
		}
	}
	if(StatusCode::isGood(request->getStatusCode()))
	{

		OTUInt32 delayValue = 0;
		if(inputArgs.size() >= 1)
		{
			if(inputArgs[0].getUInt32(&delayValue) != EnumStatusCode_Good || delayValue  < 2000)
			{
				// just wait a little bit to give the application a chance to cancel the request...
				// wait time needs to be greater than 1000 ms - this is the watchdog timer of stack!
				delayValue = 2000;
			}
		}
		// TODO: remove the following block when the sporadic error is found
		{
			OTChar szMessage[256];
			_stprintf(szMessage, _T("Executing method (param 1 is %u"), delayValue);
			Trace::instance()->writeLine(EnumTraceLevel_Debug, EnumTraceGroup_User1, szMessage, _T(""));
		}
		if(m_executeAssync)
		{
			SLEEP(delayValue);
		}
	}
	if(transaction->isCancelled())
	{
		//_tprintf(_T("TestMethod request %p: isCancelled!\n"), request);
		Trace::instance()->writeLine(EnumTraceLevel_Debug, EnumTraceGroup_User1, _T("Call request cancelled (or timed out)"), _T(""));
		request->complete();
		return EnumStatusCode_BadRequestCancelledByRequest;
	}


	request->setInputArgumentResults(inputStatusCodes);
	request->setOutputArguments(outputArgs);

	// Supply some diagnostic infos if requested
	if(DiagnosticInfo::returnOperationLevelDiagnostics(transaction->getDiagnosticsToReturn()))
	{
		// DiagnosticInfo
		DiagnosticInfo diagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
			DiagnosticMessages::EnumDiagnosticMessageLevel_Operation,
			StatusCode::isGood(request->getStatusCode()) ? DiagnosticMessages::EnumDiagnosticMessage_NoError : DiagnosticMessages::EnumDiagnosticMessage_GeneralError,
			transaction);

		if(transaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_ServiceLevel_AdditionalInfo)
		{
			diagnosticInfo.setAdditionalInfo(_T("No additional information available"));
		}
		if(transaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_OperationLevel_InnerDiagnostics)
		{
			DiagnosticInfo innerDiagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
				DiagnosticMessages::EnumDiagnosticMessageLevel_Operation,
				StatusCode::isGood(request->getStatusCode()) ? DiagnosticMessages::EnumDiagnosticMessage_NoError : DiagnosticMessages::EnumDiagnosticMessage_GeneralError,
				transaction);

			diagnosticInfo.setInnerDiagnosticInfo(&innerDiagnosticInfo);
		}

		EnumStatusCode setDiagnosticInfoResult = request->setDiagnosticInfo(diagnosticInfo);
		_UNUSED(setDiagnosticInfoResult);
		_ASSERTION(StatusCode::isGood(setDiagnosticInfoResult), _T("setDiagnosticInfo() failed"));

		// InputArgumentsDiagnosticInfos
		size_t inputArgumentsCount = request->getInputArguments().size();

		std::vector<DiagnosticInfo> inputArgumentsDiagnosticInfos;
		inputArgumentsDiagnosticInfos.reserve(inputArgumentsCount);

		for(size_t inputArgumentsDiagnosticsIndex = 0; inputArgumentsDiagnosticsIndex < inputArgumentsCount; inputArgumentsDiagnosticsIndex++)
		{
			DiagnosticInfo diagnosticInfo2 = DiagnosticMessages::createDiagnosticMessage(
				DiagnosticMessages::EnumDiagnosticMessageLevel_Operation,
				StatusCode::isGood(inputStatusCodes[inputArgumentsDiagnosticsIndex]) ? DiagnosticMessages::EnumDiagnosticMessage_NoError : DiagnosticMessages::EnumDiagnosticMessage_GeneralError,
				transaction);

			if(transaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_ServiceLevel_AdditionalInfo)
			{
				diagnosticInfo2.setAdditionalInfo(_T("No additional information available"));
			}
			if(transaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_OperationLevel_InnerDiagnostics)
			{
				DiagnosticInfo innerDiagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
					DiagnosticMessages::EnumDiagnosticMessageLevel_Operation, DiagnosticMessages::EnumDiagnosticMessage_NoError, transaction);

				diagnosticInfo2.setInnerDiagnosticInfo(&innerDiagnosticInfo);
			}

			inputArgumentsDiagnosticInfos.push_back(diagnosticInfo2);
		}

		setDiagnosticInfoResult = request->setInputArgumentDiagnosticInfos(inputArgumentsDiagnosticInfos);
		_UNUSED(setDiagnosticInfoResult);
		_ASSERTION(StatusCode::isGood(setDiagnosticInfoResult), _T("setDiagnosticInfo() failed"));
	}

	//_tprintf(_T("TestMethod request %p: Completing request\n"), request);
	request->complete();
	Trace::instance()->writeLine(EnumTraceLevel_Debug, EnumTraceGroup_User1, _T("Call request completed"), _T(""));

	return EnumStatusCode_Good;
}

long AsyncThread::run()
{
	AsyncExecData * asyncData = (AsyncExecData *)context;
	TestMethod * pTest = asyncData->pTestMethod;
	pTest->doRequest(asyncData->transaction, asyncData->request);
	asyncData->transaction = NULL; // set the reference counted pointer to NULL => release the reference
	asyncData->request = NULL; // set the reference counted pointer to NULL => release the reference
	free(context);
	return 0;
}


TestPerfomanceMethod::TestPerfomanceMethod() : Method()
{
}

EnumStatusCode TestPerfomanceMethod::handleExecuteRequest
	(
	Server::MethodCallTransaction* /* transaction */,
	Server::MethodCallRequest* request
	)
{
	return doRequest(request);
}

EnumStatusCode TestPerfomanceMethod::doRequest(Server::MethodCallRequest* request)
{
	EnumStatusCode result = EnumStatusCode_Good;

	_tprintf(_T("TestPerfomanceMethod request %p\n"), request);
	std::vector<Value> inputArgs = request->getInputArguments();
	if(inputArgs.size() != 1)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}
	OTUInt32 measureTime = 0;

	if(StatusCode::isSUCCEEDED(result))
	{
		if(StatusCode::isFAILED(inputArgs[0].getUInt32(&measureTime)))
		{
			result = EnumStatusCode_BadInvalidArgument;
		}
	}


	std::vector<Value> outputArgs;
	if(StatusCode::isSUCCEEDED(result))
	{
		OTUInt32 count;
		unsigned long startTick, endTick;
		OTUInt64 startTick64, endTick64;
		OTUInt64 expectedDiffMax;
		double timeForCurrentTickCount;
		Value out;

		// measure time for gettickcount
		expectedDiffMax = measureTime;
		startTick = Util_Timer::currentTickCount();
		endTick = startTick;
		for(count=0; count< 0xFFFFFFFF; count++)
		{
			endTick = Util_Timer::currentTickCount();
			if((endTick - startTick) >= expectedDiffMax)
			{
				count++;
				break;
			}
		}

		timeForCurrentTickCount = (double)(endTick - startTick) / (double)count;
		out.setDouble(timeForCurrentTickCount);
		outputArgs.push_back(out);

		// measure time for getTickCount64
		expectedDiffMax = measureTime;
		startTick64 = Util_Timer::currentTickCount64();
		endTick64 = startTick64;
		for(count=0; count< 0xFFFFFFFF; count++)
		{
			endTick64 = Util_Timer::currentTickCount64();
			if((endTick64 - startTick64) >= expectedDiffMax)
			{
				count++;
				break;
			}
		}

		timeForCurrentTickCount = (double)(endTick64 - startTick64) / (double)count;
		out.setDouble(timeForCurrentTickCount);
		outputArgs.push_back(out);

		// measure time for UtcNow
		expectedDiffMax = measureTime * 10000; // datetime has 100 ns ticks
		DateTime startTime;
		startTime.utcNow();
		startTick = startTime.getLowDateTime();
		DateTime endTime;
		for(count=0; count< 0xFFFFFFFF; count++)
		{
			endTime.utcNow();
			if((endTime.getLowDateTime() - startTick) >= expectedDiffMax)
			{
				count++;
				break;
			}
		}
		out.setDouble(((double)(endTime.getLowDateTime() - startTick) / 10000.0) / (double)count);
		outputArgs.push_back(out);

		// measure time for interlockedincrement / decrement
		expectedDiffMax = measureTime;
		OTAtomic atom = 0;
		startTick = Util_Timer::currentTickCount();
		endTick = startTick;
		for(count=0; count< 0xFFFFFFFF; count++)
		{
			interlockedIncrement(&atom);
			endTick = Util_Timer::currentTickCount();
			if((endTick - startTick) >= expectedDiffMax)
			{
				interlockedDecrement(&atom);
				count++;
				break;
			}
			interlockedDecrement(&atom);
		}
		out.setDouble(((double)(endTick - startTick) / (double)count) - timeForCurrentTickCount);
		outputArgs.push_back(out);

		// measure time for simple lock / unlock
		expectedDiffMax = measureTime;
		Util_Sync simpleLock;
		startTick = Util_Timer::currentTickCount();
		endTick = startTick;
		for(count=0; count< 0xFFFFFFFF; count++)
		{
			simpleLock.lock();
			endTick = Util_Timer::currentTickCount();
			if((endTick - startTick) >= expectedDiffMax)
			{
				simpleLock.unlock();
				count++;
				break;
			}
			simpleLock.unlock();
		}
		out.setDouble(((double)(endTick - startTick) / (double)count) - timeForCurrentTickCount);
		outputArgs.push_back(out);


		// measure time for address space lock / unlock
		expectedDiffMax = measureTime;
		Server::AddressSpaceRoot * pRoot = Server::AddressSpaceRoot::instance();;
		startTick = Util_Timer::currentTickCount();
		endTick = startTick;
		for(count=0; count< 0xFFFFFFFF; count++)
		{
			pRoot->lock();
			endTick = Util_Timer::currentTickCount();
			if((endTick - startTick) >= expectedDiffMax)
			{
				pRoot->unlock();
				count++;
				break;
			}
			pRoot->unlock();
		}
		out.setDouble(((double)(endTick - startTick) / (double)count) - timeForCurrentTickCount);
		outputArgs.push_back(out);
	}

	// for the number of input params we return a good, for the rest a bad invalid
	vector<EnumStatusCode> inputStatusCodes;
	for(unsigned i = 0; i < inputArgs.size(); i++)
	{
		if(i == 0)
		{
			inputStatusCodes.push_back(result);
		}
		else
		{
			inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
		}
	}

	request->setInputArgumentResults(inputStatusCodes);
	request->setOutputArguments(outputArgs);
	request->setStatusCode(result);
	request->complete();

	return EnumStatusCode_Good;
}

SetQueueSizeMethod::SetQueueSizeMethod() : Method()
{

}

EnumStatusCode SetQueueSizeMethod::handleExecuteRequest( Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request )
{
	EnumStatusCode result = EnumStatusCode_Good;
	OTUInt32 magic = 0;
	OTUInt32 maxSize = 0;

	vector<EnumStatusCode> inputStatusCodes;
	std::vector<Value> outputArgs;
	std::vector<Value> inputArgs = request->getInputArguments();

	for(size_t i=0; i<inputArgs.size(); i++)
	{
		switch(i)
		{
		case 0:
		case 1:
			// both have UInt32
			{
				OTUInt32 *pVal = NULL;
				if(i==0)
				{
					pVal = &magic;
				}
				else
				{
					pVal = &maxSize;
				}
				if(StatusCode::isFAILED(inputArgs[i].getUInt32(pVal)))
				{
					inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
					result = EnumStatusCode_BadInvalidArgument;
				}
				else
				{
					inputStatusCodes.push_back(EnumStatusCode_Good);
				}
			}
			break;
		default:
			inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
			result = EnumStatusCode_BadInvalidArgument;
			// all others are wrong
			break;
		}
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		if(magic != 170262)
		{
			result = EnumStatusCode_BadInvalidArgument;
		}
		if(maxSize > Application::instance()->getMaxEventMonitoringQueueSize())
		{
			// never allow to increase the size
			result = EnumStatusCode_BadInvalidArgument;
		}
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		TestServerSessionPtr session = static_cast<TestServerSession*>((Server::Session*)transaction->getSession());
		session->setMaxEventQueueSize(maxSize);
	}

	request->setInputArgumentResults(inputStatusCodes);
	request->setOutputArguments(outputArgs);
	request->setStatusCode(result);
	request->complete();

	return EnumStatusCode_Good;
}


InsertDataNotificationMethod::InsertDataNotificationMethod()
{

}

EnumStatusCode InsertDataNotificationMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	EnumStatusCode result = EnumStatusCode_Good;
	OTUInt32 subscriptionId = 0;
	OTUInt32 monitoredItemId = 0;
	std::vector<DataValue> dataValuesToReport;
	bool useFilter = false;
	bool useSleep = true;
	MonitoredItemPtr monitoredItem;

	vector<EnumStatusCode> inputStatusCodes;
	std::vector<Value> outputArgs;
	std::vector<Value> inputArgs = request->getInputArguments();

	if(inputArgs.size() < 5)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	if(inputArgs.size() > 5)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		for(size_t i=0; i<inputArgs.size(); i++)
		{
			switch(i)
			{
			case 0:
				// subscription ID
				if(StatusCode::isFAILED(inputArgs[i].getUInt32(&subscriptionId)))
				{
					inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
					result = EnumStatusCode_BadInvalidArgument;
				}
				else
				{
					inputStatusCodes.push_back(EnumStatusCode_Good);
				}
				break;
			case 1:
				// a monitored item ID
				if(StatusCode::isFAILED(inputArgs[i].getUInt32(&monitoredItemId)))
				{
					inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
					result = EnumStatusCode_BadInvalidArgument;
				}
				else
				{
					inputStatusCodes.push_back(EnumStatusCode_Good);
				}
				break;
			case 2:
				// data value to report
				{
					if(StatusCode::isFAILED(inputArgs[i].getDataValueArray(dataValuesToReport)))
					{
						inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
						result = EnumStatusCode_BadInvalidArgument;
					}
					else
					{
						inputStatusCodes.push_back(EnumStatusCode_Good);
					}
				}
				break;
			case 3:
				// use filter
				{
					if(StatusCode::isFAILED(inputArgs[i].getBoolean(&useFilter)))
					{
						inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
						result = EnumStatusCode_BadInvalidArgument;
					}
					else
					{
						inputStatusCodes.push_back(EnumStatusCode_Good);
					}
				}
				break;
			case 4:
				// use filter
				{
					if(StatusCode::isFAILED(inputArgs[i].getBoolean(&useSleep)))
					{
						inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
						result = EnumStatusCode_BadInvalidArgument;
					}
					else
					{
						inputStatusCodes.push_back(EnumStatusCode_Good);
					}
				}
				break;
			default:
				inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
				result = EnumStatusCode_BadInvalidArgument;
				// all others are wrong
				break;
			}
		}
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		SessionPtr session = transaction->getSession();
		SubscriptionPtr subscription;
		if(session)
		{
			std::vector<SubscriptionPtr> subscriptions = session->getSubscriptions();
			for(size_t i=0; i<subscriptions.size(); i++)
			{
				if(subscriptions[i]->getId() == subscriptionId)
				{
					subscription = subscriptions[i];
					break;
				}
			}
		}
		if(subscription)
		{
			std::vector<MonitoredItemPtr> monitoredItems = subscription->getMonitoredItems();
			for(size_t i=0; i<monitoredItems.size(); i++)
			{
				if(monitoredItems[i]->getId() == monitoredItemId)
				{
					monitoredItem = monitoredItems[i];
					break;
				}
			}
		}
		if(monitoredItem.isNull())
		{
			// no MI found
			result = EnumStatusCode_BadInvalidArgument;
		}
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		for(size_t i=0; i<dataValuesToReport.size(); i++)
		{
			// wait longer than sampling interval - otherwise the values would be overwritten...
			// use an additional time longer than one tick of the system clock!
			if(useSleep)
			{
				SLEEP((OTUInt32)(monitoredItem->getSamplingInterval()+ 20.0));
			}
			DateTime beforeTime;
			beforeTime.utcNow();
			monitoredItem->reportDataChange(dataValuesToReport[i], useFilter);

			DateTime afterTime;
			afterTime.utcNow();
			OTUInt64 duration = DateTime::getDurationMilliseconds(beforeTime, afterTime);
			// no sample is enqueued - may happen for the first value (just published, no new value => value is stored as intermediate value)
			// TODO: remove trace when sporadic error is found
			OTChar szMessage[100];
			_stprintf(szMessage, _T("ReportDataChange called while injecting data for index %u (%u ms)"), (OTUInt32)i, (OTUInt32)duration);
			Trace::instance()->writeLine(EnumTraceLevel_Info, EnumTraceGroup_User1, szMessage, _T(""));
		}
	}

	request->setInputArgumentResults(inputStatusCodes);
	request->setOutputArguments(outputArgs);
	request->setStatusCode(result);
	request->complete();

	return EnumStatusCode_Good;
}

#if TB5_ALARMS
InsertEventNotificationMethod::InsertEventNotificationMethod()
{

}

EnumStatusCode InsertEventNotificationMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	EnumStatusCode result = EnumStatusCode_Good;
	OTUInt32 subscriptionId = 0;
	OTUInt32 monitoredItemId = 0;
	BaseEventNotificationPtr eventNotificationToReport;
	BaseEventPtr eventToReport;
	EventFieldList eventFieldListToReport;
	NodeId eventType;
	std::vector<QualifiedName> propertiesOfEvent;
	std::vector<Value> valuesOfEvent;
	bool useFilter = false;
	MonitoredItemPtr monitoredItem;

	OTUInt32 injectOption = 0;

	vector<EnumStatusCode> inputStatusCodes;
	std::vector<Value> outputArgs;
	std::vector<Value> inputArgs = request->getInputArguments();

	if(inputArgs.size() < 7)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	if(inputArgs.size() > 7)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		for(size_t i=0; i<inputArgs.size(); i++)
		{
			switch(i)
			{
			case 0:
				// subscription ID
				if(StatusCode::isFAILED(inputArgs[i].getUInt32(&subscriptionId)))
				{
					inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
					result = EnumStatusCode_BadInvalidArgument;
				}
				else
				{
					inputStatusCodes.push_back(EnumStatusCode_Good);
				}
				break;
			case 1:
				// a monitored item ID
				if(StatusCode::isFAILED(inputArgs[i].getUInt32(&monitoredItemId)))
				{
					inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
					result = EnumStatusCode_BadInvalidArgument;
				}
				else
				{
					inputStatusCodes.push_back(EnumStatusCode_Good);
				}
				break;
			case 2:
				// option how to inject the event
				if(StatusCode::isFAILED(inputArgs[i].getUInt32(&injectOption)) || (injectOption > 2))
				{
					inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
					result = EnumStatusCode_BadInvalidArgument;
				}
				else
				{
					inputStatusCodes.push_back(EnumStatusCode_Good);
				}
				break;
			case 3:
				// node ID of event type
				if(StatusCode::isFAILED(inputArgs[i].getNodeId(eventType)))
				{
					inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
					result = EnumStatusCode_BadInvalidArgument;
				}
				else
				{
					inputStatusCodes.push_back(EnumStatusCode_Good);
				}
				break;
			case 4:
				// properties of event to report
				{
					if(StatusCode::isFAILED(inputArgs[i].getQualifiedNameArray(propertiesOfEvent)))
					{
						inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
						result = EnumStatusCode_BadInvalidArgument;
					}
					else
					{
						inputStatusCodes.push_back(EnumStatusCode_Good);
					}
				}
				break;
			case 5:
				// properties of event to report
				{
					if(StatusCode::isFAILED(inputArgs[i].getValueArray(valuesOfEvent)))
					{
						inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
						result = EnumStatusCode_BadInvalidArgument;
					}
					else
					{
						inputStatusCodes.push_back(EnumStatusCode_Good);
					}
				}
				break;
			case 6:
				// use filter
				{
					if(StatusCode::isFAILED(inputArgs[i].getBoolean(&useFilter)))
					{
						inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
						result = EnumStatusCode_BadInvalidArgument;
					}
					else
					{
						inputStatusCodes.push_back(EnumStatusCode_Good);
					}
				}
				break;
			default:
				inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
				result = EnumStatusCode_BadInvalidArgument;
				// all others are wrong
				break;
			}
		}
	}

	if(propertiesOfEvent.size() != valuesOfEvent.size())
	{
		result = EnumStatusCode_BadInvalidArgument;
	}
	if(propertiesOfEvent.size() == 0)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		switch(injectOption)
		{
		case 0: // report via EventFieldList
			for(size_t i=0; i<propertiesOfEvent.size(); i++)
			{
				eventFieldListToReport.addEventField(&valuesOfEvent[i]);
			}
			break;
		case 1: // report via BaseEventNotification
			eventNotificationToReport = BaseEventNotification::create(&eventType);
			for(size_t i=0; i<propertiesOfEvent.size(); i++)
			{
				eventNotificationToReport->setProperty(propertiesOfEvent[i], valuesOfEvent[i]);
			}
			break;
		case 2: // report via BaseEvent
			{
				eventToReport = BaseEvent::create();
				NodeId eventNodeId;
				Guid guid = Guid::generateGuid();
				eventNodeId.setGuid(2, &guid);
				eventToReport->setNodeId(eventNodeId);
				eventToReport->init(&eventType);
				eventToReport->insertTree();
			}
			for(size_t i=0; i<propertiesOfEvent.size(); i++)
			{
				eventToReport->setProperty(propertiesOfEvent[i], valuesOfEvent[i]);
			}
			break;
		default: // invalid inject option
			break;
		}

		SessionPtr session = transaction->getSession();
		SubscriptionPtr subscription;
		if(session)
		{
			std::vector<SubscriptionPtr> subscriptions = session->getSubscriptions();
			for(size_t i=0; i<subscriptions.size(); i++)
			{
				if(subscriptions[i]->getId() == subscriptionId)
				{
					subscription = subscriptions[i];
					break;
				}
			}
		}
		if(subscription)
		{
			std::vector<MonitoredItemPtr> monitoredItems = subscription->getMonitoredItems();
			for(size_t i=0; i<monitoredItems.size(); i++)
			{
				if(monitoredItems[i]->getId() == monitoredItemId)
				{
					monitoredItem = monitoredItems[i];
					break;
				}
			}
		}
		if(monitoredItem.isNull())
		{
			// no MI found
			result = EnumStatusCode_BadInvalidArgument;
		}
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		switch(injectOption)
		{
		case 0: // report via EventFieldList
			if(eventFieldListToReport.getEventFieldCount() != 0)
			{
				monitoredItem->reportEvent(eventFieldListToReport);;
			}
			else
			{
				result = EnumStatusCode_BadInternalError;
			}
			break;
		case 1: // report via BaseEventNotification
			if(eventNotificationToReport.isNotNull())
			{
				monitoredItem->reportEvent(eventNotificationToReport, useFilter);;
			}
			else
			{
				result = EnumStatusCode_BadInternalError;
			}
			break;
		case 2: // report via BaseEvent
			if(eventToReport.isNotNull())
			{
				monitoredItem->reportEvent(eventToReport, useFilter);
				eventToReport->removeTree();
			}
			else
			{
				result = EnumStatusCode_BadInternalError;
			}
			break;
		default:
			break;
		}
	}

	request->setInputArgumentResults(inputStatusCodes);
	request->setOutputArguments(outputArgs);
	request->setStatusCode(result);
	request->complete();

	return EnumStatusCode_Good;
}
#endif

EnableTracingMethod::EnableTracingMethod()
{
	m_maxFileSize = 0;
	m_maxBackups = 0;
	m_oldTraceLevel =EnumTraceLevel_None;
	m_oldTraceGroup = EnumTraceGroup_None;

}

EnumStatusCode EnableTracingMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	EnumStatusCode result = EnumStatusCode_Good;
	bool enableTracing=false;
	tstring traceFileName;
	OTUInt32 maxBackups = 0;
	OTUInt32 maxFileSize = 0;
	OTUInt32 traceGroup = 0;
	OTUInt32 traceLevel = 0;

	vector<EnumStatusCode> inputStatusCodes;
	std::vector<Value> outputArgs;
	std::vector<Value> inputArgs = request->getInputArguments();

	if(inputArgs.size() < 6)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	if(inputArgs.size() > 6)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		EnumStatusCode argumentStatus = EnumStatusCode_Good;
		if(StatusCode::isBad(inputArgs[0].getBoolean(&enableTracing)))
		{
			argumentStatus = EnumStatusCode_BadInvalidArgument;
			result = EnumStatusCode_BadInvalidArgument;
		}
		inputStatusCodes.push_back(argumentStatus);

		argumentStatus = EnumStatusCode_Good;
		if(enableTracing)
		{
			if(StatusCode::isBad(inputArgs[1].getString(traceFileName)) || traceFileName.empty())
			{
				argumentStatus = EnumStatusCode_BadInvalidArgument;
				result = EnumStatusCode_BadInvalidArgument;
			}
		}
		inputStatusCodes.push_back(argumentStatus);

		argumentStatus = EnumStatusCode_Good;
		if(enableTracing && StatusCode::isBad(inputArgs[2].getUInt32(&maxFileSize)))
		{
			argumentStatus = EnumStatusCode_BadInvalidArgument;
			result = EnumStatusCode_BadInvalidArgument;
		}
		inputStatusCodes.push_back(argumentStatus);

		argumentStatus = EnumStatusCode_Good;
		if(enableTracing && StatusCode::isBad(inputArgs[3].getUInt32(&maxBackups)))
		{
			argumentStatus = EnumStatusCode_BadInvalidArgument;
			result = EnumStatusCode_BadInvalidArgument;
		}
		inputStatusCodes.push_back(argumentStatus);

		argumentStatus = EnumStatusCode_Good;
		if(enableTracing && StatusCode::isBad(inputArgs[4].getUInt32(&traceGroup)))
		{
			argumentStatus = EnumStatusCode_BadInvalidArgument;
			result = EnumStatusCode_BadInvalidArgument;
		}
		inputStatusCodes.push_back(argumentStatus);

		argumentStatus = EnumStatusCode_Good;
		if(enableTracing && StatusCode::isBad(inputArgs[5].getUInt32(&traceLevel)))
		{
			argumentStatus = EnumStatusCode_BadInvalidArgument;
			result = EnumStatusCode_BadInvalidArgument;
		}
		inputStatusCodes.push_back(argumentStatus);
	}

	if(enableTracing && !m_oldFileName.empty())
	{
		// tracing was already enabled before
		result = EnumStatusCode_BadInvalidState;
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		if(enableTracing)
		{
			m_oldFileName = Trace::instance()->getFileName();
			m_maxFileSize = Trace::instance()->getFileMaxSize();
			m_maxBackups = Trace::instance()->getMaximumBackups();
			Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User1, _T("Switched trace file to "), traceFileName);
			Trace::instance()->enableTraceLevel(traceGroup, (EnumTraceLevel)traceLevel);
			Trace::instance()->enableFileLogging(traceFileName, maxFileSize, maxBackups, 0);
			Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User1, _T("Started tracing in "), traceFileName);
		}
		else
		{
			// it is not possible to restore trace level - actual setting is not available
			Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User1, _T("Stopped tracing in "), traceFileName);
			Trace::instance()->disableTraceLevel(EnumTraceGroup_All, EnumTraceLevel_All);
			Trace::instance()->disableFileLogging();
			if(!m_oldFileName.empty())
			{
				Trace::instance()->enableTraceLevel(traceGroup, (EnumTraceLevel)traceLevel);
				Trace::instance()->enableFileLogging(m_oldFileName, m_maxFileSize, m_maxBackups, 0);
				Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User1, _T("Switched trace file from "), traceFileName);
				m_oldFileName.clear();
			}
		}
	}

	request->setInputArgumentResults(inputStatusCodes);
	request->setOutputArguments(outputArgs);
	request->setStatusCode(result);
	request->complete();

	return EnumStatusCode_Good;
}

SetAttributeByTypeMethod::SetAttributeByTypeMethod()
{

}

EnumStatusCode SetAttributeByTypeMethod::handleExecuteRequest(Server::MethodCallTransaction* /* transaction */, Server::MethodCallRequest* request)
{
	//ToDo: Add pw parameter for preventing erroneous calls
	EnumStatusCode result = EnumStatusCode_Good;

	NodeId nodeIdToChange;
	OTUInt32 attributId = 0;

	vector<EnumStatusCode> inputStatusCodes;
	std::vector<Value> outputArgs;
	std::vector<Value> inputArgs = request->getInputArguments();

	if(inputArgs.size() < 3)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	if(inputArgs.size() > 3)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		BaseNodePtr node;
		for(size_t i=0; i<inputArgs.size(); i++)
		{
			switch(i)
			{
			case 0:
				// subscription ID
				if(StatusCode::isFAILED(inputArgs[i].getNodeId(nodeIdToChange)))
				{
					inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
					result = EnumStatusCode_BadInvalidArgument;
				}
				else
				{
					node = AddressSpaceRoot::instance()->getNode(&nodeIdToChange);
					if(node.isNull())
					{
						inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
						result = EnumStatusCode_BadInvalidArgument;
					}
					else
					{
						inputStatusCodes.push_back(EnumStatusCode_Good);
					}
				}
				break;
			case 1:
				// a monitored item ID
				if(StatusCode::isFAILED(inputArgs[i].getUInt32(&attributId)))
				{
					inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
					result = EnumStatusCode_BadInvalidArgument;
				}
				else
				{
					inputStatusCodes.push_back(EnumStatusCode_Good);
				}
				break;
			case 2:
				// data value to report
				{
					switch(attributId)
					{
					case EnumAttributeId_BrowseName:
						{
							if(StatusCode::isGood(result))
							{
								QualifiedName browseName;
								EnumStatusCode getResult = inputArgs[i].getQualifiedName(browseName);
								if(StatusCode::isGood(getResult))
								{
									EnumStatusCode setResult = node->setBrowseName(browseName);
									inputStatusCodes.push_back(setResult);
								}
								else
								{
									inputStatusCodes.push_back(getResult);
								}
							}
						}
						break;

					case EnumAttributeId_DisplayName:
						{
							if(StatusCode::isGood(result))
							{
								LocalizedText displayName;
								EnumStatusCode getResult = inputArgs[i].getLocalizedText(displayName);
								if(StatusCode::isGood(getResult))
								{
									EnumStatusCode setResult = node->setDisplayName(displayName);
									inputStatusCodes.push_back(setResult);
								}
								else
								{
									inputStatusCodes.push_back(getResult);
								}
							}
						}
						break;

					case EnumAttributeId_Description:
						{
							if(StatusCode::isGood(result))
							{
								LocalizedText description;
								EnumStatusCode getResult = inputArgs[i].getLocalizedText(description);
								if(StatusCode::isGood(getResult))
								{
									EnumStatusCode setResult = node->setDescription(description);
									inputStatusCodes.push_back(setResult);
								}
								else
								{
									inputStatusCodes.push_back(getResult);
								}
							}
						}
						break;

					case EnumAttributeId_WriteMask:
						{
							if(StatusCode::isGood(result))
							{
								OTUInt32 writeMask;
								EnumStatusCode getResult = inputArgs[i].getUInt32(&writeMask);
								if(StatusCode::isGood(getResult))
								{
									EnumStatusCode setResult = node->setWriteMask(writeMask);
									inputStatusCodes.push_back(setResult);
								}
								else
								{
									inputStatusCodes.push_back(getResult);
								}
							}
						}
						break;
					case EnumAttributeId_NodeId:
					case EnumAttributeId_NodeClass:
					case EnumAttributeId_UserWriteMask:
					case EnumAttributeId_IsAbstract:
					case EnumAttributeId_Symmetric:
					case EnumAttributeId_InverseName:
					case EnumAttributeId_ContainsNoLoops:
					case EnumAttributeId_EventNotifier:
					case EnumAttributeId_Value:
					case EnumAttributeId_DataType:
					case EnumAttributeId_ValueRank:
					case EnumAttributeId_ArrayDimensions:
					case EnumAttributeId_AccessLevel:
					case EnumAttributeId_UserAccessLevel:
					case EnumAttributeId_MinimumSamplingInterval:
					case EnumAttributeId_Historizing:
					case EnumAttributeId_Executable:
					case EnumAttributeId_UserExecutable:
						{
							inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
						}
						break;
					default:
						inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
						result = EnumStatusCode_BadInvalidArgument;
						break;
					}
				}
				break;
			default:
				inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
				result = EnumStatusCode_BadInvalidArgument;
				// all others are wrong
				break;
			}
		}
	}

	request->setInputArgumentResults(inputStatusCodes);
	request->setOutputArguments(outputArgs);
	request->setStatusCode(result);
	request->complete();

	return EnumStatusCode_Good;
}

TestParallelAccessMethod::TestParallelAccessMethod()
{

}

ObjectPointer<TestParallelAccessMethod> TestParallelAccessMethod::initialize(OTUInt16 namespaceIndex, const tstring& name)
{
	tstring nodeName;
	ObjectPointer<TestParallelAccessMethod> testMethod = new TestParallelAccessMethod();
	testMethod->setNodeId(NodeId(namespaceIndex, name));
	testMethod->setBrowseName(QualifiedName(name, namespaceIndex));
	testMethod->setDisplayName(LocalizedText(name, _T("")));
	testMethod->setDescription(LocalizedText(_T("Tests parallel read/write access within server"), _T("")));

	std::vector<Argument> inputArguments;
	inputArguments.push_back(Argument());
	inputArguments.back().setName(_T("NodeIds"));
	inputArguments.back().setDescription(LocalizedText(_T("The NodeIds used"), _T("")));
	inputArguments.back().setDataType(Statics::DataTypeId_NodeId);
	inputArguments.back().setValueRank(1);

	inputArguments.push_back(Argument());
	inputArguments.back().setName(_T("Execution time"));
	inputArguments.back().setDescription(LocalizedText(_T("Time to execute (ms)"), _T("")));
	inputArguments.back().setDataType(Statics::DataTypeId_UInt32);
	inputArguments.back().setValueRank(ValueRank_Scalar);

	inputArguments.push_back(Argument());
	inputArguments.back().setName(_T("PrintDetails"));
	inputArguments.back().setDescription(LocalizedText(_T("Print information at server"), _T("")));
	inputArguments.back().setDataType(Statics::DataTypeId_Boolean);
	inputArguments.back().setValueRank(ValueRank_Scalar);

	nodeName = name;
	nodeName += _T("_InputArguments");
	NodeId inputArgId(namespaceIndex, nodeName);
	testMethod->setInputArguments(&inputArgId, inputArguments);
	testMethod->init(NULL);

	return testMethod;
}

EnumStatusCode TestParallelAccessMethod::handleExecuteRequest(Server::MethodCallTransaction* /* transaction */, Server::MethodCallRequest* request)
{
	std::vector<Value> inputArguments = request->getInputArguments();
	std::vector<EnumStatusCode> inputArgumentResults;

	EnumStatusCode result = validateInputArguments(inputArguments, inputArgumentResults);
	if(StatusCode::isGood(result))
	{
		ParallelAccessWorkItem* workItem = new ParallelAccessWorkItem(request);
		if(!getApplicationModule()->addWorkItem(workItem))
		{
			workItem->execute();
			delete workItem;
		}
	}
	else
	{
		request->setInputArgumentResults(inputArgumentResults);
		request->setStatusCode(result);
		request->complete();
	}

	return EnumStatusCode_Good;
}

class ParallelAccessReadThread : public Util_Thread
{
public:
	ParallelAccessReadThread(OTUInt32 timeToRun, const std::vector<VariablePtr>& variables, bool printDetails);
	virtual long run();
private:
	ParallelAccessReadThread();
	OTUInt32 m_timeToRun;
	std::vector<VariablePtr> m_variables;
	bool m_printDetailt;
};

ParallelAccessReadThread::ParallelAccessReadThread(OTUInt32 timeToRun, const std::vector<VariablePtr>& variables, bool printDetails)
{
	m_timeToRun = timeToRun;
	m_variables = variables;
	m_printDetailt = printDetails;
}

ParallelAccessReadThread::ParallelAccessReadThread()
{
	m_timeToRun = 0;
	m_printDetailt = false;
}

long ParallelAccessReadThread::run()
{
	unsigned long startTick = Util_Timer::currentTickCount();
	unsigned long currentTick = Util_Timer::currentTickCount();
	bool printOutput = m_printDetailt;
	do
	{
		for(size_t i = 0; i < m_variables.size(); i++)
		{
			// get a copy of the data value
			DataValue dv(m_variables[i]->getDataValue());
			// if printing, just print once
			if(printOutput)
			{
				_tprintf(_T("Got value for %s: %s\n"), m_variables[i]->getNodeId()->toString().c_str(), dv.getValue()->toString().c_str());
			}

            // Special handling for EventFilter type to stress the accessor of inner value
            const IValue* value = m_variables[i]->getDataValue()->getValue();
            if(value && EnumDataTypeId_EventFilter == value->getDataType())
            {
                OTUInt32 count = value->getEventFilter().getSelectClauseCount();
                EventFilter filter = value->getEventFilter();
                OTUInt32 step = 1000;
                while(step > 0)
                {
                    while(count > 0)
                    {
                        SimpleAttributeOperand op = filter.getSelectClause(count - 1);
                        _UNUSED(op);
                        count--;
                    }
                    step--;
                }

            }
		}
		printOutput = false;
		currentTick = Util_Timer::currentTickCount();
	} while((currentTick - startTick) < m_timeToRun);

	return 0;
}

class ParallelAccessWriteThread : public Util_Thread
{
public:
	ParallelAccessWriteThread(OTUInt32 timeToRun, const std::vector<VariablePtr> &variables, const std::vector<Value> &valuesToWrite, bool printDetails);
	virtual long run();
private:
	ParallelAccessWriteThread();
	OTUInt32 m_timeToRun;
	std::vector<VariablePtr> m_variables;
	std::vector<Value> m_valuesToWrite;
	bool m_printDetailt;
};

ParallelAccessWriteThread::ParallelAccessWriteThread(OTUInt32 timeToRun, const std::vector<VariablePtr>& variables, const std::vector<Value>& valuesToWrite, bool printDetails)
{
	m_timeToRun = timeToRun;
	m_variables = variables;
	m_valuesToWrite = valuesToWrite;
	m_printDetailt = printDetails;
}

ParallelAccessWriteThread::ParallelAccessWriteThread()
{
	m_timeToRun = 0;
	m_printDetailt = false;
}

long ParallelAccessWriteThread::run()
{
	unsigned long startTick = Util_Timer::currentTickCount();
	unsigned long currentTick = Util_Timer::currentTickCount();
	bool printOutput = m_printDetailt;
	do
	{
		for(size_t i = 0; i < m_variables.size(); i++)
		{
			// set a "new" value
			DataValue dv;
			dv.setValue(m_valuesToWrite[i]);
			m_variables[i]->setDataValue(dv);
			// if printing, just print once
			if(printOutput)
			{
				_tprintf(_T("Value written to %s: %s\n"), m_variables[i]->getNodeId()->toString().c_str(), m_valuesToWrite[i].toString().c_str());
			}
		}
		printOutput = false;
		currentTick = Util_Timer::currentTickCount();
	} while((currentTick - startTick) < m_timeToRun);
	return 0;
}


ParallelAccessWorkItem::ParallelAccessWorkItem(SoftingOPCToolbox5::Server::MethodCallRequestPtr request)
{
	m_request = request;
}

ParallelAccessWorkItem::~ParallelAccessWorkItem()
{
}

void ParallelAccessWorkItem::execute()
{
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<NodeId> nodeIdsToTest;
	OTUInt32 timeToTest;
	std::vector<Server::VariablePtr> variablesTorTest;
	std::vector<Value> origValues;
	std::vector<Value> inputArguments = m_request->getInputArguments();
	bool printDetails = inputArguments[2].getBoolean();

	nodeIdsToTest = inputArguments[0].getNodeIdArray();
	timeToTest = inputArguments[1].getUInt32();

	for(size_t i = 0; i < nodeIdsToTest.size(); i++)
	{
		Server::VariablePtr var = AddressSpaceRoot::instance()->getVariable(&nodeIdsToTest[i]);
		if(!var)
		{
			result = EnumStatusCode_BadNodeIdUnknown;
			break;
		}
		origValues.push_back(var->getDataValue()->getValue());
		variablesTorTest.push_back(var);
	}

	if(StatusCode::isGood(result))
	{
		ParallelAccessWriteThread* writeTest = new ParallelAccessWriteThread(timeToTest, variablesTorTest, origValues, printDetails);
		ParallelAccessReadThread* readTest = new ParallelAccessReadThread(timeToTest, variablesTorTest, printDetails);
		writeTest->start();
		readTest->start();

		writeTest->wait(INFINITE);
		readTest->wait(INFINITE);

		delete writeTest;
		delete readTest;

		if(printDetails)
		{
			_tprintf(_T("Parallel test finished\n"));
		}
	}

	m_request->setStatusCode(result);
	m_request->complete();
}

bool ArgumentDescriptionMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("ArgumentDescriptionMethod");
	setNodeId(NodeId(namespaceIndex, methodName));
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Test argument value validation and default value using HasArgumentDescription references."), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	// One mandatory and one optional input argument
	NodeId inputArgsNodeId(namespaceIndex, methodName + _T("_InputArguments"));
	std::vector<Argument> inputArguments;
	Argument currentArgument;
	currentArgument.setName(_T("Load"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_Double));
	currentArgument.setDescription(LocalizedText(_T("Machine load."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("RequestedSpeed"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_Double));
	currentArgument.setDescription(LocalizedText(_T("Requested machine speed."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	if(StatusCode::isBad(setInputArguments(&inputArgsNodeId, inputArguments)))
	{
		_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
		return false;
	}

	// One output argument
	NodeId outputArgNodeId(namespaceIndex, methodName + _T("_OutputArguments"));
	vector<Argument> outputArguments;
	currentArgument.setName(_T("ActualSpeed"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_Double));
	currentArgument.setDescription(LocalizedText(_T("Actual machine speed."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	outputArguments.push_back(currentArgument);

	if(StatusCode::isBad(setOutputArguments(&outputArgNodeId, outputArguments)))
	{
		_tprintf(_T("Failed to set the OutputArguments of %s\n"), methodName.c_str());
		return false;
	}
	if(StatusCode::isBad(init(NULL)))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return false;
	}

	// Metadata for arguments...
	tstring strName;
	// AnalogItem for 'Load' argument
	{
		Range range;
		// EURange
		range.setLow(0.0);
		range.setHigh(50.0);
		strName = _T("Load");
		AnalogItemPtr analogItem = AnalogItem::create(NodeId(namespaceIndex, strName),
			Statics::DataTypeId_Double,
			LocalizedText(strName, _T("en")),
			QualifiedName(strName, namespaceIndex),
			range);

		// InstrumentRange
		range.setLow(0.0);
		range.setHigh(99.9);
		analogItem->setInstrumentRange(range);

		// Property EngineeringUnits
		EUInformation euInformation;
		euInformation.setDescription(LocalizedText(_T("kilogram"), _T("en")));
		euInformation.setDisplayName(LocalizedText(_T("kg"), _T("en")));
		analogItem->setEngineeringUnits(euInformation);

		// Property Definition
		analogItem->setDefinition(_T("Machine load"));

		// Property ValuePrecision
		analogItem->setPrecision(0.1);

		Value value;
		value.setDouble(0.0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasArgumentDescription, analogItem);
	}
	// AnalogItem for 'RequestedSpeed' argument
	{
		Range range;
		// EURange
		range.setLow(-2.0);
		range.setHigh(2.0);
		strName = _T("RequestedSpeed");
		AnalogItemPtr analogItem = AnalogItem::create(NodeId(namespaceIndex, strName),
			Statics::DataTypeId_Double,
			LocalizedText(strName, _T("en")),
			QualifiedName(strName, namespaceIndex),
			range);

		// InstrumentRange
		range.setLow(-99.99);
		range.setHigh(99.99);
		analogItem->setInstrumentRange(range);

		// Property EngineeringUnits
		EUInformation euInformation;
		euInformation.setDescription(LocalizedText(_T("meter per second"), _T("en")));
		euInformation.setDisplayName(LocalizedText(_T("m/s"), _T("en")));
		analogItem->setEngineeringUnits(euInformation);

		// Property Definition
		analogItem->setDefinition(_T("Requested speed"));

		// Property ValuePrecision
		analogItem->setPrecision(0.01);

		// Provide default value for the argument
		Value value;
		value.setDouble(0.5);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasOptionalInputArgumentDescription, analogItem);
	}
	// AnalogItem for 'ActualSpeed' output argument
	{
		Range range;
		// EURange
		range.setLow(-2.0);
		range.setHigh(2.0);
		strName = _T("ActualSpeed");
		AnalogItemPtr analogItem = AnalogItem::create(NodeId(namespaceIndex, strName),
			Statics::DataTypeId_Double,
			LocalizedText(strName, _T("en")),
			QualifiedName(strName, namespaceIndex),
			range);

		// InstrumentRange
		range.setLow(-9.99);
		range.setHigh(9.99);
		analogItem->setInstrumentRange(range);

		// Property EngineeringUnits
		EUInformation euInformation;
		euInformation.setDescription(LocalizedText(_T("meter per second"), _T("en")));
		euInformation.setDisplayName(LocalizedText(_T("m/s"), _T("en")));
		analogItem->setEngineeringUnits(euInformation);

		// Property Definition
		analogItem->setDefinition(_T("Actual speed"));

		// Property ValuePrecision
		analogItem->setPrecision(0.01);

		// Provide default value for the argument
		Value value;
		value.setDouble(0.5);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasArgumentDescription, analogItem);
	}

	return true;
}

EnumStatusCode ArgumentDescriptionMethod::handleExecuteRequest(
	SoftingOPCToolbox5::Server::MethodCallTransaction* transaction,
	SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArguments = request->getInputArguments();
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<EnumStatusCode> inputArgumentResults;
	double load = 0.0;
	double requestedSpeed = 0.0;
	double actualSpeed = 0.0;

	if(StatusCode::isGood(result))
	{
		// Validate input arguments
		result = validateInputArguments(inputArguments, inputArgumentResults);
	}

	if(StatusCode::isGood(result))
	{
		// Mandatory 'machine load' argument
		load = inputArguments[0].getDouble();
		if(load < 0)
		{
			// Argument validation failed our expectations
			result = EnumStatusCode_BadInternalError;
			_tprintf(_T("ArgumentDescriptionMethod: Argument validation failed\n"));
		}

		if(StatusCode::isGood(result))
		{
			// Optional 'requested speed' argument
			if(inputArguments.size() == 2)
			{
				requestedSpeed = inputArguments[1].getDouble();
			}
			else
			{
				// Use default value if optional argument not provided
				Value defaultValue;
				//result = getDefaultInputArgumentValue(_T("RequestedSpeed"), defaultValue);
				result = getDefaultInputArgumentValue(1, defaultValue);
				if(StatusCode::isGood(result))
				{
					requestedSpeed = defaultValue.getDouble();
				}
			}
		}
	}

	if(StatusCode::isGood(result))
	{
		// Reflect requested speed value in actual speed value
		actualSpeed = requestedSpeed;
		std::vector<Value> outputs;
		outputs.push_back(Value(actualSpeed));
		
		// Validate output arguments, which can result in BadOutOfRange (the requested speed can be greater than the allowed actual speed)
		// normally the bad validation result should be mapped to BadInternalError,
		// but we cause this error by intention and want to see the validation result
		result = validateOutputArguments(outputs);
		if(StatusCode::isGood(result))
		{
			result = request->setOutputArguments(outputs);
		}
	}

	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}
