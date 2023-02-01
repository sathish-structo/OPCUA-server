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
#include "ApplicationModule.h"
#include "SimVarManager.h"
#include "Statics.h"
#include "Application.h"
#include "ModelerModule.h"
#include "EventingModule.h"
#include "StructoPrinter.h"
#include "OffTheShelfAlarmsModule.h"
#include "AlarmsAndConditionsStressTestModule.h"
#include "ServerFile.h"
#include "ServerFileDirectory.h"
#include "ServerRole.h"
#include "IdentityMappingRuleType.h"
#include "EndpointType.h"
#include "Constants.h"


using namespace SoftingOPCToolbox5;


/*! ApplicationModule global instance */
ApplicationModule* m_applicationModuleInstance = NULL;

bool ApplicationModule::s_accessPatternEnabled = false;

// user access
const OTUInt32 ApplicationModule::s_UserGroup_Default = 0; // anonymous / eve
const OTUInt32 ApplicationModule::s_UserGroup_Group1 = 1; // usr / certificate
const OTUInt32 ApplicationModule::s_UserGroup_Group2 = 2; // usr2
const OTUInt32 ApplicationModule::s_UserGroup_Query = 3; // query
const OTUInt32 ApplicationModule::s_UserGroup_RedundancyAccess = 4;

const OTUInt32 ApplicationModule::s_AccessKey_ReadHistory_Read_Read_None_None = 1;
const OTUInt32 ApplicationModule::s_AccessKey_Read_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None = 2;
const OTUInt32 ApplicationModule::s_AccessKey_None_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None = 3;
const OTUInt32 ApplicationModule::s_AccessKey_Read_Read_Read_Read_ReadCall = 4;
const OTUInt32 ApplicationModule::s_AccessKey_Read_ReadWriteCall_Read_Read_Read = 5;
const OTUInt32 ApplicationModule::s_AccessKey_None_ReadCall_ReadWriteCall_ReadCall_ReadCall = 6;
const OTUInt32 ApplicationModule::s_AccessKey_ForAccessModifications = 7;

// user access via roles
NodeId* ApplicationModule::s_RoleIdDefault = NULL;
NodeId* ApplicationModule::s_RoleIdGroup1 = NULL;
NodeId* ApplicationModule::s_RoleIdGroup2 = NULL;
NodeId* ApplicationModule::s_RoleIdQuery = NULL;
NodeId* ApplicationModule::s_RoleIdRedundancy = NULL;


/* @brief Getter for the ApplicationModule global instance
 * @return Pointer to ApplicationModule global instance */
ApplicationModule* getApplicationModule()
{
	return m_applicationModuleInstance;
}

/* @brief Setter for the ApplicationModule global instance
 * @param appModule Pointer to ApplicationModule global instance */
void setApplicationModule(ApplicationModule* appModule)
{
	if(appModule)
	{
		m_applicationModuleInstance = appModule;
	}
}

/* @brief Constructor */
ApplicationModule::ApplicationModule()
: m_shutDown(0), m_workItemProcessor(NULL)
{
}

/* @brief Destructor */
ApplicationModule::~ApplicationModule()
{
	_ASSERTION(m_workItemProcessor == NULL, _T("work item processor still alive"));
	if(this == m_applicationModuleInstance)
	{
		m_applicationModuleInstance = NULL;
	}
}

/* @brief Inits the application module
 * It is building the OPC server address space, initializing all the submodules
 * (simulation module, remote module, statistic module)
 * @return bool */
