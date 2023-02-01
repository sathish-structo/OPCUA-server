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
#include "TestModuleRolePermission.h"
#include "Statics.h"
#include "Argument.h"
#include "AccessPattern.h"
#include "ServerMethodCallRequest.h"
#include "Application.h"
#include "ServerObject.h"
#include "ApplicationModule.h"
#include "ServerMethodCallTransaction.h"
#include "ServerSession.h"
#include "ServerVariableType.h"
#include "ServerDataType.h"
#include "ServerObjectType.h"
#include "ServerView.h"
#include "ServerBaseEvent.h"
#include "TestEventType.h"
#include "TestEvent.h"
#include "HistorizingVariable.h"

using namespace SoftingOPCToolbox5;


EnumStatusCode RolePermissionModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	if(ApplicationModule::s_accessPatternEnabled)
	{
		// if configuration of access patterns is enabled, this module cannot define specific role permissions
		return result;
	}

	// role permissions may be not accessible - store it in a variable
	tstring stringId = _T("CurrentPermissionsVar");
	Server::VariablePtr currentPermissions = Server::Variable::create();
	currentPermissions->setNodeId(NodeId(namespaceIndex, stringId));
	currentPermissions->setBrowseName(QualifiedName(stringId, namespaceIndex));
	currentPermissions->setDisplayName(LocalizedText(stringId, _T("")));
	currentPermissions->setDataType(Statics::DataTypeId_RolePermissionType);
	currentPermissions->setValueRank(1);
	currentPermissions->setAccessLevel(EnumAccessLevel_CurrentRead);
	currentPermissions->init(NULL);
	this->addReference(Statics::ReferenceTypeId_HasComponent, currentPermissions);

	DataValue initialValue;
	initialValue.setValue((OTInt32)10);

	stringId = _T("RolePermissionsReadWriteVar");
	Server::VariablePtr readWriteVar = TestVariableForAuditEvents::create();
	readWriteVar->setNodeId(NodeId(namespaceIndex, stringId));
	readWriteVar->setBrowseName(QualifiedName(stringId, namespaceIndex));
	readWriteVar->setDisplayName(LocalizedText(stringId, _T("")));
	readWriteVar->setDataType(Statics::DataTypeId_Int32);
	readWriteVar->setDataValue(initialValue);
	readWriteVar->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	readWriteVar->init(NULL);
	this->addReference(Statics::ReferenceTypeId_HasComponent, readWriteVar);

	stringId = _T("RolePermissionsReadOnlyVar");
	Server::VariablePtr readOnlyVar = Server::Variable::create();
	readOnlyVar->setNodeId(NodeId(namespaceIndex, stringId));
	readOnlyVar->setBrowseName(QualifiedName(stringId, namespaceIndex));
	readOnlyVar->setDisplayName(LocalizedText(stringId, _T("")));
	readOnlyVar->setDescription(LocalizedText(_T("The valut attribute is read-only, but all other attributes are writable"), _T("")));
	readOnlyVar->setDataType(Statics::DataTypeId_Int32);
	readOnlyVar->setDataValue(initialValue);
	readOnlyVar->setAccessLevel(EnumAccessLevel_CurrentRead);
	// allow to write any attribute
	readOnlyVar->setWriteMask(EnumWriteMask_AccessLevel |
		EnumWriteMask_ArrayDimensions |
		EnumWriteMask_BrowseName |
		EnumWriteMask_ContainsNoLoops |
		EnumWriteMask_DataType |
		EnumWriteMask_Description |
		EnumWriteMask_DisplayName |
		EnumWriteMask_EventNotifier |
		EnumWriteMask_Executable |
		EnumWriteMask_Historizing |
		EnumWriteMask_InverseName |
		EnumWriteMask_IsAbstract |
		EnumWriteMask_MinimumSamplingInterval |
		EnumWriteMask_NodeClass |
		EnumWriteMask_NodeId |
		EnumWriteMask_Symmetric |
		EnumWriteMask_UserAccessLevel |
		EnumWriteMask_UserExecutable |
		EnumWriteMask_UserWriteMask |
		EnumWriteMask_ValueRank |
		EnumWriteMask_WriteMask |
		EnumWriteMask_ValueForVariableType |
		EnumWriteMask_DataTypeDefinition |
		EnumWriteMask_RolePermissions |
		EnumWriteMask_AccessRestrictions |
		EnumWriteMask_AccessLevelEx);
	readOnlyVar->init(NULL);
	this->addReference(Statics::ReferenceTypeId_HasComponent, readOnlyVar);

	TestRolePermissionMethodPtr testMethod = TestRolePermissionMethod::initialize(namespaceIndex, _T("RolePermissionTestMethod"));
	this->addReference(Statics::ReferenceTypeId_HasComponent, testMethod);

	ModifyRolePermissionsMethodPtr modifyMethod = ModifyRolePermissionsMethod::create();
	modifyMethod->initialize(namespaceIndex, _T("ModifyRolePermissions"), testMethod, readOnlyVar, readWriteVar, currentPermissions);
	this->addReference(Statics::ReferenceTypeId_HasComponent, modifyMethod);

	GetSessionRoleMethodPtr getSessionRolesMethod = GetSessionRoleMethod::create();
	getSessionRolesMethod->initialize(namespaceIndex, _T("GetSessionRoles"));
	this->addReference(Statics::ReferenceTypeId_HasComponent, getSessionRolesMethod);

	ModifySessionRoleMethodPtr modifySessionRolesMethod = ModifySessionRoleMethod::create();
	modifySessionRolesMethod->initialize(namespaceIndex, _T("ModifySessionRoles"));
	this->addReference(Statics::ReferenceTypeId_HasComponent, modifySessionRolesMethod);


	// create some nodes with specific permissions - in general read/write
	std::vector<RolePermissionType> rolePermissions;
	Server::VariablePtr var;
	var = createVar(_T("NotBrowsable"), namespaceIndex);
	rolePermissions.clear();
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	rolePermissions.back().setPermissions(EnumPermissionType_Read);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(EnumPermissionType_Read);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(EnumPermissionType_Write | EnumPermissionType_Read);
	var->setRolePermissions(rolePermissions);

	var = createVar(_T("NotAnonymBrowsable"), namespaceIndex);
	rolePermissions.clear();
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	rolePermissions.back().setPermissions(EnumPermissionType_Read);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Write | EnumPermissionType_Read);
	var->setRolePermissions(rolePermissions);

	Server::ObjectPtr obj = createObj(_T("NoBrowseObj"), namespaceIndex);
	rolePermissions.clear();
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	rolePermissions.back().setPermissions(EnumPermissionType_Read);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_ReceiveEvents);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_ReceiveEvents);
	obj->setRolePermissions(rolePermissions);

	TestRolePermissionMethodPtr testMethod1 = TestRolePermissionMethod::initialize(namespaceIndex, _T("NoBrowseMeth"));
	this->addReference(Statics::ReferenceTypeId_HasComponent, testMethod1);
	rolePermissions.clear();
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	rolePermissions.back().setPermissions(EnumPermissionType_Call);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Call);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Call);
	testMethod1->setRolePermissions(rolePermissions);

	// Create some nodes for browse test
	result = initBrowseTestNodes(namespaceIndex);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	// Create some nodes for event reporting
	result = initEventTestNodes(namespaceIndex);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	// Create some nodes for write mask testing
	result = initWriteMaskTestNodes(namespaceIndex);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	return result;
}

