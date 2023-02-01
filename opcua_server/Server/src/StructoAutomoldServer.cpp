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
#include <iostream>

#include "Toolbox.h"
#include "Application.h"
#include "ServerEndpoint.h"
#include "ServerVariable.h"
#include "ServerAddressSpaceRoot.h"
#include "ServerDiagnostics.h"
#include "ToolkitTrace.h"
#include "ServerStatusDataType.h"
#include "Constants.h"
#include "ServerMultiStateItem.h"
#include "PkiStoreConfiguration.h"
#include "ServerSession.h"
#include "tb5version.h"
#include "UserIdentityToken.h"

#include "ApplicationModule.h"
#include "TestServerCreator.h"
#include "CommandLineOptions.h"
#include "TestEndpoint.h"
#include "SimVarManager.h"
#include "DynamicNodeCache.h"
#include "MyTrace.h"
#include "StructoAutomoldApplication.h"
#include "UserInterface.h"
#include "EndpointManipulationMethods.h"

#ifdef TBC_OS_WINDOWS
#include <crtdbg.h>
#endif
#ifdef SODEVENV_VXWORKS
#include <unistd.h>
#endif

#include "CustomCallbackEvent.h"
#include <limits.h>

#if defined SOOS_WINDOWS && ! defined SOOS_WINDOWS_CE
#include <iphlpapi.h>
// pragma comment places a library search comment in the object file, otherwise we would have to add this to the project settings
#pragma comment(lib, "Iphlpapi.lib")
#endif
#ifdef SODEVENV_LINUX
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <ifaddrs.h>
#endif

using namespace SoftingOPCToolbox5;

static NodeId* g_pPendingGdsRequest;

#ifdef SOOS_POSIX
int _kbhit(void)
{
	struct timeval tv;
	fd_set fd;
	memset(&tv, 0, sizeof(tv));
	memset(&fd, 0, sizeof(fd));
	FD_SET(0, &fd);
	return select(1, &fd, NULL, NULL, &tv)>0;
}
#endif

#ifdef _WIN32_WCE
inline int _kbhit()
{
	return 0;
}
#endif

// class to dump used memory in a file - appends to existing file
// Works in the moment only in VxWorks
// #define DUMP_MEMORYUSAGE
#ifdef DUMP_MEMORYUSAGE

#ifdef __VXWORKS__
#include <version.h>
#include <types/vxTypesOld.h>
#include <memPartLib.h>
#include <memLib.h>
#endif

// write every 60 minutes
#define MEMUSAGE_CYCLE 60 * 60 * 1000
class ServerMemInfoAction
	: public Util_ScheduledAction
{
public:
	ServerMemInfoAction()
	{
		char szMemInfoFile[260];
		// write e.g. to file "Vx70MemInfo.csv" ("69" is the VxWorks version number)
		// Assumes that there is a writable share named "/hostfs/ws"m which is available on VxWorks7 targets when having a connection established to workbench
#ifdef _WRS_VXWORKS_MINOR
		sprintf(szMemInfoFile, "/hostfs/ws/Vx%d%dMemInfo.csv", _WRS_VXWORKS_MAJOR, _WRS_VXWORKS_MINOR);
#else
		sprintf(szMemInfoFile, "/hostfs/ws/Vx%dMemInfo.csv", _WRS_VXWORKS_MAJOR);
#endif
		m_filename = szMemInfoFile;
	}

	virtual void execute()
	{
		MEM_PART_STATS memStatus;
		STATUS status = memInfoGet(&memStatus);
		if(status == OK)
		{
			FILE* fout = fopen(m_filename.c_str(), "a+");
			if(fout != NULL)
			{
				fprintf(fout, "%s  ;%lu;%lu;%lu;%lu;%lu;%lu\n", DateTime::getUtcNow().toString().c_str(),
					memStatus.numBytesFree, memStatus.numBlocksFree, memStatus.maxBlockSizeFree, memStatus.numBytesAlloc, memStatus.numBlocksAlloc, memStatus.maxBytesAlloc);
				fclose(fout);
			}
		}
	}

	void printHeader()
	{
		FILE* fout = fopen(m_filename.c_str(), "a+");
		if(fout != NULL)
		{
			fprintf(fout, "Time  ;Free bytes;Free blocks;max free block size;alloc bytes;alloc blocks;max alloc bytes\n");
			fclose(fout);
		}
	}

private:


	tstring m_filename;
};
#endif

tstring getAbsolutePath(const tstring& path);
tstring getAbsoluteDirectoryFromFile(const OTChar* filePath);
tstring getPathToSourceFolder(const tstring& binaryPath);
void pushCWD(const tstring& workingDir);
void popCWD();
void updateHostName(tstring& hostName);
void updateWindowTitle(const tstring& title);
void configureTrace(const CommandLineOptions& commandLineOptions);
// appends a "/" or "\" and the end of the path if not yet present
tstring appendMissingPathSeparator(const tstring& path);
PkiStoreConfiguration createApplicationPkiStoreConfig(const CommandLineOptions& commandLineOptions, const tstring& pathToPKIFolder);
std::vector<tstring> fillEndpointURLs(const CommandLineOptions& commandLineOptions, tstring& hostname);

// assigns all application settings (except endpoints), before and after Application::initialize(), the "Specific" settings are the settings that can be assigned by a config file as well
void assignCommonPreInitApplicationSettings(const CommandLineOptions& commandLineOptions, const tstring& pathToPKIFolder);
ApplicationDescription assignSpecificPreInitApplicationSettings(const CommandLineOptions& commandLineOptions, const tstring& pathToPKIFolder, const tstring& hostname, const std::vector<tstring>& discoveryUrls);
void assignCommonPostInitSettings(const CommandLineOptions& commandLineOptions, const tstring& pathToPKIFolder);
void assignSpecificPostInitSettings();

bool importNodes(const CommandLineOptions& commandLineOptions);
void exportNodes(const CommandLineOptions& commandLineOptions);

bool setInstanceCertificate(const CommandLineOptions& commandLineOptions, const tstring& pathToPKIFolder);
bool validateOwnCertificate();

tstring toExpandedSecurityPolicyUri(const tstring& securityPolicyUri);
std::vector<Server::EndpointPtr> addTestEndpoints(const CommandLineOptions& commandLineOptions, const std::vector<tstring>& endpointUrls, const tstring& pathToUserPKIFolder);
EnumStatusCode configureCommonEndpointSettings(Server::EndpointPtr endpoint, const CommandLineOptions& commandLineOptions, const tstring& pathToUserPKIFolder);
EnumStatusCode configureEccEndpointSettings(Server::EndpointPtr endpoint, const CommandLineOptions& commandLineOptions);


#ifdef TBC_OS_WINDOWS
/*! @brief A helper for setting the current dir to the startup path
That makes the app independent from the current directory in the debug settings
and the relative paths for certificates work always */
void setCurrentDirFromExePath(const OTChar* exePath);
#endif

// any EnumMessageSecurityMode except None is not recommended in future LDS which will not allow anything else (on local host only)
// actual LDS does not support None !!!
EnumMessageSecurityMode discoveryServerMessageSecurityMode = EnumMessageSecurityMode_SignAndEncrypt;
unsigned int discoveryServerRegistrationRefresh = 10 * 60 * 1000;

void testTrace();
void initApplicationDescription(ApplicationDescription& applicationDescription, const std::vector<tstring>& discoveryUrls, const tstring& hostName);

// shows the usage of the root menu of the user interface
void usage();
void runUserInterface(bool be_quiet);

// initializes all namespaces before the address space is built to ensure that all namespaces are at the expected index (especially the SDK namespace)
void initNamespaces();

void handleEndpointConsoleMenu(tstringstream& inputStream);
void handleEndpointSecurityConsoleMenu(tstringstream& inputStream, const std::vector<Server::EndpointPtr>& allEndpoints);
void handleEndpointUserConsoleMenu(tstringstream& inputStream, const std::vector<Server::EndpointPtr>& allEndpoints);
void handleGdsConsoleMenu(tstringstream& inputStream);

tstring getRealIp()
{
	tstring realIp;
#ifdef SODEVENV_LINUX
	struct ifaddrs* pIfAddrs = NULL;
	struct ifaddrs* ifa = NULL;

	if(getifaddrs(&pIfAddrs) == 0)
	{
		for(ifa = pIfAddrs; ifa != NULL; ifa = ifa->ifa_next)
		{
			char host[NI_MAXHOST];

			if(ifa->ifa_addr == NULL)
			{
				continue;
			}

			if(ifa->ifa_addr->sa_family != AF_INET)
			{
				continue;
			}

			if(getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
				host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) != 0)
			{
				continue;
			}

			if(strcmp("127.0.0.1", host) == 0)
			{
				continue;
			}

			realIp = host;
		}
		freeifaddrs(pIfAddrs);
	}
#endif

#if defined SOOS_WINDOWS && ! defined SOOS_WINDOWS_CE
	DWORD dwSize = 0;
	GetIpAddrTable(NULL, &dwSize, false);
	PMIB_IPADDRTABLE ifTable = (PMIB_IPADDRTABLE)malloc(dwSize);
	if(GetIpAddrTable(ifTable, &dwSize, false) == NO_ERROR)
	{
		for(DWORD i = 0; i < ifTable->dwNumEntries; i++)
		{
			OTChar szAddr[16];
			if((ifTable->table[i].wType & (MIB_IPADDR_DISCONNECTED | MIB_IPADDR_DELETED | MIB_IPADDR_TRANSIENT)) != 0)
			{
				// do not use these IP addresses
				continue;
			}
			if(ifTable->table[i].dwAddr == 0x0100007f)
			{
				// do not use 127.0.0.1 (localhost)
				continue;
			}
			_stprintf(szAddr, _T("%u.%u.%u.%u"),
				(ifTable->table[i].dwAddr & 0x000000FF),
				(ifTable->table[i].dwAddr & 0x0000FF00) >> 8,
				(ifTable->table[i].dwAddr & 0x00FF0000) >> 16,
				(ifTable->table[i].dwAddr & 0xFF000000) >> 24);
			realIp = szAddr;
			break;
		}
	}
	if(ifTable)
	{
		free(ifTable);
	}
#endif

#ifdef SODEVENV_VXWORKS
	// To extract the real IP we are highly dependent on exact VxWorks version / VSB package
	// in VxWorks 7 the IP_NET package is needed - which implies OPCEN_SSL (which may not be always included)
	// header files may exist - or not :-(
#if 0
#include <hostLib.h>
#include <inetLib.h>

	char hostname[MAXHOSTNAMELEN + 1];
	hostname[0] = '\0';
	if(0 == gethostname(hostname, MAXHOSTNAMELEN + 1))
	{
		int addr = hostGetByName(hostname);
		if(addr > 0)
		{
			struct in_addr ip_addr;
			ip_addr.s_addr = addr;
			realIp = inet_ntoa(ip_addr);
		}
	}
#endif
	// simply try local host - tests for restricted endpoints will fail!
	realIp = "127.0.0.1";
#endif

	if(realIp.empty())
	{
		_tprintf(_T("Failed to get real IP address\n"));
		realIp = _T("127.0.0.1");
	}
	return realIp;
}

void setServerStatus(EnumServerState status)
{
	VariablePtr variable;
	NodeId nodeId(EnumNumericNodeId_Server_ServerStatus_State);
	NodeId statusNode(EnumNumericNodeId_Server_ServerStatus);

	DataValue dataValue;
	DateTime now;
	Value val;
	BaseNodePtr node = AddressSpaceRoot::instance()->getNode(&nodeId);
	if(node.isNotNull() && node->isInstanceOfUaType(Statics::VariableTypeId_BaseVariableType, true))
	{
		variable = (Variable*)(BaseNode*)node;
	}

	if(variable.isNotNull())
	{
		val.setInt32(status);
		now.utcNow();
		dataValue.setServerTimestamp(now);
		dataValue.setSourceTimestamp(now);
		dataValue.setValue(val);
		variable->setDataValue(dataValue);
	}

	node = AddressSpaceRoot::instance()->getNode(&statusNode);
	if(node.isNotNull() && node->isInstanceOfUaType(Statics::VariableTypeId_BaseVariableType, true))
	{
		variable = (Variable*)(BaseNode*)node;
	}

	if(variable.isNotNull())
	{
		ServerStatusDataType statusData;
		if(StatusCode::isGood(variable->getDataValue()->getValue()->getServerStatusDataType(statusData)))
		{
			statusData.setState(status);
			val.setServerStatusDataType(&statusData);
			now.utcNow();
			dataValue.setServerTimestamp(now);
			dataValue.setSourceTimestamp(now);
			dataValue.setValue(val);
			variable->setDataValue(dataValue);
		}
	}
}

void setServerShutdownInfo(OTUInt32 timeTillShutdown, const ILocalizedText* shutdownReason)
{
	VariablePtr variable;
	NodeId secondsTillShutdownNode(EnumNumericNodeId_Server_ServerStatus_SecondsTillShutdown);
	NodeId shutdownReasonNode(EnumNumericNodeId_Server_ServerStatus_ShutdownReason);
	NodeId statusNode(EnumNumericNodeId_Server_ServerStatus);

	DateTime now;
	now.utcNow();
	DataValue dataValue;
	Value val;
	dataValue.setServerTimestamp(now);
	dataValue.setSourceTimestamp(now);
	dataValue.setStatusCode(EnumStatusCode_Good);

	BaseNodePtr node = AddressSpaceRoot::instance()->getNode(&secondsTillShutdownNode);
	if(node->isInstanceOfUaType(Statics::VariableTypeId_BaseVariableType, true))
	{
		variable = (Variable*)(BaseNode*)node;
	}
	if(variable.isNotNull())
	{
		val.setUInt32(timeTillShutdown);
		dataValue.setValue(val);
		variable->setDataValue(dataValue);
	}

	node = AddressSpaceRoot::instance()->getNode(&shutdownReasonNode);
	if(node->isInstanceOfUaType(Statics::VariableTypeId_BaseVariableType, true))
	{
		variable = (Variable*)(BaseNode*)node;
	}
	if(variable.isNotNull())
	{
		val.setLocalizedText(shutdownReason);
		dataValue.setValue(val);
		variable->setDataValue(dataValue);
	}

	node = AddressSpaceRoot::instance()->getNode(&statusNode);
	if(node.isNotNull() && node->isInstanceOfUaType(Statics::VariableTypeId_BaseVariableType, true))
	{
		variable = (Variable*)(BaseNode*)node;
	}

	if(variable.isNotNull())
	{
		ServerStatusDataType statusData;
		if(StatusCode::isGood(variable->getDataValue()->getValue()->getServerStatusDataType(statusData)))
		{
			statusData.setSecondsTillShutdown(timeTillShutdown);
			statusData.setShutdownReason(shutdownReason);
			val.setServerStatusDataType(&statusData);
			now.utcNow();
			dataValue.setServerTimestamp(now);
			dataValue.setSourceTimestamp(now);
			dataValue.setValue(val);
			variable->setDataValue(dataValue);

		}
	}
}