bool ApplicationModule::init(const ApplicationDescription &applicationDescription, const std::vector<EndpointPtr>& localEndpoints, EnumRedundancySupport redMode, const std::vector<tstring> &redUrls, const tstring &pathToServerFiles)
{
	bool result = true;
	// Global instance initialization
	setApplicationModule(this);

	AddressSpaceRoot::instance()->addNamespace(_T("http://softing.com/OPCToolbox/Console"), &m_nameSpaceIndex);
	m_namespaceUriType = _T("http://industrial.softing.com/UA/Refrigerator");
	AddressSpaceRoot::instance()->addNamespace(m_namespaceUriType, &m_nameSpaceIndexTypes);

	if(s_accessPatternEnabled)
	{
		initializeUserAccessPatterns();
	}
	else
	{
		// use same permissions on all roles
		std::vector<RolePermissionType> rolePermissions;
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(NodeId(EnumNumericNodeId_WellKnownRole_SecurityAdmin));
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Call | EnumPermissionType_Read);

		// create role objects
		RolePtr role;
		std::vector<IdentityMappingRuleType> identities;

		role = Role::create();
		role->setNodeId(s_RoleIdDefault);
		role->setBrowseName(QualifiedName(_T("RoleDefault"), s_RoleIdDefault->getNamespaceIndex()));
		role->setDisplayName(LocalizedText(_T("RoleDefault"), _T("")));
		// configure who shall be granted with this role
		identities.clear();
		identities.push_back(IdentityMappingRuleType());
		identities.back().setCriteriaType(EnumIdentityMappingType_Anonymous);
		identities.push_back(IdentityMappingRuleType());
		identities.back().setCriteriaType(EnumIdentityMappingType_UserName);
		identities.back().setCriteria(_T("eve"));
		role->setIdentities(identities);

		/* for testing import / export*/
		/*
		std::vector<EndpointType> endpointTypes;
		endpointTypes.push_back(EndpointType());
		endpointTypes.back().setEndpointUrl(_T("Enpoint1_Url"));
		endpointTypes.back().setSecurityMode(EnumMessageSecurityMode_SignAndEncrypt);
		endpointTypes.back().setSecurityPolicyUri(SecurityPolicyUri_Aes128Sha256nistP256);
		endpointTypes.back().setTransportProfileUri(_T("Transport_1"));
		endpointTypes.push_back(EndpointType());
		endpointTypes.back().setTransportProfileUri(_T("Transport_2"));
		endpointTypes.push_back(EndpointType());
		endpointTypes.back().setEndpointUrl(_T("Enpoint3_Url"));
		role->setEndpoints(endpointTypes, false);

		std::vector<tstring> applications;
		applications.push_back(_T("App1"));
		applications.push_back(_T("App2"));
		role->setApplications(applications, true);
		*/

		// do not set role permissions on the role node itself - otherwise it is not browseable by any other role
		//role->setRolePermissions(rolePermissions);
		Server::AddressSpaceRoot::instance()->addRole(role, rolePermissions);

		role = Role::create();
		role->setNodeId(s_RoleIdGroup1);
		role->setBrowseName(QualifiedName(_T("RoleGroup1"), s_RoleIdGroup1->getNamespaceIndex()));
		role->setDisplayName(LocalizedText(_T("RoleGroup1"), _T("")));
		// configure who shall be granted with this role
		identities.clear();
		identities.push_back(IdentityMappingRuleType());
		identities.back().setCriteriaType(EnumIdentityMappingType_UserName);
		identities.back().setCriteria(_T("usr"));
		role->setIdentities(identities);
		//role->setRolePermissions(rolePermissions);
		Server::AddressSpaceRoot::instance()->addRole(role, rolePermissions);

		role = Role::create();
		role->setNodeId(s_RoleIdGroup2);
		role->setBrowseName(QualifiedName(_T("RoleGroup2"), s_RoleIdGroup2->getNamespaceIndex()));
		role->setDisplayName(LocalizedText(_T("RoleGroup2"), _T("")));
		// configure who shall be granted with this role
		identities.clear();
		identities.push_back(IdentityMappingRuleType());
		identities.back().setCriteriaType(EnumIdentityMappingType_UserName);
		identities.back().setCriteria(_T("usr2"));
		role->setIdentities(identities);
		//role->setRolePermissions(rolePermissions);
		Server::AddressSpaceRoot::instance()->addRole(role, rolePermissions);

		role = Role::create();
		role->setNodeId(s_RoleIdQuery);
		role->setBrowseName(QualifiedName(_T("RoleQuery"), s_RoleIdQuery->getNamespaceIndex()));
		role->setDisplayName(LocalizedText(_T("RoleQuery"), _T("")));
		// configure who shall be granted with this role
		identities.clear();
		identities.push_back(IdentityMappingRuleType());
		identities.back().setCriteriaType(EnumIdentityMappingType_UserName);
		identities.back().setCriteria(_T("query"));
		role->setIdentities(identities);
		//role->setRolePermissions(rolePermissions);
		Server::AddressSpaceRoot::instance()->addRole(role, rolePermissions);

		role = Role::create();
		role->setNodeId(s_RoleIdRedundancy);
		role->setBrowseName(QualifiedName(_T("RoleRedundancy"), s_RoleIdRedundancy->getNamespaceIndex()));
		role->setDisplayName(LocalizedText(_T("RoleRedundancy"), _T("")));
		//role->setRolePermissions(rolePermissions);
		Server::AddressSpaceRoot::instance()->addRole(role, rolePermissions);

		role = Role::getWellKnown(EnumNumericNodeId_WellKnownRole_SecurityAdmin);
		if(role)
		{
			// well-known roles are already configured with browse- and displayname
			identities.clear();
			identities.push_back(IdentityMappingRuleType());
			identities.back().setCriteriaType(EnumIdentityMappingType_UserName);
			identities.back().setCriteria(_T("SecurityAdmin"));
			role->setIdentities(identities);
			Server::AddressSpaceRoot::instance()->addRole(role, rolePermissions);
		}
	}
	_tprintf(_T("Start Timer\n"));
	// start timer thread
	// the Util_TimerThread must be started before adding actions, otherwise they cannot be added
	start();
	m_workItemProcessor = new Util_WorkItemProcessor();
	m_workItemProcessor->start();

	m_threadReaperAction.init(this);
	m_threadReaperAction.setCyclic(true);
	m_threadReaperAction.setTimeout(1000);
	addAction(&m_threadReaperAction);
	_tprintf(_T("addAction\n"));
	m_pathToServerFilesFolder = pathToServerFiles;

	result = StatusCode::isGood(initModules(m_nameSpaceIndex, applicationDescription, localEndpoints, redMode, redUrls));
	_tprintf(_T("App init exit\n"));
	return result;
}