void RolePermissionModule::endModule()
{
	endWriteMaskTestNodes();
	endEventTestNodes();
}

Server::VariablePtr RolePermissionModule::createVar(tstring name, OTUInt16 namespaceIndex)
{
	Server::VariablePtr var = TestVariableForAuditEvents::create();
	var->setNodeId(NodeId(namespaceIndex, name));
	var->setBrowseName(QualifiedName(name, namespaceIndex));
	var->setDisplayName(LocalizedText(name, _T("")));
	var->setDataType(Statics::DataTypeId_UInt32);
	var->setValueRank(ValueRank_Scalar);
	var->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	Value val((OTUInt32)10);
	DataValue dv;
	dv.setValue(val);
	var->setDataValue(dv);
	var->init(NULL);
	addReference(Statics::ReferenceTypeId_HasComponent, var);
	return var;
}

SoftingOPCToolbox5::Server::ObjectPtr RolePermissionModule::createObj(tstring name, OTUInt16 namespaceIndex)
{
	Server::ObjectPtr obj = Server::Object::create();
	obj->setNodeId(NodeId(namespaceIndex, name));
	obj->setBrowseName(QualifiedName(name, namespaceIndex));
	obj->setDisplayName(LocalizedText(name, _T("")));
	obj->init(NULL);
	addReference(Statics::ReferenceTypeId_HasComponent, obj);

	return obj;
}