#ifdef _DEBUG
#ifdef TBC_OS_WINDOWS
int __cdecl allocHook(
							int						/* allocType */,
							void *					/* userData */,
							size_t nSize,
							int						/* blockType */,
							long lRequest,
							const unsigned char *	/* filename */,
							int						/* lineNumber */ )
{
	if(nSize == 124 && lRequest > 32740)
	{
		int dummyForAnyCode = 0;
		int dummy2 = dummyForAnyCode;
		dummyForAnyCode = dummy2;
	}
	return 1;
}

_CRT_ALLOC_HOOK g_OldHook = 0;
#endif
#endif	// _DEBUG

bool g_cttCompatible = false;

bool g_resetAddressSpace = false;

ApplicationModule* g_ServerModule;


class CttAlarmWorkItem : public Util_WorkItem
{
public:
	CttAlarmWorkItem(const std::vector<NodeId>& nodeIds)
	{
		m_nodeIds = nodeIds;
	}

	~CttAlarmWorkItem() {};

	virtual void execute()
	{
		for(size_t i = 0; i < m_nodeIds.size(); i++)
		{
			VariablePtr var = AddressSpaceRoot::instance()->getVariable(&m_nodeIds[i]);
			if(var)
			{
				OTDouble dblVal;
				DataValue dv;
				Value val = var->getDataValue()->getValue();
				if(StatusCode::isGood(val.getDouble(&dblVal)))
				{
					// switch between normal (10.0 .. 15.0) and high (15.0 .. 20.0)
					if(dblVal > 15.0)
					{
						dblVal = 12.0;
					}
					else
					{
						dblVal = 17.0;
					}
				}
				val.setDouble(dblVal);
				dv.setValue(val);
				var->setDataValue(dv);
			}
		}
	};

	private:
		CttAlarmWorkItem();
		std::vector<NodeId> m_nodeIds;
};

	class CttAlarmAction
		: public Util_ScheduledAction
	{
	public:
		CttAlarmAction(const std::vector<NodeId>& nodeIds)
		{
			m_nodeIds = nodeIds;
		};
		virtual ~CttAlarmAction() {};
		virtual void execute()
		{
			CttAlarmWorkItem* workItem = new CttAlarmWorkItem(m_nodeIds);
			if(!g_ServerModule->addWorkItem(workItem))
			{
				workItem->execute();
				delete workItem;
			}
		}

		virtual void cleanUp() {};
	private:
		CttAlarmAction();
		std::vector<NodeId> m_nodeIds;
	}
	;