BEGIN_IGNORE_DEPRECATED()
EnumStatusCode ApplicationModule::initializeUserAccessPatterns()
{
	EnumStatusCode result = EnumStatusCode_Good;

	unsigned char rights[5];
	rights[s_UserGroup_Default] = EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead; // Anonymous / Default
	rights[s_UserGroup_Group1] = EnumAccessLevel_CurrentRead; // User Group 1
	rights[s_UserGroup_Group2] = EnumAccessLevel_CurrentRead; // User Group 2
	rights[s_UserGroup_Query] = EnumAccessLevel_None; // Group for query service tests. Read access is not granted.
	rights[s_UserGroup_RedundancyAccess] = EnumAccessLevel_None; // Group for redundancy access method access


	ByteString accessField;
	if(accessField.init(sizeof(rights) / sizeof(*rights), rights))
	{
		AccessPattern accessPattern;
		accessPattern.setKey(s_AccessKey_ReadHistory_Read_Read_None_None);
		accessPattern.setAccessField(accessField);

		result = Application::instance()->setAccessPattern(&accessPattern);
	}
	else
	{
		result = EnumStatusCode_BadInternalError;
	}

	// AccessPattern to allow only read access for anonymous users
	if(StatusCode::isGood(result))
	{
		rights[s_UserGroup_Default] = EnumAccessLevel_CurrentRead; // Anonymous / Default
		rights[s_UserGroup_Group1] = EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable; // User Group 1
		rights[s_UserGroup_Group2] = EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable; // User Group 2
		rights[s_UserGroup_Query] = EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable; // Group for query service tests. Read access is not granted.
		rights[s_UserGroup_RedundancyAccess] = EnumAccessLevel_None; // Group for redundancy access method access

		ByteString accessField2;
		if(accessField2.init(sizeof(rights) / sizeof(*rights), rights))
		{
			AccessPattern accessPattern;
			accessPattern.setKey(s_AccessKey_Read_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None);
			accessPattern.setAccessField(accessField2);

			result = Application::instance()->setAccessPattern(&accessPattern);
		}
		else
		{
			result = EnumStatusCode_BadInternalError;
		}
	}

	// AccessPattern to forbid any access for anonymous users
	if(StatusCode::isGood(result))
	{
		rights[s_UserGroup_Default] = EnumAccessLevel_None; // Anonymous / Default
		rights[s_UserGroup_Group1] = EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable; // User Group 1
		rights[s_UserGroup_Group2] = EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable; // User Group 2
		rights[s_UserGroup_Query] = EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable; // Group for query service tests. Read access is not granted.
		rights[s_UserGroup_RedundancyAccess] = EnumAccessLevel_None; // Group for redundancy access method access

		ByteString accessField3;
		if(accessField3.init(sizeof(rights) / sizeof(*rights), rights))
		{
			AccessPattern accessPattern;
			accessPattern.setKey(s_AccessKey_None_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None);
			accessPattern.setAccessField(accessField3);

			result = Application::instance()->setAccessPattern(&accessPattern);
		}
		else
		{
			result = EnumStatusCode_BadInternalError;
		}
	}

	// AccessPattern to allow execution of sync methods
	if(StatusCode::isGood(result))
	{
		rights[s_UserGroup_Default] = EnumAccessLevel_CurrentRead; // Anonymous / Default
		rights[s_UserGroup_Group1] = EnumAccessLevel_CurrentRead; // User Group 1
		rights[s_UserGroup_Group2] = EnumAccessLevel_CurrentRead; // User Group 2
		rights[s_UserGroup_Query] = EnumAccessLevel_CurrentRead; // Group for query service tests. Read access is not granted.
		rights[s_UserGroup_RedundancyAccess] = EnumAccessLevel_CurrentRead | EnumAccessLevel_Executable; // Group for redundancy access method access

		ByteString accessField4;
		if(accessField4.init(sizeof(rights) / sizeof(*rights), rights))
		{
			AccessPattern accessPattern;
			accessPattern.setKey(s_AccessKey_Read_Read_Read_Read_ReadCall);
			accessPattern.setAccessField(accessField4);

			result = Application::instance()->setAccessPattern(&accessPattern);
		}
		else
		{
			result = EnumStatusCode_BadInternalError;
		}
	}

	// AccessPattern to allow executable for user group 1
	if(StatusCode::isGood(result))
	{
		rights[s_UserGroup_Default] = EnumAccessLevel_CurrentRead; // Anonymous / Default
		rights[s_UserGroup_Group1] = EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable; // User Group 1
		rights[s_UserGroup_Group2] = EnumAccessLevel_CurrentRead; // User Group 2
		rights[s_UserGroup_Query] = EnumAccessLevel_CurrentRead; // Group for query service tests. Read access is not granted.
		rights[s_UserGroup_RedundancyAccess] = EnumAccessLevel_CurrentRead; // Group for redundancy access method access

		ByteString accessField2;
		if(accessField2.init(sizeof(rights) / sizeof(*rights), rights))
		{
			AccessPattern accessPattern;
			accessPattern.setKey(s_AccessKey_Read_ReadWriteCall_Read_Read_Read);
			accessPattern.setAccessField(accessField2);

			result = Application::instance()->setAccessPattern(&accessPattern);
		}
		else
		{
			result = EnumStatusCode_BadInternalError;
		}
	}
	// AccessPattern to allow executable for user group 1
	if(StatusCode::isGood(result))
	{
		rights[s_UserGroup_Default] =			EnumAccessLevel_None; // Anonymous / Default
		rights[s_UserGroup_Group1] =			EnumAccessLevel_CurrentRead | EnumAccessLevel_Executable; // User Group 1 (usr1)
		rights[s_UserGroup_Group2] =			EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable; // User Group 2 (usr2)
		rights[s_UserGroup_Query] =				EnumAccessLevel_CurrentRead | EnumAccessLevel_Executable; // Group for query service tests. Read access is not granted.
		rights[s_UserGroup_RedundancyAccess] =	EnumAccessLevel_CurrentRead | EnumAccessLevel_Executable; // Group for redundancy access method access

		ByteString accessField2;
		if(accessField2.init(sizeof(rights) / sizeof(*rights), rights))
		{
			AccessPattern accessPattern;
			accessPattern.setKey(s_AccessKey_None_ReadCall_ReadWriteCall_ReadCall_ReadCall);
			accessPattern.setAccessField(accessField2);

			result = Application::instance()->setAccessPattern(&accessPattern);
		}
		else
		{
			result = EnumStatusCode_BadInternalError;
		}
	}

	return result;
}
END_IGNORE_DEPRECATED()