EnumStatusCode RolePermissionModule::initBrowseTestNodes(OTUInt16 namespaceIndex)
{
	EnumStatusCode result = EnumStatusCode_Good;

	tstring name;

	// BrowseTest folder (not browsable for anonymous)
	std::vector<RolePermissionType> rolePermissions;
	ObjectPtr browseTestFolder = Object::create();
	name = _T("BrowseTest");
	browseTestFolder->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.BrowseTest")));
	browseTestFolder->setBrowseName(QualifiedName(name, namespaceIndex));
	browseTestFolder->setDisplayName(LocalizedText(name, _T("")));
	result = browseTestFolder->init(Statics::ObjectTypeId_FolderType);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	rolePermissions.clear();
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse);
	browseTestFolder->setRolePermissions(rolePermissions);
	addReference(Statics::ReferenceTypeId_Organizes, browseTestFolder);

	// Create a variable with no role permissions
	Server::VariablePtr varNoRolePermissions = Variable::create();
	name = _T("NoRolePermissions");
	varNoRolePermissions->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.NoRolePermissions")));
	varNoRolePermissions->setBrowseName(QualifiedName(name, namespaceIndex));
	varNoRolePermissions->setDisplayName(LocalizedText(name, _T("")));
	result = varNoRolePermissions->init(NULL);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	browseTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, varNoRolePermissions);

	// Create a variable with browse permission for all roles
	Server::VariablePtr varAllBrowse = Variable::create();
	name = _T("AllBrowse");
	varAllBrowse->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.AllBrowse")));
	varAllBrowse->setBrowseName(QualifiedName(name, namespaceIndex));
	varAllBrowse->setDisplayName(LocalizedText(name, _T("")));
	result = varAllBrowse->init(NULL);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	rolePermissions.clear();
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Write | EnumPermissionType_Read);
	browseTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, varAllBrowse);

	// Create some nodes browsable only for RoleIdGroup1 (usr)
	// Note: Same set of nodes as in "BrowseTest" module
	rolePermissions.clear();
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse);

	//////////////////////////////////////////////////////////////////////////
	// Create variable 'NodeClass_Variable'
	//////////////////////////////////////////////////////////////////////////
	VariablePtr nodeClassVariable = Variable::create();
	name = (_T("NodeClass_Variable"));
	nodeClassVariable->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.NodeClass_Variable")));
	nodeClassVariable->setBrowseName(QualifiedName(name, namespaceIndex));
	nodeClassVariable->setDisplayName(LocalizedText(name, _T("")));
	result = nodeClassVariable->init(Statics::VariableTypeId_BaseDataVariableType);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	nodeClassVariable->setRolePermissions(rolePermissions);
	browseTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassVariable);

	//////////////////////////////////////////////////////////////////////////
	// Create variable type 'NodeClass_VariableType'
	//////////////////////////////////////////////////////////////////////////
	VariableTypePtr nodeClassVariableType = VariableType::create();
	name = _T("NodeClass_VariableType");
	nodeClassVariableType->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.NodeClass_VariableType")));
	nodeClassVariableType->setBrowseName(QualifiedName(name, namespaceIndex));
	nodeClassVariableType->setDisplayName(LocalizedText(name, _T("")));
	result = nodeClassVariableType->init(NULL);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	nodeClassVariableType->setRolePermissions(rolePermissions);
	browseTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassVariableType);

	//////////////////////////////////////////////////////////////////////////
	// Create object 'NodeClass_Object'
	//////////////////////////////////////////////////////////////////////////
	ObjectPtr nodeClassObject = Object::create();
	name = _T("NodeClass_Object");
	nodeClassObject->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.NodeClass_Object")));
	nodeClassObject->setBrowseName(QualifiedName(name, namespaceIndex));
	nodeClassObject->setDisplayName(LocalizedText(name, _T("")));
	result = nodeClassObject->init(Statics::ObjectTypeId_BaseObjectType);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	nodeClassObject->setRolePermissions(rolePermissions);
	browseTestFolder->addReference(Statics::ReferenceTypeId_Organizes, nodeClassObject);

	//////////////////////////////////////////////////////////////////////////
	// Create object type 'NodeClass_ObjectType'
	//////////////////////////////////////////////////////////////////////////
	ObjectTypePtr nodeClassObjectType = ObjectType::create();
	name = _T("NodeClass_ObjectType");
	nodeClassObjectType->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.NodeClass_ObjectType")));
	nodeClassObjectType->setBrowseName(QualifiedName(name, namespaceIndex));
	nodeClassObjectType->setDisplayName(LocalizedText(name, _T("")));
	result = nodeClassObjectType->init(NULL);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	nodeClassObjectType->setRolePermissions(rolePermissions);
	browseTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassObjectType);

	//////////////////////////////////////////////////////////////////////////
	// Create method 'NodeClass_Method'
	//////////////////////////////////////////////////////////////////////////
	MethodPtr nodeClassMethod = Method::create();
	name = _T("NodeClass_Method");
	nodeClassMethod->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.NodeClass_Method")));
	nodeClassMethod->setBrowseName(QualifiedName(name, namespaceIndex));
	nodeClassMethod->setDisplayName(LocalizedText(name, _T("")));
	result = nodeClassMethod->init(NULL);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	nodeClassMethod->setRolePermissions(rolePermissions);
	browseTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassMethod);

	//////////////////////////////////////////////////////////////////////////
	// Create data type 'NodeClass_DataType'
	//////////////////////////////////////////////////////////////////////////
	DataTypePtr nodeClassDataType = DataType::create();
	name = _T("NodeClass_DataType");
	nodeClassDataType->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.NodeClass_DataType")));
	nodeClassDataType->setBrowseName(QualifiedName(name, namespaceIndex));
	nodeClassDataType->setDisplayName(LocalizedText(name, _T("")));
	result = nodeClassDataType->init(NULL);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	nodeClassDataType->setRolePermissions(rolePermissions);
	browseTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassDataType);

	//////////////////////////////////////////////////////////////////////////
	// Create view 'NodeClass_View'
	//////////////////////////////////////////////////////////////////////////
	ViewPtr nodeClassView = View::create();
	name = _T("NodeClass_View");
	nodeClassView->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.NodeClass_View")));
	nodeClassView->setBrowseName(QualifiedName(name, namespaceIndex));
	nodeClassView->setDisplayName(LocalizedText(name, _T("")));
	result = nodeClassView->init(NULL);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	nodeClassView->setRolePermissions(rolePermissions);
	browseTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassView);

	ViewPtr nodeClassView2 = View::create();
	name = _T("NodeClass_View2");
	nodeClassView2->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.NodeClass_View2")));
	nodeClassView2->setBrowseName(QualifiedName(name, namespaceIndex));
	nodeClassView2->setDisplayName(LocalizedText(name, _T("")));
	nodeClassView2->setContainsNoLoops(true);
	nodeClassView2->setEventNotifier(EnumEventNotifier_SubscribeToEvents);
	result = nodeClassView2->init(NULL);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	nodeClassView2->setRolePermissions(rolePermissions);
	browseTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassView2);

	return result;
}

