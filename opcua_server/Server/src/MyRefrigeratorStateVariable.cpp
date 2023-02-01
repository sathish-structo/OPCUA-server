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
#include "MyRefrigeratorStateVariable.h"
#include "ServerReferenceIterator.h"
#include "Statics.h"
#include "ServerAddressSpaceRoot.h"
#include "EnumValueType.h"
#include "ServerDataType.h"
#include "EnumField.h"
#include "EnumerationDataTypeDescription.h"
#include "ServerBaseEventNotification.h"

using namespace SoftingOPCToolbox5;

MyRefrigeratorStateVariable::MyRefrigeratorStateVariable()
	: TestVariableForAuditEvents()
{
}

MyRefrigeratorStateVariable::~MyRefrigeratorStateVariable()
{
}

ObjectPointer<MyRefrigeratorStateVariable> MyRefrigeratorStateVariable::create()
{
	return new MyRefrigeratorStateVariable();
}

EnumStatusCode MyRefrigeratorStateVariable::init(const INodeId* typeId)
{
	EnumStatusCode result = TestVariableForAuditEvents::init(typeId);
	setDataValueChangeCallbackEnabled(true);
	return result;
}

void MyRefrigeratorStateVariable::onDataValueChanged(const IDataValue* pNewDataValue)
{
	// if value has changed (and don't report on bad status)
	if(StatusCode::isGood(pNewDataValue->getStatusCode()) &&
		m_lastValue.getValue()->compare(pNewDataValue->getValue()) != 0)
	{
		m_lastValue.set(pNewDataValue);

#if TB5_ALARMS
		// create an event notification
		Server::BaseEventNotificationPtr refrigeratorStatusEventNotification;
		refrigeratorStatusEventNotification = Server::BaseEventNotification::create(&m_eventTypeIdToReport);
		refrigeratorStatusEventNotification->setMessage(LocalizedText(_T("RefrigeratorStatus has changed."), _T("en")));
		refrigeratorStatusEventNotification->setSeverity(100);
		refrigeratorStatusEventNotification->setTime(DateTime::getUtcNow());
		refrigeratorStatusEventNotification->setReceiveTime(DateTime::getUtcNow());
		refrigeratorStatusEventNotification->setSourceName(getDisplayName()->getText());
		refrigeratorStatusEventNotification->setSourceNode(getNodeId());
		// set the custom data
		refrigeratorStatusEventNotification->setProperty(QualifiedName(_T("RefrigeratorStatus"), m_eventTypeIdToReport.getNamespaceIndex()), pNewDataValue->getValue());
		// report the event notification
		EnumStatusCode reportResult = refrigeratorStatusEventNotification->report(&m_reportingNodeId);
		_UNUSED(reportResult);
		// should only fail when the node does not exist
		_ASSERTION(StatusCode::isGood(reportResult), _T("Failed to report event"));
#endif
	}
}

void MyRefrigeratorStateVariable::setEventTypeToReport(const SoftingOPCToolbox5::INodeId& eventTypeIdToReport)
{
	m_eventTypeIdToReport = eventTypeIdToReport;
}

void MyRefrigeratorStateVariable::setReportingNode(const INodeId& nodeIdToReport)
{
	m_reportingNodeId = nodeIdToReport;
}