BEGIN_IGNORE_DEPRECATED()
void ApplicationModule::assignAccessPattern(Server::BaseNodePtr node, OTUInt32 accessPatternKey)
{
	if(node->getNodeClass() == EnumNodeClass_Variable)
	{
		VariablePtr variable = static_pointer_cast<Variable>(node);
		variable->setAccessPatternKey(accessPatternKey);
	}
	else if(node->getNodeClass() == EnumNodeClass_Method)
	{
		MethodPtr method = static_pointer_cast<Method>(node);
		method->setAccessPatternKey(accessPatternKey);
	}
	else if(node->getApiClass() == EnumApiClassType_ServerFile)
	{
		FilePtr file = static_pointer_cast<File>(node);
		file->setAccessPatternKey(accessPatternKey);
	}
	else if(node->getApiClass() == EnumApiClassType_ServerFileDirectory)
	{
		FileDirectoryPtr directory = static_pointer_cast<FileDirectory>(node);
		directory->setAccessPatternKey(accessPatternKey);
	}
}
END_IGNORE_DEPRECATED()

void ApplicationModule::assignAccessPatternAndRolePermissions(Server::BaseNodePtr node, OTUInt32 accessPatternKey)
{
	if(accessPatternKey != 0)
	{
		if(s_accessPatternEnabled)
		{
			assignAccessPattern(node, accessPatternKey);
		}
		else
		{
			std::vector<RolePermissionType> rolePermissions;
			getRolePermissionsForKey(accessPatternKey, rolePermissions);

			// append the role permissions to the ones which were generated from the access pattern
			// they will have almost pretty similar rights but different RoleIds,
			// we need both sets when we want to support both ways
			std::vector<RolePermissionType> combinedPermissions = node->getRolePermissions();
			combinedPermissions.insert(combinedPermissions.end(), rolePermissions.begin(), rolePermissions.end());
			node->setRolePermissions(combinedPermissions);
		}
	}
}