EnumStatusCode RolePermissionModule::initEventTestNodes(OTUInt16 namespaceIndex)
{
	EnumStatusCode result = EnumStatusCode_Good;

	// create the event type
	TestEventType::initialize(namespaceIndex);
	TestUserSpecificEventType::initialize(namespaceIndex);

	// EventTest folder (not browsable for anonymous, only group 2 shall receive events)
	m_eventTestFolder = Object::create();
	m_eventTestFolder->setNodeId(NodeId(namespaceIndex, _T("RolePermissionTest.EventTest")));
	m_eventTestFolder->setDisplayName(LocalizedText(_T("EventTest"), _T("")));
	m_eventTestFolder->setDescription(LocalizedText(_T("Both group 1 and group 2 can receive events"), _T("")));
	m_eventTestFolder->setBrowseName(QualifiedName(_T("EventTest"), namespaceIndex));
	m_eventTestFolder->setEventNotifier(EnumEventNotifier_SubscribeToEvents);
	result = m_eventTestFolder->init(Statics::ObjectTypeId_FolderType);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}

	// the rights which shall be valid for all users on all nodes
	EnumPermissionType commonRights = EnumPermissionType_Browse;

	// general event folder has permissions to receive events both for group1 and group2
	std::vector<RolePermissionType> folderRolePermissions;
	folderRolePermissions.clear();

	folderRolePermissions.push_back(RolePermissionType());
	folderRolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	folderRolePermissions.back().setPermissions(commonRights);

	folderRolePermissions.push_back(RolePermissionType());
	folderRolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	folderRolePermissions.back().setPermissions(commonRights | EnumPermissionType_ReceiveEvents | EnumPermissionType_Call);

	folderRolePermissions.push_back(RolePermissionType());
	folderRolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	folderRolePermissions.back().setPermissions(commonRights | EnumPermissionType_ReceiveEvents | EnumPermissionType_Call);

	m_eventTestFolder->setRolePermissions(folderRolePermissions);
	addReference(Statics::ReferenceTypeId_Organizes, m_eventTestFolder);


	// create generic objects which can subscribe to events and events for them
	std::vector<ObjectPtr> eventObjects;
	std::vector<BaseEventPtr> testEvents;

	// create two events which can be received by both group1 and group2 and one event, received only by group2
	std::vector<INodeId*> eventTypesToCreate;
	eventTypesToCreate.push_back(TestEventType::s_typeId);
	eventTypesToCreate.push_back(TestEventType::s_typeId);
	eventTypesToCreate.push_back(TestUserSpecificEventType::s_typeId);

	for(size_t i = 0; i < eventTypesToCreate.size(); i++)
	{
		OTChar szNumber[10];
		_sntprintf(szNumber, 10, _T("_%u"), (OTUInt32)i);
		tstring nodeIdString = _T("RolePermissionTest.EventObject");
		nodeIdString += szNumber;
		tstring browseNameString = _T("EventObject");
		browseNameString += szNumber;


		eventObjects.push_back(Object::create());
		eventObjects.back()->setNodeId(NodeId(namespaceIndex, nodeIdString));
		eventObjects.back()->setDisplayName(LocalizedText(browseNameString, _T("")));
		eventObjects.back()->setBrowseName(QualifiedName(browseNameString, namespaceIndex));
		eventObjects.back()->setEventNotifier(EnumEventNotifier_SubscribeToEvents);
		result = eventObjects.back()->init(Statics::ObjectTypeId_BaseObjectType);
		if(!StatusCode::isSUCCEEDED(result))
		{
			return result;
		}

		m_eventTestFolder->addReference(Statics::ReferenceTypeId_Organizes, eventObjects.back());
		m_eventTestFolder->addReference(Statics::ReferenceTypeId_HasNotifier, eventObjects.back());

		browseNameString = _T("Event");
		browseNameString += szNumber;
		testEvents.push_back(TestEvent::create());
		testEvents.back()->setNodeId(NodeId(namespaceIndex, browseNameString));
		testEvents.back()->setDisplayName(LocalizedText(_T("Event"), _T("")));
		testEvents.back()->setBrowseName(QualifiedName(browseNameString, namespaceIndex));
		testEvents.back()->init(eventTypesToCreate[i]);
		testEvents.back()->setSourceNode(eventObjects.back()->getNodeId());
		eventObjects.back()->addReference(Statics::ReferenceTypeId_Organizes, testEvents.back());
		eventObjects.back()->addReference(Statics::ReferenceTypeId_HasNotifier, testEvents.back());
	}

	// configure objects and events according to role permissions
	// # 0 can receive 
	eventObjects[0]->setDescription(LocalizedText(_T("Both group 1 and group 2 can receive events"), _T("")));
	eventObjects[0]->setDisplayName(LocalizedText(_T("ObjectNotRestricted"), _T("")));

	std::vector<RolePermissionType> rolePermissions;
	rolePermissions.clear();
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	rolePermissions.back().setPermissions(commonRights);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(commonRights);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(commonRights | EnumPermissionType_ReceiveEvents);
	eventObjects[1]->setDescription(LocalizedText(_T("Only group 2 can receive events"), _T("")));
	eventObjects[1]->setDisplayName(LocalizedText(_T("RestrictedToRole2"), _T("")));
	eventObjects[1]->setRolePermissions(rolePermissions);

	eventObjects[2]->setDescription(LocalizedText(_T("Only group 2 can receive events"), _T("")));
	eventObjects[2]->setDisplayName(LocalizedText(_T("RestrictedByEventType"), _T("")));

	ReportEventsMethodPtr reportMethod = ReportEventsMethod::create();
	reportMethod->initialize(namespaceIndex, _T("ReportEvents"), testEvents, eventObjects);
	m_eventTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, reportMethod);
	return result;
}

