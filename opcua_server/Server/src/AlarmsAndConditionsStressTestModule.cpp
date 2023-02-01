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
#if TB5_ALARMS
#include "AlarmsAndConditionsStressTestModule.h"
#include "Util_SingleLock.h"
#include "ServerAddressSpaceRoot.h"
#include "Statics.h"
#include "ServerMethodCallRequest.h"
#include "ServerBaseNode.h"
#include "ServerTwoStateVariable.h"
#include "DataValue.h"
#include "ServerVariable.h"
#include "Value.h"
#include "Argument.h"
#include "Util_Timer.h"
#include "Constants.h"

CreateNodesForAlarmsAndConditionsStressTest::CreateNodesForAlarmsAndConditionsStressTest(OTUInt16 namespaceIndex)
: m_pContext(NULL)
{
	m_objectsNamespaceIndex = namespaceIndex;
}

void CreateNodesForAlarmsAndConditionsStressTest::setContext(AlarmsAndConditionsStressTestContext * pContext)
{
	m_pContext = pContext;
}

EnumStatusCode CreateNodesForAlarmsAndConditionsStressTest::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	(void) transaction;
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<Value> inputArguments = request->getInputArguments();
	if (1 == inputArguments.size())
	{
		std::vector<EnumStatusCode> inputReturns(1);
		if ( (EnumValueArrayType_Scalar == inputArguments[0].getArrayType()) && (EnumDataTypeId_UInt32 == inputArguments[0].getDataType()) )
		{
			inputReturns[0]=EnumStatusCode_Good;
		}
		else
		{
			inputReturns[0] = EnumStatusCode_BadInvalidArgument;
			result = EnumStatusCode_BadInvalidArgument;
		}
		request->setInputArgumentResults(inputReturns);

		OTUInt32 objectCount = inputArguments[0].getUInt32();
		if (m_pContext)
		{
			UTIL_SINGLE_LOCK(Util_Sync,lock, &(m_pContext->getSync()));
			if (m_pContext->hasObjects())
			{
				result = EnumStatusCode_BadInvalidState;
			}
			if (StatusCode::isSUCCEEDED(result))
			{
				bool createStatus = m_pContext->createObjects(objectCount, m_objectsNamespaceIndex);
				if (!createStatus)
				{
					result = EnumStatusCode_Bad;
				}
			}
		}
	}
	else
	{
		result = EnumStatusCode_BadInvalidArgument;
	}
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

DestroyNodesForAlarmsAndConditionsStressTest::DestroyNodesForAlarmsAndConditionsStressTest()
: m_pContext(NULL)
{
}

void DestroyNodesForAlarmsAndConditionsStressTest::setContext(AlarmsAndConditionsStressTestContext * pContext)
{
	m_pContext = pContext;
}

EnumStatusCode DestroyNodesForAlarmsAndConditionsStressTest::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	(void) transaction;
	EnumStatusCode result = EnumStatusCode_Good;
	if (m_pContext)
	{
		UTIL_SINGLE_LOCK(Util_Sync,lock, &(m_pContext->getSync()));
		if ( !m_pContext->hasObjects())
		{
			result = EnumStatusCode_BadInvalidState;
		}
		if (StatusCode::isSUCCEEDED(result))
		{
			m_pContext->destroyObjects();
		}
	}
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

StartEventFlowOnTheNodesForAlarmsAndConditionsStressTest::StartEventFlowOnTheNodesForAlarmsAndConditionsStressTest()
: m_pContext(NULL)
{
}

void StartEventFlowOnTheNodesForAlarmsAndConditionsStressTest::setContext(AlarmsAndConditionsStressTestContext * pContext)
{
	m_pContext = pContext;
}

