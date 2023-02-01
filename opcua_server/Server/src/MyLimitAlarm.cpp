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
#include "MyLimitAlarm.h"

MyLimitAlarm::MyLimitAlarm(VariablePtr isEnabled): m_isEnabled(isEnabled)
{
}

MyLimitAlarm::~MyLimitAlarm()
{
}

EnumStatusCode MyLimitAlarm::init(const INodeId* typeId)
{
	EnumStatusCode result = LimitAlarm::init(typeId);
	setEventNotifier(EnumEventNotifier_SubscribeToEvents);
	return result;
}

void MyLimitAlarm::onEnable()
{
	DataValue dataValue;
	Value value;
	value.setBoolean(true);
	dataValue.setValue(value);
	m_isEnabled->setDataValue(dataValue);
	_tprintf(_T("onEnable\n"));
}

void MyLimitAlarm::onDisable()
{
	DataValue dataValue;
	Value value;
	value.setBoolean(false);
	dataValue.setValue(value);
	m_isEnabled->setDataValue(dataValue);
	_tprintf(_T("onDisable\n"));
}
#endif