void RolePermissionModule::endEventTestNodes()
{
	if(m_eventTestFolder)
	{
		m_eventTestFolder->removeTree();
	}
	TestUserSpecificEventType::uninitialize();
	TestEventType::uninitialize();
}

EnumStatusCode RolePermissionModule::initWriteMaskTestNodes(OTUInt16 namespaceIndex)
{
	EnumStatusCode result = EnumStatusCode_Good;

	// WriteMaskTest folder
	tstring name;
	tstring nodeIdString;
	tstring folderNodeIdString;
	name = _T("WriteMaskTest");
	nodeIdString = _T("RolePermissionTest");
	nodeIdString += _T(".");
	nodeIdString += name;
	folderNodeIdString = nodeIdString;
	m_writeMaskTestFolder = Object::create();
	m_writeMaskTestFolder->setNodeId(NodeId(namespaceIndex, nodeIdString));
	m_writeMaskTestFolder->setDisplayName(LocalizedText(name, _T("")));
	m_writeMaskTestFolder->setBrowseName(QualifiedName(name, namespaceIndex));
	result = m_writeMaskTestFolder->init(Statics::ObjectTypeId_FolderType);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}

	addReference(Statics::ReferenceTypeId_Organizes, m_writeMaskTestFolder);

	// all users shall be able to see and read the variables and call the methods
	EnumPermissionType commonRights = EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Call;
	std::vector<RolePermissionType> rolePermissions;
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	rolePermissions.back().setPermissions(commonRights);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(commonRights | EnumPermissionType_WriteAttribute);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(commonRights);

	// Create a variable with writable attributes
	Server::VariablePtr varAttWritable = Variable::create();
	name = _T("AttributeWritable");
	nodeIdString = folderNodeIdString;
	nodeIdString += _T(".");
	nodeIdString += name;
	varAttWritable->setNodeId(NodeId(namespaceIndex, nodeIdString));
	varAttWritable->setBrowseName(QualifiedName(name, namespaceIndex));
	varAttWritable->setDisplayName(LocalizedText(name, _T("")));
	// attribute display name and value rank are writable in principle
	varAttWritable->setWriteMask(EnumWriteMask_DisplayName | EnumWriteMask_DataType);
	varAttWritable->setDescription(LocalizedText(_T("For group 1"), _T("")));

	varAttWritable->setRolePermissions(rolePermissions);
	result = varAttWritable->init(NULL);
	if(!StatusCode::isSUCCEEDED(result))
	{
		return result;
	}
	m_writeMaskTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, varAttWritable);

	// create a method where some users are allowed to write attribute (e.g. call bit)
	name = _T("TestMethod");
	nodeIdString = folderNodeIdString;
	nodeIdString += _T(".");
	nodeIdString += name;
	commonRights = EnumPermissionType_Browse;
	TestRolePermissionMethodPtr testMethodAttrib = TestRolePermissionMethod::initialize(namespaceIndex, nodeIdString);
	testMethodAttrib->setBrowseName(QualifiedName(name, namespaceIndex));
	testMethodAttrib->setDisplayName(LocalizedText(name, _T("")));
	// attribute executable bit as writable in principle
	testMethodAttrib->setWriteMask(EnumWriteMask_Executable);
	testMethodAttrib->setRolePermissions(rolePermissions);
	m_writeMaskTestFolder->addReference(Statics::ReferenceTypeId_HasComponent, testMethodAttrib);

	return result;
}