void ApplicationModule::getRolePermissionsForKey(OTUInt32 accessPatternKey, std::vector<RolePermissionType>& rolePermissions)
{
	if(s_RoleIdDefault == NULL)
	{
		_REPORT_ASSERTION(_T("User access not initialized"));
		return;
	}
	rolePermissions.clear();

	switch(accessPatternKey)
	{
	case s_AccessKey_ReadHistory_Read_Read_None_None:
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_ReadHistory);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdQuery);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdRedundancy);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse);
		break;

	case s_AccessKey_Read_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None:
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_ReadHistory | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_ReadHistory | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdQuery);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_ReadHistory | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdRedundancy);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse);
		break;

	case s_AccessKey_None_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None:
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_ReadHistory | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_ReadHistory | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdQuery);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_ReadHistory | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdRedundancy);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse);
		break;

	case s_AccessKey_Read_Read_Read_Read_ReadCall:
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdQuery);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdRedundancy);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Call);
		break;

	case s_AccessKey_Read_ReadWriteCall_Read_Read_Read:
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdQuery);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdRedundancy);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		break;

	case s_AccessKey_None_ReadCall_ReadWriteCall_ReadCall_ReadCall:
		// add call right as well since these permissions are used at file tests which now evaluate the "call" permission as well
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdQuery);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Call);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdRedundancy);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Call);
		break;
	default:
		_REPORT_ASSERTION(_T("user access for unknown key"));
		break;
	}
}