EnumStatusCode StartEventFlowOnTheNodesForAlarmsAndConditionsStressTest::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	(void) transaction;
	EnumStatusCode result = EnumStatusCode_Good;

	std::vector<Value> inputArguments = request->getInputArguments();
	if (1 == inputArguments.size())
	{
		std::vector<EnumStatusCode> inputReturns(1);
		if ( (EnumValueArrayType_Scalar == inputArguments[0].getArrayType()) && (EnumDataTypeId_UInt32 == inputArguments[0].getDataType()) )
		{
			inputReturns[0]=EnumStatusCode_Good;
		}
		else
		{
			inputReturns[0] = EnumStatusCode_BadInvalidArgument;
			result = EnumStatusCode_BadInvalidArgument;
		}
		request->setInputArgumentResults(inputReturns);

		OTUInt32 timeout = inputArguments[0].getUInt32();
		if (m_pContext)
		{
			UTIL_SINGLE_LOCK(Util_Sync,lock, &(m_pContext->getSync()));
			if ( !m_pContext->hasObjects() || (m_pContext->hasObjects() && m_pContext->hasEventFlow()) )
			{
				result = EnumStatusCode_BadInvalidState;
			}
			if (StatusCode::isSUCCEEDED(result))
			{
				bool startFlowStatus = m_pContext->startEventFlow(timeout);
				if ( !startFlowStatus)
				{
					result = EnumStatusCode_Bad;
				}
			}
		}
	}
	else
	{
		result = EnumStatusCode_BadInvalidArgument;
	}
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

StopEventFlowOnTheNodesForlarmsAndConditionsStressTest::StopEventFlowOnTheNodesForlarmsAndConditionsStressTest()
: m_pContext(NULL)
{
}

void StopEventFlowOnTheNodesForlarmsAndConditionsStressTest::setContext(AlarmsAndConditionsStressTestContext * pContext)
{
	m_pContext = pContext;
}

EnumStatusCode StopEventFlowOnTheNodesForlarmsAndConditionsStressTest::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	(void) transaction;
	EnumStatusCode result = EnumStatusCode_Good;
	if (m_pContext)
	{
		m_pContext->setShuttingDown();
		UTIL_SINGLE_LOCK(Util_Sync,lock, &(m_pContext->getSync()));
		if ( !m_pContext->hasObjects() || (m_pContext->hasObjects() && !m_pContext->hasEventFlow()) )
		{
			result = EnumStatusCode_BadInvalidState;
		}
		if (StatusCode::isSUCCEEDED(result))
		{
			m_pContext->stopEventFlow();
		}
	}
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

EventFlowAction::EventFlowAction()
: Util_ScheduledAction(3000, true)
, m_pContext(NULL)
{
}

void EventFlowAction::setContext(AlarmsAndConditionsStressTestContext * pContent)
{
	m_pContext = pContent;
}

void EventFlowAction::execute()
{
	if (NULL != m_pContext)
	{
		UTIL_SINGLE_LOCK(Util_Sync,lock,&(m_pContext->getSync()));
		if (!m_pContext->hasEventFlow())
		{
			return;
		}

		// copy the eventSources, so that clearing the event sources during shutdown does not break the looping thread
		std::vector<EventSourceObjectPtr> eventSources = m_pContext->getEventSources();
		triggerEvents(eventSources);
	}
}

void EventFlowAction::triggerEvents(const std::vector<EventSourceObjectPtr> & eventSources)
{

	if (m_pContext->getShuttingDown())
	{
		return;
	}

	std::vector<EventSourceObjectPtr>::const_iterator proceedingIterator, endIterator;
	endIterator = eventSources.end();
	unsigned long ts0, ts1;
	bool keepRunning = eventSources.begin() != endIterator;
	ts0 = Util_Timer::currentTickCount();
	unsigned long maxdiff = m_pContext->getTimeout();
	OTUInt32 count = 0;
	while (keepRunning)
	{
		for (proceedingIterator = eventSources.begin(); keepRunning && (proceedingIterator != endIterator); proceedingIterator++)
		{
			EventSourceObjectPtr eventSource = *proceedingIterator;
			Server::LimitAlarmPtr alarm = m_pContext->getAlarm();
			if(alarm)
			{
				alarm->report(eventSource->getNodeId());
			}
			

			ts1 = Util_Timer::currentTickCount();
			keepRunning = (Util_Timer::tickCountDiff(ts0, ts1) < maxdiff) && !m_pContext->getShuttingDown();
			++ count;
		}
	}
	OTUInt32 currentCount = m_pContext->getEventCount() + count;
	Server::VariablePtr eventCountVar = m_pContext->getEventCountVar();
	if(eventCountVar)
	{
		DataValue dataValue;
		Value value;
		value.setUInt32(currentCount);
		dataValue.setValue(value);
		eventCountVar->setDataValue(dataValue);
	}
	m_pContext->setEventCount(currentCount);
}