void RolePermissionModule::endWriteMaskTestNodes()
{

}

ModifyRolePermissionsMethod::ModifyRolePermissionsMethod()
{
	m_testMethod = NULL;
	m_readWriteVar = NULL;
	m_readOnlyVar = NULL;
	m_currentPermissions = NULL;
}

ModifyRolePermissionsMethod::~ModifyRolePermissionsMethod()
{
	// just release nodes
	m_testMethod = NULL;
	m_readOnlyVar = NULL;
	m_readWriteVar = NULL;
	m_currentPermissions = NULL;
}

ModifyRolePermissionsMethodPtr ModifyRolePermissionsMethod::create()
{
	return new ModifyRolePermissionsMethod();
}

EnumStatusCode ModifyRolePermissionsMethod::initialize(OTUInt16 namespaceIndex,const tstring& name,
	TestRolePermissionMethodPtr testMethod, Server::VariablePtr readOnlyVar, Server::VariablePtr readWriteVar, Server::VariablePtr currentPermissions)
{
	m_readOnlyVar = readOnlyVar;
	m_readWriteVar = readWriteVar;
	m_testMethod = testMethod;
	m_currentPermissions = currentPermissions;

	// initial role permissions
	std::vector<RolePermissionType> rolePermissions;
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_Call);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_Call);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_Call);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdQuery);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_Call);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdRedundancy);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_Call);

	m_readWriteVar->setRolePermissions(rolePermissions);
	m_readOnlyVar->setRolePermissions(rolePermissions);
	m_testMethod->setRolePermissions(rolePermissions);
	DataValue dv;
	Value val;
	val.setRolePermissionTypeArray(rolePermissions);
	dv.setValue(val);
	m_currentPermissions->setDataValue(dv);

	setNodeId(NodeId(namespaceIndex, name));
	setBrowseName(QualifiedName(name, namespaceIndex));
	setDisplayName(LocalizedText(name, _T("")));
	setDescription(LocalizedText(_T("Allows to modify the role permissions"), _T("")));

	std::vector<Argument> inputArguments;
	Argument accessFieldArg;
	accessFieldArg.setName(_T("RolePermissions"));
	tstring description = _T("Defines the RolePermissions for the nodes. Role NodeIds:\n");
	description += ApplicationModule::s_RoleIdDefault->toString();
	description += _T("\n");
	description += ApplicationModule::s_RoleIdGroup1->toString();
	description += _T("\n");
	description += ApplicationModule::s_RoleIdGroup2->toString();
	description += _T("\n");
	description += ApplicationModule::s_RoleIdQuery->toString();
	accessFieldArg.setDescription(LocalizedText(description, _T("")));
	accessFieldArg.setDataType(Statics::DataTypeId_RolePermissionType);
	accessFieldArg.setValueRank(1);
	inputArguments.push_back(accessFieldArg);

	tstring inpArgName = name + _T("_InputArguments");
	NodeId inputArgId(namespaceIndex, inpArgName);
	setInputArguments(&inputArgId, inputArguments);
	init(NULL);

	return EnumStatusCode_Good;
}

EnumStatusCode ModifyRolePermissionsMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArguments = request->getInputArguments();
	std::vector<EnumStatusCode> inputArgumentResults;

	EnumStatusCode result = validateInputArguments(inputArguments, inputArgumentResults);
	if(StatusCode::isGood(result))
	{
		std::vector<RolePermissionType> newRolePermissions = inputArguments[0].getRolePermissionTypeArray();
		m_readWriteVar->setRolePermissions(newRolePermissions);
		m_readOnlyVar->setRolePermissions(newRolePermissions);
		m_testMethod->setRolePermissions(newRolePermissions);
		DataValue dv;
		Value val;
		val.setRolePermissionTypeArray(newRolePermissions);
		dv.setValue(val);
		m_currentPermissions->setDataValue(dv);
	}

	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

TestRolePermissionMethod::TestRolePermissionMethod()
{
}

TestRolePermissionMethod::~TestRolePermissionMethod()
{
}

