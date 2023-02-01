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
#include "TestVariable.h"
#include "ServerReadTransaction.h"
#include "ServerReadRequest.h"
#include "ServerWriteTransaction.h"
#include "ServerWriteRequest.h"
#include "DiagnosticMessages.h"
#include "ApplicationModule.h"
#include "ServerAuditWriteUpdateEvent.h"
#include "ServerSession.h"
#include "ServerWriteTransaction.h"
#include "Application.h"

DataValue* TestVariable::c_pDirectHandledVarValue;
NodeId* TestVariable::c_pDirectHandledVarValueNodeId;

void TestVariable::s_init()
{
	if(c_pDirectHandledVarValueNodeId == NULL)
	{
		c_pDirectHandledVarValueNodeId = new(NodeId);
		c_pDirectHandledVarValueNodeId->setString(getApplicationModule()->getNameSpaceIndex(), _T("InsertDataNotificationVar"));

	}
	if(c_pDirectHandledVarValue == NULL)
	{
		DateTime now = DateTime::getUtcNow();
		Value initialVal;
		c_pDirectHandledVarValue = new DataValue;
		initialVal.setUInt32(1);
		c_pDirectHandledVarValue->setValue(initialVal);
		c_pDirectHandledVarValue->setServerTimestamp(now);
		c_pDirectHandledVarValue->setSourceTimestamp(now);
	}
}

void TestVariable::s_end()
{
	if(c_pDirectHandledVarValueNodeId)
	{
		delete c_pDirectHandledVarValueNodeId;
		c_pDirectHandledVarValueNodeId = NULL;
	}
	if(c_pDirectHandledVarValue)
	{
		delete c_pDirectHandledVarValue;
		c_pDirectHandledVarValue = NULL;
	}
}

const SoftingOPCToolbox5::INodeId* TestVariable::getDirectHandledVarNodeId()
{
	return c_pDirectHandledVarValueNodeId;
}

/**
* @brief Constructor
*/
TestVariable::TestVariable()
: TestVariableForAuditEvents()
{
}

/**
* @brief Constructor
* @param otbHandle Void pointer
*/
TestVariable::TestVariable(void* otbHandle)
: TestVariableForAuditEvents(otbHandle)
{
}

/**
* @brief Destructor
*/
TestVariable::~TestVariable()
{
}

/**
* @brief Overloaded handleRequest
* @param pTransaction Pointer to DataTransition
* @param pRequest Pointer to DataRequest
* @return void
*/
EnumStatusCode TestVariable::handleReadRequest(Server::ReadTransaction* pTransaction, Server::ReadRequest* pRequest)
{
	if((getNodeId()->compare(getDirectHandledVarNodeId()) == 0) && (pRequest->getValueId()->getAttributeId() == EnumAttributeId_Value))
	{
		DateTime now = DateTime::getUtcNow();
		c_pDirectHandledVarValue->setServerTimestamp(now);
		c_pDirectHandledVarValue->setSourceTimestamp(now);
		pRequest->setDataValue(c_pDirectHandledVarValue);
		pRequest->complete();
		return EnumStatusCode_Good;
	}

	// Return some diagnostic information if requested
	if(DiagnosticInfo::returnOperationLevelDiagnostics(pTransaction->getDiagnosticsToReturn()))
	{
		DiagnosticInfo diagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
			DiagnosticMessages::EnumDiagnosticMessageLevel_Operation, DiagnosticMessages::EnumDiagnosticMessage_NoError, pTransaction);

		if(pTransaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_ServiceLevel_AdditionalInfo)
		{
			diagnosticInfo.setAdditionalInfo(_T("No additional information available"));
		}
		if(pTransaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_OperationLevel_InnerDiagnostics)
		{
			DiagnosticInfo innerDiagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
				DiagnosticMessages::EnumDiagnosticMessageLevel_Operation, DiagnosticMessages::EnumDiagnosticMessage_NoError, pTransaction);

			diagnosticInfo.setInnerDiagnosticInfo(&innerDiagnosticInfo);
		}

		EnumStatusCode setDiagnosticInfoResult = pRequest->setDiagnosticInfo(diagnosticInfo);
		_UNUSED(setDiagnosticInfoResult);
		_ASSERTION(StatusCode::isGood(setDiagnosticInfoResult), _T("setDiagnosticInfo() failed"));
	}

	EnumAttributeId attributeId = pRequest->getValueId()->getAttributeId();
	if (attributeId == EnumAttributeId_Value)
	{ // Read value
		// fake a new "read from device" if server time is too old

		DateTime now = DateTime::getUtcNow();
		double maxAge = pTransaction->getMaxAge();
		bool fakeNecessary = (maxAge == 0.0); // always fake if max age is 0.0
		if(!fakeNecessary)
		{
			// now compare times
			DateTime serverTime = getDataValue()->getServerTimestamp();
			if(serverTime.compare(now) < 0)
			{
				OTUInt64 ulDiff = DateTime::getDurationMilliseconds(getDataValue()->getServerTimestamp(), now);
				fakeNecessary = ((double)ulDiff > maxAge);
			}
		}
		if (fakeNecessary)
		{
			// fake a new "read from device"
			DataValue newVal = getDataValue();
			newVal.setServerTimestamp(now);
			newVal.setServerPicoseconds(0);
			this->setDataValue(newVal);
		}
	}

	return TestVariableForAuditEvents::handleReadRequest(pTransaction, pRequest);
}

