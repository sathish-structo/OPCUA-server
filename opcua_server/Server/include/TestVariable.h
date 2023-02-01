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
#ifndef _TESTVARIABLE_H_
#define _TESTVARIABLE_H_

#include "ServerVariable.h"
#include "ServerTwoStateItem.h"
#include "ServerMultiStateItem.h"
#include "ServerAnalogItem.h"
#include "ServerDataTransaction.h"
#include "ServerMonitoredItem.h"

using namespace SoftingOPCToolbox5;

class TestVariableForAuditEvents
	: public Server::Variable
{
public:
	static ObjectPointer<Server::Variable> create();
	TestVariableForAuditEvents();
	TestVariableForAuditEvents(void* otbHandle);
	virtual ~TestVariableForAuditEvents();
	virtual EnumStatusCode handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest);

private:
};

class TwoStateItemForAuditEvents
	: public Server::TwoStateItem
{
public:
	static ObjectPointer<Server::TwoStateItem> create();
	static ObjectPointer<Server::TwoStateItem> create(
		const INodeId& nodeId,
		const ILocalizedText& displayName,
		const IQualifiedName& browseName,
		const ILocalizedText& trueStateText,
		const ILocalizedText& falseStateText);

	TwoStateItemForAuditEvents();
	TwoStateItemForAuditEvents(void* otbHandle);
	TwoStateItemForAuditEvents(const INodeId& nodeId,
		const ILocalizedText& displayName,
		const IQualifiedName& browseName,
		const ILocalizedText& trueStateText,
		const ILocalizedText& falseStateText);

	virtual ~TwoStateItemForAuditEvents();
	virtual EnumStatusCode handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest);

private:
};

class MultiStateItemForAuditEvents
	: public Server::MultiStateItem
{
public:
	static ObjectPointer<Server::MultiStateItem> create();
	static ObjectPointer<Server::MultiStateItem> create(
		const INodeId& nodeId,
		const ILocalizedText& displayName,
		const IQualifiedName& browseName,
		const INodeId* enumStringsPropertyNodeId = NULL);

	MultiStateItemForAuditEvents();
	MultiStateItemForAuditEvents(void* otbHandle);
	MultiStateItemForAuditEvents(const INodeId& nodeId,
		const ILocalizedText& displayName,
		const IQualifiedName& browseName,
		const INodeId* enumStringsPropertyNodeId = NULL);

	virtual ~MultiStateItemForAuditEvents();
	virtual EnumStatusCode handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest);

private:
};

class AnalogItemForAuditEvents
	: public Server::AnalogItem
{
public:
	static ObjectPointer<Server::AnalogItem> create();
	static ObjectPointer<Server::AnalogItem> create(
		const INodeId& nodeId,
		const NodeId* dataType,
		const ILocalizedText& displayName,
		const IQualifiedName& browseName,
		const IRange& EURange);

	AnalogItemForAuditEvents();
	AnalogItemForAuditEvents(void* otbHandle);
	AnalogItemForAuditEvents(const INodeId& nodeId,
		const NodeId* dataType,
		const ILocalizedText& displayName,
		const IQualifiedName& browseName,
		const IRange& EURange);

	virtual ~AnalogItemForAuditEvents();
	virtual EnumStatusCode handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest);

private:
};

/**
 * @brief TestVariables will be used as OPC variables representing variables having all the types
 * (Test purpose)
 */
class TestVariable
	: public TestVariableForAuditEvents
{
public:
	TestVariable();
	TestVariable(void* otbHandle);
	virtual ~TestVariable();
	/* do not hide BaseNode::init with this static init */
	static void s_init();
	static void s_end();
	static const INodeId* getDirectHandledVarNodeId();

	virtual EnumStatusCode handleReadRequest(Server::ReadTransaction* pTransaction, Server::ReadRequest* pRequest);
	virtual EnumStatusCode handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest);

	EnumStatusCode addMonitoredIItem(Server::MonitoredItemPtr monitoredItem);
	EnumStatusCode removeMonitoredItem(Server::MonitoredItemPtr monitoredItem);
	EnumInitialValueProvider onProvideInitialValues(Server::MonitoredItemPtr monitoredItem);

private:
	static DataValue* c_pDirectHandledVarValue;
	static NodeId* c_pDirectHandledVarValueNodeId;
	std::map<Server::MonitoredItem*, Server::MonitoredItemPtr> m_monitoredItems;
};

typedef ObjectPointer<TestVariable> TestVariablePtr;
typedef ObjectPointer<const TestVariable> TestVariableConstPtr;


#endif