TestRolePermissionMethodPtr TestRolePermissionMethod::initialize(OTUInt16 namespaceIndex, const tstring& name)
{
	tstringstream ss;
	TestRolePermissionMethodPtr testMethod = new TestRolePermissionMethod();
	testMethod->setNodeId(NodeId(namespaceIndex, name));
	testMethod->setBrowseName(QualifiedName(name, namespaceIndex));
	testMethod->setDisplayName(LocalizedText(name, _T("")));
	testMethod->setDescription(LocalizedText(_T("Test method for role permissions, it adds two values"), _T("")));

	std::vector<Argument> inputArguments;
	std::vector<Argument> outputArguments;
	Argument a;
	a.setName(_T("A"));
	a.setDescription(LocalizedText(_T("The first summand"), _T("")));
	a.setDataType(Statics::DataTypeId_Int32);
	a.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(a);

	Argument b;
	b.setName(_T("B"));
	b.setDescription(LocalizedText(_T("The second summand"), _T("")));
	b.setDataType(Statics::DataTypeId_Int32);
	b.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(b);

	Argument sum;
	sum.setName(_T("Sum"));
	sum.setDescription(LocalizedText(_T("The sum of the addition"), _T("")));
	sum.setDataType(Statics::DataTypeId_Int32);
	sum.setValueRank(ValueRank_Scalar);
	outputArguments.push_back(sum);

	ss << name << _T("_InputArguments");
	NodeId inputArgId(namespaceIndex, ss.str());
	ss.clear();
	testMethod->setInputArguments(&inputArgId, inputArguments);
	ss << name << _T("_OutputArguments");
	NodeId outputArgId(namespaceIndex, ss.str());
	ss.clear();
	testMethod->setOutputArguments(&outputArgId, outputArguments);
	testMethod->init(NULL);

	return testMethod;
}

EnumStatusCode TestRolePermissionMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArguments = request->getInputArguments();
	std::vector<Value> outputArguments;
	std::vector<EnumStatusCode> inputArgumentResults;

	EnumStatusCode result = validateInputArguments(inputArguments, inputArgumentResults);
	if(StatusCode::isGood(result))
	{
		OTInt32 a = inputArguments[0].getInt32();
		OTInt32 b = inputArguments[1].getInt32();

		OTInt32 sum = add(a, b);

		outputArguments.push_back(Value(sum));
		request->setOutputArguments(outputArguments);
	}

	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}
OTInt32 TestRolePermissionMethod::add(OTInt32 a, OTInt32 b)
{
	return a + b;
}

ModifySessionRoleMethod::~ModifySessionRoleMethod()
{

}

ModifySessionRoleMethodPtr ModifySessionRoleMethod::create()
{
	return new ModifySessionRoleMethod();
}

EnumStatusCode ModifySessionRoleMethod::initialize(OTUInt16 namespaceIndex, const tstring& name)
{
	setNodeId(NodeId(namespaceIndex, name));
	setBrowseName(QualifiedName(name, namespaceIndex));
	setDisplayName(LocalizedText(name, _T("")));
	setDescription(LocalizedText(_T("Allows to modify the session roles"), _T("")));

	std::vector<Argument> inputArguments;
	inputArguments.push_back(Argument());
	inputArguments.back().setName(_T("AddRole"));
	tstring description = _T("Adds or removed a role (false will remove the role)");
	inputArguments.back().setDescription(LocalizedText(description, _T("")));
	inputArguments.back().setDataType(Statics::DataTypeId_Boolean);
	inputArguments.back().setValueRank(ValueRank_ScalarOrOneDimension);

	inputArguments.push_back(Argument());
	inputArguments.back().setName(_T("RoleId"));
	description = _T("The role to add / remove	");
	inputArguments.back().setDescription(LocalizedText(description, _T("")));
	inputArguments.back().setDataType(Statics::DataTypeId_NodeId);
	inputArguments.back().setValueRank(ValueRank_ScalarOrOneDimension);


	tstring inpArgName = name + _T("_InputArguments");
	NodeId inputArgId(namespaceIndex, inpArgName);
	setInputArguments(&inputArgId, inputArguments);
	init(NULL);

	return EnumStatusCode_Good;
}