#if defined(__VXWORKS__) && !defined(__RTP__)
extern "C" int TestServer_main(char* arg1, char* arg2, char* arg3, char* arg4, char* arg5, char* arg6, char* arg7, char* arg8, char* arg9)
{
	int argc = 1;
	char exeName[] = "StructoAutomoldServer";
	char* argv[] = {exeName, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, NULL};
	while(argv[argc]) argc++;
#else
int _tmain(int argc, OTChar* argv[])
{
#endif
restart_server:
	bool startServer = true;
	bool be_quiet = g_resetAddressSpace;
	tstring configFile;
	CttAlarmAction* pCttAction = NULL;
	tstring pathToSourceFolder = getPathToSourceFolder(getAbsoluteDirectoryFromFile(argv[0]));

	g_resetAddressSpace = false;

	TestServerCreator creator;

#if defined(__VXWORKS__) && defined(__RTP__)
	/* NOTE: stack overflow because gmtime_r() calls tzload().
	 * tzload() uses 8KB stack in vxWorks 6.8 and before
	 * but 40KB stack in vxWorks 6.9 !!!
	 * (in RTP runtime only, kernel don't do this).
	 * this does no longer fit into the
	 * default pthread stack size of 20KB.
	 * => therefore increase stack to 64KB. */
	pthread_attr_t vxworks_thread_attr;
	pthread_attr_init(&vxworks_thread_attr);
	pthread_attr_setstacksize(&vxworks_thread_attr, 0x10000);
	opcua_p_thread_attr = &vxworks_thread_attr;
#endif

	// memory leak debug code for windows
#ifdef TBC_OS_WINDOWS
#ifdef _DEBUG
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// to find mem leaks...
	g_OldHook = _CrtSetAllocHook(allocHook);

#endif // _DEBUG
#endif //TBC_OS_WINDOWS

#ifdef TBC_OS_WINDOWS
	// set current dir from startup path
	setCurrentDirFromExePath(argv[0]);
#endif

	tstring hostName = _T("localhost");
	updateHostName(hostName);
	if(!be_quiet)
	{
		_tprintf(_T("Host name: %s \r\n"), hostName.c_str());
	}
	_tprintf(_T("Host name: %s \r\n"), hostName.c_str());
	// init the toolbox with the appropriate creator
	if(loadToolbox(&creator) == EnumStatusCode_Good)
	{
		CommandLineOptions commandLineOptions;
		commandLineOptions.read(argc, argv);

		configFile = commandLineOptions.getConfigurationFilePath();

#ifdef CTT
		g_cttCompatible = true;
#endif
		if(commandLineOptions.getCttCompatible())
		{
			g_cttCompatible = true;
		}

		configureTrace(commandLineOptions);

		tstring pathToPKIFolder = pathToSourceFolder + _T("./PKI/test_server/");

		if(!commandLineOptions.getPathPKI().empty())
		{
			pathToPKIFolder = appendMissingPathSeparator(commandLineOptions.getPathPKI());
		}

        // Prosanta 
        pathToPKIFolder = _T("./PKI/test_server/");

		tstring pathToUserPKIFolder = pathToPKIFolder;	// by default, use the application store for the user certificates
#if CTT
		// CTT always required used PKI path at this place
		pathToUserPKIFolder = pathToSourceFolder + _T("./PKI/user_store/");
#else
		if(!commandLineOptions.getPathUserPKI().empty())
		{
			pathToUserPKIFolder = appendMissingPathSeparator(commandLineOptions.getPathUserPKI());
		}
#endif
        
		
         _tprintf(_T("--PKI store configuration Path--  = %s\n"), pathToPKIFolder.c_str());
		 
		 _tprintf(_T("--PKI store configuration Path--  = %s\n"), pathToUserPKIFolder.c_str());
		

		ApplicationPtr pApp = Application::instance();
		assignCommonPreInitApplicationSettings(commandLineOptions, pathToPKIFolder);
		// initialize the static RoleIds before Application::initialize(), those IDs must to be available
		// when the endpoint is initialized.
		// This happens when the Endpoint is created e.g. via config file during Application::initialize().
		ApplicationModule::initializeStatics();

		EnumStatusCode appInitResult = EnumStatusCode_Good;
		std::vector<tstring> endpointUrls;
		if(configFile.empty())
		{
			endpointUrls = fillEndpointURLs(commandLineOptions, hostName);
			_ASSERTION(!endpointUrls.empty(), _T("This point should not be reached without a URL"));
			ApplicationDescription applicationDescription = assignSpecificPreInitApplicationSettings(commandLineOptions, pathToPKIFolder, hostName, endpointUrls);

			appInitResult = pApp->initialize(&applicationDescription);

			if(!setInstanceCertificate(commandLineOptions, pathToPKIFolder))
			{
				startServer = false;
			}
		}
		else
		{
			std::vector<XmlElement> configExtensions;
			// set the current dir while loading config file
			pushCWD(pathToSourceFolder);
			Trace::instance()->enableStdOutLogging();
			// for testing 
			Trace::instance()->enableTraceLevel(EnumTraceGroup_All, EnumTraceLevel_WarningAndHigher);
			appInitResult = pApp->initialize(configFile, _T("pass"), &configExtensions);
			Trace::instance()->disableStdOutLogging();
			popCWD();

			// Configuration extensions not used by CPPTestServer, just report an info message if found
			for(std::vector<XmlElement>::iterator it = configExtensions.begin(); it != configExtensions.end(); ++it)
			{
				tstring message = tstring(_T("Ignored application specific extension: ")) + it->getStringFromUtf8();
				Trace::instance()->writeLine(EnumTraceLevel_Info, EnumTraceGroup_User1, message, _T("StructoAutomoldServer"));
			}
		}

		if(StatusCode::isGood(appInitResult))
		{
			if(!commandLineOptions.getSkipOwnCertValidation())
			{
				startServer = validateOwnCertificate();
			}

			// Verify the configured application type. We try to start a server so a configuration for client should not be allowed.
			EnumApplicationType appType = pApp->getDescription()->getApplicationType();
			if(appType != EnumApplicationType_Server &&
				appType != EnumApplicationType_Both)
			{
				appInitResult = EnumStatusCode_BadInvalidState;
				tstring message = tstring(_T("Server cannot start without a server application type (")) + tstring(getEnumStatusCodeString(appInitResult)) + tstring(_T(")\n"));
				Trace::instance()->writeLine(EnumTraceLevel_Info, EnumTraceGroup_User1, message, _T("StructoAutomoldServer"));
				_tprintf(_T("%s\n"), message.c_str());
				startServer = false;
			}
		}

		if(StatusCode::isGood(appInitResult))
		{
			assignCommonPostInitSettings(commandLineOptions, pathToPKIFolder);

			if(configFile.empty())
			{
				assignSpecificPostInitSettings();
			}

			// just make a test on the trace functions
			testTrace();
			// make a simple test on PKI store last update time, based on the certificate files, this is complex to test with UnitTests
			{
				DateTime updateTime = pApp->getPkiStoreLastUpdateTime();
				if(updateTime.isNull())
				{
					tstring message = _T("LastUpdateTime was not correctly calculated from file system");
					Trace::instance()->writeLine(EnumTraceLevel_Info, EnumTraceGroup_User1, message, _T("StructoAutomoldServer"));
					_tprintf(_T("%s\n"), message.c_str());
				}
			}

			if(importNodes(commandLineOptions))
			{
				// Diagnostics
				// Keep the configured or default enabling (disabled) and the default interval (1000) but allow clients to enable collection of diagnostic information on demand
				// Note: This can only be set when we have the standard address space, so this has to be done after the import step
				Diagnostics::instance()->setEnabledFlagWritable(true);
				Diagnostics::instance()->setUpdateIntervalWritable(true);
			}
			else
			{
				_tprintf(_T("Server Failed\n"));
				startServer = false;
			}

			// initialize all namespaces up front to have a defined order
			initNamespaces();
			_tprintf(_T("Namespace Init\n"));

			SimVarManager::init(1);	// use the application namespace 1 for simulation variables
			(void)DynamicNodeCache::instance();	// initialize the dynamic node cache
			/*ApplicationModule* */g_ServerModule = new ApplicationModule();

			// use either access pattern or role permissions to configure user access
			g_ServerModule->enableAccessPattern(commandLineOptions.getEnableAccessPattern());

			g_ServerModule->setSourceFolder(pathToSourceFolder);
			_tprintf(_T("Start Appln\n"));
			// start the application
			EnumStatusCode appStartResult = pApp->start();
			if(appStartResult == EnumStatusCode_Good)
			{
				std::vector<EndpointPtr> endpoints;
				// initialize and add the server's endpoints
				if(configFile.empty())
				{
					endpoints = addTestEndpoints(commandLineOptions, endpointUrls, pathToUserPKIFolder);
					_tprintf(_T("Add Endpoints\n"));
				}
				else
				{
					// Endpoints already added when the configuration file was loaded
					endpoints = Application::instance()->getEndpoints();
					_tprintf(_T("Add Endpoints Again\n"));
					for(size_t i = 0; i < endpoints.size(); i++)
					{
						EnumStatusCode configureResult = configureCommonEndpointSettings(endpoints[i], commandLineOptions, pathToUserPKIFolder);
						if(StatusCode::isBad(configureResult))
						{
							_tprintf(_T("Failed to configure endpoint '%s': %s\n"), endpoints[i]->getUrl().c_str(), getEnumStatusCodeString(configureResult));
						}
					}
				}

				if(endpoints.empty())
				{
					tstring message = tstring(_T("No endpoints defined, check 'BaseAddresses' entry in the configuration file"));
					Trace::instance()->writeLine(EnumTraceLevel_Info, EnumTraceGroup_User1, message, _T("StructoAutomoldServer"));
					_tprintf(_T("%s\n"), message.c_str());
					startServer = false;
				}
				else
				{
					// update the title of the console with the first (successfully added) endpoint
					updateWindowTitle(endpoints[0]->getUrl().c_str());
					_tprintf(_T("updateWindowTitle \n"));
				}

				// init the server internal modules (test / sim etc.)
				// also opens the endpoint
				if(startServer)
				{
					bool initSucceed = g_ServerModule->init(pApp->getDescription(), endpoints, commandLineOptions.getRedundancySupport(), commandLineOptions.getRedundantUrls(), commandLineOptions.getTestServerFilesPath());
					if(!initSucceed)
					{
						startServer = false;
					}
					if(commandLineOptions.getAchillesCompat() || g_cttCompatible)
					{
						g_ServerModule->setServiceDelay(EnumNumericNodeId_CloseSessionRequest, 0);
						g_ServerModule->setServiceDelay(EnumNumericNodeId_HistoryReadRequest, 0);
						g_ServerModule->setServiceDelay(EnumNumericNodeId_DeleteSubscriptionsRequest, 0);
						g_ServerModule->setServiceDelay(EnumNumericNodeId_CreateMonitoredItemsRequest, 0);
						g_ServerModule->setServiceDelay(EnumNumericNodeId_ModifyMonitoredItemsRequest, 0);
						g_ServerModule->setServiceDelay(EnumNumericNodeId_DeleteMonitoredItemsRequest, 0);

						// CTT 1.4.9.401 tries to invoke query (without anything - e.g. type nodes) and does not expect "BadNothingTODo"
						Application::instance()->setEnableServiceGroup(EnumServiceGroup_Query, false);
						_tprintf(_T("At g_cttCompatible\n"));
					}
					_tprintf(_T("g_ServerModule->init \n"));
				}

				if(commandLineOptions.getModelDesignerEnable())
				{
					_tprintf(_T("getModelDesignerEnable \n"));
					EnumModelDesignerStatus enableResult = AddressSpaceRoot::instance()->enableModeling(AddressSpaceRoot::instance()->getServerObject());
					_ASSERTION(enableResult == EnumModelDesignerStatus_Good, _T("Failed to enable the model designer"));
					_tprintf(_T("AddressSpaceRoot \n"));
					if(enableResult == EnumModelDesignerStatus_Good)
					{
						// add the namespace after the SDK namespace was added (the SDK namespace was added latest during enableModeling)
						AddressSpaceRoot::instance()->addNamespace(_T("http://softing.com/ModelDesignerTests"), NULL);
					}
					else
					{
						_tprintf(_T("Failed to enable model designer. Missing standard address space? \n"));
					}
				}
				if(startServer)
				{
					_tprintf(_T("EnumServerState_Running \n"));
					setServerStatus(EnumServerState_Running);
					if(!be_quiet)
					{
						_tprintf(_T("Server started ....\n\n"));
					}

					// if anything is to be exported, do it now after address space is built up
					exportNodes(commandLineOptions);
					if(g_cttCompatible)
					{
						_tprintf(_T("Console Namespace started ....\n\n"));
						// launch a cyclic action to report alarms
						OTUInt16 namespaceIndex;
						AddressSpaceRoot::instance()->addNamespace(_T("http://softing.com/OPCToolbox/Console"), &namespaceIndex);

						std::vector<NodeId> alarmVarNodeIds;
						alarmVarNodeIds.push_back(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.exclusiveLimitAlarmVar")));
						alarmVarNodeIds.push_back(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.exclusiveLevelAlarmVar")));
						alarmVarNodeIds.push_back(NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.OffTheShelfAlarm.nonExclusiveLimitAlarmVar")));
						pCttAction = new CttAlarmAction(alarmVarNodeIds);
						pCttAction->setCyclic(true);
						pCttAction->setTimeout(10000);
						pCttAction->setAutoDelete(false);
						g_ServerModule->addAction(pCttAction);
					}

					if(startServer)
					{
						runUserInterface(be_quiet);
					}

					if(pCttAction)
					{
						g_ServerModule->removeAction(pCttAction);
						delete pCttAction;
						pCttAction = NULL;
					}

					setServerStatus(EnumServerState_Shutdown);
					if(g_cttCompatible)
					{
						// wait a little bit until real shutdown
						LocalizedText shudownReason(_T("normal shutdown"), _T("en"));

						for(OTUInt32 timeTillShutdown=10; timeTillShutdown > 0; timeTillShutdown--)
						{
							setServerShutdownInfo(timeTillShutdown, &shudownReason);
							SLEEP(1000);
						}
					}
				}
				else
				{
					_tprintf(_T("initialisation failed .... shut down!\n"));
					SLEEP(2000);
				}
				// close the endpoints to avoid further incoming requests
				for(size_t i = 0; i < endpoints.size(); i++)
				{
					endpoints[i]->close();
				}

				// close all endpoints created by method calls
				endpoints = Application::instance()->getEndpoints();
				{
					for(size_t i = 0; i < endpoints.size(); i++)
					{
						// avoid error traces when trying to close endpoints already closed
						if(endpoints[i]->getIsOpen())
						{
							endpoints[i]->close();
						}
					}
				}

				SimVarManager::getInstance().stopAllSimulations();
				g_ServerModule->end();
				delete g_ServerModule;
				ApplicationModule::uninitializeStatics();
				SimVarManager::exit();

				pApp->stop();

#if TB5_ALARMS
				CustomCallbackEvent::uninitialize();
#endif
				DynamicNodeCache::deleteInstance();

			}	// if appStartResult
			else
			{
				_tprintf(_T("Failed to start the application: '%s'\n"), getEnumStatusCodeString(appStartResult));
			}
			pApp->uninitialize();
			Trace::instance()->stop();
			pApp = NULL;
		}	// if appInitResult
	}
	if(g_pPendingGdsRequest)
	{
		delete g_pPendingGdsRequest;
		g_pPendingGdsRequest = NULL;
	}
	unloadToolbox();

	if(g_resetAddressSpace && startServer)
	{
		_tprintf(_T("Reset address space ....\n"));
		goto restart_server;
	}
	return 0;
}

void initApplicationDescription(ApplicationDescription& applicationDescription, const std::vector<tstring>& discoveryUrls, const tstring& hostName)
{
	LocalizedText appName;
	appName.defineTranslation(_T("Softing OPC UA StructoAutomoldServer"), _T(""));
//	appName.defineTranslation(_T("Softing OPC UA StructoAutomoldServer (DE)"), _T("de"));

	applicationDescription.setApplicationName(appName);

	applicationDescription.setApplicationUri(buildAppUri(hostName, SERVER_APP_IDENTIFIER));
	applicationDescription.setProductUri(_T("urn:Softing/Products/OpcUa/StructoAutomoldServer"));

	// use all endpoint URLs as discovery URLs
	applicationDescription.setDiscoveryUrls(discoveryUrls);
}

#define LOG_ERROR(msg)\
	Trace::instance()->writeSourceLine(EnumTraceLevel_Error, EnumTraceGroup_User1, msg, _T(__FILE__), __LINE__)

void testTrace()
{
	LOG_ERROR(_T("Begin with Trace tests"));
	LOG_ERROR(_T("Error test message."));
	Trace::instance()->writeSourceLine(EnumTraceLevel_Fatal , EnumTraceGroup_User1, _T("Testing TraceMessage"), _T(__FILE__), __LINE__);
	Trace::instance()->writeLine(EnumTraceLevel_Error		, EnumTraceGroup_User1, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Warning		, EnumTraceGroup_User1, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Info		, EnumTraceGroup_User1, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Debug		, EnumTraceGroup_User1, _T("Testing TraceMessage"), _T("ObjectID"));

	Trace::instance()->writeLine(EnumTraceLevel_Fatal  , EnumTraceGroup_User2, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Error  , EnumTraceGroup_User2, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User2, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Info   , EnumTraceGroup_User2, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Debug  , EnumTraceGroup_User2, _T("Testing TraceMessage"), _T("ObjectID"));

	Trace::instance()->writeLine(EnumTraceLevel_Fatal  , EnumTraceGroup_User3, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Error  , EnumTraceGroup_User3, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User3, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Info   , EnumTraceGroup_User3, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Debug  , EnumTraceGroup_User3, _T("Testing TraceMessage"), _T("ObjectID"));

	Trace::instance()->writeLine(EnumTraceLevel_Fatal  , EnumTraceGroup_User4, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Error  , EnumTraceGroup_User4, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User4, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Info   , EnumTraceGroup_User4, _T("Testing TraceMessage"), _T("ObjectID"));
	Trace::instance()->writeLine(EnumTraceLevel_Debug  , EnumTraceGroup_User4, _T("Testing TraceMessage"), _T("ObjectID"));
	LOG_ERROR(_T("End of Trace tests"));
}

#ifdef TBC_OS_WINDOWS
void setCurrentDirFromExePath(const OTChar* exePath)
{
	size_t slen = _tcslen(exePath);
	while(slen > 0)
	{
		if(exePath[slen] == _T('\\') || exePath[slen] == _T('/'))
		{
			tstring currentDirString;
			currentDirString.assign(exePath, slen);
			SetCurrentDirectory(currentDirString.c_str());
			return;
		}
		slen--;
	}
}
#endif // TBC_OS_WINDOWS

tstring getAbsolutePath(const tstring& path)
{
	tstring result;
	if(!path.empty())
	{
#ifdef TBC_OS_WINDOWS
		OTChar* pAbsPath = NULL;
		DWORD length = GetFullPathName(path.c_str(), 0, NULL, NULL);
		pAbsPath = (TCHAR*)malloc(length * sizeof(TCHAR));
		if(pAbsPath)
		{
			if(GetFullPathName(path.c_str(), length, pAbsPath, NULL) == 0)
			{
				// Error occurred
				free(pAbsPath);
				pAbsPath = NULL;
			}
		}
		if(pAbsPath)
		{
			result = pAbsPath;
			free(pAbsPath);
		}
#endif
#ifdef SOOS_POSIX
#ifdef SODEVENV_VXWORKS
		// No implementation for VxWorks - use original path
		result = path;
#else
		OTChar* pAbsPath = NULL;
		pAbsPath = realpath(path.c_str(), NULL);
		if(pAbsPath)
		{
			result = pAbsPath;
			free(pAbsPath);
		}
#endif
#endif
	}
	return result;
}

tstring getAbsoluteDirectoryFromFile(const OTChar* filePath)
{
	tstring absPath = getAbsolutePath(filePath);
	// now remove binary name
	size_t pos = absPath.length();
	while(pos > 0)
	{
		pos--;
		if(absPath[pos] == _T('\\') || absPath[pos] == _T('/'))
		{
			absPath = absPath.substr(0, pos + 1);
			break;
		}
	}

	return absPath;
}

tstring getPathToSourceFolder(const tstring& binaryPath)
{
#ifdef SODEVENV_VXWORKS
	return _T("/sd0a/sdk");
#else
#ifdef SODEVENV_LINUX
	// Linux has one folder depth more
	return getAbsolutePath(binaryPath + ("/../../../../.."));
#else
	return getAbsolutePath(binaryPath + _T("/../../../.."));
#endif
#endif
}

static std::vector<tstring> workingDirs;

void pushCWD(const tstring& workingDir)
{

	tstring currentDir;
#ifdef TBC_OS_WINDOWS
	TCHAR currDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currDir);
	currentDir = currDir;
#endif
#ifdef SOOS_POSIX
	char buffer[PATH_MAX + 1];
	buffer[0] = '\0';
	char* pCurr = getcwd(buffer, PATH_MAX + 1);
	currentDir = pCurr;
#endif

	workingDirs.push_back(currentDir);

#ifdef TBC_OS_WINDOWS
	if(!SetCurrentDirectory(workingDir.c_str()))
#endif
#ifdef SOOS_POSIX
	if(chdir(workingDir.c_str()) != 0)
#endif
	{
		_tprintf(_T("cannot change to directory %s\n"), workingDir.c_str());
	}
}

void popCWD()
{
	if(workingDirs.size() > 0)
	{
		tstring currentDir = workingDirs.back();
		workingDirs.pop_back();
#ifdef TBC_OS_WINDOWS
		if(!SetCurrentDirectory(currentDir.c_str()))
#endif
#ifdef SOOS_POSIX
		if(chdir(currentDir.c_str()) != 0)
#endif
		{
			_tprintf(_T("cannot change to directory %s\n"), currentDir.c_str());
		}
	}

}

void updateHostName(tstring& hostName)
{
#ifdef _WIN32_WCE
	_UNUSED(hostName);
#endif
#ifdef TBC_OS_WINDOWS
	OTChar buf[256];
	DWORD size = sizeof(buf);
	if(GetComputerName(buf, &size))
	{
		hostName = buf;
	}
	else
	{
		_tprintf(_T("Unable to get local host name\r\n"));
	}
#endif
#if defined SODEVENV_LINUX
	OTChar buf[256];
	if(gethostname(buf, sizeof(buf)) == 0)
	{
		hostName = buf;
	}
	else
	{
		_tprintf(_T("Unable to get local host name\r\n"));
	}
#endif
#if defined SODEVENV_VXWORKS
	/*VxWorks 6.9 seem to have a different interface (or header) */
//#include <unistd.h>
	_UNUSED(hostName);
#endif

#ifdef CTT
	hostName = getRealIp();
#endif
}

void updateWindowTitle(const tstring& title)
{
#ifdef _WIN32_WCE
	_UNUSED(title);
#endif
#ifdef TBC_OS_WINDOWS
	OTChar sysCommand[1024];
#if _MSC_VER >= 1400
	_stprintf_s(sysCommand, _T("title \"CPPConsoleTestServer: %s\""), title.c_str());
#else
	_stprintf(sysCommand, _T("title \"CPPConsoleTestServer: %s\""), title.c_str());
#endif
	::_tsystem(sysCommand);
#endif
#ifdef SODEVENV_LINUX
	if(isatty(1) && !strcmp(getenv("TERM")?:"","xterm"))
	{
		_tprintf(_T("\033]0;CPPTestServer: %s\007"), title.c_str());
	}
#endif
}

void configureTrace(const CommandLineOptions& commandLineOptions)
{
	if(!commandLineOptions.getTraceFile().empty())
	{
		tstring traceFile = commandLineOptions.getTraceFile();
		Trace::instance()->enableFileLogging(traceFile, 10 * 1024 * 1024, 5, 100);
	}
	// trace options are set via command line
	if(commandLineOptions.getTraceEnabled())
	{
		Trace::instance()->enableTraceLevel(commandLineOptions.getTraceGroup(), commandLineOptions.getTraceLevel());
		if(commandLineOptions.getCallbackTraceFile().length()>0)
		{
			const tstring & cbTraceFile = commandLineOptions.getCallbackTraceFile();
			MyTrace::setTraceFile(cbTraceFile);
			Trace::instance()->enableCallbackLogging(&MyTrace::onLogMessage);
		}
	}
}

tstring appendMissingPathSeparator(const tstring& path)
{
	if((!path.empty()) &&
		(path.c_str()[path.length() - 1] != _T('/')) &&
		(path.c_str()[path.length() - 1] != _T('\\')))
	{
		return path + DIR_SEPARATOR;
	}
	else
	{
		return path;
	}
}

PkiStoreConfiguration createApplicationPkiStoreConfig(const CommandLineOptions& commandLineOptions, const tstring& pathToPKIFolder)
{
	PkiStoreConfiguration storeConfiguration;
	bool hasAnyIndividualPath = false;
	// TrustList
	tstring certificateTrustListLocation	= pathToPKIFolder + _T("trusted") + DIR_SEPARATOR;
	if(!commandLineOptions.getTrustListLocation().empty())
	{
		certificateTrustListLocation		= commandLineOptions.getTrustListLocation();
		hasAnyIndividualPath = true;
	}

	// IssuerList
	tstring issuerCertificateListLocation	= pathToPKIFolder + _T("issuer") + DIR_SEPARATOR;
	if(!commandLineOptions.getIssuerCertificateListLocation().empty())
	{
		issuerCertificateListLocation		= commandLineOptions.getIssuerCertificateListLocation();
		hasAnyIndividualPath = true;
	}

	// RevocationList
	tstring certificateRevocationListLocation = pathToPKIFolder + _T("trusted_crls") + DIR_SEPARATOR;
	if(!commandLineOptions.getRevocationListLocation().empty())
	{
		certificateRevocationListLocation	= commandLineOptions.getRevocationListLocation();
		hasAnyIndividualPath = true;
	}

	// Rejected Certificates
#ifdef SODEVENV_VXWORKS
	tstring rejectedCertificatesLocation	= _T("");
#else
	tstring rejectedCertificatesLocation	= pathToPKIFolder + _T("rejected");
#endif
	if(!commandLineOptions.getRejectedCertificatePath().empty())
	{
		rejectedCertificatesLocation		= commandLineOptions.getRejectedCertificatePath();
		hasAnyIndividualPath = true;
	}

	if(hasAnyIndividualPath)
	{
		// use the individual paths only when explicitly required
		storeConfiguration.setCertificateTrustListLocation(certificateTrustListLocation);
		storeConfiguration.setIssuerCertificateListLocation(issuerCertificateListLocation);
		storeConfiguration.setCertificateRevocationListLocation(certificateRevocationListLocation);
		storeConfiguration.setRejectedCertificateLocation(rejectedCertificatesLocation);
	}
	else
	{
		// otherwise use the base path
		storeConfiguration.setBasePath(pathToPKIFolder);
	}

	// Validation options
	EnumCertificateCheck certificateCheckOption	= commandLineOptions.getCertificateCheckOption();
	EnumCrlCheckOption crlCheckOption			= commandLineOptions.getCrlCheckOption();
	bool allowProxyCerts						= commandLineOptions.getAllowProxyCerts();
	bool checkSelfSignedSignature				= commandLineOptions.getCheckSelfSignedSignature();
	bool checkStrictKeyUsage = false;	// not directly settable from command line
	bool checkUrlMatch = false;			// not directly settable from command line

	// override due to CTT option
	if(g_cttCompatible)
	{
		// CTT requires these checks and options
		crlCheckOption = EnumCrlCheckOption_AllExceptSelfSigned;
		checkSelfSignedSignature = true;
		checkStrictKeyUsage = true;
		checkUrlMatch = true;
	}

	storeConfiguration.setSuppressedCertificateChecks(certificateCheckOption);
	storeConfiguration.setRevocationCheckOptions(crlCheckOption);
	storeConfiguration.allowProxyCertificates(allowProxyCerts);
	storeConfiguration.checkSelfSignedSignature(checkSelfSignedSignature);
	storeConfiguration.setEnableStrictKeyUsageCheck(checkStrictKeyUsage);
	storeConfiguration.checkUrlMatch(checkUrlMatch);

	return storeConfiguration;
}

std::vector<tstring> fillEndpointURLs(const CommandLineOptions& commandLineOptions, tstring& hostname)
{
	std::vector<tstring> endpointUrls;
	// use the host name of the first valid endpoint for the application description
	tstring firstHostName;
#ifdef CTT
	tstring realIp = getRealIp();
	_UNUSED(commandLineOptions);
	_UNUSED(hostname);
	endpointUrls.push_back(_T("opc.tcp://"));
	endpointUrls.back().append(realIp);
	endpointUrls.back().append(_T(":4880/Softing/OpcUa/StructoAutomoldServer"));
#else
	for(size_t i = 0; i < commandLineOptions.getEndPointURLs().size(); i++)
	{
		tstring currentUrl = commandLineOptions.getEndPointURLs()[i];
		tstring transport;
		OTUInt16 port = 0;
		tstring urlPath;
		// store only the endpointURLs, that can be parsed
		// retrieve the host name only once
		if(Application::parseEndpointURL(currentUrl, transport, hostname, port, urlPath))
		{
			if(firstHostName.empty())
			{
				firstHostName = hostname;
			}
			endpointUrls.push_back(currentUrl);
		}
		else
		{
			_tprintf(_T("Error in parsing URL %s\n"), currentUrl.c_str());
		}
	}
	// use the hostname of the first valid endpoint
	if(!firstHostName.empty())
	{
		hostname = firstHostName;
	}

	// no endpoint or no valid endpoints from command line
	if(endpointUrls.empty())
	{
		// use a default endpoint
		if(commandLineOptions.getAchillesCompat())
		{
			endpointUrls.push_back(_T("opc.tcp://") + hostname + _T(":4840/") + SERVER_APP_IDENTIFIER);
		}
		else
		{
			endpointUrls.push_back(_T("opc.tcp://") + hostname + _T(":4880/") + SERVER_APP_IDENTIFIER);
		}

		_tprintf(_T("---End pint URL %s---\n"),endpointUrls[0].c_str());

	}
#endif
	return endpointUrls;
}

void assignCommonPreInitApplicationSettings(const CommandLineOptions& commandLineOptions, const tstring& pathToPKIFolder)
{
	ApplicationPtr pApp = Application::instance();
	// License
	// pApp->activateLicense(EnumFeature_Server, _T("XXXXX-XXXXX-XXXXX-XXXXX-XXXXX"));
	commandLineOptions.loadLicense(pApp);

	// DHParam File
	tstring applicationDhParamFile            = pathToPKIFolder + _T("own") + DIR_SEPARATOR + _T("dhparam.pem");
	// use it if it exists
	if(!FILE_OR_DIRECTORY_EXISTS(applicationDhParamFile.c_str()))
	{
		applicationDhParamFile = _T("");
	}
	if(!commandLineOptions.getDhParamFile().empty())
	{
		applicationDhParamFile            = commandLineOptions.getDhParamFile();
	}
	if(!applicationDhParamFile.empty())
	{
		if(StatusCode::isBad(pApp->setDhParamFileLocation(applicationDhParamFile)))
		{
			_tprintf(_T("Failed to set the DH param file location\n"));
		}
	}

	// Audit events
	// we enable audit events
	pApp->setEnableAuditEvents(true);

	// Demo time
	if(commandLineOptions.getDemoTime() != 0)
	{
		OTUInt32 demoTime = commandLineOptions.getDemoTime();
		pApp->setDemoTime(demoTime);
		if(pApp->getDemoTime() != demoTime)
		{
			_tprintf(_T("The demo time cannot be set to %u ms\n"), demoTime);
		}
	}

	// Preferred Locales (not settable via config file for the server, unless the application is Client and Server, the config file can overwrite this)
	// set the locales(s) which are supported by this server
	{
		vector<tstring> preferredLocales;
		preferredLocales.push_back(_T("en"));
		preferredLocales.push_back(_T("de"));
		// if configured via config file, let config file set the configuration
		pApp->setPreferredLocales(preferredLocales);
	}

	// Suppress address space creation
	// the nodeset files will be loaded after Application::initialize()
	if(!commandLineOptions.getNodeSet2XMLFiles().empty())
	{
		pApp->suppressAddresspaceCreation(true);
	}

	// SendOwnCertificateAsChain
	// overwrite only if the new value is expected to change the default value (false),
	// this way we are able to test the default setting
	if(commandLineOptions.getSendOwnCertAsChain())
	{
		_ASSERTION(!pApp->getSendOwnCertificateAsChain(), _T("The default of SendOwnCertificateAsChain is expected to be disabled"));
		pApp->setSendOwnCertificateAsChain(true);
	}
}

ApplicationDescription assignSpecificPreInitApplicationSettings(const CommandLineOptions& commandLineOptions, const tstring& pathToPKIFolder, const tstring& hostname, const std::vector<tstring>& discoveryUrls)
{
	ApplicationPtr pApp = Application::instance();

    _tprintf(_T("PKI store configuration Path  = %s \n"), pathToPKIFolder.c_str());

	// PkiStore
	PkiStoreConfiguration storeConfiguration = createApplicationPkiStoreConfig(commandLineOptions, pathToPKIFolder);
	if(StatusCode::isBad(pApp->setPkiStoreConfiguration(&storeConfiguration)))
	{
		_tprintf(_T("Failed to set the PKI store configuration\n"));
	}

	// ApplicationDescription
	ApplicationDescription applicationDescription;
	// ApplicationType
	if((commandLineOptions.getRedundancySupport() == EnumRedundancySupport_None) &&
		commandLineOptions.getGdsUrl().empty()) // for GDS we need client functionality for explicite unregister
	{
		applicationDescription.setApplicationType(EnumApplicationType_Server);
	}
	else
	{
		// our test server implements redundancy link as OPCUA client - then both is needed
		applicationDescription.setApplicationType(EnumApplicationType_Both);
	}
	initApplicationDescription(applicationDescription, discoveryUrls, hostname);

	// Server Information
	pApp->setManufacturerName(_T("Softing Industrial Automation GmbH, D-85540 Haar/Munich, http://industrial.softing.com"));
	pApp->setMajorVersion(TB_MAJOR_VERSION);
	pApp->setMinorVersion(TB_MINOR_VERSION);
	pApp->setPatchVersion(TB_PATCH_VERSION);
	pApp->setBuildNumber(TB_BUILD_VERSION);

	{
		DateTime buildDate;
		const char *date_time = __DATE__ "/" __TIME__;
		char month[10];
		int year, mon, day, hour, min, sec;

		if (sscanf(date_time, "%s %d %d/%d:%d:%d", month, &day, &year, &hour, &min, &sec) == 6) 
		{

			if (!strcmp(month, "Jan"))
				mon = 1;
			else if (!strcmp(month, "Feb"))
				mon = 2;
			else if (!strcmp(month, "Mar"))
				mon = 3;
			else if (!strcmp(month, "Apr"))
				mon = 4;
			else if (!strcmp(month, "May"))
				mon = 5;
			else if (!strcmp(month, "Jun"))
				mon = 6;
			else if (!strcmp(month, "Jul"))
				mon = 7;
			else if (!strcmp(month, "Aug"))
				mon = 8;
			else if (!strcmp(month, "Sep"))
				mon = 9;
			else if (!strcmp(month, "Oct"))
				mon = 10;
			else if (!strcmp(month, "Nov"))
				mon = 11;
			else if (!strcmp(month, "Dec"))
				mon = 12;
			else 
			{
				mon = 0;
			}
			if(mon != 0)
			{
				OTChar szDateTime[30];
				_stprintf(szDateTime, _T("%04u-%02u-%02uT%02u:%02u:%02uZ"), year, mon, day, hour, min, sec);
				// parse succeeded
				buildDate.parse(szDateTime);
				pApp->setBuildDate(buildDate);
			}
		}
	}
	return applicationDescription;
}

void assignCommonPostInitSettings(const CommandLineOptions& commandLineOptions, const tstring& pathToPKIFolder)
{
	ApplicationPtr pApp = Application::instance();

	// Discovery Server configuration
	// discovery server certificate is not needed if localhost / security policy none / message security none
	tstring discoveryServerCertificate        = pathToPKIFolder + _T("..") + DIR_SEPARATOR + _T("all_certificates") + DIR_SEPARATOR + _T("certs") + DIR_SEPARATOR + _T("cert_discovery_server.der");
	// URL for discovery registration shall be localhost
	// Future implementations of LDS will allow server registration on this url only.
	// Security profile for discovery registration shall be none in future LDS implementations
	// All other profiles will be removed in future LDS implementations
	// the actual implementation requires security profile  !!!
	tstring discoveryServerUrl = _T("opc.tcp://localhost:4840/UADiscovery");
	tstring discoveryServerSecurityProfile	= SecurityPolicyUri_Basic128Rsa15;
	bool registerAtDiscovery = commandLineOptions.useDiscovery();
#ifdef CTT
	registerAtDiscovery = true;
#endif
	if(registerAtDiscovery)
	{
		if(!commandLineOptions.getDiscoveryCertificate().empty())
		{
			discoveryServerCertificate = commandLineOptions.getDiscoveryCertificate();
		}
		if(!commandLineOptions.getDiscoveryServerURL().empty())
		{
			discoveryServerUrl = commandLineOptions.getDiscoveryServerURL();
		}

		// register at the discovery service
		pApp->loadDiscoveryServerCertificate(discoveryServerCertificate);

		// define server capabilities
		// (see: http://www.opcfoundation.org/UA/schemas/1.04/ServerCapabilities.csv)
		std::vector<tstring> serverCapabilities;
		serverCapabilities.push_back(_T("DA"));		// Provides current data.
		serverCapabilities.push_back(_T("HD"));		// Provides historical data.

		pApp->enableServerRegistration(
			discoveryServerUrl,
			discoveryServerSecurityProfile,
			discoveryServerMessageSecurityMode,
			discoveryServerRegistrationRefresh,
			serverCapabilities);
	}

	// CTT options for post init
	if(g_cttCompatible)
	{
		pApp->setMinPublishingInterval(50); // as configured in CTT project
		pApp->setMaxPipedPublishRequests(10);
		AddressSpaceRoot::instance()->setOperationLimit(EnumServiceType_CreateMonitoredItems, 10000);
		AddressSpaceRoot::instance()->setOperationLimit(EnumServiceType_RegisterNodes, 100);
	}

	// configure authentication watcher to reject multiple incorrect user authentication
	OTUInt32 maxFailedAttempts = 0;
	OTUInt32 checkInterval = 0;
	OTUInt32 lockInterval = 0;
	if(g_cttCompatible && !commandLineOptions.getDisableLockout())
	{
		maxFailedAttempts = 3;
		checkInterval = 30000;
		lockInterval = 300000;
	}

	_UNUSED(maxFailedAttempts);
	_UNUSED(checkInterval);
	_UNUSED(lockInterval);
	(void)Application::instance()->configureRepeatedConnectAttempts(maxFailedAttempts, checkInterval, lockInterval);

	// operation limits
	std::map<EnumServiceType, OTUInt32> operationLimits = commandLineOptions.getOperationLimits();
	for(std::map<EnumServiceType, OTUInt32>::const_iterator it = operationLimits.begin(); it != operationLimits.end(); ++it)
	{
		EnumStatusCode operationLimitResult = AddressSpaceRoot::instance()->setOperationLimit(it->first, it->second);
		if(StatusCode::isBad(operationLimitResult))
		{
			_tprintf(_T("Failed to set OperationLimit for %s to %u: %s"), getEnumServiceTypeString(it->first), it->second, getEnumStatusCodeString(operationLimitResult));
		}
	}

	// GDS settings
	// don't try the GDS config if we have individual paths set (we need the PKI store via base path)
	if(commandLineOptions.getTrustListLocation().empty() &&
		commandLineOptions.getIssuerCertificateListLocation().empty() &&
		commandLineOptions.getRevocationListLocation().empty() &&
		commandLineOptions.getRejectedCertificatePath().empty())
	{
		// we could initialize the GDS settings before Application::initialize(), but we need the PKI store to be set before
		// As for config file the PKI store will be assigned during Application::initialize(), so we need to do this after the initialization
		// thus we have to test for the automatic registration during Application::start()
		tstring gdsUrl = commandLineOptions.getGdsUrl();
		tstring gdsUser = commandLineOptions.getGdsUser();
		tstring gdsPassword = commandLineOptions.getGdsPassword();
		OTUInt32 gdsUpdateCycle = commandLineOptions.getGdsUpdateCycle();	// the default is 0
		UserIdentityToken gdsToken;
		UserNameIdentityToken userNameToken;
		userNameToken.setUserName(gdsUser);
		userNameToken.setPassword(ByteString(gdsPassword));
		gdsToken.setUserNameIdentityToken(&userNameToken);
		{
			EnumStatusCode gdsConfigStatus = pApp->gdsSetConfiguration(gdsUrl, gdsToken, SecurityPolicyUri_Basic256Sha256, gdsUpdateCycle);
			if(StatusCode::isBad(gdsConfigStatus))
			{
				_tprintf(_T("gdsSetConfiguration failed with status %s\n"), getEnumStatusCodeString(gdsConfigStatus));
			}
		}
	}
}

void assignSpecificPostInitSettings()
{
	ApplicationPtr pApp = Application::instance();

	// ServerProfiles
	// specific facets supported by server
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Transport/https-uabinary"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/StandardUA2017"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/StandardDataChangeSubscription2017"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/StandardEventSubscription"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/AddressSpaceNotifier"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/ACExclusiveAlarming "));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/ACNon-ExclusiveAlarming"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/HistoricalRawData"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/AggregateHistorical"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/BaseHistoricalEvent"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/FileAccess"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/ComplexTypes2017"));
	pApp->addServerProfileURI(_T("http://opcfoundation.org/UA-Profile/Server/Auditing"));
}