/* @brief Ends the application module
 * It is ending all the submodules(simulation, remote and statistic modules)
 * @return void */
void ApplicationModule::end()
{
	// stop all actions (must be the first!)
	stop();

	// stop the workitem processor after stopping timer (may produce new work items)
	if(m_workItemProcessor)
	{
		m_workItemProcessor->stop();
		m_workItemProcessor->wait(INFINITE);
		delete m_workItemProcessor;
		m_workItemProcessor = NULL;
	}

	// cleanup in reverse order
	for(OTInt32 i = (OTInt32)m_modules.size() - 1; i >= 0; i--)
	{
		m_modules[i]->endModule();
	}
	m_modules.clear();

	if(m_testModule)
	{
		m_testModule->endModule();
		m_testModule.reset();
	}

	// join all threads
	m_threadReaper.end();
	wait(INFINITE);
}

/* @brief Getter for the namespace index
 * @return Namespace index */
OTUInt16 ApplicationModule::getNameSpaceIndex()
{
	return m_nameSpaceIndex;
}


OTUInt16 ApplicationModule::getTypesNameSpaceIndex()
{
	return m_nameSpaceIndexTypes;
}

EnumStatusCode ApplicationModule::initModules(OTUInt16 namespaceIndex, const ApplicationDescription& applicationDescription, const std::vector<EndpointPtr>& localEndpoints, EnumRedundancySupport redMode, const std::vector<tstring> &redUrls)
{
	ObjectPtr objectsFolder = AddressSpaceRoot::instance()->getObjectsFolder();
	if(objectsFolder.isNull())
	{
		_tprintf(_T("ERR BadNode\n"));
		return EnumStatusCode_BadNodeIdUnknown;
	}

	// Store path to server files. It will be accessible for all test modules inheriting from ModuleFolder.
	ModuleFolder::setPathToServerFiles(m_pathToServerFilesFolder);


	// TestVariables	
	ModuleFolderPtr testVariables = new TestVariablesModule();
	EnumStatusCode result = testVariables->initModule(_T("StructoAutomoldVariable"), namespaceIndex, objectsFolder);
	m_modules.push_back(testVariables);

	// TestModule
	if(StatusCode::isGood(result))
	{
		// store the test module extra, to have access to its methods
		TestModulePtr testModule = new TestModule();
		// prosanta "TestModule"
		result = testModule->initModule(_T("StructoAutoMold"), namespaceIndex, objectsFolder);
		m_testModule = testModule;
	}
/*
// store the test module extra, to have access to its methods
		TestModulePtr testModule = new TestModule();
		// prosanta "TestModule"
		result = testModule->initModule(_T("StructoAutoMold"), namespaceIndex, objectsFolder);
		m_testModule = testModule;
*/

    /*
	// Initializing the Modeler Module
	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr modelerModule = new ModelerModule();
		result = modelerModule->initModule(_T("ModelerModule"), namespaceIndex, m_testModule);
		m_modules.push_back(modelerModule);
	}
    */
#if TB5_ALARMS
	// Initialize the eventing module
	/*
	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr eventingModule = new EventingModule();
		// all nodes are in namespace 1
		result = eventingModule->initModule(_T("EventingModule"), 1, m_testModule);
		m_modules.push_back(eventingModule);
	}
	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr boilerMonitoringModule = new BoilerMonitoringModule();
		// all nodes are in namespace 1
		result = boilerMonitoringModule->initModule(_T("BoilerMonitoringModule"), 1, m_testModule);
		m_modules.push_back(boilerMonitoringModule);
	}
	*/
	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr printerModule = new PrinterModule();
		// all nodes are in namespace 1
		result = printerModule->initModule(_T("PrinterModule"), 1, m_testModule);
		m_modules.push_back(printerModule);
	}
