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
#include "OffTheShelfAlarmsModule.h"
#include "ApplicationModule.h"
#include "ServerTwoStateVariable.h"
#include "Constants.h"

using namespace SoftingOPCToolbox5::Server;

extern bool g_cttCompatible;

OffTheShelfAlarmsModule::OffTheShelfAlarmsModule()
{
}

EnumStatusCode OffTheShelfAlarmsModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	ObjectPtr shelvingStateMachine;
	// create the alarms and a variable and link them together with HasCondition
	DateTime datetime;
	DataValue value;
	const double lowLimit = 10.0;
	const double lowLowLimit = 5.0;
	const double highLimit = 15.0;
	const double highHighLimit = 20.0;
	const double defaultInitValue = 13.14;
	const double defaultNormalStateValue = 15.14;
	const double defaultSetPointValue = 4.5;

	// group the alarms under an object "OffTheShelfAlarms"
	ObjectPtr offTheShelfAlarms = Object::create();
	offTheShelfAlarms->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarms")));
	offTheShelfAlarms->init(NULL);
	offTheShelfAlarms->setBrowseName(QualifiedName(_T("OffNormalAlarms"), namespaceIndex));
	offTheShelfAlarms->setDisplayName(LocalizedText(_T("Sample Alarms"),_T("en")));
	offTheShelfAlarms->setEventNotifier(EnumEventNotifier_SubscribeToEvents);

	DataValue conditionClassIdValue;
	conditionClassIdValue.setValue(Value(Statics::ObjectTypeId_ProcessConditionClassType));
	DataValue conditionClassNameValue;
	LocalizedText conditionClassName(BrowseName_ProcessConditionClassType, _T(""));
	conditionClassNameValue.setValue(&conditionClassName);
	 
	// OffNormalAlarm
	m_offNormalAlarm = OffNormalAlarm::create();
	m_offNormalAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.OffNormalAlarm")));
	m_offNormalAlarm->init(NULL);
	m_offNormalAlarm->setBrowseName(QualifiedName(_T("OffNormalAlarm"), namespaceIndex));
	m_offNormalAlarm->setDisplayName(LocalizedText(_T("OffNormalAlarm"),_T("en")));
	m_offNormalAlarm->setConditionName(_T("OffNormalAlarm"));
	m_offNormalAlarm->setSeverity(100);
	m_offNormalAlarm->setPropertyValue(BrowseName_ConditionClassId, &conditionClassIdValue);
	m_offNormalAlarm->setPropertyValue(BrowseName_ConditionClassName, &conditionClassNameValue);
	if(g_cttCompatible)
	{
		shelvingStateMachine = m_offNormalAlarm->getObject(BrowseName_ShelvingState);
		if(shelvingStateMachine)
		{
			shelvingStateMachine->removeTree();
		}
	}

	// set the NormalState
	VariablePtr offNormalAlarmNormalState = TestVariableForAuditEvents::create();
	offNormalAlarmNormalState->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.offNormalAlarmNormalState")));
	offNormalAlarmNormalState->init(NULL);
	offNormalAlarmNormalState->setBrowseName(QualifiedName(_T("offNormalAlarmNormalState"), namespaceIndex));
	offNormalAlarmNormalState->setDisplayName(LocalizedText(_T("offNormalAlarmNormalState"),_T("en")));
	offNormalAlarmNormalState->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultNormalStateValue);
	value.setStatusCode(EnumStatusCode_Good);
	offNormalAlarmNormalState->setDataValue(value);
	offNormalAlarmNormalState->setAccessLevel(EnumAccessLevel_CurrentRead);

	m_offNormalAlarm->setNormalState(offNormalAlarmNormalState->getNodeId());

	VariablePtr offNormalAlarmVar = TestVariableForAuditEvents::create();
	offNormalAlarmVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.offNormalAlarmVar")));
	offNormalAlarmVar->init(NULL);
	offNormalAlarmVar->setBrowseName(QualifiedName(_T("offNormalAlarmVar"), namespaceIndex));
	offNormalAlarmVar->setDisplayName(LocalizedText(_T("offNormalAlarmVar"),_T("en")));
	offNormalAlarmVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultNormalStateValue);
	value.setStatusCode(EnumStatusCode_Good);
	offNormalAlarmVar->setDataValue(value);
	offNormalAlarmVar->setAccessLevel(offNormalAlarmVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	//m_offNormalAlarm->addReference(Statics::ReferenceTypeId_HasCondition, offNormalAlarmVar);
	offNormalAlarmVar->addReference(Statics::ReferenceTypeId_HasCondition, m_offNormalAlarm);
	m_offNormalAlarm->setSourceName(offNormalAlarmVar->getDisplayName()->getText());

	// TripAlarm
	m_tripAlarm = TripAlarm::create();
	m_tripAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.TripAlarm")));
	m_tripAlarm->init(NULL);
	m_tripAlarm->setBrowseName(QualifiedName(_T("TripAlarm"), namespaceIndex));
	m_tripAlarm->setDisplayName(LocalizedText(_T("TripAlarm"),_T("en")));
	m_tripAlarm->setConditionName(_T("TripAlarm"));
	m_tripAlarm->setSeverity(100);
	m_tripAlarm->setPropertyValue(BrowseName_ConditionClassId, &conditionClassIdValue);
	m_tripAlarm->setPropertyValue(BrowseName_ConditionClassName, &conditionClassNameValue);
	if(g_cttCompatible)
	{
		shelvingStateMachine = m_tripAlarm->getObject(BrowseName_ShelvingState);
		if(shelvingStateMachine)
		{
			shelvingStateMachine->removeTree();
		}
	}

	// set the NormalState
	VariablePtr tripAlarmNormalState = TestVariableForAuditEvents::create();
	tripAlarmNormalState->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.tripAlarmNormalState")));
	tripAlarmNormalState->init(NULL);
	tripAlarmNormalState->setBrowseName(QualifiedName(_T("tripAlarmNormalState"), namespaceIndex));
	tripAlarmNormalState->setDisplayName(LocalizedText(_T("tripAlarmNormalState"),_T("en")));
	tripAlarmNormalState->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultNormalStateValue);
	value.setStatusCode(EnumStatusCode_Good);
	tripAlarmNormalState->setDataValue(value);
	tripAlarmNormalState->setAccessLevel(EnumAccessLevel_CurrentRead);

	m_tripAlarm->setNormalState(tripAlarmNormalState->getNodeId());

	VariablePtr tripAlarmVar = TestVariableForAuditEvents::create();
	tripAlarmVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.tripAlarmVar")));
	tripAlarmVar->init(NULL);
	tripAlarmVar->setBrowseName(QualifiedName(_T("tripAlarmVar"), namespaceIndex));
	tripAlarmVar->setDisplayName(LocalizedText(_T("tripAlarmVar"),_T("en")));
	tripAlarmVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultNormalStateValue);
	value.setStatusCode(EnumStatusCode_Good);
	tripAlarmVar->setDataValue(value);
	tripAlarmVar->setAccessLevel(tripAlarmVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	//m_tripAlarm->addReference(Statics::ReferenceTypeId_HasCondition, tripAlarmVar);
	tripAlarmVar->addReference(Statics::ReferenceTypeId_HasCondition, m_tripAlarm);
	m_tripAlarm->setSourceName(tripAlarmVar->getDisplayName()->getText());

	// ExclusiveLimitAlarm
	m_exclusiveLimitAlarm = ExclusiveLimitAlarm::create();
	m_exclusiveLimitAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.ExclusiveLimitAlarm")));
	m_exclusiveLimitAlarm->init(NULL);
	m_exclusiveLimitAlarm->setBrowseName(QualifiedName(_T("ExclusiveLimitAlarm"), namespaceIndex));
	m_exclusiveLimitAlarm->setDisplayName(LocalizedText(_T("ExclusiveLimitAlarm"),_T("en")));
	m_exclusiveLimitAlarm->setConditionName(_T("ExclusiveLimitAlarm"));
	m_exclusiveLimitAlarm->setSeverity(100);
	m_exclusiveLimitAlarm->setPropertyValue(BrowseName_ConditionClassId, &conditionClassIdValue);
	m_exclusiveLimitAlarm->setPropertyValue(BrowseName_ConditionClassName, &conditionClassNameValue);
	if(g_cttCompatible)
	{
		shelvingStateMachine = m_exclusiveLimitAlarm->getObject(BrowseName_ShelvingState);
		if(shelvingStateMachine)
		{
			shelvingStateMachine->removeTree();
		}
	}

	// set the limits
	m_exclusiveLimitAlarm->setLowLimit(lowLimit);
	m_exclusiveLimitAlarm->setLowLowLimit(lowLowLimit);
	m_exclusiveLimitAlarm->setHighLimit(highLimit);
	m_exclusiveLimitAlarm->setHighHighLimit(highHighLimit);

	VariablePtr exclusiveLimitAlarmVar = TestVariableForAuditEvents::create();
	exclusiveLimitAlarmVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.exclusiveLimitAlarmVar")));
	exclusiveLimitAlarmVar->init(NULL);
	exclusiveLimitAlarmVar->setBrowseName(QualifiedName(_T("exclusiveLimitAlarmVar"), namespaceIndex));
	exclusiveLimitAlarmVar->setDisplayName(LocalizedText(_T("exclusiveLimitAlarmVar"),_T("en")));
	exclusiveLimitAlarmVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultInitValue);
	value.setStatusCode(EnumStatusCode_Good);
	exclusiveLimitAlarmVar->setDataValue(value);
	exclusiveLimitAlarmVar->setAccessLevel(exclusiveLimitAlarmVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	//m_exclusiveLimitAlarm->addReference(Statics::ReferenceTypeId_HasCondition, exclusiveLimitAlarmVar);
	exclusiveLimitAlarmVar->addReference(Statics::ReferenceTypeId_HasCondition, m_exclusiveLimitAlarm);
	m_exclusiveLimitAlarm->setSourceName(exclusiveLimitAlarmVar->getDisplayName()->getText());

	// NonExclusiveLimitAlarm
	m_nonExclusiveLimitAlarm = NonExclusiveLimitAlarm::create();
	m_nonExclusiveLimitAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.NonExclusiveLimitAlarm")));
	m_nonExclusiveLimitAlarm->init(NULL);
	m_nonExclusiveLimitAlarm->setBrowseName(QualifiedName(_T("NonExclusiveLimitAlarm"), namespaceIndex));
	m_nonExclusiveLimitAlarm->setDisplayName(LocalizedText(_T("NonExclusiveLimitAlarm"),_T("en")));
	m_nonExclusiveLimitAlarm->setConditionName(_T("NonExclusiveLimitAlarm"));
	m_nonExclusiveLimitAlarm->setSeverity(100);
	m_nonExclusiveLimitAlarm->setPropertyValue(BrowseName_ConditionClassId, &conditionClassIdValue);
	m_nonExclusiveLimitAlarm->setPropertyValue(BrowseName_ConditionClassName, &conditionClassNameValue);
	if(g_cttCompatible)
	{
		shelvingStateMachine = m_nonExclusiveLimitAlarm->getObject(BrowseName_ShelvingState);
		if(shelvingStateMachine)
		{
			shelvingStateMachine->removeTree();
		}
	}

	// set the limits
	m_nonExclusiveLimitAlarm->setLowLimit(lowLimit);
	m_nonExclusiveLimitAlarm->setLowLowLimit(lowLowLimit);
	m_nonExclusiveLimitAlarm->setHighLimit(highLimit);
	m_nonExclusiveLimitAlarm->setHighHighLimit(highHighLimit);

	VariablePtr nonExclusiveLimitAlarmVar = TestVariableForAuditEvents::create();
	nonExclusiveLimitAlarmVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.nonExclusiveLimitAlarmVar")));
	nonExclusiveLimitAlarmVar->init(NULL);
	nonExclusiveLimitAlarmVar->setBrowseName(QualifiedName(_T("nonExclusiveLimitAlarmVar"), namespaceIndex));
	nonExclusiveLimitAlarmVar->setDisplayName(LocalizedText(_T("nonExclusiveLimitAlarmVar"),_T("en")));
	nonExclusiveLimitAlarmVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultInitValue);
	value.setStatusCode(EnumStatusCode_Good);
	nonExclusiveLimitAlarmVar->setDataValue(value);
	nonExclusiveLimitAlarmVar->setAccessLevel(nonExclusiveLimitAlarmVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	//m_nonExclusiveLimitAlarm->addReference(Statics::ReferenceTypeId_HasCondition, nonExclusiveLimitAlarmVar);
	nonExclusiveLimitAlarmVar->addReference(Statics::ReferenceTypeId_HasCondition, m_nonExclusiveLimitAlarm);
	m_nonExclusiveLimitAlarm->setSourceName(nonExclusiveLimitAlarmVar->getDisplayName()->getText());

	// ExclusiveLevelAlarm
	m_exclusiveLevelAlarm = ExclusiveLevelAlarm::create();
	m_exclusiveLevelAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.ExclusiveLevelAlarm")));
	m_exclusiveLevelAlarm->init(NULL);
	m_exclusiveLevelAlarm->setBrowseName(QualifiedName(_T("ExclusiveLevelAlarm"), namespaceIndex));
	m_exclusiveLevelAlarm->setDisplayName(LocalizedText(_T("ExclusiveLevelAlarm"),_T("en")));
	m_exclusiveLevelAlarm->setConditionName(_T("ExclusiveLevelAlarm"));
	m_exclusiveLevelAlarm->setSeverity(100);
	m_exclusiveLevelAlarm->setPropertyValue(BrowseName_ConditionClassId, &conditionClassIdValue);
	m_exclusiveLevelAlarm->setPropertyValue(BrowseName_ConditionClassName, &conditionClassNameValue);
	if(g_cttCompatible)
	{
		shelvingStateMachine = m_exclusiveLevelAlarm->getObject(BrowseName_ShelvingState);
		if(shelvingStateMachine)
		{
			shelvingStateMachine->removeTree();
		}
	}

	// set the limits
	m_exclusiveLevelAlarm->setLowLimit(lowLimit);
	m_exclusiveLevelAlarm->setLowLowLimit(lowLowLimit);
	m_exclusiveLevelAlarm->setHighLimit(highLimit);
	m_exclusiveLevelAlarm->setHighHighLimit(highHighLimit);

	VariablePtr exclusiveLevelAlarmVar = TestVariableForAuditEvents::create();
	exclusiveLevelAlarmVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.exclusiveLevelAlarmVar")));
	exclusiveLevelAlarmVar->init(NULL);
	exclusiveLevelAlarmVar->setBrowseName(QualifiedName(_T("exclusiveLevelAlarmVar"), namespaceIndex));
	exclusiveLevelAlarmVar->setDisplayName(LocalizedText(_T("exclusiveLevelAlarmVar"),_T("en")));
	exclusiveLevelAlarmVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultInitValue);
	value.setStatusCode(EnumStatusCode_Good);
	exclusiveLevelAlarmVar->setDataValue(value);
	exclusiveLevelAlarmVar->setAccessLevel(exclusiveLevelAlarmVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	//m_exclusiveLevelAlarm->addReference(Statics::ReferenceTypeId_HasCondition, exclusiveLevelAlarmVar);
	exclusiveLevelAlarmVar->addReference(Statics::ReferenceTypeId_HasCondition, m_exclusiveLevelAlarm);
	m_exclusiveLevelAlarm->setSourceName(exclusiveLevelAlarmVar->getDisplayName()->getText());

	// NonExclusiveLevelAlarm
	m_nonExclusiveLevelAlarm = NonExclusiveLevelAlarm::create();
	m_nonExclusiveLevelAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.NonExclusiveLevelAlarm")));
	m_nonExclusiveLevelAlarm->init(NULL);
	m_nonExclusiveLevelAlarm->setBrowseName(QualifiedName(_T("NonExclusiveLevelAlarm"), namespaceIndex));
	m_nonExclusiveLevelAlarm->setDisplayName(LocalizedText(_T("NonExclusiveLevelAlarm"),_T("en")));
	m_nonExclusiveLevelAlarm->setConditionName(_T("NonExclusiveLevelAlarm"));
	m_nonExclusiveLevelAlarm->setSeverity(100);
	m_nonExclusiveLevelAlarm->setPropertyValue(BrowseName_ConditionClassId, &conditionClassIdValue);
	m_nonExclusiveLevelAlarm->setPropertyValue(BrowseName_ConditionClassName, &conditionClassNameValue);
	if(g_cttCompatible)
	{
		shelvingStateMachine = m_nonExclusiveLevelAlarm->getObject(BrowseName_ShelvingState);
		if(shelvingStateMachine)
		{
			shelvingStateMachine->removeTree();
		}
	}

	// set the limits
	m_nonExclusiveLevelAlarm->setLowLimit(lowLimit);
	m_nonExclusiveLevelAlarm->setLowLowLimit(lowLowLimit);
	m_nonExclusiveLevelAlarm->setHighLimit(highLimit);
	m_nonExclusiveLevelAlarm->setHighHighLimit(highHighLimit);

	VariablePtr nonExclusiveLevelAlarmVar = TestVariableForAuditEvents::create();
	nonExclusiveLevelAlarmVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.nonExclusiveLevelAlarmVar")));
	nonExclusiveLevelAlarmVar->init(NULL);
	nonExclusiveLevelAlarmVar->setBrowseName(QualifiedName(_T("nonExclusiveLevelAlarmVar"), namespaceIndex));
	nonExclusiveLevelAlarmVar->setDisplayName(LocalizedText(_T("nonExclusiveLevelAlarmVar"),_T("en")));
	nonExclusiveLevelAlarmVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultInitValue);
	value.setStatusCode(EnumStatusCode_Good);
	nonExclusiveLevelAlarmVar->setDataValue(value);
	nonExclusiveLevelAlarmVar->setAccessLevel(nonExclusiveLevelAlarmVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	//m_nonExclusiveLevelAlarm->addReference(Statics::ReferenceTypeId_HasCondition, nonExclusiveLevelAlarmVar);
	nonExclusiveLevelAlarmVar->addReference(Statics::ReferenceTypeId_HasCondition, m_nonExclusiveLevelAlarm);
	m_nonExclusiveLevelAlarm->setSourceName(nonExclusiveLevelAlarmVar->getDisplayName()->getText());

	// ExclusiveRateOfChangeAlarm
	m_exclusiveRateOfChangeAlarm = ExclusiveRateOfChangeAlarm::create();
	m_exclusiveRateOfChangeAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.ExclusiveRateOfChangeAlarm")));
	m_exclusiveRateOfChangeAlarm->init(NULL);
	m_exclusiveRateOfChangeAlarm->setBrowseName(QualifiedName(_T("ExclusiveRateOfChangeAlarm"), namespaceIndex));
	m_exclusiveRateOfChangeAlarm->setDisplayName(LocalizedText(_T("ExclusiveRateOfChangeAlarm"),_T("en")));
	m_exclusiveRateOfChangeAlarm->setConditionName(_T("ExclusiveRateOfChangeAlarm"));
	m_exclusiveRateOfChangeAlarm->setSeverity(100);
	m_exclusiveRateOfChangeAlarm->setPropertyValue(BrowseName_ConditionClassId, &conditionClassIdValue);
	m_exclusiveRateOfChangeAlarm->setPropertyValue(BrowseName_ConditionClassName, &conditionClassNameValue);
	if(g_cttCompatible)
	{
		shelvingStateMachine = m_exclusiveRateOfChangeAlarm->getObject(BrowseName_ShelvingState);
		if(shelvingStateMachine)
		{
			shelvingStateMachine->removeTree();
		}
	}

	// set the limits
	m_exclusiveRateOfChangeAlarm->setLowLimit(lowLimit);
	m_exclusiveRateOfChangeAlarm->setLowLowLimit(lowLowLimit);
	m_exclusiveRateOfChangeAlarm->setHighLimit(highLimit);
	m_exclusiveRateOfChangeAlarm->setHighHighLimit(highHighLimit);

	VariablePtr exclusiveRateOfChangeAlarmVar = TestVariableForAuditEvents::create();
	exclusiveRateOfChangeAlarmVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.exclusiveRateOfChangeAlarmVar")));
	exclusiveRateOfChangeAlarmVar->init(NULL);
	exclusiveRateOfChangeAlarmVar->setBrowseName(QualifiedName(_T("exclusiveRateOfChangeAlarmVar"), namespaceIndex));
	exclusiveRateOfChangeAlarmVar->setDisplayName(LocalizedText(_T("exclusiveRateOfChangeAlarmVar"),_T("en")));
	exclusiveRateOfChangeAlarmVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultInitValue);
	value.setStatusCode(EnumStatusCode_Good);
	exclusiveRateOfChangeAlarmVar->setDataValue(value);
	exclusiveRateOfChangeAlarmVar->setAccessLevel(exclusiveRateOfChangeAlarmVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	//m_exclusiveRateOfChangeAlarm->addReference(Statics::ReferenceTypeId_HasCondition, exclusiveRateOfChangeAlarmVar);
	exclusiveRateOfChangeAlarmVar->addReference(Statics::ReferenceTypeId_HasCondition, m_exclusiveRateOfChangeAlarm);
	m_exclusiveRateOfChangeAlarm->setSourceName(exclusiveRateOfChangeAlarmVar->getDisplayName()->getText());

	// NonExclusiveRateOfChangeAlarm
	m_nonExclusiveRateOfChangeAlarm = NonExclusiveRateOfChangeAlarm::create();
	m_nonExclusiveRateOfChangeAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.NonExclusiveRateOfChangeAlarm")));
	m_nonExclusiveRateOfChangeAlarm->init(NULL);
	m_nonExclusiveRateOfChangeAlarm->setBrowseName(QualifiedName(_T("NonExclusiveRateOfChangeAlarm"), namespaceIndex));
	m_nonExclusiveRateOfChangeAlarm->setDisplayName(LocalizedText(_T("NonExclusiveRateOfChangeAlarm"),_T("en")));
	m_nonExclusiveRateOfChangeAlarm->setConditionName(_T("NonExclusiveRateOfChangeAlarm"));
	m_nonExclusiveRateOfChangeAlarm->setSeverity(100);
	m_nonExclusiveRateOfChangeAlarm->setPropertyValue(BrowseName_ConditionClassId, &conditionClassIdValue);
	m_nonExclusiveRateOfChangeAlarm->setPropertyValue(BrowseName_ConditionClassName, &conditionClassNameValue);
	if(g_cttCompatible)
	{
		shelvingStateMachine = m_nonExclusiveRateOfChangeAlarm->getObject(BrowseName_ShelvingState);
		if(shelvingStateMachine)
		{
			shelvingStateMachine->removeTree();
		}
	}

	// set the limits
	m_nonExclusiveRateOfChangeAlarm->setLowLimit(lowLimit);
	m_nonExclusiveRateOfChangeAlarm->setLowLowLimit(lowLowLimit);
	m_nonExclusiveRateOfChangeAlarm->setHighLimit(highLimit);
	m_nonExclusiveRateOfChangeAlarm->setHighHighLimit(highHighLimit);

	VariablePtr nonExclusiveRateOfChangeAlarmVar = TestVariableForAuditEvents::create();
	nonExclusiveRateOfChangeAlarmVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.nonExclusiveRateOfChangeAlarmVar")));
	nonExclusiveRateOfChangeAlarmVar->init(NULL);
	nonExclusiveRateOfChangeAlarmVar->setBrowseName(QualifiedName(_T("nonExclusiveRateOfChangeAlarmVar"), namespaceIndex));
	nonExclusiveRateOfChangeAlarmVar->setDisplayName(LocalizedText(_T("nonExclusiveRateOfChangeAlarmVar"),_T("en")));
	nonExclusiveRateOfChangeAlarmVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultInitValue);
	value.setStatusCode(EnumStatusCode_Good);
	nonExclusiveRateOfChangeAlarmVar->setDataValue(value);
	nonExclusiveRateOfChangeAlarmVar->setAccessLevel(nonExclusiveRateOfChangeAlarmVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	//m_nonExclusiveRateOfChangeAlarm->addReference(Statics::ReferenceTypeId_HasCondition, nonExclusiveRateOfChangeAlarmVar);
	nonExclusiveRateOfChangeAlarmVar->addReference(Statics::ReferenceTypeId_HasCondition, m_nonExclusiveRateOfChangeAlarm);
	m_nonExclusiveRateOfChangeAlarm->setSourceName(nonExclusiveRateOfChangeAlarmVar->getDisplayName()->getText());

	// ExclusiveDeviationAlarm
	m_exclusiveDeviationAlarm = ExclusiveDeviationAlarm::create();
	m_exclusiveDeviationAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.ExclusiveDeviationAlarm")));
	m_exclusiveDeviationAlarm->init(NULL);
	m_exclusiveDeviationAlarm->setBrowseName(QualifiedName(_T("ExclusiveDeviationAlarm"), namespaceIndex));
	m_exclusiveDeviationAlarm->setDisplayName(LocalizedText(_T("ExclusiveDeviationAlarm"),_T("en")));
	m_exclusiveDeviationAlarm->setConditionName(_T("ExclusiveDeviationAlarm"));
	m_exclusiveDeviationAlarm->setSeverity(100);
	m_exclusiveDeviationAlarm->setPropertyValue(BrowseName_ConditionClassId, &conditionClassIdValue);
	m_exclusiveDeviationAlarm->setPropertyValue(BrowseName_ConditionClassName, &conditionClassNameValue);
	if(g_cttCompatible)
	{
		shelvingStateMachine = m_exclusiveDeviationAlarm->getObject(BrowseName_ShelvingState);
		if(shelvingStateMachine)
		{
			shelvingStateMachine->removeTree();
		}
	}

	// set the limits
	m_exclusiveDeviationAlarm->setLowLimit(lowLimit);
	m_exclusiveDeviationAlarm->setLowLowLimit(lowLowLimit);
	m_exclusiveDeviationAlarm->setHighLimit(highLimit);
	m_exclusiveDeviationAlarm->setHighHighLimit(highHighLimit);

	// create the Setpoint variable
	VariablePtr exclusiveDeviationAlarmSetPointVar = TestVariableForAuditEvents::create();
	exclusiveDeviationAlarmSetPointVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.exclusiveDeviationAlarmSetPointVar")));
	exclusiveDeviationAlarmSetPointVar->init(NULL);
	exclusiveDeviationAlarmSetPointVar->setBrowseName(QualifiedName(_T("exclusiveDeviationAlarmSetPointVar"), namespaceIndex));
	exclusiveDeviationAlarmSetPointVar->setDisplayName(LocalizedText(_T("exclusiveDeviationAlarmSetPointVar"),_T("en")));
	exclusiveDeviationAlarmSetPointVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultSetPointValue);
	value.setStatusCode(EnumStatusCode_Good);
	exclusiveDeviationAlarmSetPointVar->setDataValue(value);
	exclusiveDeviationAlarmSetPointVar->setAccessLevel(exclusiveDeviationAlarmSetPointVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	VariablePtr exclusiveDeviationAlarmVar = TestVariableForAuditEvents::create();
	exclusiveDeviationAlarmVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.exclusiveDeviationAlarmVar")));
	exclusiveDeviationAlarmVar->init(NULL);
	exclusiveDeviationAlarmVar->setBrowseName(QualifiedName(_T("exclusiveDeviationAlarmVar"), namespaceIndex));
	exclusiveDeviationAlarmVar->setDisplayName(LocalizedText(_T("exclusiveDeviationAlarmVar"),_T("en")));
	exclusiveDeviationAlarmVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultSetPointValue+defaultInitValue);
	value.setStatusCode(EnumStatusCode_Good);
	exclusiveDeviationAlarmVar->setDataValue(value);
	exclusiveDeviationAlarmVar->setAccessLevel(exclusiveDeviationAlarmVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	m_exclusiveDeviationAlarm->setSetpointVar(exclusiveDeviationAlarmSetPointVar);
	//m_exclusiveDeviationAlarm->addReference(Statics::ReferenceTypeId_HasCondition, exclusiveDeviationAlarmVar);
	exclusiveDeviationAlarmVar->addReference(Statics::ReferenceTypeId_HasCondition, m_exclusiveDeviationAlarm);
	m_exclusiveDeviationAlarm->setSourceName(exclusiveDeviationAlarmVar->getDisplayName()->getText());

	// NonExclusiveDeviationAlarm
	m_nonExclusiveDeviationAlarm = NonExclusiveDeviationAlarm::create();
	m_nonExclusiveDeviationAlarm->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.NonExclusiveDeviationAlarm")));
	m_nonExclusiveDeviationAlarm->init(NULL);
	m_nonExclusiveDeviationAlarm->setBrowseName(QualifiedName(_T("NonExclusiveDeviationAlarm"), namespaceIndex));
	m_nonExclusiveDeviationAlarm->setDisplayName(LocalizedText(_T("NonExclusiveDeviationAlarm"),_T("en")));
	m_nonExclusiveDeviationAlarm->setConditionName(_T("NonExclusiveDeviationAlarm"));
	m_nonExclusiveDeviationAlarm->setSeverity(100);
	m_nonExclusiveDeviationAlarm->setPropertyValue(BrowseName_ConditionClassId, &conditionClassIdValue);
	m_nonExclusiveDeviationAlarm->setPropertyValue(BrowseName_ConditionClassName, &conditionClassNameValue);
	if(g_cttCompatible)
	{
		shelvingStateMachine = m_nonExclusiveDeviationAlarm->getObject(BrowseName_ShelvingState);
		if(shelvingStateMachine)
		{
			shelvingStateMachine->removeTree();
		}
	}

	// set the limits
	m_nonExclusiveDeviationAlarm->setLowLimit(lowLimit);
	m_nonExclusiveDeviationAlarm->setLowLowLimit(lowLowLimit);
	m_nonExclusiveDeviationAlarm->setHighLimit(highLimit);
	m_nonExclusiveDeviationAlarm->setHighHighLimit(highHighLimit);

	// create the Setpoint variable
	VariablePtr nonExclusiveDeviationAlarmSetPointVar = TestVariableForAuditEvents::create();
	nonExclusiveDeviationAlarmSetPointVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.nonExclusiveDeviationAlarmSetPointVar")));
	nonExclusiveDeviationAlarmSetPointVar->init(NULL);
	nonExclusiveDeviationAlarmSetPointVar->setBrowseName(QualifiedName(_T("nonExclusiveDeviationAlarmSetPointVar"), namespaceIndex));
	nonExclusiveDeviationAlarmSetPointVar->setDisplayName(LocalizedText(_T("nonExclusiveDeviationAlarmSetPointVar"),_T("en")));
	nonExclusiveDeviationAlarmSetPointVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultSetPointValue);
	value.setStatusCode(EnumStatusCode_Good);
	nonExclusiveDeviationAlarmSetPointVar->setDataValue(value);
	nonExclusiveDeviationAlarmSetPointVar->setAccessLevel(nonExclusiveDeviationAlarmSetPointVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	VariablePtr nonExclusiveDeviationAlarmVar = TestVariableForAuditEvents::create();
	nonExclusiveDeviationAlarmVar->setNodeId(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.nonExclusiveDeviationAlarmVar")));
	nonExclusiveDeviationAlarmVar->init(NULL);
	nonExclusiveDeviationAlarmVar->setBrowseName(QualifiedName(_T("nonExclusiveDeviationAlarmVar"), namespaceIndex));
	nonExclusiveDeviationAlarmVar->setDisplayName(LocalizedText(_T("nonExclusiveDeviationAlarmVar"),_T("en")));
	nonExclusiveDeviationAlarmVar->setDataType(Statics::DataTypeId_Double);
	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setValue(defaultSetPointValue+defaultInitValue);
	value.setStatusCode(EnumStatusCode_Good);
	nonExclusiveDeviationAlarmVar->setDataValue(value);
	nonExclusiveDeviationAlarmVar->setAccessLevel(nonExclusiveDeviationAlarmVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

	m_nonExclusiveDeviationAlarm->setSetpointVar(nonExclusiveDeviationAlarmSetPointVar);
	//m_nonExclusiveDeviationAlarm->addReference(Statics::ReferenceTypeId_HasCondition, nonExclusiveDeviationAlarmVar);
	nonExclusiveDeviationAlarmVar->addReference(Statics::ReferenceTypeId_HasCondition, m_nonExclusiveDeviationAlarm);
	m_nonExclusiveDeviationAlarm->setSourceName(nonExclusiveDeviationAlarmVar->getDisplayName()->getText());

	// add all the alarms to the organizing object
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_Organizes, m_offNormalAlarm);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_Organizes, m_tripAlarm);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_Organizes, m_exclusiveLimitAlarm);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_Organizes, m_nonExclusiveLimitAlarm);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_Organizes, m_exclusiveLevelAlarm);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_Organizes, m_nonExclusiveLevelAlarm);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_Organizes, m_exclusiveDeviationAlarm);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_Organizes, m_nonExclusiveDeviationAlarm);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_Organizes, m_exclusiveRateOfChangeAlarm);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_Organizes, m_nonExclusiveRateOfChangeAlarm);

	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, offNormalAlarmNormalState);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, offNormalAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, tripAlarmNormalState);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, tripAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, exclusiveLimitAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, nonExclusiveLimitAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, exclusiveLevelAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, nonExclusiveLevelAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, exclusiveDeviationAlarmSetPointVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, exclusiveDeviationAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, nonExclusiveDeviationAlarmSetPointVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, nonExclusiveDeviationAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, exclusiveRateOfChangeAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasComponent, nonExclusiveRateOfChangeAlarmVar);

	// link the variables so the events are propagated to the object
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasEventSource, offNormalAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasEventSource, tripAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasEventSource, exclusiveLimitAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasEventSource, nonExclusiveLimitAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasEventSource, exclusiveLevelAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasEventSource, nonExclusiveLevelAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasEventSource, exclusiveDeviationAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasEventSource, nonExclusiveDeviationAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasEventSource, exclusiveRateOfChangeAlarmVar);
	offTheShelfAlarms->addReference(Statics::ReferenceTypeId_HasEventSource, nonExclusiveRateOfChangeAlarmVar);

	offTheShelfAlarms->insertTree();

	// insert the linked variables in the AddressSpace
	/*offNormalAlarmVar->insert();
	tripAlarmVar->insert();
	exclusiveLimitAlarmVar->insert();
	nonExclusiveLimitAlarmVar->insert();
	exclusiveLevelAlarmVar->insert();
	nonExclusiveLevelAlarmVar->insert();
	exclusiveDeviationAlarmVar->insert();
	nonExclusiveDeviationAlarmVar->insert();
	exclusiveRateOfChangeAlarmVar->insert();
	nonExclusiveRateOfChangeAlarmVar->insert();*/

	// enable the Alarms
	m_offNormalAlarm->enable();
	m_tripAlarm->enable();
	m_exclusiveLimitAlarm->enable();
	m_nonExclusiveLimitAlarm->enable();
	m_exclusiveLevelAlarm->enable();
	m_nonExclusiveLevelAlarm->enable();
	m_exclusiveDeviationAlarm->enable();
	m_nonExclusiveDeviationAlarm->enable();
	m_exclusiveRateOfChangeAlarm->enable();
	m_nonExclusiveRateOfChangeAlarm->enable();

	// link the off normal alarms to the organizing node
	NodeId nodeId = NodeId(1, _T("StructoAutomoldServer.MonitoredNotificationArea"));
	this->addReference(Statics::ReferenceTypeId_Organizes, offTheShelfAlarms);
	this->addReference(Statics::ReferenceTypeId_HasNotifier, offTheShelfAlarms);

	// and link the notifier chain to the off normal node
	AddressSpaceRoot::instance()->getNode(&nodeId)->addReference(Statics::ReferenceTypeId_HasNotifier, offTheShelfAlarms);

	return EnumStatusCode_Good;
}

void OffTheShelfAlarmsModule::endModule()
{
	m_offNormalAlarm->removeTree();
	m_tripAlarm->removeTree();
	m_exclusiveLimitAlarm->removeTree();
	m_nonExclusiveLimitAlarm->removeTree();
	m_exclusiveLevelAlarm->removeTree();
	m_nonExclusiveLevelAlarm->removeTree();
	m_exclusiveDeviationAlarm->removeTree();
	m_nonExclusiveDeviationAlarm->removeTree();
	m_exclusiveRateOfChangeAlarm->removeTree();
	m_nonExclusiveRateOfChangeAlarm->removeTree();

	ModuleFolder::endModule();
}

#endif	// TB5_ALARMS