bool importNodes(const CommandLineOptions& commandLineOptions)
{
	// Import of Nodeset Files
	// configure the handling for duplicate imported nodes
	static_pointer_cast<TestServerAddressSpaceRoot>(Server::AddressSpaceRoot::instance())->setImportDuplicateNodeHandling(commandLineOptions.getImportDuplicateNodeHandling());
	std::vector<tstring> nodeSet2XMLFiles = commandLineOptions.getNodeSet2XMLFiles();
	for(std::vector<tstring>::iterator it = nodeSet2XMLFiles.begin(); it != nodeSet2XMLFiles.end(); ++it)
	{
		DateTime now = DateTime::getUtcNow();
		std::vector<XmlElement> extensions;
		tstring nodeSetVersion;
		DateTime lastModified;
		_tprintf(_T("start import of %s\n\n"), it->c_str());
		EnumModelDesignerStatus importNodesResult = AddressSpaceRoot::instance()->importNodes(*it, EnumApiObjectCreation_AllExceptDefaultNodes, extensions, nodeSetVersion, lastModified);
		_UNUSED(nodeSetVersion);
		_UNUSED(lastModified);
		if(importNodesResult != EnumModelDesignerStatus_Good)
		{
			_tprintf(_T("ERROR: Failed to import address space from NodeSet2 XML file %s (%s)\n"), (*it).c_str(), getEnumModelDesignerStatusString(importNodesResult));
		}
		else
		{
			if(!extensions.empty())
			{
				_tprintf(_T("Global Extensions within NodeSet:\n"));
				for(size_t i = 0; i < extensions.size(); i++)
				{
					_tprintf(_T("%s\n"), extensions[i].toString().c_str());
				}
			}
		}
		_tprintf(_T("import of %s finished after \n%f seconds\n"), it->c_str(), ((double)DateTime::getDurationMilliseconds(now, DateTime::getUtcNow())/1000.0));
	}
	// and validate the imported nodesets
	if(commandLineOptions.getValidateAddressSpace())
	{
		DateTime now = DateTime::getUtcNow();
		_tprintf(_T("start validation at %s\n"), now.toString().c_str());
		AddressSpaceRoot::instance()->validate();
		_tprintf(_T("validation finished after %f seconds\n"), ((double)DateTime::getDurationMilliseconds(now, DateTime::getUtcNow())/1000.0));
	}

	// Node Values of the Server object
	setServerStatus(EnumServerState_NoConfiguration);
	AddressSpaceRoot::instance()->setServiceLevel(255); // server has very good health - service level can change in case of redundant servers

	return true;
}