/**
* @brief Overloaded handleRequest
* @param pTransaction Pointer to WriteTransition
* @param pRequest Pointer to WriteRequest
* @return void
*/
EnumStatusCode TestVariable::handleWriteRequest(Server::WriteTransaction* pTransaction , Server::WriteRequest* pRequest)
{
	if((getNodeId()->compare(getDirectHandledVarNodeId()) == 0) && 
		(pRequest->getWriteValue()->getAttributeId() == EnumAttributeId_Value) &&
		(StatusCode::isGood(pRequest->getStatusCode())))
	{
		DateTime now = DateTime::getUtcNow();
		c_pDirectHandledVarValue->setServerTimestamp(now);
		c_pDirectHandledVarValue->setSourceTimestamp(pRequest->getWriteValue()->getDataValue()->getSourceTimestamp());
		c_pDirectHandledVarValue->setValue(pRequest->getWriteValue()->getDataValue()->getValue());
		c_pDirectHandledVarValue->setStatusCode(pRequest->getWriteValue()->getDataValue()->getStatusCode());
		pRequest->complete();

		// now notify the MI
		for(std::map<Server::MonitoredItem*, Server::MonitoredItemPtr>::iterator it = m_monitoredItems.begin(); it != m_monitoredItems.end();++it)
		{
			it->second->reportDataChange(*c_pDirectHandledVarValue);
		}
		return EnumStatusCode_Good;
	}

	// Return some diagnostic information if requested
	if(DiagnosticInfo::returnOperationLevelDiagnostics(pTransaction->getDiagnosticsToReturn()))
	{
		DiagnosticMessages::EnumDiagnosticMessage messageType =
			StatusCode::isGood(pRequest->getStatusCode()) ?
			DiagnosticMessages::EnumDiagnosticMessage_NoError :
		DiagnosticMessages::EnumDiagnosticMessage_GeneralError;

		DiagnosticInfo diagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
			DiagnosticMessages::EnumDiagnosticMessageLevel_Operation, messageType, pTransaction);

		if(pTransaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_ServiceLevel_AdditionalInfo)
		{
			diagnosticInfo.setAdditionalInfo(_T("No additional information available"));
		}
		if(pTransaction->getDiagnosticsToReturn() & EnumDiagnosticsToReturn_OperationLevel_InnerDiagnostics)
		{
			DiagnosticInfo innerDiagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
				DiagnosticMessages::EnumDiagnosticMessageLevel_Operation, messageType, pTransaction);

			diagnosticInfo.setInnerDiagnosticInfo(&innerDiagnosticInfo);
		}

		EnumStatusCode setDiagnosticInfoResult = pRequest->setDiagnosticInfo(diagnosticInfo);
		_UNUSED(setDiagnosticInfoResult);
		_ASSERTION(StatusCode::isGood(setDiagnosticInfoResult), _T("setDiagnosticInfo() failed"));
	}

	return TestVariableForAuditEvents::handleWriteRequest(pTransaction, pRequest);
}

EnumStatusCode TestVariable::addMonitoredIItem(Server::MonitoredItemPtr monitoredItem)
{
	EnumStatusCode result = EnumStatusCode_Good;
	if(!m_monitoredItems.insert(make_pair(monitoredItem.p(), monitoredItem)).second)
	{
		result = EnumStatusCode_BadEntryExists;
	}
	// do not automatically report data changes - only when asked via onProvideInitialValues
	//monitoredItem->reportDataChange(*c_pDirectHandledVarValue);
	return result;
}