AlarmsAndConditionsStressTestContext::AlarmsAndConditionsStressTestContext()
: m_hasObjects(false)
, m_hasEventFlow(false)
, m_eventCount(0)
{
	m_eventFlowAction.setContext(this);
}

AlarmsAndConditionsStressTestContext::~AlarmsAndConditionsStressTestContext()
{
}

bool AlarmsAndConditionsStressTestContext::createObjects(OTUInt32 count, OTUInt16 namespaceIndex)
{
	if (m_hasObjects)
	{
		return false;
	}
	NodeId theNodeIdOfTheFolder(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea"));
	Server::BaseNodePtr theFolder = Server::AddressSpaceRoot::instance()->getNode(&theNodeIdOfTheFolder);
	if (theFolder.isNull())
	{
		return false;
	}
	EventSourceObjectPtr eventSink = new EventSourceObject();
	eventSink->initialize((Server::Object*) (Server::BaseNode*) (theFolder),
		NodeId(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.EventSink")),
		LocalizedText(_T("EventSink"),_T("en")),
		QualifiedName(_T("EventSink"), 0));

	std::vector<EventSourceObjectPtr> eventSources(count);
	unsigned i;
	for (i=0; i<count; i++)
	{
		eventSources[i] = new EventSourceObject();
		if (eventSources[i].isNull())
		{
			return false;
		}
		OTChar b1[200], b2[200];
		_stprintf(b1, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.EventSink.EventSource%u"), i);
		_stprintf(b2, _T("EventSource%u"), i);
		eventSources[i]->initialize(eventSink, NodeId(namespaceIndex, b1), LocalizedText(b2, _T("en")), QualifiedName(b2, 0));
		eventSink->addReference(Statics::ReferenceTypeId_HasComponent, eventSources[i]);
		eventSink->addReference(Statics::ReferenceTypeId_HasEventSource, eventSources[i]);
	}
	theFolder->addReference(Statics::ReferenceTypeId_HasComponent, eventSink);

	Server::LimitAlarmPtr limitAlarm = Server::LimitAlarm::create();
	if (limitAlarm.isNull())
	{
		return false;
	}

	DateTime datetime;
	DataValue value;
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue((double)0.0);
	value.setStatusCode(EnumStatusCode_Good);

	if (m_limitAlarm)
	{
		m_limitAlarm->removeTree();
		m_limitAlarm = NULL;
	}

	limitAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.TheAlarm")));
	limitAlarm->setConditionName(_T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.TheAlarm"));
	if (StatusCode::isGood(limitAlarm->init(NULL)))
	{
		ObjectPointer<Server::TwoStateVariable> activeState = limitAlarm->getActiveStateVar();
		if (activeState)
		{
			activeState->setCurrentState(true);
		}
		theFolder->addReference(Statics::ReferenceTypeId_HasComponent, limitAlarm);
	}

	Server::VariablePtr eventCountVar = Server::Variable::create();
	if (eventCountVar.isNull())
	{
		return false;
	}

	Value ecValue;
	DataValue ecDataValue;
	ecValue.setUInt32(0);
	ecDataValue.setValue(ecValue);
	eventCountVar->setDataValue(ecDataValue);
	eventCountVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.EventCount")));
	eventCountVar->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	eventCountVar->insert();
	eventCountVar->setBrowseName(QualifiedName(_T("EventCount"), namespaceIndex));
	eventCountVar->setDisplayName(LocalizedText(_T("EventCount"), _T("en")));
	eventCountVar->setAccessLevel(EnumAccessLevel_CurrentRead);
	eventCountVar->setDataType(NodeId(0, EnumNumericNodeId_UInt32));

	eventSink->addReference(Statics::ReferenceTypeId_HasProperty, eventCountVar);

	m_limitAlarm = limitAlarm;
	eventSink->insertTree();
	m_eventSink = eventSink;
	m_eventSources.swap(eventSources);
	m_hasObjects = true;
	m_eventCountVar = eventCountVar;
	return true;
}

void AlarmsAndConditionsStressTestContext::destroyObjects()
{
	if (m_eventSink)
	{
		m_eventSink->removeTree();
		m_eventSink = NULL;
	}
	if(m_limitAlarm)
	{
		m_limitAlarm->removeTree();
		m_limitAlarm = NULL;
	}
	if(m_eventCountVar)
	{
		m_eventCountVar->removeTree();
		m_eventCountVar = NULL;
	}
	std::vector<EventSourceObjectPtr> local;
	local.swap(m_eventSources);
	m_hasObjects = false;
}

bool AlarmsAndConditionsStressTestContext::startEventFlow(OTUInt32 timeout)
{
	if ( !m_hasObjects || m_hasEventFlow )
	{
		return false;
	}
	m_shuttingDown = false;
	m_timeout = timeout;
	m_eventFlowAction.setCyclic(true);
	m_eventFlowAction.setTimeout(timeout);
	m_eventFlowThread.addAction(&m_eventFlowAction);
	m_eventFlowThread.start();
	m_hasEventFlow = true;
	return true;
}

void AlarmsAndConditionsStressTestContext::stopEventFlow()
{
	if ( !m_hasEventFlow )
	{
		return;
	}
	m_eventFlowThread.removeAction(&m_eventFlowAction);
	// (void) m_eventFlowThread.stop();
	m_hasEventFlow = false;
}

void AlarmsAndConditionsStressTestContext::waitAfterEventFlowThread()
{
	if (m_eventFlowThread.isRunning())
	{
		(void) m_eventFlowThread.stop();
		(void) m_eventFlowThread.wait(UTIL_MAX_TIMERTHREAD_TIMEOUT);
	}
}

AlarmsAndConditionsStressTestModule::AlarmsAndConditionsStressTestModule()
{
}


EnumStatusCode AlarmsAndConditionsStressTestModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	m_theFolder = new EventSourceObject();
	if (m_theFolder.isNull())
	{
		return EnumStatusCode_BadOutOfMemory;
	}
	m_theFolder->initialize(this,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea")),
		LocalizedText(_T("AlarmsAndConditionsStressTestArea"),_T("en")),
		QualifiedName(_T("AlarmsAndConditionsStressTestArea"), 0));

	m_createNodesMethod = new CreateNodesForAlarmsAndConditionsStressTest(namespaceIndex);
	if (m_createNodesMethod.isNull())
	{
		return EnumStatusCode_BadOutOfMemory;
	}
	m_createNodesMethod->setContext(&m_context);
	m_createNodesMethod->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.CreateNodes")));
	m_createNodesMethod->init(NULL);
	m_createNodesMethod->setBrowseName(QualifiedName(_T("CreateNodes"), namespaceIndex));
	m_createNodesMethod->setDisplayName(LocalizedText(_T("CreateNodes"), _T("en")));
	m_createNodesMethod->setExecutable(true);
	Server::VariablePtr createNodesMethodInputArgs = Server::Variable::create();
	if (createNodesMethodInputArgs.isNull())
	{
		return EnumStatusCode_BadOutOfMemory;
	}

	std::vector<Argument> args;
	Argument arg;
	arg.setDataType(NodeId(0, EnumDataTypeId_UInt32));
	arg.setName(_T("nodeCount"));
	arg.setValueRank(ValueRank_Scalar);
	args.push_back(arg);
	Value containedValue;
	containedValue.setArgumentArray(args);
	DataValue value;
	value.setValue(containedValue);
	createNodesMethodInputArgs->setDataValue(value);
	createNodesMethodInputArgs->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.CreateNodes_InputArguments")));
	createNodesMethodInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
	createNodesMethodInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
	createNodesMethodInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
	createNodesMethodInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
	m_createNodesMethod->addReference(Statics::ReferenceTypeId_HasProperty, createNodesMethodInputArgs);

	args.clear();

	m_startEventFlowOnNodesMethod = new StartEventFlowOnTheNodesForAlarmsAndConditionsStressTest;
	if (m_startEventFlowOnNodesMethod.isNull())
	{
		return EnumStatusCode_BadOutOfMemory;
	}
	m_startEventFlowOnNodesMethod->setContext(&m_context);
	m_startEventFlowOnNodesMethod->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.StartEventFlowOnNodes")));
	m_startEventFlowOnNodesMethod->init(NULL);
	m_startEventFlowOnNodesMethod->setBrowseName(QualifiedName(_T("StartEventFlowOnNodes"), namespaceIndex));
	m_startEventFlowOnNodesMethod->setDisplayName(LocalizedText(_T("StartEventFlowOnNodes"), _T("en")));

	Server::VariablePtr startEventFlowOnNodesMethodInputArgs = Server::Variable::create();
	if (startEventFlowOnNodesMethodInputArgs.isNull())
	{
		return EnumStatusCode_BadOutOfMemory;
	}

	arg.setDataType(NodeId(0, EnumDataTypeId_UInt32));
	arg.setName(_T("timeout"));
	arg.setValueRank(ValueRank_Scalar);
	args.push_back(arg);
	containedValue.setArgumentArray(args);
	value.setValue(containedValue);
	startEventFlowOnNodesMethodInputArgs->setDataValue(value);
	startEventFlowOnNodesMethodInputArgs->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.StartEventFlowOnNodes_InputArguments")));
	startEventFlowOnNodesMethodInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
	startEventFlowOnNodesMethodInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
	startEventFlowOnNodesMethodInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
	startEventFlowOnNodesMethodInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
	m_startEventFlowOnNodesMethod->addReference(Statics::ReferenceTypeId_HasProperty, startEventFlowOnNodesMethodInputArgs);


	m_stopEventFlowOnMethodsFlowMethod = new StopEventFlowOnTheNodesForlarmsAndConditionsStressTest;
	if (m_stopEventFlowOnMethodsFlowMethod.isNull())
	{
		return EnumStatusCode_BadOutOfMemory;
	}
	m_stopEventFlowOnMethodsFlowMethod->setContext(&m_context);
	m_stopEventFlowOnMethodsFlowMethod->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.StopEventFlowOnNodes")));
	m_stopEventFlowOnMethodsFlowMethod->init(NULL);
	m_stopEventFlowOnMethodsFlowMethod->setBrowseName(QualifiedName(_T("StopEventFlowOnNodes"), namespaceIndex));
	m_stopEventFlowOnMethodsFlowMethod->setDisplayName(LocalizedText(_T("StoptEventFlowOnNodes"), _T("en")));

	m_destroyNodesMethod = new DestroyNodesForAlarmsAndConditionsStressTest;
	if (m_destroyNodesMethod.isNull())
	{
		return EnumStatusCode_BadOutOfMemory;
	}
	m_destroyNodesMethod->setContext(&m_context);
	m_destroyNodesMethod->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.AlarmsAndConditionsStressTestArea.DestroyNodes")));
	m_destroyNodesMethod->init(NULL);
	m_destroyNodesMethod->setBrowseName(QualifiedName(_T("DestroyNodes"), namespaceIndex));
	m_destroyNodesMethod->setDisplayName(LocalizedText(_T("DestroyNodes"), _T("en")));

	m_theFolder->addReference(Statics::ReferenceTypeId_HasComponent, m_createNodesMethod);
	m_theFolder->addReference(Statics::ReferenceTypeId_HasComponent, m_startEventFlowOnNodesMethod);
	m_theFolder->addReference(Statics::ReferenceTypeId_HasComponent, m_stopEventFlowOnMethodsFlowMethod);
	m_theFolder->addReference(Statics::ReferenceTypeId_HasComponent, m_destroyNodesMethod);

	EnumStatusCode insertStatus = m_theFolder->insertTree();
	if (StatusCode::isFAILED(insertStatus))
	{
		return EnumStatusCode_BadOutOfMemory;
	}

	return EnumStatusCode_Good;
}

void AlarmsAndConditionsStressTestModule::endModule()
{
	m_context.setShuttingDown();
	m_context.stopEventFlow();
	m_context.destroyObjects();
	m_context.waitAfterEventFlowThread();

	ModuleFolder::endModule();
}

#endif