void exportNodes(const CommandLineOptions& commandLineOptions)
{
	if(!commandLineOptions.getExportFile().empty())
	{
		std::vector<tstring> exportNamespaces = commandLineOptions.getExportNamespaces();
		EnumModelDesignerStatus exportNodesResult = AddressSpaceRoot::instance()->exportNodes(exportNamespaces, commandLineOptions.getExportFile());
		if(exportNodesResult != EnumModelDesignerStatus_Good)
		{
			_tprintf(_T("ERROR: Failed to export address space (%s)\n"), getEnumModelDesignerStatusString(exportNodesResult));
		}
		else
		{
			_tprintf(_T("Successfully export address space.\n"));
		}
	}
}

bool setInstanceCertificate(const CommandLineOptions& commandLineOptions, const tstring& pathToPKIFolder)
{
	bool startServer = true;
	// Instance Certificate + PrivateKey
	tstring applicationCertificate = pathToPKIFolder + _T("own") + DIR_SEPARATOR;
	tstring applicationprivateKey  = pathToPKIFolder + _T("own") + DIR_SEPARATOR;
	if(commandLineOptions.getEnableECC() == 2)
	{
		applicationCertificate += _T("cert_server_self_signed_ed.der");
		applicationprivateKey  += _T("private_key_server_self_signed_ed.pem");
	}
	else if(commandLineOptions.getEnableECC())
	{
		applicationCertificate += _T("cert_server_self_signed_ec.der");
		applicationprivateKey  += _T("private_key_server_self_signed_ec.pem");
	}
	else
	{
		applicationCertificate += _T("cert_server_self_signed.der");
		applicationprivateKey  += _T("private_key_server_self_signed.pem");
	}
	if(!commandLineOptions.getAppCertificate().empty())
	{
		applicationCertificate = commandLineOptions.getAppCertificate();
	}
	if(!commandLineOptions.getAppPrivateKey().empty())
	{
		applicationprivateKey = commandLineOptions.getAppPrivateKey();
	}


	// PrivateKey password
	// default value "pass" is within command line options! Empty password is valid and passed by /appprivatekeypass ""
	tstring applicationprivateKeyPwd          = commandLineOptions.getAppPrivateKeyPass();

	if(StatusCode::isBad(Application::instance()->setInstanceCertificate(applicationCertificate, applicationprivateKey, applicationprivateKeyPwd)))
	{
		_tprintf(_T("Failed to set the application instance certificate\n"));
		startServer = false;
	}
	
	// just for tests...
	/*EnumStatusCode rv = pApp->createSelfSignedCertificate(SecurityPolicyUri_Aes256Sha384nistP384,
		pathToPKIFolder + _T("own") + DIR_SEPARATOR + _T("selfsigned_test.der"),
		pathToPKIFolder + _T("own") + DIR_SEPARATOR + _T("selfsigned_private.pem"), _T(""),
		14 * 24 * 3600,
		_T("Softing UA C++ SDK StructoAutomoldServer"), _T("Softing"), _T("IP:127.0.0.1, DNS:localhost"));
	printf("pApp->createSelfSignedCertificate -> %x\n", rv);*/

	return startServer;
}

bool validateOwnCertificate()
{
	ApplicationPtr pApp = Application::instance();
	// validate own certificate
	bool startServer = true;
	ByteString ownCertificate = pApp->getInstanceCertificateData();
	tstring ownAppUri = pApp->getDescription()->getApplicationUri();

	PkiStoreConfiguration ownCertCheckConfiguration = pApp->getPkiStoreConfiguration();
	tstring ownCertDirectory = getAbsoluteDirectoryFromFile(pApp->getInstanceCertificate().c_str());

	if(!ownCertCheckConfiguration.getBasePath().empty())
	{
		// all paths are already generated from the base path, copy an reinsert all paths after they were cleared
		// the trust list and rejected folders are modified in any case, the issuer crls cannot be set manually
		tstring issuerList = ownCertCheckConfiguration.getIssuerCertificateListLocation();
		tstring trustedCrls = ownCertCheckConfiguration.getCertificateRevocationListLocation();
		// remove base path configuration, otherwise setting of trusted folder wont work, this will clear all individual paths
		ownCertCheckConfiguration.setBasePath(_T(""));
		ownCertCheckConfiguration.setIssuerCertificateListLocation(issuerList);
		ownCertCheckConfiguration.setCertificateRevocationListLocation(trustedCrls);
	}
	// don't store the own certificate in the rejected folder in case of failed validation
	ownCertCheckConfiguration.setRejectedCertificateLocation(_T(""));
	// to treat the own as trusted, take the directory of the own certificate as the trust list
	ownCertCheckConfiguration.setCertificateTrustListLocation(ownCertDirectory);

	EnumStatusCode validationResult = pApp->validateCertificate(&ownCertCheckConfiguration, &ownCertificate, &ownAppUri);
	if(StatusCode::isBad(validationResult))
	{
		tstring message = tstring(_T("Validation of own certificate failed, reason: ")) + tstring(getEnumStatusCodeString(validationResult));
		Trace::instance()->writeLine(EnumTraceLevel_Warning,
			EnumTraceGroup_User1,
			message,
			_T("StructoAutomoldServer"));
		_tprintf(_T("%s\n"), message.c_str());

		startServer = false;
	}
	return startServer;
}

tstring toExpandedSecurityPolicyUri(const tstring& securityPolicyUri)
{
	tstring result;
	if(securityPolicyUri.empty())
	{
		return securityPolicyUri;
	}
	// append the security policy prefixes if they don't exist to allow to specify only the significant last part
	const tstring SECURITYPOLICY_PREFIX = _T("http://opcfoundation.org/UA/SecurityPolicy#");
	if(securityPolicyUri.substr(0, SECURITYPOLICY_PREFIX.length()).compare(SECURITYPOLICY_PREFIX) != 0)
	{
		result = SECURITYPOLICY_PREFIX + securityPolicyUri;
	}
	else
	{
		result = securityPolicyUri;
	}
	return result;
}

std::vector<Server::EndpointPtr> addTestEndpoints(const CommandLineOptions& commandLineOptions, const std::vector<tstring>& endpointUrls, const tstring& pathToUserPKIFolder)
{
	std::vector<Server::EndpointPtr> endpoints;
	for(size_t i = 0; i < endpointUrls.size(); i++)
	{
		EnumStatusCode result = EnumStatusCode_Good;
		EndpointPtr endpoint = new TestEndpoint();
		endpoint->setUrl(endpointUrls[i]);

		result = configureCommonEndpointSettings(endpoint, commandLineOptions, pathToUserPKIFolder);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to configure endpoint '%s': %s\n"), endpoint->getUrl().c_str(), getEnumStatusCodeString(result));
			continue;
		}

		if(StatusCode::isGood(result))
		{
			result = CreateEndpointMethod::setDefaultSecurityConfigurations(endpoint);
		}
		if(StatusCode::isGood(result))
		{
			result = CreateEndpointMethod::setDefaultUserTokenPolicies(endpoint);
		}

		if(StatusCode::isGood(result))
		{
			if(commandLineOptions.getAchillesCompat())
			{
				UserTokenPolicy userTokenPolicy;
				userTokenPolicy.setPolicyId(_T("0"));
				userTokenPolicy.setTokenType(EnumUserTokenType_Anonymous);
				userTokenPolicy.setSecurityPolicyUri(SecurityPolicyUri_None);
				if((result = endpoint->addUserTokenPolicy(&userTokenPolicy)) != EnumStatusCode_Good)
				{
					printf("ERROR: failed to set anonymous policy\r\n");
				}
			}
		}

		if(StatusCode::isGood(result))
		{
			result = configureEccEndpointSettings(endpoint, commandLineOptions);
		}

		if(StatusCode::isGood(result))
		{
			result = Application::instance()->addEndpoint(endpoint);
			endpoints.push_back(endpoint);
		}
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to add endpoint '%s': %s\n"), endpointUrls[i].c_str(), getEnumStatusCodeString(result));
		}
	}
	return endpoints;
}

