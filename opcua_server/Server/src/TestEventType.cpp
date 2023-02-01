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
#include "TestEventType.h"
#include "ServerObjectType.h"
#include "ServerAddressSpaceRoot.h"
#include "Statics.h"
#include "ServerVariable.h"



#include "ApplicationModule.h"

NodeId* TestEventType::s_typeId = NULL;
QualifiedName* TestEventType::s_propertyBrowseName = NULL;
NodeId* TestEventType::s_propertyId = NULL;
QualifiedName* TestEventType::s_userSpecificPropertyBrowseName = NULL;
NodeId* TestEventType::s_userSpecificPropertyId = NULL;

TestEventType::TestEventType()
{

}

TestEventType::~TestEventType()
{

}

EnumStatusCode TestEventType::initialize(OTUInt16 namespaceIndex)
{
	s_typeId = new NodeId(namespaceIndex, _T("StructoAutomoldServer.TestEventType"));
	s_propertyId = new NodeId(namespaceIndex, _T("StructoAutomoldServer.TestEventType.TestProperty"));
	s_userSpecificPropertyId = new NodeId(namespaceIndex, _T("StructoAutomoldServer.TestEventType.UserProperty"));
	s_propertyBrowseName = new QualifiedName(_T("TestProperty"), namespaceIndex);
	s_userSpecificPropertyBrowseName = new QualifiedName(_T("UserSpecifcProperty"), namespaceIndex);

	NodeId baseEventTypeId(Statics::ObjectTypeId_BaseEventType);
	NodeId propertyTypeId(0, EnumNumericNodeId_PropertyType);
	NodeId mandatoryId(0, EnumNumericNodeId_ModellingRule_Mandatory);
	BaseNodePtr baseEventType = AddressSpaceRoot::instance()->getNode(&baseEventTypeId);
	BaseNodePtr propertyType = AddressSpaceRoot::instance()->getNode(&propertyTypeId);
	BaseNodePtr mandatoryObject = AddressSpaceRoot::instance()->getNode(&mandatoryId);

	ObjectPointer<TestEventType> typeNode = new TestEventType();
	typeNode->setBrowseName(QualifiedName(_T("TestEventType"), namespaceIndex));
	typeNode->setDisplayName(LocalizedText(_T("TestEventType"), _T("en")));
	typeNode->setDescription(LocalizedText(_T("Event type with userspecific property"), _T("en")));
	typeNode->setIsAbstract(false);
	typeNode->setNodeId(s_typeId);
	typeNode->setWriteMask(0);

	VariablePtr testProperty = TestVariableForAuditEvents::create();
	testProperty->setBrowseName(s_propertyBrowseName);
	testProperty->setDisplayName(LocalizedText(_T("TestProperty"), _T("en")));
	testProperty->setNodeId(s_propertyId);
	testProperty->setWriteMask(0);
	testProperty->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	testProperty->setDataType(NodeId(EnumNumericNodeId_String));
	testProperty->addReference(Statics::ReferenceTypeId_HasTypeDefinition, propertyType);
	testProperty->addReference(Statics::ReferenceTypeId_HasModellingRule, mandatoryObject);
	typeNode->addReference(Statics::ReferenceTypeId_HasProperty, testProperty);

	testProperty = TestVariableForAuditEvents::create();
	testProperty->setBrowseName(s_userSpecificPropertyBrowseName);
	testProperty->setDisplayName(LocalizedText(_T("TestUserProperty"), _T("en")));
	testProperty->setDescription(LocalizedText(_T("Readable for group 2"), _T("")));
	testProperty->setNodeId(s_userSpecificPropertyId);
	testProperty->setWriteMask(0);
	testProperty->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	testProperty->setDataType(NodeId(EnumNumericNodeId_String));
	std::vector<RolePermissionType> rolePermissions;
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse); // browsable not "really" needed, but otherwise generic clients cannot select the property
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse); // browsable not "really" needed, but otherwise generic clients cannot select the property
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read); // browsable not "really" needed, but otherwise generic clients cannot select the property
	testProperty->setRolePermissions(rolePermissions);
	testProperty->addReference(Statics::ReferenceTypeId_HasTypeDefinition, propertyType);
	testProperty->addReference(Statics::ReferenceTypeId_HasModellingRule, mandatoryObject);
	typeNode->addReference(Statics::ReferenceTypeId_HasProperty, testProperty);

	baseEventType->addReference(Statics::ReferenceTypeId_HasSubtype, typeNode);
	typeNode->insertTree();

	return EnumStatusCode_Good;
}

void TestEventType::uninitialize()
{
	BaseNodePtr typeNode = AddressSpaceRoot::instance()->getNode(s_typeId);
	if(typeNode)
	{
		typeNode->removeTree();
	}

	delete s_typeId;
	s_typeId = NULL;

	delete s_propertyBrowseName;
	s_propertyBrowseName = NULL;
	delete s_propertyId;
	s_propertyId = NULL;

	delete s_userSpecificPropertyBrowseName;
	s_userSpecificPropertyBrowseName = NULL;
	delete s_userSpecificPropertyId;
	s_userSpecificPropertyId = NULL;
}


NodeId* TestUserSpecificEventType::s_typeId = NULL;

TestUserSpecificEventType::TestUserSpecificEventType()
{

}

TestUserSpecificEventType::~TestUserSpecificEventType()
{

}

EnumStatusCode TestUserSpecificEventType::initialize(OTUInt16 namespaceIndex)
{
	BaseNodePtr baseEventType = AddressSpaceRoot::instance()->getNode(TestEventType::s_typeId);
	if(!baseEventType)
	{
		return EnumStatusCode_BadInternalError;
	}

	s_typeId = new NodeId(namespaceIndex, _T("StructoAutomoldServer.TestUserSpecificEventType"));

	ObjectPointer<TestUserSpecificEventType> typeNode = new TestUserSpecificEventType();
	typeNode->setBrowseName(QualifiedName(_T("UserSpecifcEventType"), namespaceIndex));
	typeNode->setDisplayName(LocalizedText(_T("UserSpecifcEventType"), _T("en")));
	typeNode->setDescription(LocalizedText(_T("Event type received only by Group 2"), _T("en")));
	typeNode->setIsAbstract(false);
	typeNode->setNodeId(s_typeId);

	std::vector<RolePermissionType> rolePermissions;
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
	rolePermissions.push_back(RolePermissionType());
	rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
	rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_ReceiveEvents);
	typeNode->setRolePermissions(rolePermissions);

	baseEventType->addReference(Statics::ReferenceTypeId_HasSubtype, typeNode);
	typeNode->insertTree();

	return EnumStatusCode_Good;
}

void TestUserSpecificEventType::uninitialize()
{
	BaseNodePtr typeNode = AddressSpaceRoot::instance()->getNode(s_typeId);
	if(typeNode)
	{
		typeNode->removeTree();
	}

	delete s_typeId;
	s_typeId = NULL;
}

#endif