EnumStatusCode TestVariable::removeMonitoredItem(Server::MonitoredItemPtr monitoredItem)
{
	EnumStatusCode result = EnumStatusCode_Good;
	if(m_monitoredItems.erase(monitoredItem) == 0)
	{
		result = EnumStatusCode_BadEntryExists;
	}
	return result;
}

EnumInitialValueProvider TestVariable::onProvideInitialValues(Server::MonitoredItemPtr monitoredItem)
{
	// only report data changes when asked
	monitoredItem->reportDataChange(*c_pDirectHandledVarValue);

	return EnumInitialValueProvider_UnderlyingSystem;
}

SoftingOPCToolbox5::Server::VariablePtr TestVariableForAuditEvents::create()
{
	return new TestVariableForAuditEvents();
}

TestVariableForAuditEvents::TestVariableForAuditEvents() : Variable()
{
}

TestVariableForAuditEvents::TestVariableForAuditEvents(void* otbHandle) : Variable(otbHandle)
{
}

TestVariableForAuditEvents::~TestVariableForAuditEvents()
{
}

EnumStatusCode TestVariableForAuditEvents::handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest)
{
	EnumStatusCode result = EnumStatusCode_Good;
	AuditWriteUpdateEventPtr auditEvent;
	if (Application::instance()->getEnableAuditEvents())
	{
		auditEvent = AuditWriteUpdateEvent::create();
		auditEvent->setAttributeId(pRequest->getWriteValue()->getAttributeId());
		auditEvent->setIndexRange(pRequest->getWriteValue()->getIndexRange());
		auditEvent->setOldValue(getDataValue()->getValue());
		auditEvent->setClientUserId(pTransaction->getSession()->getUserClientId());
	}

	result = Server::Variable::handleWriteRequest(pTransaction, pRequest);

	if (auditEvent)
	{
		auditEvent->setNewValue(getDataValue()->getValue());
		auditEvent->setStatus(StatusCode::isGood(pRequest->getStatusCode()));
		auditEvent->report(NULL); // report on server node
	}

	return result;
}

ObjectPointer<SoftingOPCToolbox5::Server::AnalogItem> AnalogItemForAuditEvents::create()
{
	return new AnalogItemForAuditEvents();
}

ObjectPointer<SoftingOPCToolbox5::Server::AnalogItem> AnalogItemForAuditEvents::create(const INodeId& nodeId,
	const NodeId* dataType, const ILocalizedText& displayName, const IQualifiedName& browseName, const IRange& EURange)
{
	return Server::AnalogItem::create(nodeId, dataType, displayName, browseName, EURange);
}

AnalogItemForAuditEvents::AnalogItemForAuditEvents() : Server::AnalogItem()
{
}

AnalogItemForAuditEvents::AnalogItemForAuditEvents(void* otbHandle) : Server::AnalogItem(otbHandle)
{
}

AnalogItemForAuditEvents::AnalogItemForAuditEvents(const INodeId& nodeId,
	const NodeId* dataType,
	const ILocalizedText& displayName,
	const IQualifiedName& browseName,
	const IRange& EURange) : AnalogItem(nodeId, dataType, displayName, browseName, EURange)
{
}

AnalogItemForAuditEvents::~AnalogItemForAuditEvents()
{
}

EnumStatusCode AnalogItemForAuditEvents::handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest)
{
	EnumStatusCode result = EnumStatusCode_Good;
	AuditWriteUpdateEventPtr auditEvent;
	if (Application::instance()->getEnableAuditEvents())
	{
		auditEvent = AuditWriteUpdateEvent::create();
		auditEvent->setAttributeId(pRequest->getWriteValue()->getAttributeId());
		auditEvent->setIndexRange(pRequest->getWriteValue()->getIndexRange());
		auditEvent->setOldValue(getDataValue()->getValue());
		auditEvent->setClientUserId(pTransaction->getSession()->getUserClientId());
	}

	result = Server::AnalogItem::handleWriteRequest(pTransaction, pRequest);

	if (auditEvent)
	{
		auditEvent->setNewValue(getDataValue()->getValue());
		auditEvent->setStatus(StatusCode::isGood(pRequest->getStatusCode()));
		auditEvent->report(NULL); // report on server node
	}

	return result;
}

ObjectPointer<Server::TwoStateItem> TwoStateItemForAuditEvents::create()
{
	return new TwoStateItemForAuditEvents();
}