EnumStatusCode configureCommonEndpointSettings(Server::EndpointPtr endpoint, const CommandLineOptions& commandLineOptions, const tstring& pathToUserPKIFolder)
{
	// User PKI Store
	EnumStatusCode result = EnumStatusCode_Good;
	// Configure user PKI store and other application specific options on the endpoints
	if(!pathToUserPKIFolder.empty())
	{
		// currently user PKI do neither have CRL nor issuer, trusted certificates are stored at "certs"
		PkiStoreConfiguration userPkiConf;
		tstring trustedLocation = pathToUserPKIFolder + _T("trusted") + DIR_SEPARATOR;
		if(FILE_OR_DIRECTORY_EXISTS(trustedLocation.c_str()))
		{
			userPkiConf.setCertificateTrustListLocation(trustedLocation);
		}

		tstring crlLocation = pathToUserPKIFolder + _T("trusted_crls") + DIR_SEPARATOR;
		if(FILE_OR_DIRECTORY_EXISTS(crlLocation.c_str()))
		{
			userPkiConf.setCertificateRevocationListLocation(crlLocation);
			userPkiConf.setRevocationCheckOptions(EnumCrlCheckOption_AllExceptSelfSigned);
		}
		tstring issuerLocation = pathToUserPKIFolder + _T("issuer") + DIR_SEPARATOR;
		if(FILE_OR_DIRECTORY_EXISTS(issuerLocation.c_str()))
		{
			userPkiConf.setIssuerCertificateListLocation(issuerLocation);
		}
		if(g_cttCompatible)
		{
			userPkiConf.checkSelfSignedSignature(true);
			userPkiConf.setRevocationCheckOptions(EnumCrlCheckOption_AllExceptSelfSigned);
		}

		result = endpoint->setUserPkiConfiguration(&userPkiConf);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set user Pki configuration: %s\n"), getEnumStatusCodeString(result));
		}
	}

	// Endpoint restriction
	if(StatusCode::isSUCCEEDED(result))
	{
		// with the current implementation, the endpoint restriction is specified globally,
		// so use the same restrict endpoint option for all endpoints
		result = endpoint->setRestrictToSingleInterface(commandLineOptions.getRestrictEndpoint());
	}
	return result;
}

EnumStatusCode configureEccEndpointSettings(Server::EndpointPtr endpoint, const CommandLineOptions& commandLineOptions)
{
	EnumStatusCode result = EnumStatusCode_Good;
	tstring endpointSecurity1 = toExpandedSecurityPolicyUri(commandLineOptions.getEndpointSecurity1());
	tstring endpointSecurity2 = toExpandedSecurityPolicyUri(commandLineOptions.getEndpointSecurity2());
	if(endpointSecurity1.empty())
	{
		if(commandLineOptions.getEnableECC() == 2)
		{
			endpointSecurity1 = SecurityPolicyUri_None;
		}
		else if(commandLineOptions.getEnableECC())
		{
			endpointSecurity1 = SecurityPolicyUri_Aes128Sha256nistP256;
		}
	}

	tstring userSecurityPolicy1 = toExpandedSecurityPolicyUri(commandLineOptions.getUserSecurityPolicy1());
	tstring userSecurityPolicy2 = toExpandedSecurityPolicyUri(commandLineOptions.getUserSecurityPolicy1());

	if(!endpointSecurity1.empty() || !endpointSecurity2.empty())
	{
		if(!endpointSecurity1.empty())
		{
			if(endpointSecurity1 != SecurityPolicyUri_None)
			{
				endpoint->addSecurityConfiguration(EnumMessageSecurityMode_Sign, endpointSecurity1);
				endpoint->addSecurityConfiguration(EnumMessageSecurityMode_SignAndEncrypt, endpointSecurity1);
			}
			else
			{
				endpoint->addSecurityConfiguration(EnumMessageSecurityMode_None, endpointSecurity1);
			}
		}
		if(!endpointSecurity2.empty() && endpointSecurity2 != endpointSecurity1)
		{
			if(endpointSecurity2 != SecurityPolicyUri_None)
			{
				endpoint->addSecurityConfiguration(EnumMessageSecurityMode_Sign, endpointSecurity2);
				endpoint->addSecurityConfiguration(EnumMessageSecurityMode_SignAndEncrypt, endpointSecurity2);
			}
			else
			{
				endpoint->addSecurityConfiguration(EnumMessageSecurityMode_None, endpointSecurity2);
			}
		}
		UserTokenPolicy userTokenPolicy;
		userTokenPolicy.setPolicyId(_T("Anonymous"));
		userTokenPolicy.setTokenType(EnumUserTokenType_Anonymous);
		userTokenPolicy.setSecurityPolicyUri(SecurityPolicyUri_None);
		if((result = endpoint->addUserTokenPolicy(&userTokenPolicy)) != EnumStatusCode_Good)
		{
			printf("ERROR: failed to set anonymous policy\r\n");
			return result;
		}
		userTokenPolicy.setPolicyId(_T("UserName_1"));
		userTokenPolicy.setTokenType(EnumUserTokenType_UserName);
		userTokenPolicy.setSecurityPolicyUri(userSecurityPolicy1);
		if((result = endpoint->addUserTokenPolicy(&userTokenPolicy)) != EnumStatusCode_Good)
		{
			printf("ERROR: failed to set username policy\r\n");
			return result;
		}
		if(!userSecurityPolicy2.empty() && userSecurityPolicy2 != userSecurityPolicy1)
		{
			userTokenPolicy.setPolicyId(_T("UserName_2"));
			userTokenPolicy.setTokenType(EnumUserTokenType_UserName);
			userTokenPolicy.setSecurityPolicyUri(userSecurityPolicy2);
			if((result = endpoint->addUserTokenPolicy(&userTokenPolicy)) != EnumStatusCode_Good)
			{
				printf("ERROR: failed to set username policy\r\n");
				return result;
			}
		}
	}
	return result;
}

void usage()
{
	UserInterface::printHeader();
	UserInterface::printCommand0(_T("info"),	_T("Show this usage"));
	UserInterface::printCommand0(_T("x,q"),		_T("Shut down the server"));
	UserInterface::printMenu(_T("endpoint"),	_T("Submenu for endpoints"));
	// disabled and hidden for compliance tests (don't certify this now, maybe next time)
	if(!g_cttCompatible)
	{
		UserInterface::printMenu(_T("gds"), _T("Submenu for GDS"));
	}
	UserInterface::printFooter();
}

void runUserInterface(bool be_quiet)
{
	bool keepRunning = true;
#ifdef TBC_OS_WINDOWS
#ifdef _DEBUG
	_CrtMemState s1, s2, sout;
#endif
#endif
#ifdef DUMP_MEMORYUSAGE
	ServerMemInfoAction memInfoAction;
	memInfoAction.setCyclic(true);
	memInfoAction.setTimeout(MEMUSAGE_CYCLE);
	memInfoAction.printHeader();
	getApplicationModule()->addAction(&memInfoAction);
#endif

	// be_quiet only affect the initial printing,
	// any response due to manual user interface interaction shall still be printed
	if(!be_quiet)
	{
		usage();
	}

	while(keepRunning && !getApplicationModule()->getShutDownFlag())
	{
		// continue if nothing is typed
		if(_kbhit())
		{
			// note: Some automated tests (fuzzy tests) may pass the _kbhit() even though no real input was provided via "</dev/null"
			// make sure to not print anything unless we have some real input

			bool showUsage = false;
			tstring input;
			// blocks until return is pressed (the same as for every other interface to read from stdin)
			getline(tcin, input);
			tstringstream inputStream(input);

			tstring command;
			if(inputStream >> command)	// read the function
			{
				// add a separation line after each typed line
				_tprintf(_T("\n"));
				if(UserInterface::parseCommand0(command, _T("info")))
				{
					showUsage = true;
				}
				else if(UserInterface::parseCommand0(command, _T("x")) ||
					UserInterface::parseCommand0(command, _T("q")))
				{
					_tprintf(_T("Shutdown\n"));
					keepRunning = false;
				}
				else if(UserInterface::parseMenu(command, _T("endpoint")))
				{
					handleEndpointConsoleMenu(inputStream);
				}
				// disabled and hidden for compliance tests (don't certify this now, maybe next time)
				else if(!g_cttCompatible && UserInterface::parseMenu(command, _T("gds")))
				{
					handleGdsConsoleMenu(inputStream);
				}
				// internal commands which are not printed to the user interface
#ifdef TBC_OS_WINDOWS
#ifdef _DEBUG
				else if(command == _T("m"))
				{
					_tprintf(_T("Creating memory checkpoint...\n"));
					_CrtMemCheckpoint(&s1);
				}
				else if(command == _T("M"))
				{
					_CrtMemCheckpoint(&s2);
					if(_CrtMemDifference(&sout, &s1, &s2))
					{
						_tprintf(_T("Dumping memeory leaks...\n"));
						_CrtDumpMemoryLeaks();
					}
					else
					{
						_tprintf(_T("Nothing to dump.\n"));
					}
				}
#endif
#endif
				else if(command == _T("v"))
				{
					// any output is printed by TestServerAddressSpaceRoot::onValidateError()
					(void)AddressSpaceRoot::instance()->validate();
				}
				else
				{
					showUsage = true;
					UserInterface::printUnknownCommandMessage(command);
				}
				// add a separation line after each performed command
				_tprintf(_T("\n"));
			}
			// don't show the usage in case no text was provided,
			// this is required for some automated tests
			//else
			//{
			//	showUsage = true;
			//}
			if(showUsage)
			{
				usage();
			}
		}
		else
		{
			SLEEP(200); // make a sleep
		}
	}
#ifdef DUMP_MEMORYUSAGE
	getApplicationModule()->removeAction(&memInfoAction);
#endif
}

void initNamespaces()
{
	OTUInt16 namespaceIndex = 0;
	AddressSpaceRoot* pAddressSpace = AddressSpaceRoot::instance();
	// the namespaces 0 and 1 are already created by the SDK
	// 0: http://opcfoundation.org/UA/
	// 1: urn:localhost/Softing/OpcUa/StructoAutomoldServer
	// 2:
	pAddressSpace->addNamespace(_T("http://softing.com/OPCToolbox/Console"), &namespaceIndex);
	// 3:
	pAddressSpace->addNamespace(_T("http://industrial.softing.com/UA/Refrigerator"), &namespaceIndex);
	// 4: http://www.softing.com/OPCUA_Toolkit
	pAddressSpace->getToolkitNamespaceIndex(&namespaceIndex);
	// 5:
	pAddressSpace->addNamespace(_T("http://softing.com/ModelDesignerTests"), &namespaceIndex);
}

void handleEndpointConsoleMenu(tstringstream& inputStream)
{
	tstring menuName = _T("endpoint");
	ApplicationPtr pApp = Application::instance();
	std::vector<Server::EndpointPtr> allEndpoints = pApp->getEndpoints();

	bool showUsage = false;
	tstring command;

	if(inputStream >> command)
	{
		bool parametersValid = true;
		size_t endpointIndex = 0;
		// ensures the parsed endpointIndex is in range
		IndexStreamProxy endpointIndexProxy(&endpointIndex, allEndpoints.size(), _T("endpointIndex"));
		tstring url;

		if(UserInterface::parseCommand0(command, _T("info")))
		{
			_tprintf(_T("Display current Endpoints and Sessions:\n"));
			for(size_t e = 0; e < allEndpoints.size(); e++)
			{
				_tprintf(_T("    Endpoint %u: %s%s (%s)\n"),
					(OTUInt32)e, allEndpoints[e]->getUrl().c_str(),
					allEndpoints[e]->isRestrictedToSingleInterface() ? _T(" (Restricted)") : _T(""),
					allEndpoints[e]->getIsOpen() ? _T("Open") : _T("Closed"));
				std::vector<Server::SessionPtr> sessions = allEndpoints[e]->getSessions();
				for(size_t s = 0; s < sessions.size(); s++)
				{
					_tprintf(
						_T("        Session %s:\n")
						_T("            Id: %u\n")
						_T("            IP: %s\n"),
						sessions[s]->getName().c_str(),
						sessions[s]->getId().getIdentifier().getUInt32(),
						sessions[s]->getClientIpAddress().c_str());
				}
			}
		}
		else if(UserInterface::parseCommand1(command, inputStream, _T("close"), parametersValid, endpointIndexProxy))
		{
			// this ensures to behave similar as the UA method call and prints some useful output
			DisableEndpointMethod::closeEndpoint(allEndpoints[endpointIndex]);
		}
		else if((UserInterface::parseCommand1(command, inputStream, _T("open"), parametersValid, endpointIndexProxy) ||
			UserInterface::parseCommand1(command, inputStream, _T("reopen"), parametersValid, endpointIndexProxy)))
		{
			// this ensures to behave similar as the UA method call and prints some useful output
			DisableEndpointMethod::reopenEndpoint(allEndpoints[endpointIndex]);
		}
		else if(UserInterface::parseCommand1(command, inputStream, _T("create"), parametersValid, url))
		{
			// this ensures to behave similar as the UA method call and prints some useful output
			CreateEndpointMethod::createEndpoint(url);
		}
		else if(UserInterface::parseCommand1(command, inputStream, _T("remove"), parametersValid, endpointIndexProxy))
		{
			// this ensures to behave similar as the UA method call and prints some useful output
			RemoveEndpointMethod::removeEndpoint(allEndpoints[endpointIndex]);
		}
		else if(UserInterface::parseMenu(command, _T("security")))
		{
			handleEndpointSecurityConsoleMenu(inputStream, allEndpoints);
		}
		else if(UserInterface::parseMenu(command, _T("user")))
		{
			handleEndpointUserConsoleMenu(inputStream, allEndpoints);
		}
		else
		{
			showUsage = true;
			// invalid parameters mean a command was found but rejected
			if(parametersValid)
			{
				UserInterface::printUnknownCommandMessage(command, menuName);
			}
		}
	}
	else
	{
		showUsage = true;
	}
	if(showUsage)
	{
		_tprintf(_T("Available Endpoints:\n"));
		for(size_t e = 0; e < allEndpoints.size(); e++)
		{
			_tprintf(_T("    Endpoint %u: %s%s (%s)\n"),
				(OTUInt32)e, allEndpoints[e]->getUrl().c_str(),
				allEndpoints[e]->isRestrictedToSingleInterface() ? _T(" (Restricted)") : _T(""),
				allEndpoints[e]->getIsOpen() ? _T("Open") : _T("Closed"));
		}

		UserInterface::printHeader(menuName);
		UserInterface::printCommand0(_T("info"),						_T("Displays all sessions on all endpoints"));
		UserInterface::printCommand1(_T("close"),		_T("EP-Index"),	_T("Closes an endpoint"));
		UserInterface::printCommand1(_T("open,reopen"),	_T("EP-Index"),	_T("Re-opens a previously closed endpoint"));
		UserInterface::printCommand1(_T("create"),		_T("Url"),		_T("Creates a new endpoint"));
		UserInterface::printCommand1(_T("remove"),		_T("EP-Index"),	_T("Closes and removes an endpoint"));
		UserInterface::printMenu(_T("security"),						_T("Submenu for endpoint security configs"));
		UserInterface::printMenu(_T("user"),							_T("Submenu for endpoint user configs"));
		UserInterface::printFooter();
	}
}

