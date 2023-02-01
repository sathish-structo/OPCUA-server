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
#ifndef _APPLICATIONMODULE_H_
#define _APPLICATIONMODULE_H_

// includes
#include "ServerBaseNode.h"
#include "ServerAddressSpaceRoot.h"

#include "TestModule.h"
#include "RedModule.h"
#include "Util_WorkItemProcessor.h"
#include "Util_WorkItem.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

#define SERVER_APP_IDENTIFIER _T("Softing/OpcUa/StructoAutomoldServer")

class ShutdownAction
	: public Util_ScheduledAction
{
public:
	ShutdownAction()
		: m_pApplicationModule(0)
	{
	}

	void init(ApplicationModule* pApplicationModule)
	{
		m_pApplicationModule = pApplicationModule;
	}

	virtual void execute();

	ApplicationModule* m_pApplicationModule;
};

class ThreadReaperAction
	: public Util_ScheduledAction
{
public:
	ThreadReaperAction()
		: m_pApplicationModule(0)
	{
	}

	void init(ApplicationModule* pApplicationModule)
	{
		m_pApplicationModule = pApplicationModule;
	}

	virtual void execute();

	ApplicationModule* m_pApplicationModule;
};

class ThreadReaper
{
private:
	struct threadlist
	{
		Util_Thread *pthread;
		threadlist *next;
		threadlist *prev;
	};
	threadlist m_head;
	Util_Sync m_sync;
	bool m_acceptThreads;

public:
	ThreadReaper();
	bool addThread(Util_Thread *pthread);
	void timer();
	void end();
};

class TestVariablesModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	virtual void endModule();
};

/**
 * The ApplicationModule class manages the server namespace and the submodules
 * like statistics, simulation etc...
 */
class ApplicationModule
		: public Util_TimerThread
{
public:
	/*! Constrcutor */
	ApplicationModule();

	/*! Destructor */
	~ApplicationModule();

	/*! Initializes the static RoleIds */
	static void initializeStatics();
	static void uninitializeStatics();

	/*! Will be called during startup
	inits the namespace with all folders and modules, registers commands and parameter values */
	bool init(const ApplicationDescription &applicationDescription, const std::vector<EndpointPtr>& localEndpoints, EnumRedundancySupport redMode, const std::vector<tstring> &redUrls, const tstring &pathToServerFiles);

	/*! Assign both, AccessPattern and RolePermissions to a node
	*
	* @note Both describe the same rights, but generate different RoleIds for them, so we need both */
	static void assignAccessPatternAndRolePermissions(Server::BaseNodePtr node, OTUInt32 accessPatternKey);

	// get the role permissions as formerly defined by user groups
	static void getRolePermissionsForKey(OTUInt32 accessPatternKey, std::vector<RolePermissionType> & rolePermissions);

	/*! Will be called during shut down of the server. Performs required deinitializations. */
	void end();

	/*! Returns true if the server shall shutdown. */
	bool getShutDownFlag();

	/*! Sets the shutdown in the server */
	void setShutDownFlag();

	/*! Sets the shutdown flag in the application module for a client stimulated shutdown */
	void shutDown(unsigned timeout);

	/*! Getter for the namespace index */
	OTUInt16 getNameSpaceIndex();

	/*! Getter for the namespace index of the RefrigeratorType */
	OTUInt16 getTypesNameSpaceIndex();

	ThreadReaper* getThreadReaper()
	{
		return &m_threadReaper;
	}

	OTUInt32 getServiceDelay(EnumNumericNodeId serviceId);
	EnumStatusCode setServiceDelay(EnumNumericNodeId serviceId, OTUInt32 delay);

	bool addWorkItem(Util_WorkItem* item);
	void setSourceFolder(const tstring &sourceFolder);
	tstring getSourceFolder();

	void enableAccessPattern(bool enabled);

	std::vector<ModuleFolderPtr>* GetAppModule() { return &m_modules;}

private:

	EnumStatusCode initializeUserAccessPatterns();

	static void assignAccessPattern(Server::BaseNodePtr node, OTUInt32 accessPatternKey);
	std::vector<ModuleFolderPtr> m_modules;

	// store this extra to have access to the test module methods
	TestModulePtr m_testModule;

	/*! Thread joiner */
	ThreadReaper	m_threadReaper;

	/*! Namespace index */
	OTUInt16 m_nameSpaceIndex;

	tstring m_namespaceUriType;
	OTUInt16 m_nameSpaceIndexTypes;

#if TB5_CLIENT
	/*! Redundancy SubModule */
	RedModule m_redModule;
#endif

	tstring m_sourceFolder;

	tstring m_pathToServerFilesFolder;

	/*! flag will be set by the shutdown command  */
	OTAtomic m_shutDown;

	/*! Builds the address space(Only the three folders) */
	EnumStatusCode initModules(OTUInt16 namespaceIndex, const ApplicationDescription &applicationDescription, const std::vector<EndpointPtr>& localEndpoints, EnumRedundancySupport redMode, const std::vector<tstring> &redUrls);

	ShutdownAction m_shutDownAction;
	ThreadReaperAction m_threadReaperAction;
	Util_WorkItemProcessor* m_workItemProcessor;

public:

	static bool s_accessPatternEnabled;

	// user access
	static const OTUInt32 s_UserGroup_Default; // anonymous / eve
	static const OTUInt32 s_UserGroup_Group1; // usr / certificate
	static const OTUInt32 s_UserGroup_Group2; // usr2
	static const OTUInt32 s_UserGroup_Query; // query
	static const OTUInt32 s_UserGroup_RedundancyAccess;

	static const OTUInt32 s_AccessKey_SDK_Default;
	static const OTUInt32 s_AccessKey_ReadHistory_Read_Read_None_None;
	static const OTUInt32 s_AccessKey_Read_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None;
	static const OTUInt32 s_AccessKey_None_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None;
	static const OTUInt32 s_AccessKey_Read_Read_Read_Read_ReadCall;
	static const OTUInt32 s_AccessKey_Read_ReadWriteCall_Read_Read_Read;
	static const OTUInt32 s_AccessKey_None_ReadCall_ReadWriteCall_ReadCall_ReadCall;
	static const OTUInt32 s_AccessKey_ForAccessModifications;

	// user access via roles
	static NodeId* s_RoleIdDefault;
	static NodeId* s_RoleIdGroup1;
	static NodeId* s_RoleIdGroup2;
	static NodeId* s_RoleIdQuery;
	static NodeId* s_RoleIdRedundancy;
};

ApplicationModule* getApplicationModule();

tstring buildAppUri(tstring hostname, tstring applicationIdentifier);

#endif