ObjectPointer<Server::TwoStateItem> TwoStateItemForAuditEvents::create(const INodeId& nodeId, const ILocalizedText& displayName, const IQualifiedName& browseName, const ILocalizedText& trueStateText, const ILocalizedText& falseStateText)
{
	return new TwoStateItemForAuditEvents(nodeId, displayName, browseName, trueStateText, falseStateText);
}

TwoStateItemForAuditEvents::TwoStateItemForAuditEvents() : Server::TwoStateItem()
{
}

TwoStateItemForAuditEvents::TwoStateItemForAuditEvents(void* otbHandle) : Server::TwoStateItem(otbHandle)
{
}

TwoStateItemForAuditEvents::TwoStateItemForAuditEvents(const INodeId& nodeId,
	const ILocalizedText& displayName,
	const IQualifiedName& browseName,
	const ILocalizedText& trueStateText,
	const ILocalizedText& falseStateText) : Server::TwoStateItem(nodeId,
		displayName,
		browseName,
		trueStateText,
		falseStateText)
{
}

TwoStateItemForAuditEvents::~TwoStateItemForAuditEvents()
{
}

EnumStatusCode TwoStateItemForAuditEvents::handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest)
{
	EnumStatusCode result = EnumStatusCode_Good;
	AuditWriteUpdateEventPtr auditEvent;
	auditEvent = AuditWriteUpdateEvent::create();
	if (Application::instance()->getEnableAuditEvents())
	{
		auditEvent->setAttributeId(pRequest->getWriteValue()->getAttributeId());
		auditEvent->setIndexRange(pRequest->getWriteValue()->getIndexRange());
		auditEvent->setOldValue(getDataValue()->getValue());
		auditEvent->setClientUserId(pTransaction->getSession()->getUserClientId());
	}

	result = Server::TwoStateItem::handleWriteRequest(pTransaction, pRequest);

	if (auditEvent)
	{
		auditEvent->setNewValue(getDataValue()->getValue());
		auditEvent->setStatus(StatusCode::isGood(pRequest->getStatusCode()));
		auditEvent->report(NULL); // report on server node
	}

	return result;
}

SoftingOPCToolbox5::ObjectPointer<SoftingOPCToolbox5::Server::MultiStateItem> MultiStateItemForAuditEvents::create()
{
	return new MultiStateItemForAuditEvents();
}

SoftingOPCToolbox5::ObjectPointer<SoftingOPCToolbox5::Server::MultiStateItem> MultiStateItemForAuditEvents::create(const INodeId& nodeId,
	const ILocalizedText& displayName,
	const IQualifiedName& browseName,
	const INodeId* enumStringsPropertyNodeId)
{
	return new MultiStateItemForAuditEvents(nodeId, displayName, browseName, enumStringsPropertyNodeId);
}

MultiStateItemForAuditEvents::MultiStateItemForAuditEvents() : Server::MultiStateItem()
{

}

MultiStateItemForAuditEvents::MultiStateItemForAuditEvents(void* otbHandle) : Server::MultiStateItem(otbHandle)
{
}

MultiStateItemForAuditEvents::MultiStateItemForAuditEvents(const INodeId& nodeId,
	const ILocalizedText& displayName,
	const IQualifiedName& browseName,
	const INodeId* enumStringsPropertyNodeId) : Server::MultiStateItem(nodeId, displayName, browseName, enumStringsPropertyNodeId)
{
}

MultiStateItemForAuditEvents::~MultiStateItemForAuditEvents()
{
}

EnumStatusCode MultiStateItemForAuditEvents::handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest)
{
	EnumStatusCode result = EnumStatusCode_Good;
	AuditWriteUpdateEventPtr auditEvent;
	if (Application::instance()->getEnableAuditEvents())
	{
		auditEvent = AuditWriteUpdateEvent::create();
		auditEvent->setAttributeId(pRequest->getWriteValue()->getAttributeId());
		auditEvent->setIndexRange(pRequest->getWriteValue()->getIndexRange());
		auditEvent->setOldValue(getDataValue()->getValue());
		auditEvent->setClientUserId(pTransaction->getSession()->getUserClientId());
	}

	result = Server::MultiStateItem::handleWriteRequest(pTransaction, pRequest);

	if (auditEvent)
	{
		auditEvent->setNewValue(getDataValue()->getValue());
		auditEvent->setStatus(StatusCode::isGood(pRequest->getStatusCode()));
		auditEvent->report(NULL); // report on server node
	}

	return result;
}