void handleEndpointSecurityConsoleMenu(tstringstream& inputStream, const std::vector<Server::EndpointPtr>& allEndpoints)
{
	tstring menuName = _T("security");
	ApplicationPtr pApp = Application::instance();

	bool showUsage = false;
	tstring command;

	if(inputStream >> command)
	{
		bool parametersValid = true;
		size_t endpointIndex = 0;
		// ensures the parsed endpointIndex is in range
		IndexStreamProxy endpointIndexProxy(&endpointIndex, allEndpoints.size(), _T("endpointIndex"));
		EnumMessageSecurityMode securityMode = EnumMessageSecurityMode_Invalid;
		// can be used on stringstream to write the content to the target variable
		EnumStreamProxy<EnumMessageSecurityMode> securityModeStreamWrapper(&securityMode, &getEnumMessageSecurityModeFromString);
		tstring securityUri;
		size_t securityConfigIndex = 0;
		IndexStreamProxy securityConfigIndexProxy(&securityConfigIndex, 0 /* yet unknown */, _T("securityConfigIndex"));

		if(UserInterface::parseCommand1(command, inputStream, _T("info"), parametersValid, endpointIndexProxy))
		{
			std::vector<EnumMessageSecurityMode> modes;
			std::vector<tstring> uris;
			allEndpoints[endpointIndex]->getSecurityConfigurations(modes, uris);
			_tprintf(_T("Available security configurations of endpoint %u:\n"), (OTUInt32)endpointIndex);
			for(size_t i = 0; i < modes.size(); i++)
			{
				_tprintf(
					_T("    %u:\n")
					_T("        Mode:      %s\n")
					_T("        PolicyUri: %s\n"),
					(OTUInt32)i, getEnumMessageSecurityModeString(modes[i]), uris[i].c_str());
			}
		}
		else if(UserInterface::parseCommand3(command, inputStream, _T("add"), parametersValid, endpointIndexProxy, securityModeStreamWrapper, securityUri))
		{
			// add the common URI part if it is missing
			securityUri = toExpandedSecurityPolicyUri(securityUri);

			EnumStatusCode addResult = allEndpoints[endpointIndex]->addSecurityConfiguration(securityMode, securityUri);
			if(StatusCode::isGood(addResult))
			{
				_tprintf(_T("Added message security configuration:\n")
					_T("    Mode:      %s\n")
					_T("    PolicyUri: %s\n"),
					getEnumMessageSecurityModeString(securityMode), securityUri.c_str());
			}
			else
			{
				_tprintf(_T("Failed to add security configuration: %s\n"), getEnumStatusCodeString(addResult));
			}
		}
		else if(UserInterface::parseCommand2(command, inputStream, _T("remove"), parametersValid, endpointIndexProxy, securityConfigIndex))
		{
			std::vector<EnumMessageSecurityMode> modes;
			std::vector<tstring> uris;
			allEndpoints[endpointIndex]->getSecurityConfigurations(modes, uris);
			if(securityConfigIndexProxy.validateRange(modes.size()))	// cannot be validated during parse
			{
				securityMode = modes[securityConfigIndex];
				securityUri = uris[securityConfigIndex];
				modes.erase(modes.begin() + securityConfigIndex);
				uris.erase(uris.begin() + securityConfigIndex);

				EnumStatusCode removeResult = allEndpoints[endpointIndex]->setSecurityConfigurations(modes, uris);
				if(StatusCode::isGood(removeResult))
				{
					_tprintf(_T("Security configuration removed:\n")
						_T("    Mode:      %s\n")
						_T("    PolicyUri: %s\n"),
						getEnumMessageSecurityModeString(securityMode), securityUri.c_str());
				}
				else
				{
					_tprintf(_T("Failed to remove security configuration: %s\n"), getEnumStatusCodeString(removeResult));
				}
			}
		}
		else
		{
			showUsage = true;
			// invalid parameters mean a command was found but rejected
			if(parametersValid)
			{
				UserInterface::printUnknownCommandMessage(command, menuName);
			}
		}
	}
	else
	{
		showUsage = true;
	}
	if(showUsage)
	{
		_tprintf(_T("Security configurations can only be added or removed while the endpoint is closed.\n"));
		UserInterface::printHeader(menuName);
		UserInterface::printCommand1(_T("info"),	_T("EP-Index"),											_T("Shows all message security configs"));
		UserInterface::printCommand3(_T("add"),		_T("EP-Index"), _T("SecurityMode"), _T("PolicyUri"),	_T("Adds a security config"));
		UserInterface::printCommand2(_T("remove"),	_T("EP-Index"), _T("SC-Index"),							_T("Removes a security config"));
		UserInterface::printFooter();
	}
}

void handleEndpointUserConsoleMenu(tstringstream& inputStream, const std::vector<Server::EndpointPtr>& allEndpoints)
{
	tstring menuName = _T("user");
	ApplicationPtr pApp = Application::instance();

	bool showUsage = false;
	tstring command;

	if(inputStream >> command)
	{
		bool parametersValid = true;
		size_t endpointIndex = 0;
		// ensures the parsed endpointIndex is in range
		IndexStreamProxy endpointIndexProxy(&endpointIndex, allEndpoints.size(), _T("endpointIndex"));
		EnumUserTokenType userTokenType = EnumUserTokenType_Anonymous;
		// can be used on stringstream to write the content to the target variable
		EnumStreamProxy<EnumUserTokenType> userTokenTypeStreamWrapper(&userTokenType, &oldGetEnumUserTokenTypeFromString);
		tstring securityUri;
		size_t userTokenIndex = 0;
		IndexStreamProxy userTokenIndexProxy(&userTokenIndex, 0 /* yet unknown */, _T("userTokenIndex"));

		if(UserInterface::parseCommand1(command, inputStream, _T("info"), parametersValid, endpointIndexProxy))
		{
			std::vector<UserTokenPolicy> userTokens = allEndpoints[endpointIndex]->getUserTokenPolicies();
			_tprintf(_T("Available security configurations of endpoint %u:\n"), (OTUInt32)endpointIndex);
			for(size_t i = 0; i < userTokens.size(); i++)
			{
				_tprintf(
					_T("    %u:\n")
					_T("        TokenType: %s\n")
					_T("        PolicyUri: %s\n")
					_T("        PolicyId:  %s\n"),
					(OTUInt32)i, oldGetEnumUserTokenTypeString(userTokens[i].getTokenType()),
					userTokens[i].getSecurityPolicyUri().c_str(),
					userTokens[i].getPolicyId().c_str());
			}
		}
		else if(UserInterface::parseCommand3(command, inputStream, _T("add"), parametersValid, endpointIndexProxy, userTokenTypeStreamWrapper, securityUri))
		{
			tstring fullSecurityUri;
			if(securityUri == _T("\"\""))
			{
				// allow "" for an empty string
				// use this string for the policy add on
				securityUri = _T("Channel");
				// fullSecurityUri stays empty
			}
			else
			{
				// add the common URI part if it is missing
				fullSecurityUri = toExpandedSecurityPolicyUri(securityUri);
			}
			UserTokenPolicy tokenPolicy = CreateEndpointMethod::createUserTokenPolicy(userTokenType, securityUri, fullSecurityUri);

			EnumStatusCode addResult = allEndpoints[endpointIndex]->addUserTokenPolicy(&tokenPolicy);
			if(StatusCode::isGood(addResult))
			{
				_tprintf(_T("Added user token policy:\n")
					_T("    Type:      %s\n")
					_T("    PolicyUri: %s\n")
					_T("    PolicyId:  %s\n"),
					oldGetEnumUserTokenTypeString(tokenPolicy.getTokenType()),
					tokenPolicy.getSecurityPolicyUri().c_str(),
					tokenPolicy.getPolicyId().c_str());
			}
			else
			{
				_tprintf(_T("Failed to add user token policy: %s\n"), getEnumStatusCodeString(addResult));
			}
		}
		else if(UserInterface::parseCommand2(command, inputStream, _T("remove"), parametersValid, endpointIndexProxy, userTokenIndex))
		{
			std::vector<UserTokenPolicy> userTokenPolicies = allEndpoints[endpointIndex]->getUserTokenPolicies();
			if(userTokenIndexProxy.validateRange(userTokenPolicies.size()))	// cannot be validated during parse
			{
				UserTokenPolicy tokenToRemove = userTokenPolicies[userTokenIndex];
				userTokenPolicies.erase(userTokenPolicies.begin() + userTokenIndex);

				EnumStatusCode removeResult = allEndpoints[endpointIndex]->setUserTokenPolicies(userTokenPolicies);
				if(StatusCode::isGood(removeResult))
				{
					_tprintf(_T("User token policy removed:\n")
						_T("    Type:      %s\n")
						_T("    PolicyUri: %s\n")
						_T("    PolicyId:  %s\n"),
						oldGetEnumUserTokenTypeString(tokenToRemove.getTokenType()),
						tokenToRemove.getSecurityPolicyUri().c_str(),
						tokenToRemove.getPolicyId().c_str());
				}
				else
				{
					_tprintf(_T("Failed to remove security configuration: %s\n"), getEnumStatusCodeString(removeResult));
				}
			}
		}
		else
		{
			showUsage = true;
			// invalid parameters mean a command was found but rejected
			if(parametersValid)
			{
				UserInterface::printUnknownCommandMessage(command, menuName);
			}
		}
	}
	else
	{
		showUsage = true;
	}
	if(showUsage)
	{
		_tprintf(_T("User token policies can only be added or removed while the endpoint is closed.\n"));
		UserInterface::printHeader(menuName);
		UserInterface::printCommand1(_T("info"),	_T("EP-Index"),											_T("Shows all user token policies"));
		UserInterface::printCommand3(_T("add"),		_T("EP-Index"),	_T("TokenType"),	_T("PolicyUri"),	_T("Adds a user token policy to the endpoint"));
		UserInterface::printCommand2(_T("remove"),	_T("EP-Index"),	_T("SC-Index"),							_T("Removes a user token policy from the endpoint"));
		UserInterface::printFooter();
	}
}