EnumStatusCode ModifySessionRoleMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArguments = request->getInputArguments();
	std::vector<EnumStatusCode> inputArgumentResults;

	std::vector<bool> addRemove;
	std::vector<NodeId> affectedRoles;

	Server::SessionPtr session = transaction->getSession();
	std::vector<NodeId> sessionRoles;

	EnumStatusCode result = validateInputArguments(inputArguments, inputArgumentResults);
	// argument handling
	if(StatusCode::isGood(result))
	{
		if(inputArguments[0].getArrayType() == EnumValueArrayType_Scalar)
		{
			addRemove.push_back(inputArguments[0].getBoolean());
		}
		else
		{
			addRemove = inputArguments[0].getBooleanArray();
		}

		if(inputArguments[1].getArrayType() == EnumValueArrayType_Scalar)
		{
			affectedRoles.push_back(inputArguments[1].getNodeId());
		}
		else
		{
			affectedRoles = inputArguments[1].getNodeIdArray();
		}

		if(addRemove.size() != affectedRoles.size())
		{
			result = EnumStatusCode_BadTypeMismatch;
		}
	}

	// add or remove
	if(StatusCode::isGood(result))
	{
		sessionRoles = session->getRoles();

		for(size_t a = 0; a < affectedRoles.size(); a++)
		{
			if(addRemove[a])
			{
				// add
				for(size_t s = 0; s < sessionRoles.size(); s++)
				{
					if(sessionRoles[s] == affectedRoles[a])
					{
						result = EnumStatusCode_BadAlreadyExists;
						break;	// for session roles
					}
				}
				sessionRoles.push_back(affectedRoles[a]);
			}
			else
			{
				// remove
				result = EnumStatusCode_BadNotFound;
				for(size_t s = 0; s < sessionRoles.size(); s++)
				{
					if(sessionRoles[s] == affectedRoles[a])
					{
						sessionRoles.erase(sessionRoles.begin() + s);
						result = EnumStatusCode_Good;
						break;	// find in session roles
					}
				}
			}
			if(StatusCode::isBad(result))
			{
				break;	// for affectedRoles
			}
		}
	}

	// apply new settings
	if(StatusCode::isGood(result))
	{
		result = session->setRoles(sessionRoles);
	}

	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

ModifySessionRoleMethod::ModifySessionRoleMethod()
{

}

GetSessionRoleMethod::~GetSessionRoleMethod()
{

}

GetSessionRoleMethodPtr GetSessionRoleMethod::create()
{
	return new GetSessionRoleMethod();
}

EnumStatusCode GetSessionRoleMethod::initialize(OTUInt16 namespaceIndex, const tstring& name)
{
	setNodeId(NodeId(namespaceIndex, name));
	setBrowseName(QualifiedName(name, namespaceIndex));
	setDisplayName(LocalizedText(name, _T("")));
	setDescription(LocalizedText(_T("Returns the session roles"), _T("")));

	std::vector<Argument> outputArguments;
	outputArguments.push_back(Argument());
	outputArguments.back().setName(_T("SessionRoles"));
	outputArguments.back().setDataType(Statics::DataTypeId_NodeId);
	outputArguments.back().setValueRank(1);


	tstring outpArgName = name + _T("_OutputArguments");
	NodeId outputArgId(namespaceIndex, outpArgName);
	setOutputArguments(&outputArgId, outputArguments);
	init(NULL);

	return EnumStatusCode_Good;
}

EnumStatusCode GetSessionRoleMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	std::vector<Value> inputArguments = request->getInputArguments();
	std::vector<Value> outputArguments;
	std::vector<EnumStatusCode> inputArgumentResults;

	EnumStatusCode result = validateInputArguments(inputArguments, inputArgumentResults);
	if(StatusCode::isGood(result))
	{
		outputArguments.push_back(Value());
		outputArguments.back().setNodeIdArray(transaction->getSession()->getRoles());
	}

	request->setOutputArguments(outputArguments);
	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

GetSessionRoleMethod::GetSessionRoleMethod()
{

}

ReportEventsMethod::~ReportEventsMethod()
{

}

ReportEventsMethodPtr ReportEventsMethod::create()
{
	return new ReportEventsMethod();
}

EnumStatusCode ReportEventsMethod::initialize(OTUInt16 namespaceIndex, const tstring& name,
	std::vector<SoftingOPCToolbox5::Server::BaseEventPtr> eventsToReport,
	std::vector<SoftingOPCToolbox5::Server::ObjectPtr> nodesToReportEvent)
{
	if(eventsToReport.size() != nodesToReportEvent.size())
	{
		return EnumStatusCode_BadInvalidArgument;
	}
	setNodeId(NodeId(namespaceIndex, name));
	setBrowseName(QualifiedName(name, namespaceIndex));
	setDisplayName(LocalizedText(name, _T("")));
	setDescription(LocalizedText(_T("Reports events on all objects"), _T("")));

	m_eventsToReport = eventsToReport;
	m_nodesToReportEvent = nodesToReportEvent;
	return EnumStatusCode_Good;
}

EnumStatusCode ReportEventsMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArguments = request->getInputArguments();
	std::vector<Value> outputArguments;
	std::vector<EnumStatusCode> inputArgumentResults;

	EnumStatusCode result = validateInputArguments(inputArguments, inputArgumentResults);
	if(StatusCode::isGood(result))
	{
		// report events
		for(size_t i = 0; i < m_eventsToReport.size(); i++)
		{
			m_eventsToReport[i]->report(m_nodesToReportEvent[i]->getNodeId());
		}
	}

	request->setOutputArguments(outputArguments);
	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

ReportEventsMethod::ReportEventsMethod()
{

}