/*
	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr offTheShelfAlarmsModule = new OffTheShelfAlarmsModule();
		result = offTheShelfAlarmsModule->initModule(_T("OffTheShelfAlarmsModule"), 2, m_testModule);
		m_modules.push_back(offTheShelfAlarmsModule);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr alarmsAndConditionsStressTestModule = new AlarmsAndConditionsStressTestModule();
		// all nodes are in namespace 1
		result = alarmsAndConditionsStressTestModule->initModule(_T("AlarmsAndConditionsStressTestModule"), 1, m_testModule);
		m_modules.push_back(alarmsAndConditionsStressTestModule);
	}
*/	
#endif

#if TB5_CLIENT
	bool openResult = m_redModule.init(this, applicationDescription, localEndpoints, redMode, redUrls);
	if(StatusCode::isGood(result))
	{
		result = openResult ? EnumStatusCode_Good : EnumStatusCode_BadConfigurationError;
	}
#else
	_UNUSED(redMode);
	_UNUSED(redUrls);
	_UNUSED(applicationDescription);

	Application::instance()->setServerRedundancyMode(EnumRedundancySupport_None);

	for(size_t i = 0; i < localEndpoints.size(); i++)
	{
		EnumStatusCode openResult = localEndpoints[i]->open();
		if(StatusCode::isBad(openResult))
		{
			_tprintf(_T("ERR openResult: %x\n",openResult));
			_tprintf(_T("Failed to open endpoint %d:'%s': %s\n"), i, localEndpoints[i]->getUrl().c_str(), getEnumStatusCodeString(openResult));
			return openResult;
		}
	}

#endif

	return result;
}

void ApplicationModule::initializeStatics()
{
	// create roles - in namespace 1 (server)
	s_RoleIdDefault = new NodeId(1, _T("Role_Default"));
	s_RoleIdGroup1 = new NodeId(1, _T("Role_Group1"));
	s_RoleIdGroup2 = new NodeId(1, _T("Role_Group2"));
	s_RoleIdQuery = new NodeId(1, _T("Role_Query"));
	s_RoleIdRedundancy = new NodeId(1, _T("Role_Redundancy"));
}

void ApplicationModule::uninitializeStatics()
{
	delete s_RoleIdDefault;
	s_RoleIdDefault = NULL;
	delete s_RoleIdGroup1;
	s_RoleIdGroup1 = NULL;
	delete s_RoleIdGroup2;
	s_RoleIdGroup2 = NULL;
	delete s_RoleIdQuery;
	s_RoleIdQuery = NULL;
	delete s_RoleIdRedundancy;
	s_RoleIdRedundancy = NULL;
}

void ApplicationModule::shutDown(unsigned millisecs)
{
	m_shutDownAction.init(this);
	m_shutDownAction.setTimeout(millisecs);
	addAction(&m_shutDownAction);
}

bool ApplicationModule::getShutDownFlag()
{
	return (interlockedGet(&m_shutDown) != 0);
}

void ApplicationModule::setShutDownFlag()
{
	interlockedSet(&m_shutDown, 1);
}

OTUInt32 ApplicationModule::getServiceDelay(EnumNumericNodeId serviceId)
{
	if(m_testModule)
	{
		return m_testModule->getServiceDelay(serviceId);
	}
	return 0;
}

EnumStatusCode ApplicationModule::setServiceDelay(EnumNumericNodeId serviceId, OTUInt32 delay)
{
	if(m_testModule)
	{
		return m_testModule->setServiceDelay(serviceId, delay);
	}
	return EnumStatusCode_BadInvalidState;
}