void handleGdsConsoleMenu(tstringstream& inputStream)
{
	tstring menuName = _T("gds");
	ApplicationPtr pApp = Application::instance();
	bool showUsage = false;
	bool parametersValid = true;

	tstring command;
	EnumStatusCode result = EnumStatusCode_Good;
	if(inputStream >> command)
	{
		tstring url;
		tstring securityProfile;
		OTUInt32 updateCycle = 0;
		UserIdentityToken userToken;
		tstring username;
		tstring password;

		if(UserInterface::parseCommand0(command, _T("info")))
		{
			pApp->gdsGetConfiguration(url, userToken, securityProfile, updateCycle);
			_tprintf(_T("GDS settings:\n")
				_T("URL:             %s\n")
				_T("TokenType:       "), url.c_str());
			switch(userToken.getType())
			{
			case EnumUserTokenType_Anonymous:
				_tprintf(_T("Anonymous\n"));
				break;
			case EnumUserTokenType_UserName:
			{
				UserNameIdentityToken userNameToken;
				userToken.getUserNameIdentityToken(userNameToken);
				_tprintf(_T("UserName(\"%s\"-\"****\")\n"), userNameToken.getUserName().c_str());
			}
				break;
			case EnumUserTokenType_Certificate:
			case EnumUserTokenType_IssuedToken:
			default:
				_tprintf(_T("Not handled\n"));
				break;
			}
			_tprintf(_T("SecurityProfile: %s\n")
				_T("UpdateCycle:     %u"),
				securityProfile.c_str(), updateCycle);
		}
		else if(UserInterface::parseCommand1(command, inputStream, _T("url"), parametersValid, url))
		{
			if(url == _T("\"\""))
			{
				url.clear();
			}
			tstring oldUrl;
			pApp->gdsGetConfiguration(oldUrl, userToken, securityProfile, updateCycle);
			EnumStatusCode setResult = pApp->gdsSetConfiguration(url, userToken, securityProfile, updateCycle);
			if(StatusCode::isGood(result))
			{
				_tprintf(_T("Updated GDS URL to %s\n"), url.c_str());
			}
			else
			{
				_tprintf(_T("Failed to update GDS configuration: %s\n"), getEnumStatusCodeString(setResult));
			}
		}
		else if(UserInterface::parseCommand2(command, inputStream, _T("user"), parametersValid, username, password))
		{
			if(username == _T("\"\""))
			{
				userToken.clear();
			}
			else
			{
				if(password == _T("\"\""))
				{
					password.clear();
				}
				UserNameIdentityToken userNameToken;
				userNameToken.setUserName(username);
				ByteString bs;
				bs.setUtf8String(password);
				userNameToken.setPassword(bs);
				userToken.setUserNameIdentityToken(&userNameToken);
			}
			UserIdentityToken oldToken;
			pApp->gdsGetConfiguration(url, oldToken, securityProfile, updateCycle);
			EnumStatusCode setResult = pApp->gdsSetConfiguration(url, userToken, securityProfile, updateCycle);
			if(StatusCode::isGood(result))
			{
				if(username.empty())
				{
					_tprintf(_T("Updated GDS user to Anonymous\n"));
				}
				else
				{
					_tprintf(_T("Updated GDS user to UserName (\"%s\"-\"****\")\n"), username.c_str());
				}
			}
			else
			{
				_tprintf(_T("Failed to update GDS configuration: %s\n"), getEnumStatusCodeString(setResult));
			}
		}
		else if(UserInterface::parseCommand1(command, inputStream, _T("security"), parametersValid, securityProfile))
		{
			securityProfile = toExpandedSecurityPolicyUri(securityProfile);
			tstring oldProfile;
			pApp->gdsGetConfiguration(url, userToken, oldProfile, updateCycle);
			EnumStatusCode setResult = pApp->gdsSetConfiguration(url, userToken, securityProfile, updateCycle);
			if(StatusCode::isGood(result))
			{
				_tprintf(_T("Updated GDS security profile URI to %s\n"), securityProfile.c_str());
			}
			else
			{
				_tprintf(_T("Failed to update GDS configuration: %s\n"), getEnumStatusCodeString(setResult));
			}
		}
		else if(UserInterface::parseCommand1(command, inputStream, _T("updateCycle"), parametersValid, updateCycle))
		{
			OTUInt32 oldCycle = 0;
			pApp->gdsGetConfiguration(url, userToken, securityProfile, oldCycle);
			EnumStatusCode setResult = pApp->gdsSetConfiguration(url, userToken, securityProfile, updateCycle);
			if(StatusCode::isGood(result))
			{
				_tprintf(_T("Updated GDS Pull update cycle to %u\n"), updateCycle);
			}
			else
			{
				_tprintf(_T("Failed to update GDS configuration: %s\n"), getEnumStatusCodeString(setResult));
			}
		}
		else if(UserInterface::parseCommand0(command, _T("register")))
		{
			NodeId gdsApplicationId = pApp->gdsGetApplicationId();
			result = pApp->gdsRegisterApplication();
			NodeId gdsApplicationIdAfter = pApp->gdsGetApplicationId();

			_tprintf(_T("GDS registration returned %s\n"), getEnumStatusCodeString(result));
			_tprintf(_T("    NodeId: %s -> %s\n"), gdsApplicationId.toString().c_str(), gdsApplicationIdAfter.toString().c_str());
		}
		else if(UserInterface::parseCommand0(command, _T("unregister")))
		{
			NodeId applicationId;
			OTUInt16 gdsNamespaceIndex = 0;
			Client::SessionPtr gdsSession = Client::Session::create();
			StringTableConstPtr namespaceTable;

			result = pApp->gdsGetConfiguration(url, userToken, securityProfile, updateCycle);
			applicationId = pApp->gdsGetApplicationId();

			if(StatusCode::isGood(result))
			{
				result = gdsSession->configure(url, userToken, EnumMessageSecurityMode_SignAndEncrypt, securityProfile);
			}
			if(StatusCode::isGood(result))
			{
				result = pApp->addSession(gdsSession);
			}
			if(StatusCode::isGood(result))
			{
				result = gdsSession->connect(true);
			}
			if(StatusCode::isGood(result))
			{
				result = gdsSession->getNamespaceTable(&namespaceTable);
			}
			if(StatusCode::isGood(result))
			{
				result = namespaceTable->getIndex(_T("http://opcfoundation.org/UA/GDS/"), gdsNamespaceIndex);
			}
			if(StatusCode::isGood(result))
			{
				NodeId objectId(gdsNamespaceIndex, 141);	// Directory
				NodeId methodId(gdsNamespaceIndex, 149);	// Directory/UnregisterApplication
				std::vector<Value> inputArguments;
				inputArguments.push_back(Value(&applicationId));
				std::vector<Value> outputArguments;
				std::vector<EnumStatusCode> inputArgumentResults;
				EnumStatusCode callResult = EnumStatusCode_Good;

				result = gdsSession->callMethod(&objectId, &methodId, inputArguments, outputArguments, inputArgumentResults, &callResult);
			}
			gdsSession->disconnect();
			pApp->removeSession(gdsSession);
			NodeId gdsApplicationIdAfter = pApp->gdsGetApplicationId();
			_tprintf(_T("GDS unregistration returned %s\n"), getEnumStatusCodeString(result));
			_tprintf(_T("    NodeId: %s -> %s\n"), applicationId.toString().c_str(), gdsApplicationIdAfter.toString().c_str());
		}
		// "create" and "sign" are handled in the else part
		else if(UserInterface::parseCommand0(command, _T("finish")))
		{
			GdsCreateSignedCertificateInfo gdsCreateInfo;
			if(g_pPendingGdsRequest)
			{
				result = pApp->gdsFinishCertificateAsync(&gdsCreateInfo, g_pPendingGdsRequest);
				_tprintf(_T("Started finishing request %s for GDS signed certificate (%s):...\n"), g_pPendingGdsRequest->toString().c_str(),
					getEnumStatusCodeString(result));;
				if(StatusCode::isGood(result))
				{
					gdsCreateInfo.m_gdsEvent.waitFor();
					_tprintf(_T("Finishing request completed %s\n"), getEnumStatusCodeString(gdsCreateInfo.m_creationResult));
					if(StatusCode::isGood(gdsCreateInfo.m_creationResult) && gdsCreateInfo.m_certificateGroup.isNull())
					{
						// this is the default application group
						(void)Application::instance()->gdsReplaceInstanceCertificate(gdsCreateInfo.m_certificate,
							gdsCreateInfo.m_privateKey, gdsCreateInfo.m_privateKeyPassword,
							gdsCreateInfo.m_issuerCertificates);
					}
					else if(gdsCreateInfo.m_creationResult == EnumStatusCode_BadNothingToDo)
					{
						_tprintf(_T("Pending request ID: %s"), gdsCreateInfo.m_pendingRequestId.toString().c_str());
						g_pPendingGdsRequest = new NodeId(gdsCreateInfo.m_pendingRequestId);
					}
					else
					{
						delete g_pPendingGdsRequest;
						g_pPendingGdsRequest = NULL;
					}
				}
			}
			else
			{
				_tprintf(_T("No pending request to finish\n"));
			}
		}
		else if(UserInterface::parseCommand0(command, _T("groups")))
		{
			std::vector<NodeId> certificateGroups;
			std::vector<LocalizedText> displayNames;
			std::vector<std::vector<NodeId> > certificateTypes;
			result = pApp->gdsGetCertificateGroups(certificateGroups, displayNames, certificateTypes);
			_tprintf(_T("Get certificate groups returned %u groups (%s)\n"), (OTUInt32)certificateGroups.size(), getEnumStatusCodeString(result));
			for(size_t i = 0; i < certificateGroups.size(); i++)
			{
				_tprintf(_T("   %s: %s\n"), displayNames[i].getText().c_str(), certificateGroups[i].toString().c_str());
				_tprintf(_T("       Certificate types: "));
				if(certificateTypes[i].empty())
				{
					_tprintf(_T("None"));
				}
				for(size_t j = 0; j < certificateTypes[i].size(); j++)
				{
					_tprintf(_T("%s "), certificateTypes[i][j].toString().c_str());
				}
				_tprintf(_T("\n"));
			}
		}
		else if(UserInterface::parseCommand0(command, _T("trust")))
		{
			std::vector<NodeId> certificateGroups;
			std::vector<LocalizedText> displayNames;
			std::vector<std::vector<NodeId> > certificateTypes;
			result = pApp->gdsGetCertificateGroups(certificateGroups, displayNames, certificateTypes);
			_tprintf(_T("Get certificate groups returned %u groups (%s)\n"), (OTUInt32)certificateGroups.size(), getEnumStatusCodeString(result));
			for(size_t i = 0; i < certificateGroups.size(); i++)
			{
				_tprintf(_T("   %s: %s\n"), displayNames[i].getText().c_str(), certificateGroups[i].toString().c_str());
			}
			if(certificateGroups.size() > 0)
			{
				GdsGetTrustListInfo getTrustListInfo;

				result = pApp->gdsGetTrustListAsync(&getTrustListInfo, certificateGroups[0]);
				_tprintf(_T("Started getting trust list for %s from GDS (%s):...\n"), certificateGroups[0].toString().c_str(), getEnumStatusCodeString(result));;
				if(StatusCode::isGood(result))
				{
					getTrustListInfo.m_gdsEvent.waitFor();
					tstring certFile = pApp->getInstanceCertificate();
					tstring pkiPath;
					tstring newPkiPath;

					_tprintf(_T("getTrustList completed %s\n"), getEnumStatusCodeString(getTrustListInfo.m_getResult));

					if(StatusCode::isGood(getTrustListInfo.m_getResult))
					{
						PkiStoreConfiguration pkiConf = pApp->getPkiStoreConfiguration();
						EnumStatusCode replaceResult = pApp->replacePkiContent(pkiConf, getTrustListInfo.m_trustedCertificates, getTrustListInfo.m_trustedCrls,
							getTrustListInfo.m_issuerCertificates, getTrustListInfo.m_issuerCrls, getTrustListInfo.m_lastUpdateTime);
						_tprintf(_T("TrustList updated: %s\n"), getEnumStatusCodeString(replaceResult));
					}
				}
			}
		}
		else if(UserInterface::parseCommand0(command, _T("status")))
		{
			NodeId certGroup;
			NodeId certType;
			bool needsUpdate;
			result = pApp->gdsGetCertificateStatus(needsUpdate, certGroup, certType);
			_tprintf(_T("GetCertificateStatus from GDS returned %s, (%s)\n"),
				needsUpdate ? _T("update needed") : _T("no update needed"), getEnumStatusCodeString(result));
		}
		else
		{
			bool isSign = UserInterface::parseCommand0(command, _T("sign"));
			bool isCreate = UserInterface::parseCommand0(command, _T("create"));
			if(isSign || isCreate)
			{
				tstring securityPolicy = SecurityPolicyUri_Basic256Sha256;
				tstring certFile = Application::instance()->getInstanceCertificate();
				tstring certPath;
				tstring appUri = Application::instance()->getDescription()->getApplicationUri();
				tstring keyfile;
				size_t pos;
				bool isUserCert = false;
				NodeId certificatGroupId;
				tstring commonName = _T("Softing OPC UA StructoAutomoldServer");

				tstring hostName = _T("localhost");
				updateHostName(hostName);

				// get the path of own cert
				std::replace(certFile.begin(), certFile.end(), _T('\\'), _T('/'));
				pos = certFile.find_last_of(_T('/'));
				if(std::wstring::npos != pos)
				{
					certPath = certFile.substr(0, pos);
				}

				if(isSign)
				{
					GdsCreateSignedCertificateInfo gdsCreateInfo;

					result = Application::instance()->gdsCreateSignedCertificateAsync(&gdsCreateInfo, securityPolicy, _T("pass"),
						commonName, _T("Softing IA GmbH"), _T("DNS:") + hostName, isUserCert, certificatGroupId);

					_tprintf(_T("Started creation of %s GDS signed certificate (%s):...\n"), isUserCert ? _T("user") : _T("instance"),
						getEnumStatusCodeString(result));;
					if(StatusCode::isGood(result))
					{
						// if good, continue and try to use the expected result for a later status
						gdsCreateInfo.m_gdsEvent.waitFor();

						_tprintf(_T("Create of certificates completes %s\n"), getEnumStatusCodeString(gdsCreateInfo.m_creationResult));
						if(StatusCode::isGood(gdsCreateInfo.m_creationResult) && gdsCreateInfo.m_certificateGroup.isNull())
						{
							// this is the default application group
							(void)Application::instance()->gdsReplaceInstanceCertificate(gdsCreateInfo.m_certificate,
								gdsCreateInfo.m_privateKey, gdsCreateInfo.m_privateKeyPassword,
								gdsCreateInfo.m_issuerCertificates);
						}
						else if(gdsCreateInfo.m_creationResult == EnumStatusCode_BadNothingToDo)
						{
							_tprintf(_T("Pending request ID: %s"), gdsCreateInfo.m_pendingRequestId.toString().c_str());
							g_pPendingGdsRequest = new NodeId(gdsCreateInfo.m_pendingRequestId);
						}
					}
				}
				else	// create
				{
					// create self signed
					keyfile = certPath + _T("/newSelfSigned.pem");
					certFile = certPath + _T("/newSelfSigned.der");

					result = pApp->createSelfSignedCertificate(securityPolicy, certFile, keyfile, _T("pass"), 180 * 24 * 3600,
						commonName, _T("Softing IA GmbH"), _T("DNS:") + hostName);

					_tprintf(_T("Created self-signed certificate %s:  %s\n"), certFile.c_str(), getEnumStatusCodeString(result));;
				}
			}
			else
			{
				showUsage = true;
				UserInterface::printUnknownCommandMessage(command, menuName);
			}
		}
	}
	else
	{
		showUsage = true;
	}
	if(showUsage)
	{
		UserInterface::printHeader(menuName);
		UserInterface::printCommand0(_T("info"),										_T("Shows the GDS configuration"));
		UserInterface::printCommand1(_T("url"),			_T("URL"),						_T("Updates the configured GDS URL"));
		UserInterface::printCommand2(_T("user"),		_T("Username"),	_T("Password"),	_T("Updates the configured GDS user"));
		UserInterface::printCommand1(_T("security"),	_T("URI"),						_T("Updates the configured GDS security"));
		UserInterface::printCommand1(_T("updateCycle"),	_T("Millisecs"),				_T("Updates the configured GDS Pull update cycle"));
		UserInterface::printCommand0(_T("register"),									_T("Registers this application to the GDS"));
		UserInterface::printCommand0(_T("unregister"),									_T("Unregisters this application from the GDS"));
		UserInterface::printCommand0(_T("sign"),										_T("Creates a singned application certificate using the GDS"));
		UserInterface::printCommand0(_T("create"),										_T("Creates a self-signed application certificate locally"));
		UserInterface::printCommand0(_T("finish"),										_T("Finishes a pending sign request"));
		UserInterface::printCommand0(_T("groups"),										_T("Calls GetCertificateGroups on the GDS"));
		UserInterface::printCommand0(_T("trust"),										_T("Updates the TrustList via the GDS"));
		UserInterface::printCommand0(_T("status"),										_T("Calls GetCertificateStatus on the GDS"));
		UserInterface::printFooter();
	}
}
