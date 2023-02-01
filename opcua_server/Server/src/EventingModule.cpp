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
#include "EventingModule.h"

#include "ServerBaseEvent.h"
#include "ServerStateMachine.h"
#include "ServerDiscreteAlarm.h"
#include "ServerExclusiveDeviationAlarm.h"
#include "ServerExclusiveLevelAlarm.h"
#include "ServerExclusiveRateOfChangeAlarm.h"
#include "ServerNonExclusiveDeviationAlarm.h"
#include "ServerNonExclusiveLevelAlarm.h"
#include "ServerNonExclusiveLimitAlarm.h"
#include "ServerNonExclusiveRateOfChangeAlarm.h"
#include "ServerOffNormalAlarm.h"
#include "ServerTripAlarm.h"
#include "Application.h"
#include "ServerObjectType.h"
#include "ServerVariableType.h"
#include "Argument.h"
#include "Statics.h"
#include "ToolkitTrace.h"
#include "Constants.h"


#include "ApplicationModule.h"
#include "ServerBaseEventNotification.h"
#include "TestEvent.h"
#include "Util_SingleLock.h"

NodeId* EventingModuleEventType::s_testStringProperty;
NodeId* EventingModuleEventType::s_moodProperty;
NodeId* EventingModuleEventType::s_nodeId;

extern bool g_cttCompatible;

EventingModuleEventType::EventingModuleEventType()
{
}

EventingModuleEventType::~EventingModuleEventType()
{
}

EnumStatusCode EventingModuleEventType::init(const INodeId* typeId)
{
	_UNUSED(typeId);

	EnumStatusCode result = EnumStatusCode_Good;
	s_testStringProperty = new NodeId(1, _T("StructoAutomoldServer.TestEvent.TestString"));
	s_moodProperty = new NodeId(1, _T("StructoAutomoldServer.TestEvent.Mood"));
	s_nodeId = new NodeId(1, _T("StructoAutomoldServer.TestEvent"));

	NodeId baseEventTypeId(Statics::ObjectTypeId_BaseEventType);
	NodeId propertyTypeId(0, EnumNumericNodeId_PropertyType);
	NodeId stringTypeId(0, EnumNumericNodeId_String);
	BaseNodePtr baseEventType = AddressSpaceRoot::instance()->getNode(&baseEventTypeId);
	BaseNodePtr propertyType = AddressSpaceRoot::instance()->getNode(&propertyTypeId);

	VariablePtr testStringPropertyVar = Variable::create();
	VariablePtr moodPropertyVar = Variable::create();

	setBrowseName(QualifiedName(_T("TestEvent"), 1));
	setDisplayName(LocalizedText(_T("TestEvent"), _T("en")));
	setDescription(LocalizedText(_T("A custom event for testing"), _T("en")));
	setIsAbstract(false);
	setNodeId(s_nodeId);
	setWriteMask(0);

	testStringPropertyVar->setBrowseName(QualifiedName(_T("TestString"), 1));
	testStringPropertyVar->setDisplayName(LocalizedText(_T("TestString"), _T("en")));
	testStringPropertyVar->setDescription(LocalizedText(_T("Test string info ... 2 10 2 10"), _T("en")));
	testStringPropertyVar->setNodeId(s_testStringProperty);
	testStringPropertyVar->setWriteMask(0);
	testStringPropertyVar->setAccessLevel(EnumAccessLevel_CurrentRead);
	testStringPropertyVar->setDataType(stringTypeId);

	moodPropertyVar->setBrowseName(QualifiedName(_T("Mood"), 1));
	moodPropertyVar->setDisplayName(LocalizedText(_T("Mood"), _T("en")));
	moodPropertyVar->setDescription(LocalizedText(_T("... my mood ..."), _T("en")));
	moodPropertyVar->setNodeId(s_moodProperty);
	moodPropertyVar->setWriteMask(0);
	moodPropertyVar->setAccessLevel(EnumAccessLevel_CurrentRead);
	moodPropertyVar->setDataType(stringTypeId);

	testStringPropertyVar->addReference(Statics::ReferenceTypeId_HasTypeDefinition, propertyType);
	addReference(Statics::ReferenceTypeId_HasProperty, testStringPropertyVar);

	moodPropertyVar->addReference(Statics::ReferenceTypeId_HasTypeDefinition, propertyType);
	addReference(Statics::ReferenceTypeId_HasProperty, moodPropertyVar);

	baseEventType->addReference(Statics::ReferenceTypeId_HasSubtype, this);

	insertTree();

	return result;
}


TriggerEventMethod::TriggerEventMethod(const SoftingOPCToolbox5::NodeId& eventSourceId)
{
	m_eventSourceId = eventSourceId;
}

