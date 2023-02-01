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
#ifndef TEST_MODULE_ROLE_PERMISSION_H
#define TEST_MODULE_ROLE_PERMISSION_H

#include "ServerMethod.h"
#include "ModuleFolder.h"
#include "TestEvent.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		class Object;
		typedef ObjectPointer<Object> ObjectPtr;
		class Variable;
		typedef ObjectPointer<Variable> VariablePtr;
	}
}

class ModifyRolePermissionsMethod;
typedef SoftingOPCToolbox5::ObjectPointer<ModifyRolePermissionsMethod> ModifyRolePermissionsMethodPtr;

class TestRolePermissionMethod;
typedef SoftingOPCToolbox5::ObjectPointer<TestRolePermissionMethod> TestRolePermissionMethodPtr;

/*! A simple test method to test the user executable attribute via the access pattern. */
class TestRolePermissionMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	virtual ~TestRolePermissionMethod();

	static TestRolePermissionMethodPtr initialize(OTUInt16 namespaceIndex, const tstring& name);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);

	OTInt32 add(OTInt32 a, OTInt32 b);

private:
	TestRolePermissionMethod();
	TestRolePermissionMethod(const TestRolePermissionMethod& other);
};

/*! Allows to modify role permissions */
class ModifyRolePermissionsMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	virtual ~ModifyRolePermissionsMethod();
	static ModifyRolePermissionsMethodPtr create();

	EnumStatusCode initialize(OTUInt16 namespaceIndex, const tstring& name,
		TestRolePermissionMethodPtr testMethod,
		SoftingOPCToolbox5::Server::VariablePtr readOnlyVar, SoftingOPCToolbox5::Server::VariablePtr readWriteVar,
		SoftingOPCToolbox5::Server::VariablePtr currentPermissions);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);

private:
	ModifyRolePermissionsMethod();
	ModifyRolePermissionsMethod(const ModifyRolePermissionsMethod& other);

	TestRolePermissionMethodPtr m_testMethod;
	SoftingOPCToolbox5::Server::VariablePtr m_readOnlyVar;
	SoftingOPCToolbox5::Server::VariablePtr m_readWriteVar;
	SoftingOPCToolbox5::Server::VariablePtr m_currentPermissions;
};

/*! Allows to modify the assigned role of the session */
class ModifySessionRoleMethod;
typedef SoftingOPCToolbox5::ObjectPointer<ModifySessionRoleMethod> ModifySessionRoleMethodPtr;
class ModifySessionRoleMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	virtual ~ModifySessionRoleMethod();
	static ModifySessionRoleMethodPtr create();

	EnumStatusCode initialize(OTUInt16 namespaceIndex, const tstring& name);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);

private:
	ModifySessionRoleMethod();
};

/*! Allows to get the currently assigned roles of a session */
class GetSessionRoleMethod;
typedef SoftingOPCToolbox5::ObjectPointer<GetSessionRoleMethod> GetSessionRoleMethodPtr;
class GetSessionRoleMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	virtual ~GetSessionRoleMethod();
	static GetSessionRoleMethodPtr create();

	EnumStatusCode initialize(OTUInt16 namespaceIndex, const tstring& name);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);

private:
	GetSessionRoleMethod();
	GetSessionRoleMethod(const GetSessionRoleMethod& other);
};

/*! Allows to get the currently assigned roles of a session */
class ReportEventsMethod;
typedef SoftingOPCToolbox5::ObjectPointer<ReportEventsMethod> ReportEventsMethodPtr;
class ReportEventsMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	virtual ~ReportEventsMethod();
	static ReportEventsMethodPtr create();

	EnumStatusCode initialize(OTUInt16 namespaceIndex, const tstring& name,
		std::vector<SoftingOPCToolbox5::Server::BaseEventPtr> eventsToReport,
		std::vector<SoftingOPCToolbox5::Server::ObjectPtr> nodesToReportEvent);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);

private:
	ReportEventsMethod();
	ReportEventsMethod(const ReportEventsMethod& other);

	std::vector<SoftingOPCToolbox5::Server::BaseEventPtr> m_eventsToReport;
	std::vector<SoftingOPCToolbox5::Server::ObjectPtr> m_nodesToReportEvent;
};

class RolePermissionModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	virtual void endModule();
private:
	SoftingOPCToolbox5::Server::VariablePtr createVar(tstring name, OTUInt16 namespaceIndex);
	SoftingOPCToolbox5::Server::ObjectPtr createObj(tstring name, OTUInt16 namespaceIndex);
	EnumStatusCode initBrowseTestNodes(OTUInt16 namespaceIndex);
	EnumStatusCode initEventTestNodes(OTUInt16 namespaceIndex);
	void endEventTestNodes();
	EnumStatusCode initWriteMaskTestNodes(OTUInt16 namespaceIndex);
	void endWriteMaskTestNodes();

	SoftingOPCToolbox5::Server::ObjectPtr m_eventTestFolder;
	SoftingOPCToolbox5::Server::ObjectPtr m_writeMaskTestFolder;
};

#endif	// TEST_MODULE_ROLE_PERMISSION_H