bool ApplicationModule::addWorkItem(Util_WorkItem* item)
{
	return StatusCode::isGood(m_workItemProcessor->addWorkItem(item));
}

void ApplicationModule::setSourceFolder(const tstring &sourceFolder)
{
	m_sourceFolder = sourceFolder;
}

tstring ApplicationModule::getSourceFolder()
{
	return m_sourceFolder;
}

BEGIN_IGNORE_DEPRECATED()
void ApplicationModule::enableAccessPattern(bool enabled)
{
	s_accessPatternEnabled = enabled;

	if(enabled)
	{
		EnumStatusCode result = Application::instance()->enableUserAccessPattern();
		_UNUSED(result);
		_ASSERTION(StatusCode::isGood(result), _T("Failed to enable user access apttern"));
	}
}
END_IGNORE_DEPRECATED()

void ShutdownAction::execute()
{
	m_pApplicationModule->setShutDownFlag();
}

ThreadReaper::ThreadReaper()
{
	m_head.pthread = 0;
	m_head.next = &m_head;
	m_head.prev = &m_head;
	m_acceptThreads = true;
}

bool ThreadReaper::addThread(Util_Thread *pthread)
{
	if(!m_acceptThreads)
	{
		return false;
	}

	if(pthread == NULL)
	{
		return false;
	}

	threadlist *entry = new threadlist;
	if(entry == NULL)
	{
		return false;
	}

	entry->pthread = pthread;
	m_sync.lock();
	(entry->prev = m_head.prev)->next = entry;
	(entry->next = &m_head)->prev = entry;
	m_sync.unlock();
	return true;
}

void ThreadReaper::timer()
{
	threadlist *entry;

	m_sync.lock();
	entry = m_head.next;
	while(entry != &m_head)
	{
		threadlist *join_me = NULL;
		if(!entry->pthread->isRunning())
		{
			join_me = entry;
		}
		entry = entry->next;
		if(join_me != NULL)
		{
			join_me->next->prev = join_me->prev;
			join_me->prev->next = join_me->next;
			join_me->pthread->wait(INFINITE);
			delete join_me->pthread;
			delete join_me;
		}
	}
	m_sync.unlock();
}

void ThreadReaper::end()
{
	m_sync.lock();
	m_acceptThreads = false;
	threadlist *entry = m_head.next;
	m_sync.unlock();

	while(entry != &m_head)
	{
		threadlist *join_me = entry;
		entry = entry->next;
		if(join_me != NULL)
		{
			join_me->next->prev = join_me->prev;
			join_me->prev->next = join_me->next;
			join_me->pthread->wait(INFINITE);
			delete join_me->pthread;
			delete join_me;
		}
	}

}

void ThreadReaperAction::execute()
{
	m_pApplicationModule->getThreadReaper()->timer();

    for(OTInt32 i = (OTInt32)m_pApplicationModule->GetAppModule()->size() - 1; i >= 0; i--)
    {
        //m_pApplicationModule->m_modules[i]->Update(&m_pApplicationModule);
    }

}

tstring buildAppUri(tstring hostname, tstring applicationIdentifier)
{
	return _T("urn:") + hostname + _T("/") + applicationIdentifier;
}

EnumStatusCode TestVariablesModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isGood(result))
	{
		// Emit ModelChange events for this node
		result = setVersioned(_T("0"));
		_ASSERTION(isVersioned(), _T("Is not versioned"));
	}
	if(StatusCode::isGood(result))
	{
		// Setting internally the test variables folder for the simulation module
		SimVarManager& simVarManager = SimVarManager::getInstance();
		simVarManager.setTestVariablesFolder(this);
	}
	return result;
}

void TestVariablesModule::endModule()
{
	SimVarManager& simVarManager = SimVarManager::getInstance();
	simVarManager.setTestVariablesFolder(NULL);

	ModuleFolder::endModule();
}