/*
Reports an event on the node with NodeId sourceNodeId(1, _T("StructoAutomoldServer.TriggerEventSink")).
arg0 : event type node id
arg1 : what kind of init to use:
arg1 == 0 : EnumStatusCode EventNotification::init(Server::BaseNode* instanceOrType) [ instance ]
arg1 == 1 : EnumStatusCode EventNotification::initFromTypeId(const INodeId* eventType)
arg1 == 2 : EnumStatusCode EventNotification::init(Server::BaseNode* instanceOrType) [ type ]
*/
EnumStatusCode TriggerEventMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	(void) transaction;
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<Value> inputArguments = request->getInputArguments();
	if (2 == inputArguments.size())
	{
		std::vector<EnumStatusCode> inputReturns(2);
		if ( (EnumValueArrayType_Scalar == inputArguments[0].getArrayType()) && (EnumDataTypeId_NodeId == inputArguments[0].getDataType()) )
		{
			inputReturns[0]=EnumStatusCode_Good;
		}
		else
		{
			inputReturns[0] = EnumStatusCode_BadInvalidArgument;
			result = EnumStatusCode_BadInvalidArgument;
		}
		if ( (EnumValueArrayType_Scalar == inputArguments[1].getArrayType()) && (EnumDataTypeId_UInt32 == inputArguments[1].getDataType()) )
		{
			inputReturns[1]=EnumStatusCode_Good;
		}
		else
		{
			inputReturns[1] = EnumStatusCode_BadInvalidArgument;
			result = EnumStatusCode_BadInvalidArgument;
		}
		request->setInputArgumentResults(inputReturns);
		NodeId typeId;
		result = inputArguments[0].getNodeId(typeId);
		OTUInt32 initType = inputArguments[1].getUInt32();
		if(initType > 2)
		{
			result = EnumStatusCode_BadInvalidArgument;
		}
		BaseNodePtr typeObject;
		if (StatusCode::isSUCCEEDED(result))
		{
			typeObject = Server::AddressSpaceRoot::instance()->getNode(&typeId);
			if (typeObject.isNull())
			{
				result = EnumStatusCode_BadInvalidArgument;
			}
		}
		if (StatusCode::isSUCCEEDED(result))
		{
			if ( !typeObject->isOfUaType(Statics::ObjectTypeId_BaseEventType, true))
			{
				result = EnumStatusCode_BadInvalidArgument;
			}
		}
		Server::BaseNodePtr sourceNode;
		if (StatusCode::isSUCCEEDED(result))
		{
			sourceNode = AddressSpaceRoot::instance()->getNode(&m_eventSourceId);
			if (sourceNode.isNull())
			{
				result = EnumStatusCode_BadInternalError;
			}
		}
		if (StatusCode::isSUCCEEDED(result))
		{
			switch(initType)
			{
			case 0:
				{
					Server::ConditionPtr condition;
					result = EventingModule::createCondition(typeId, condition);
					if(StatusCode::isSUCCEEDED(result))
					{
						condition->init(NULL);
						condition->insertTree();
						condition->report(&m_eventSourceId);
						condition->removeTree();
					}
				}
				break;
			case 1:
				{
					BaseEventNotificationPtr eventNotification = BaseEventNotification::create(&typeId);
					eventNotification->setSourceNode(sourceNode->getNodeId());
					result = eventNotification->report(NULL);
				}
				break;
			case 2:
				{
					BaseEventNotificationPtr eventNotification = BaseEventNotification::create(typeObject->getNodeId());
					eventNotification->setSourceNode(sourceNode->getNodeId());
					result = eventNotification->report(NULL);
				}
				break;
			default:
				break;
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

ScheduledEventAction::ScheduledEventAction()
{
}

ScheduledEventAction::~ScheduledEventAction()
{
}

void ScheduledEventAction::init(ObjectPtr object)
{
	m_object = object;
}

static bool createEvents = true;
void ScheduledEventAction::execute()
{
	if(m_object && createEvents)
	{
		{
			// Here we trigger an event, by only populating the notification object with transient data
			BaseEventNotificationPtr testEvent = BaseEventNotification::create(EventingModuleEventType::s_nodeId);

			NodeId returnedEventType = testEvent->getEventType();
			_ASSERTION(returnedEventType.compare(EventingModuleEventType::s_nodeId) == 0, _T("EventType property changed"));

			LocalizedText msg(_T("This is a test event!"), _T("en"));
			testEvent->setMessage(msg);
			LocalizedText returnedMsg = testEvent->getMessage();
			_ASSERTION(msg.compare(returnedMsg) == 0, _T("Message property changed"));

			testEvent->setSeverity(100);
			OTUInt16 returnedSeverity = testEvent->getSeverity();
			_ASSERTION(returnedSeverity == 100, _T("severity property changed"));
			_UNUSED(returnedSeverity);

			tstring sourceName = _T("EventingModule --- reporting only notification (simplified setting of values)");
			testEvent->setSourceName(sourceName);
			tstring returnedSourceName = testEvent->getSourceName();
			_ASSERTION(sourceName.compare(returnedSourceName) == 0, _T("SourceName property changed"));

			DateTime dateTime;
			dateTime.utcNow();
			testEvent->setTime(dateTime);
			DateTime returnedTime = testEvent->getTime();
			_ASSERTION(returnedTime.compare(dateTime) == 0, _T("Time property changed"));

			dateTime.setLowDateTime(dateTime.getLowDateTime() + 1); // just a different time...
			testEvent->setReceiveTime(dateTime);
			returnedTime = testEvent->getReceiveTime();
			_ASSERTION(returnedTime.compare(dateTime) == 0, _T("ReceiveTime property changed"));

			TimeZone timezone(60, true);
			testEvent->setLocalTime(timezone);
			TimeZone returnedTimezone = testEvent->getLocalTime();
			_ASSERTION(returnedTimezone.compare(timezone) == 0, _T("LocalTime property changed"));

			testEvent->setSourceNode(m_object->getNodeId());
			NodeId returnedSourceNode = testEvent->getSourceNode();
			_ASSERTION(returnedSourceNode.compare(m_object->getNodeId()) == 0, _T("SourceNode property changed"));

			Value propValMood, propValTestString;
			static OTUInt32 count = 0;
			switch(count)
			{
			case 0:
				propValMood.setString(_T("happy"));
				propValTestString.setString(_T("TestString1"));
				count++;
				break;
			case 1:
				propValMood.setString(_T("sad"));
				propValTestString.setString(_T("TestString2"));
				count++;
				break;
			case 2:
				propValMood.setString(_T("boring"));
				propValTestString.setString(_T("TestString3"));
				count = 0;
				break;
			}
			testEvent->setProperty(QualifiedName(_T("Mood"), 1), propValMood);
			testEvent->setProperty(QualifiedName(_T("TestString"), 1), propValTestString);

			testEvent->report(m_object->getNodeId());
			Trace::instance()->writeLine(EnumTraceLevel_Debug, EnumTraceGroup_User1, _T("Reported test event"), _T(""));

			ByteString returnedEventId = testEvent->getEventId();
			_ASSERTION(returnedEventId.getSize() != 0, _T("EventId is empty after reporting"));

			returnedSourceNode = testEvent->getSourceNode();
			_ASSERTION(returnedSourceNode.compare(m_object->getNodeId()) == 0, _T("SourceNode changed after report"));

		}
	}
}


EventingModule::EventingModule()
{
}

EnumStatusCode EventingModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	createCustomEventTypes();
	////////////////////////////////////////////

	m_notificationArea = new EventSourceObject();
	m_notificationArea->initialize(this,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.NotificationArea")),
		LocalizedText(_T("NotificationArea"),_T("en")),
		QualifiedName(_T("NotificationArea"), 0));

	m_boiler = new EventSourceObject();
	m_boiler->initialize(m_notificationArea,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.NotificationArea.Boiler")),
		LocalizedText(_T("Boiler"),_T("en")),
		QualifiedName(_T("Boiler"), 0));

	m_triggerEventSink = new EventSourceObject();
	m_triggerEventSink->initialize(this,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.TriggerEventSink")),
		LocalizedText(_T("TriggerEventSink"), _T("en")),
		QualifiedName(_T("TriggerEventSink"), 0));

	if(!g_cttCompatible)
	{
		// CTT does not need such strange events
		m_triggerEventMethod = new TriggerEventMethod(NodeId(namespaceIndex, _T("StructoAutomoldServer.TriggerEventSink")));

		Server::VariablePtr createNodesMethodInputArgs = Server::Variable::create();
		m_triggerEventMethod->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.TriggerEventSink.TriggerEventMethod")));
		m_triggerEventMethod->init(NULL);
		m_triggerEventMethod->setBrowseName(QualifiedName(_T("TriggerEventMethod"), namespaceIndex));
		m_triggerEventMethod->setDisplayName(LocalizedText(_T("TriggerEventMethod"), _T("en")));
		m_triggerEventMethod->setExecutable(true);
		Server::VariablePtr triggerEventMethodInputArgs = Server::Variable::create();
		std::vector<Argument> args;
		Argument arg;
		arg.setDataType(NodeId(0, EnumDataTypeId_NodeId));
		arg.setName(_T("typeId"));
		arg.setValueRank(ValueRank_Scalar);
		args.push_back(arg);
		arg.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		arg.setName(_T("initType"));
		arg.setValueRank(ValueRank_Scalar);
		args.push_back(arg);
		Value containedValue;
		containedValue.setArgumentArray(args);
		DataValue value;
		value.setValue(containedValue);
		triggerEventMethodInputArgs->setDataValue(value);
		triggerEventMethodInputArgs->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.TriggerConditionSink.TriggerEventMethod_InputArguments")));
		triggerEventMethodInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		triggerEventMethodInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		triggerEventMethodInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		triggerEventMethodInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		m_triggerEventMethod->addReference(Statics::ReferenceTypeId_HasProperty, triggerEventMethodInputArgs);
		args.clear();

		m_boilerEventGenerator.init((EventSourceObject*)m_boiler);
		m_boilerEventGenerator.setCyclic(true);
		m_boilerEventGenerator.setTimeout(1000);
		getApplicationModule()->addAction(&m_boilerEventGenerator);
		m_notificationArea->addReference(Statics::ReferenceTypeId_HasNotifier, m_boiler);

		m_triggerEventSink->addReference(Statics::ReferenceTypeId_HasComponent, m_triggerEventMethod);

		m_notificationArea->insertTree();
		m_triggerEventSink->insertTree();
		m_triggerEventMethod->insertTree();
	}

	return EnumStatusCode_Good;
}

void EventingModule::endModule()
{
	if(!g_cttCompatible)
	{
		getApplicationModule()->removeAction(&m_boilerEventGenerator);
	}

	if(m_notificationArea)
	{
		m_notificationArea->removeTree();
	}

	if(m_testEventType)
	{
		m_testEventType->removeTree();
	}

	m_notificationArea = NULL;
	m_boiler = NULL;
	m_testEventType = NULL;

	delete EventingModuleEventType::s_testStringProperty;
	delete EventingModuleEventType::s_moodProperty;
	delete EventingModuleEventType::s_nodeId;

	ModuleFolder::endModule();
}


void EventingModule::createCustomEventTypes()
{
	EnumStatusCode result = EnumStatusCode_Good;
	m_testEventType = new EventingModuleEventType();
	if(!StatusCode::isSUCCEEDED(result = m_testEventType->init(NULL)))
	{
		_tprintf(_T("Failed to initialize TestEventType: %s"), getEnumStatusCodeString(result));
		m_testEventType = NULL;
	}
}

EnumStatusCode EventingModule::createCondition(const INodeId& conditionTypeId, ConditionPtr& condition)
{
	condition = NULL;

	// only well known types in the namespace 0 are handled
	if(conditionTypeId.getNamespaceIndex() != 0 ||
		conditionTypeId.getIdentifierType() != EnumIdentifierType_Numeric)
	{
		return EnumStatusCode_BadNotImplemented;
	}

	// only a certain list of types are handled
	switch(conditionTypeId.getIdentifier().getUInt32())
	{
	case EnumObjectTypeId_AcknowledgeableConditionType:
		condition = Server::AcknowledgeableCondition::create();
		break;
	case EnumObjectTypeId_AlarmConditionType:
		condition = Server::AlarmCondition::create();
		break;
	case EnumObjectTypeId_DiscreteAlarmType:
		condition = Server::DiscreteAlarm::create();
		break;
	case EnumObjectTypeId_ExclusiveDeviationAlarmType:
		condition = Server::ExclusiveDeviationAlarm::create();
		break;
	case EnumObjectTypeId_ExclusiveLevelAlarmType:
		condition = Server::ExclusiveLevelAlarm::create();
		break;
	case EnumObjectTypeId_ExclusiveLimitAlarmType:
		condition = Server::ExclusiveLimitAlarm::create();
		break;
	case EnumObjectTypeId_ExclusiveRateOfChangeAlarmType:
		condition = Server::ExclusiveRateOfChangeAlarm::create();
		break;
	case EnumObjectTypeId_LimitAlarmType:
		condition = Server::LimitAlarm::create();
		break;
	case EnumObjectTypeId_NonExclusiveDeviationAlarmType:
		condition = Server::NonExclusiveDeviationAlarm::create();
		break;
	case EnumObjectTypeId_NonExclusiveLevelAlarmType:
		condition = Server::NonExclusiveLevelAlarm::create();
		break;
	case EnumObjectTypeId_NonExclusiveLimitAlarmType:
		condition = Server::NonExclusiveLimitAlarm::create();
		break;
	case EnumObjectTypeId_NonExclusiveRateOfChangeAlarmType:
		condition = Server::NonExclusiveRateOfChangeAlarm::create();
		break;
	case EnumObjectTypeId_OffNormalAlarmType:
		condition = Server::OffNormalAlarm::create();
		break;
	case EnumObjectTypeId_TripAlarmType:
		condition = Server::TripAlarm::create();
		break;
	default:
		// not handled
		return EnumStatusCode_BadNotImplemented;
	}
	return EnumStatusCode_Good;
}

#endif	// TB5_ALARMS
