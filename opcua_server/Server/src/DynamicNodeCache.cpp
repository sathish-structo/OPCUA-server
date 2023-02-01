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
#include "DynamicNodeCache.h"
#include "ServerAddressSpaceRoot.h"
#include "ServerObject.h"
#include "ServerObjectType.h"
#include "Constants.h"
#include "Statics.h"
#include "ServerVariable.h"
#include "ServerVariableType.h"
#include "ServerReferenceType.h"
#include "ServerDataType.h"
#include "ServerReadRequest.h"
#include "ServerMethod.h"
#include "ServerView.h"
#include "ServerBrowseContinuationPoint.h"
#include "ServerReferenceType.h"
#include "ToolkitTrace.h"
#include "Argument.h"
#include "ServerReferenceIterator.h"
#include "ServerReadTransaction.h"
#include "ApplicationModule.h"

using namespace SoftingOPCToolbox5;

extern bool g_cttCompatible;
extern bool g_resetAddressSpace;

DynamicNodeCache* DynamicNodeCache::m_instance = NULL;
#ifdef _DEBUG
bool DynamicNodeCache::m_wasUninitialized = false;
#endif

DynamicNodeCache* DynamicNodeCache::instance()
{
	if(!m_instance)
	{
#ifdef _DEBUG
		_ASSERTION(m_wasUninitialized == false, _T("Reinitialization of the dynamic node cache after it was uninitialized. This will cause a memory leak."));
#endif
		m_instance = new DynamicNodeCache();
	}
	return m_instance;
}

EnumStatusCode DynamicNodeCache::initialize(Server::BaseNodePtr parentNode)
{
	EnumStatusCode result = EnumStatusCode_Good;
	if(!parentNode)
	{
		return EnumStatusCode_BadInvalidArgument;
	}

	OTUInt16 namespaceIndex = parentNode->getNodeId()->getNamespaceIndex();

	DynamicNodeCache* pNodeCache = DynamicNodeCache::instance();
	pNodeCache->lock();

	tstring dynamicNodeParentFolderId = _T("DynamicAddressSpace");;
	NodeId parentType(EnumNumericNodeId_FolderType);

	Server::ObjectPtr dynamicNodeParentFolder = Server::Object::create();
	result = dynamicNodeParentFolder->setNodeId(NodeId(namespaceIndex, dynamicNodeParentFolderId));
	result = dynamicNodeParentFolder->setBrowseName(QualifiedName(dynamicNodeParentFolderId, namespaceIndex));
	result = dynamicNodeParentFolder->setDisplayName(LocalizedText(dynamicNodeParentFolderId, _T("")));
	result = dynamicNodeParentFolder->init(&parentType);

	//////////////////////////////////////////////////////////////////////////
	// Create variable 'nodeMICountVariable' (for first virtual variable)
	//////////////////////////////////////////////////////////////////////////
	Server::VariablePtr nodeMICountVariable = Server::Variable::create();
	tstring nodeMICountVariableId(_T("MICount"));
	NodeId miCountNodeId(namespaceIndex, nodeMICountVariableId);
	result = nodeMICountVariable->setNodeId(miCountNodeId);
	result = nodeMICountVariable->setBrowseName(QualifiedName(nodeMICountVariableId, namespaceIndex));
	result = nodeMICountVariable->setDisplayName(LocalizedText(nodeMICountVariableId, _T("")));
	result = nodeMICountVariable->setDataType(Statics::DataTypeId_UInt32);
	result = nodeMICountVariable->setAccessLevel(EnumAccessLevel_CurrentRead);
	result = nodeMICountVariable->init(Statics::VariableTypeId_BaseDataVariableType);

	result = dynamicNodeParentFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeMICountVariable);
	result = dynamicNodeParentFolder->insertTree();

	// common nodes
	Server::BaseNodePtr baseObjectTypeNode = Server::AddressSpaceRoot::instance()->getNode(Statics::ObjectTypeId_BaseObjectType);
	Server::BaseNodePtr baseDataVariableTypeNode = Server::AddressSpaceRoot::instance()->getNode(Statics::VariableTypeId_BaseDataVariableType);
	Server::BaseNodePtr propertyTypeNode = Server::AddressSpaceRoot::instance()->getNode(Statics::VariableTypeId_PropertyType);
	Server::BaseNodePtr folderTypeNode = Server::AddressSpaceRoot::instance()->getNode(Statics::ObjectTypeId_FolderType);
	Server::BaseNodePtr doubleDataTypeNode = Server::AddressSpaceRoot::instance()->getNode(Statics::DataTypeId_Double);
	Server::BaseNodePtr hierarchicalReferenceTypeNode = Server::AddressSpaceRoot::instance()->getNode(Statics::ReferenceTypeId_HierarchicalReferences);

	VirtualNodeInfo virtObjType(EnumNodeClass_ObjectType);
	VirtualNodeInfo virtVarType(EnumNodeClass_VariableType);
	VirtualNodeInfo virtRefType(EnumNodeClass_ReferenceType);
	VirtualNodeInfo virtDataType(EnumNodeClass_DataType);

	VirtualNodeInfo virtVar(EnumNodeClass_Variable);
	VirtualNodeInfo virtObject(EnumNodeClass_Object);
	VirtualNodeInfo virtVar2(EnumNodeClass_Variable);
	VirtualNodeInfo virtVar5(EnumNodeClass_Variable);
	VirtualNodeInfo virtMeth(EnumNodeClass_Method);
	VirtualNodeInfo virtInputArg(EnumNodeClass_Variable);
	VirtualNodeInfo instOfVirtObjType(EnumNodeClass_Object);
	VirtualNodeInfo instOfVirtVarType(EnumNodeClass_Variable);
	VirtualNodeInfo virtObjWithVirtRef(EnumNodeClass_Object);
	VirtualNodeInfo virtVarWithDataType(EnumNodeClass_Variable);
	VirtualNodeInfo virtVarMultiple(EnumNodeClass_Variable);
	VirtualNodeInfo virtView(EnumNodeClass_View);

	size_t virtObjTypeIndex = 0;
	size_t virtVarTypeIndex = 0;
	size_t virtRefTypeIndex = 0;
	size_t virtDataTypeIndex = 0;
	// size_t virtVarIndex = 0;
	// size_t virtObjectIndex = 0;
	// size_t virtVar2Index = 0;
	// size_t virtVar5Index = 0;
	// size_t virtMethIndex = 0;
	// size_t virtInputArgIndex = 0;
	// size_t instOfVirtObjTypeIndex = 0;
	// size_t instOfVirtVarTypeIndex = 0;
	// size_t virtObjWithVirtRefIndex = 0;
	// size_t virtVarWithDataTypeIndex = 0;
	// size_t virtVarMultipleIndex = 0;
	// size_t virtViewIndex = 0;

	std::vector<ReferenceDescription> references;
	ReferenceDescription refDesc;

	// add virtual ObjectType
	{
		virtObjType.setNodeId(NodeId(namespaceIndex, _T("VirtualObjectType")));

		virtObjType.setBrowseName(QualifiedName(_T("VirtualObjectType"), namespaceIndex));
		virtObjType.setDisplayName(LocalizedText(_T("Virtual ObjectType"), _T("en")));
		virtObjType.setDescription(LocalizedText(_T("This is a virtual ObjectType, it shall not be used for non virtual instances"), _T("en")));
		virtObjType.setWriteMask(0);
		virtObjType.setIsAbstract(false);

		references.clear();

		// is subtype of
		initializeReferenceDescription(refDesc, baseObjectTypeNode, NodeId(EnumNumericNodeId_HasSubtype), false);
		references.push_back(refDesc);

		virtObjType.setReferences(references);

		virtObjTypeIndex = pNodeCache->addVirtualNode(virtObjType);
	}

	// add virtual VariableType
	{
		virtVarType.setNodeId(NodeId(namespaceIndex, _T("VirtualVariableType")));

		virtVarType.setBrowseName(QualifiedName(_T("VirtualVariableType"), namespaceIndex));
		virtVarType.setDisplayName(LocalizedText(_T("Virtual VariableType"), _T("en")));
		virtVarType.setDescription(LocalizedText(_T("This is a virtual VariableType, it shall not be used for non virtual instances"), _T("en")));
		virtVarType.setWriteMask(0);
		virtVarType.setIsAbstract(false);
		virtVarType.setDataTypeId(NodeId(EnumNumericNodeId_Double));
		virtVarType.setValueRank(ValueRank_Scalar);
		virtVarType.setArrayDimensions(std::vector<OTUInt32>());

		references.clear();

		// is subtype of
		initializeReferenceDescription(refDesc, baseDataVariableTypeNode, NodeId(EnumNumericNodeId_HasSubtype), false);
		references.push_back(refDesc);

		virtVarType.setReferences(references);

		virtVarTypeIndex = pNodeCache->addVirtualNode(virtVarType);
	}

	// add virtual ReferenceType
	{
		// use namespace 1 for this type
		virtRefType.setNodeId(NodeId(1, _T("VirtualReferenceType")));

		virtRefType.setBrowseName(QualifiedName(_T("VirtualReferenceType"), 1));
		virtRefType.setDisplayName(LocalizedText(_T("Virtual Reference"), _T("en")));
		virtRefType.setDescription(LocalizedText(_T("This is a virtual ReferenceType, it shall not be used between two non virtual nodes"), _T("en")));
		virtRefType.setWriteMask(0);
		virtRefType.setIsAbstract(false);
		virtRefType.setInverseName(LocalizedText(_T("Inverse_VirtualReferenceType"), _T("en")));
		virtRefType.setIsSymmetric(false);

		references.clear();

		// is subtype of
		initializeReferenceDescription(refDesc, hierarchicalReferenceTypeNode, NodeId(EnumNumericNodeId_HasSubtype), false);
		references.push_back(refDesc);

		virtRefType.setReferences(references);

		virtRefTypeIndex = pNodeCache->addVirtualNode(virtRefType);
	}

	// add virtual DataType
	{
		// use namespace 1 for this type
		virtDataType.setNodeId(NodeId(1, _T("VirtualDataType")));

		virtDataType.setBrowseName(QualifiedName(_T("VirtualDataType"), 1));
		virtDataType.setDisplayName(LocalizedText(_T("Virtual DataType"), _T("en")));
		virtDataType.setDescription(LocalizedText(_T("This is a virtual DataType, it shall not be used for non virtual instances"), _T("en")));
		virtDataType.setWriteMask(0);
		virtDataType.setIsAbstract(false);

		references.clear();

		// is subtype of
		initializeReferenceDescription(refDesc, doubleDataTypeNode, NodeId(EnumNumericNodeId_HasSubtype), false);
		references.push_back(refDesc);

		virtDataType.setReferences(references);

		virtDataTypeIndex = pNodeCache->addVirtualNode(virtDataType);
	}

	{
		virtVar.setNodeId(NodeId(namespaceIndex, _T("DynamicNode")));

		virtVar.setBrowseName(QualifiedName(_T("DynamicNode"), namespaceIndex));
		virtVar.setDisplayName(LocalizedText(_T("Dynamic Node"), _T("en")));
		virtVar.setDescription(LocalizedText(_T("This node only exists on demand on the server, but there is no difference for the client"), _T("en")));
		virtVar.setWriteMask(0);
		DataValue dval;
		dval.setValue(Value(100.0));
		virtVar.setDataValue(dval);
		virtVar.setDataTypeId(NodeId(EnumNumericNodeId_Double));
		virtVar.setValueRank(ValueRank_Scalar);
		virtVar.setArrayDimensions(std::vector<OTUInt32>());
		virtVar.setAccessLevel(virtVar.getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

		references.clear();

		// type definition
		initializeReferenceDescription(refDesc, baseDataVariableTypeNode, NodeId(EnumNumericNodeId_HasTypeDefinition), true);
		references.push_back(refDesc);
		// is organized by
		initializeReferenceDescription(refDesc, dynamicNodeParentFolder, NodeId(EnumNumericNodeId_Organizes), false);
		references.push_back(refDesc);

		virtVar.setReferences(references);

		virtVar.setMonitoredItemCountNodeId(miCountNodeId);

		/* virtVarIndex = */ pNodeCache->addVirtualNode(virtVar);
	}

	// add a virtual object
	{
		virtObject.setNodeId(NodeId(namespaceIndex, _T("VirtualObject")));

		virtObject.setBrowseName(QualifiedName(_T("VirtualObject"), namespaceIndex));
		virtObject.setDisplayName(LocalizedText(_T("Virtual Object"), _T("en")));
		virtObject.setDescription(LocalizedText(_T("This object only exists on demand on the server, but there is no difference for the client"), _T("en")));
		virtObject.setWriteMask(0);

		virtObject.setEventNotifier(0);

		references.clear();
		// type definition
		initializeReferenceDescription(refDesc, folderTypeNode, NodeId(EnumNumericNodeId_HasTypeDefinition), true);
		references.push_back(refDesc);
		// is organized by
		initializeReferenceDescription(refDesc, dynamicNodeParentFolder, NodeId(EnumNumericNodeId_Organizes), false);
		references.push_back(refDesc);

		virtObject.setReferences(references);

		// do not add a variable showing the MI Count on that object
		// virtObject.setMonitoredItemCountNodeId(miCountNodeId);

		/* virtObjectIndex = */ pNodeCache->addVirtualNode(virtObject);
	}

	// add a virtual variable below the virtual object
	{
		// use namespace index 2 for this instance
		virtVar2.setNodeId(NodeId(namespaceIndex, _T("VirtualVar2")));

		virtVar2.setBrowseName(QualifiedName(_T("VirtualVar2"), namespaceIndex));
		virtVar2.setDisplayName(LocalizedText(_T("Virtual Variable 2"), _T("en")));
		virtVar2.setDescription(LocalizedText(_T("This variable does not create MI on monitoring"), _T("en")));
		virtVar2.setMonitoredItemsWithoutNode(true);
		virtVar2.setWriteMask(0);

		DataValue dval;
		dval.setValue(Value(100.0));
		virtVar2.setDataValue(dval);
		virtVar2.setDataTypeId(NodeId(EnumNumericNodeId_Double));
		virtVar2.setValueRank(ValueRank_Scalar);
		virtVar2.setArrayDimensions(std::vector<OTUInt32>());
		virtVar2.setAccessLevel(virtVar2.getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

		references.clear();
		// type definition
		initializeReferenceDescription(refDesc, propertyTypeNode, NodeId(EnumNumericNodeId_HasTypeDefinition), true);
		references.push_back(refDesc);

		// is component of virtual object
		initializeReferenceDescription(refDesc, virtObject, NodeId(EnumNumericNodeId_HasComponent), false);
		references.push_back(refDesc);

		virtVar2.setReferences(references);

		// do not add a variable showing the MI Count on that object
		// virtVar2.setMonitoredItemCountNodeId(miCountNodeId);


		/* virtVar2Index = */ pNodeCache->addVirtualNode(virtVar2);
	}

	// add a virtual variable below the Dynamic address space node
	{
		// use namespace index 5 for this instance
		virtVar5.setNodeId(NodeId(5, _T("VirtualVar5")));

		virtVar5.setBrowseName(QualifiedName(_T("VirtualVar5"), 5));
		virtVar5.setDisplayName(LocalizedText(_T("Virtual Variable 5"), _T("en")));
		virtVar5.setDescription(LocalizedText(_T("This variable exists below the virtual object"), _T("en")));
		virtVar5.setWriteMask(0);

		DataValue dval;
		dval.setValue(Value(100.0));
		virtVar5.setDataValue(dval);
		virtVar5.setDataTypeId(NodeId(EnumNumericNodeId_Double));
		virtVar5.setValueRank(ValueRank_Scalar);
		virtVar5.setArrayDimensions(std::vector<OTUInt32>());
		virtVar5.setAccessLevel(virtVar5.getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

		references.clear();
		// type definition
		initializeReferenceDescription(refDesc, propertyTypeNode, NodeId(EnumNumericNodeId_HasTypeDefinition), true);
		references.push_back(refDesc);

		// is organized by
		initializeReferenceDescription(refDesc, dynamicNodeParentFolder, NodeId(EnumNumericNodeId_HasComponent), false);
		references.push_back(refDesc);

		virtVar5.setReferences(references);

		// do not add a variable showing the MI Count on that object
		// virtVar2.setMonitoredItemCountNodeId(miCountNodeId);

		/* virtVar5Index = */ pNodeCache->addVirtualNode(virtVar5);
	}
	// add a virtual method and the input arguments below the virtual object
	{
		virtMeth.setNodeId(NodeId(namespaceIndex, _T("VirtualMeth")));

		virtMeth.setBrowseName(QualifiedName(_T("VirtualMeth"), namespaceIndex));
		virtMeth.setDisplayName(LocalizedText(_T("Virtual Method"), _T("en")));
		virtMeth.setDescription(LocalizedText(_T("This method exists below the virtual object"), _T("en")));
		virtMeth.setWriteMask(0);
		virtMeth.setExecutable(true);

		references.clear();
		// is component of virtual object
		initializeReferenceDescription(refDesc, virtObject, NodeId(EnumNumericNodeId_HasComponent), false);
		references.push_back(refDesc);
		virtMeth.setReferences(references);

		/* virtMethIndex = */ pNodeCache->addVirtualNode(virtMeth);

		// add the input arguments
		virtInputArg.setNodeId(NodeId(namespaceIndex, _T("VirtualInpuArg")));

		virtInputArg.setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		virtInputArg.setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		virtInputArg.setDescription(LocalizedText(_T("Input Arguments of the virtual method"), _T("en")));
		virtInputArg.setWriteMask(0);

		DataValue dval;
		std::vector<Argument> inputArgs;
		Argument arg1;
		arg1.setName(_T("Arg 1"));
		arg1.setDataType(EnumNumericNodeId_Double);
		arg1.setValueRank(ValueRank_Scalar);
		arg1.setDescription(LocalizedText(_T("Arg 1 for virtual method"), _T("en")));
		inputArgs.push_back(arg1);
		dval.setValue(Value(inputArgs));
		virtInputArg.setDataValue(dval);
		virtInputArg.setDataTypeId(NodeId(EnumNumericNodeId_Argument));
		virtInputArg.setValueRank(1);
		std::vector<OTUInt32> arrayDimensions;
		arrayDimensions.push_back(1);
		virtInputArg.setArrayDimensions(arrayDimensions);
		virtInputArg.setAccessLevel(EnumAccessLevel_CurrentRead);

		references.clear();
		// type definition
		initializeReferenceDescription(refDesc, propertyTypeNode, NodeId(EnumNumericNodeId_HasTypeDefinition), true);
		references.push_back(refDesc);

		// is component of method
		initializeReferenceDescription(refDesc, virtMeth, NodeId(EnumNumericNodeId_HasComponent), false);
		references.push_back(refDesc);

		virtInputArg.setReferences(references);

		// do not add a variable showing the MI Count on that object
		// virtInputArg.setMonitoredItemCountNodeId(miCountNodeId);

		/* virtInputArgIndex = */ pNodeCache->addVirtualNode(virtInputArg);
	}

	// add a virtual variable of the virtual variable type
	{
		instOfVirtVarType.setNodeId(NodeId(namespaceIndex, _T("InstanceOfVirtualVariableType")));

		instOfVirtVarType.setBrowseName(QualifiedName(_T("InstanceOfVirtualVariableType"), namespaceIndex));
		instOfVirtVarType.setDisplayName(LocalizedText(_T("Instance of Virtual Variable Type"), _T("en")));
		instOfVirtVarType.setDescription(LocalizedText(_T("This variable is a virtual instance of a virtual variable type"), _T("en")));
		instOfVirtVarType.setWriteMask(0);

		DataValue dval;
		dval.setValue(Value(100.0));
		instOfVirtVarType.setDataValue(dval);
		instOfVirtVarType.setDataTypeId(NodeId(EnumNumericNodeId_Double));
		instOfVirtVarType.setValueRank(ValueRank_Scalar);
		instOfVirtVarType.setArrayDimensions(std::vector<OTUInt32>());
		instOfVirtVarType.setAccessLevel(instOfVirtVarType.getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

		references.clear();
		// type definition
		initializeReferenceDescription(refDesc, virtVarType, NodeId(EnumNumericNodeId_HasTypeDefinition), true);
		references.push_back(refDesc);
		instOfVirtVarType.addLinkedNodeInfo(virtVarTypeIndex);	// the type must exist as long as the instance exists

		// is organized by
		initializeReferenceDescription(refDesc, dynamicNodeParentFolder, NodeId(EnumNumericNodeId_Organizes), false);
		references.push_back(refDesc);

		instOfVirtVarType.setReferences(references);

		// do not add a variable showing the MI Count on that object
		// virtVar2.setMonitoredItemCountNodeId(miCountNodeId);

		/* instOfVirtVarTypeIndex = */ pNodeCache->addVirtualNode(instOfVirtVarType);
	}

	// add a virtual object of the virtual objectType
	{
		instOfVirtObjType.setNodeId(NodeId(namespaceIndex, _T("InstanceOfVirtualObjectType")));

		instOfVirtObjType.setBrowseName(QualifiedName(_T("InstanceOfVirtualObjectType"), namespaceIndex));
		instOfVirtObjType.setDisplayName(LocalizedText(_T("Instance of Virtual Object Type"), _T("en")));
		instOfVirtObjType.setDescription(LocalizedText(_T("This object is a virtual instance of a virtual object type"), _T("en")));
		instOfVirtObjType.setWriteMask(0);

		instOfVirtObjType.setEventNotifier(0);

		references.clear();
		// type definition
		initializeReferenceDescription(refDesc, virtObjType, NodeId(EnumNumericNodeId_HasTypeDefinition), true);
		references.push_back(refDesc);
		instOfVirtObjType.addLinkedNodeInfo(virtObjTypeIndex);	// the type must exist as long as the instance exists

		// is organized by
		initializeReferenceDescription(refDesc, dynamicNodeParentFolder, NodeId(EnumNumericNodeId_Organizes), false);
		references.push_back(refDesc);

		instOfVirtObjType.setReferences(references);

		/* instOfVirtObjTypeIndex = */ pNodeCache->addVirtualNode(instOfVirtObjType);
	}

	// add a virtual object with a virtual reference of the virtual reference type
	{
		// use namespace 5 for this instance
		virtObjWithVirtRef.setNodeId(NodeId(namespaceIndex, _T("VirtualObjectWithVirtualReference")));

		virtObjWithVirtRef.setBrowseName(QualifiedName(_T("VirtualObjectWithVirtualReference"), namespaceIndex));
		virtObjWithVirtRef.setDisplayName(LocalizedText(_T("Virtual Object With Virtual Reference"), _T("en")));
		virtObjWithVirtRef.setDescription(LocalizedText(_T("This object is a virtual node with a virtual reference of a virtual reference type"), _T("en")));
		virtObjWithVirtRef.setWriteMask(0);

		virtObjWithVirtRef.setEventNotifier(0);

		references.clear();
		// type definition
		initializeReferenceDescription(refDesc, baseObjectTypeNode, NodeId(EnumNumericNodeId_HasTypeDefinition), true);
		references.push_back(refDesc);
		// is virtually referenced
		initializeReferenceDescription(refDesc, dynamicNodeParentFolder, virtRefType.getNodeId(), false);
		references.push_back(refDesc);
		virtObjWithVirtRef.addLinkedNodeInfo(virtRefTypeIndex);	// the type must exist as long as the instance exists

		virtObjWithVirtRef.setReferences(references);

		/* virtObjWithVirtRefIndex = */ pNodeCache->addVirtualNode(virtObjWithVirtRef);
	}

	// add a virtual variable with a virtual data type
	{
		virtVarWithDataType.setNodeId(NodeId(namespaceIndex, _T("VirtualVariableWithDataType")));

		virtVarWithDataType.setBrowseName(QualifiedName(_T("VirtualVariableWithDataType"), namespaceIndex));
		virtVarWithDataType.setDisplayName(LocalizedText(_T("Virtual Variable With Data Type"), _T("en")));
		virtVarWithDataType.setDescription(LocalizedText(_T("This variable is a virtual instance with a virtual data type"), _T("en")));
		virtVarWithDataType.setWriteMask(0);

		DataValue dval;
		dval.setValue(Value(100.0));
		virtVarWithDataType.setDataValue(dval);
		virtVarWithDataType.setDataTypeId(virtDataType.getNodeId());
		virtVarWithDataType.setValueRank(ValueRank_Scalar);
		virtVarWithDataType.setArrayDimensions(std::vector<OTUInt32>());
		virtVarWithDataType.setAccessLevel(virtVarWithDataType.getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

		virtVarWithDataType.addLinkedNodeInfo(virtDataTypeIndex);	// the type must exist as long as the instance exists

		references.clear();
		// type definition
		initializeReferenceDescription(refDesc, baseDataVariableTypeNode, NodeId(EnumNumericNodeId_HasTypeDefinition), true);
		references.push_back(refDesc);

		// is organized by
		initializeReferenceDescription(refDesc, dynamicNodeParentFolder, NodeId(EnumNumericNodeId_Organizes), false);
		references.push_back(refDesc);


		virtVarWithDataType.setReferences(references);

		/* virtVarWithDataTypeIndex = */ pNodeCache->addVirtualNode(virtVarWithDataType);
	}

	// add a virtual variable using multiple virtual nodes
	{
		virtVarMultiple.setNodeId(NodeId(namespaceIndex, _T("VirtualVariableMultiple")));

		virtVarMultiple.setBrowseName(QualifiedName(_T("VirtualVariableMultiple"), namespaceIndex));
		virtVarMultiple.setDisplayName(LocalizedText(_T("Virtual Variable Multiple"), _T("en")));
		virtVarMultiple.setDescription(LocalizedText(_T("This virtual variable uses a virtual variable type, data type and reference"), _T("en")));
		virtVarMultiple.setWriteMask(0);

		DataValue dval;
		dval.setValue(Value(100.0));
		virtVarMultiple.setDataValue(dval);
		virtVarMultiple.setDataTypeId(virtDataType.getNodeId());
		virtVarMultiple.setValueRank(ValueRank_Scalar);
		virtVarMultiple.setArrayDimensions(std::vector<OTUInt32>());
		virtVarMultiple.setAccessLevel(virtVarMultiple.getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

		virtVarMultiple.addLinkedNodeInfo(virtDataTypeIndex);	// the type must exist as long as the instance exists

		references.clear();
		// type definition
		initializeReferenceDescription(refDesc, virtVarType, NodeId(EnumNumericNodeId_HasTypeDefinition), true);
		references.push_back(refDesc);
		virtVarMultiple.addLinkedNodeInfo(virtVarTypeIndex);	// the type must exist as long as the instance exists

		// is virtually referenced
		initializeReferenceDescription(refDesc, dynamicNodeParentFolder, virtRefType.getNodeId(), false);
		references.push_back(refDesc);
		virtVarMultiple.addLinkedNodeInfo(virtRefTypeIndex);	// the type must exist as long as the instance exists

		virtVarMultiple.setReferences(references);

		/* virtVarMultipleIndex = */ pNodeCache->addVirtualNode(virtVarMultiple);
	}

	// add a virtual view
	{
		virtView.setNodeId(NodeId(namespaceIndex, _T("VirtualView")));

		virtView.setBrowseName(QualifiedName(_T("VirtualView"), namespaceIndex));
		virtView.setDisplayName(LocalizedText(_T("Virtual View"), _T("en")));
		virtView.setDescription(LocalizedText(_T("This virtual view references the other virtual nodes"), _T("en")));
		virtView.setWriteMask(0);
		virtView.setContainsNoLoops(true);
		virtView.setEventNotifier(EnumEventNotifier_None);

		references.clear();

		// is organized by
		initializeReferenceDescription(refDesc, dynamicNodeParentFolder, NodeId(EnumNumericNodeId_Organizes), false);
		references.push_back(refDesc);

		// organizes virtObjType
		initializeReferenceDescription(refDesc, virtObjType, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);
		// organizes virtVarType
		initializeReferenceDescription(refDesc, virtVarType, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);
		// organizes virtRefType
		initializeReferenceDescription(refDesc, virtRefType, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);
		// organizes virtDataType
		initializeReferenceDescription(refDesc, virtDataType, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);
		// organizes virtVar
		initializeReferenceDescription(refDesc, virtVar, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);
		// organizes virtObject
		initializeReferenceDescription(refDesc, virtObject, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);
		// organizes instOfVirtVarType
		initializeReferenceDescription(refDesc, instOfVirtVarType, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);
		// organizes instOfVirtObjType
		initializeReferenceDescription(refDesc, instOfVirtObjType, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);
		// organizes virtObjWithVirtRef
		initializeReferenceDescription(refDesc, virtObjWithVirtRef, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);
		// organizes virtVarWithDataType
		initializeReferenceDescription(refDesc, virtVarWithDataType, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);
		// organizes virtVarMultiple
		initializeReferenceDescription(refDesc, virtVarMultiple, NodeId(EnumNumericNodeId_Organizes), true);
		references.push_back(refDesc);

		virtView.setReferences(references);

		/* virtViewIndex = */ pNodeCache->addVirtualNode(virtView);
	}

	if(!g_cttCompatible)
	{
		result = parentNode->addReference(Statics::ReferenceTypeId_Organizes, dynamicNodeParentFolder);
	}

	// leave critical section
	pNodeCache->unlock();

	return result;
}

DynamicNodeCache::DynamicNodeCache()
{
}

DynamicNodeCache::~DynamicNodeCache()
{
}


void DynamicNodeCache::lock()
{
	m_sync.lock();
}

void DynamicNodeCache::unlock()
{
	m_sync.unlock();
}


void DynamicNodeCache::deleteInstance()
{
#ifdef _DEBUG
	if(!g_resetAddressSpace)
	{
		m_wasUninitialized = true;
	}
#endif
	if(m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}


EnumStatusCode DynamicNodeCache::handleMonitoredItemCreation(SoftingOPCToolbox5::Server::MonitoredItemPtr monitoredItem, EnumStatusCode origStatus)
{
	EnumStatusCode result = origStatus;
	lock();
	// is it a dynamic node?
	for(size_t i=0; i<m_virtualNodes.size(); i++)
	{
		if(monitoredItem->getNodeId()->compare(m_virtualNodes[i].getNodeId()) == 0)
		{
			// it is a dynamic node - assume good status
			result = m_virtualNodes[i].handleMonitoredItemCreation(monitoredItem);
			break;	// for
		}
	}

	unlock();

	return result;
}

EnumStatusCode DynamicNodeCache::handleMonitoredItemDeletion(SoftingOPCToolbox5::Server::MonitoredItemPtr monitoredItem, EnumStatusCode origStatus)
{
	EnumStatusCode result = origStatus;
	// is it a dynamic node?
	lock();
	for(size_t i = 0; StatusCode::isGood(result), i < m_virtualNodes.size(); i++)
	{
		if(monitoredItem->getNodeId()->compare(m_virtualNodes[i].getNodeId()) == 0)
		{
			result = m_virtualNodes[i].handleMonitoredItemDeletion(monitoredItem);
			break;	// for
		}
	}
	unlock();

	return result;
}

size_t DynamicNodeCache::addVirtualNode(const VirtualNodeInfo &nodeInfo)
{
	for(size_t i=0; i< m_virtualNodes.size(); i++)
	{
		if(m_virtualNodes[i].getNodeId().compare(nodeInfo.getNodeId()) == 0)
		{
			_REPORT_ASSERTION(_T("Duplicate NodeId in NodeCache"));
			return std::numeric_limits<size_t>::max();
		}
	}
	m_virtualNodes.push_back(nodeInfo);
	m_virtualNodes.back().setPointerToStore(&m_virtualNodes);
	return m_virtualNodes.size() - 1;
}

bool DynamicNodeCache::handleReadRequest(SoftingOPCToolbox5::Server::ReadTransactionPtr transaction, SoftingOPCToolbox5::Server::ReadRequestPtr request)
{
	bool requestHandled = false;
	lock();

	// is this the dynamic node?
	for(size_t i=0; i< m_virtualNodes.size(); i++)
	{
		if(request->getValueId()->getNodeId()->compare(m_virtualNodes[i].getNodeId()) == 0)
		{
			DataValue dval;
			EnumAttributeId attributeId = request->getValueId()->getAttributeId();
			// get the requested attribute
			m_virtualNodes[i].getAttribute(attributeId, dval);

			// return the requested timestamps only
			transaction->updateTimeStamps(&dval, attributeId);

			request->setDataValue(dval);
			request->complete();
			requestHandled = true;
			break;
		}
	}
	unlock();
	return requestHandled;
}

bool DynamicNodeCache::handleWriteRequest(SoftingOPCToolbox5::Server::WriteRequestPtr request)
{
	bool requestHandled = false;
	lock();

	// is this the dynamic node?
	for(size_t i=0; i< m_virtualNodes.size(); i++)
	{
		// is this the dynamic node?
		if(request->getWriteValue()->getNodeId()->compare( m_virtualNodes[i].getNodeId()) == 0)
		{
			EnumStatusCode requestResult = EnumStatusCode_Good;
			const IDataValue* dval = request->getWriteValue()->getDataValue();
			EnumAttributeId attributeId = request->getWriteValue()->getAttributeId();
			// reject writing a data value with a bad status code, except for the value
			if(StatusCode::isGood(dval->getStatusCode()) || attributeId == EnumAttributeId_Value)
			{
				requestResult = m_virtualNodes[i].setAttribute(attributeId, dval);
			}

			if(StatusCode::isGood(requestResult))
			{
				m_virtualNodes[i].updateMonitoredItems(attributeId);
			}
			// overwrite the status code
			request->setStatusCode(requestResult);
			request->complete();
			requestHandled = true;
			break;
		}
	}

	// leave the critical section
	unlock();
	return requestHandled;
}

bool DynamicNodeCache::resolveDynamicNodePath(const SoftingOPCToolbox5::INodeId& startingNodeId, EnumBrowseDirection browseDir, const SoftingOPCToolbox5::INodeId* pReferenceId, bool includeSubtypes, const SoftingOPCToolbox5::IQualifiedName* pBrowseName, SoftingOPCToolbox5::INodeId* pResultNodeId)
{
	NodeId resultNodeId;	// NULL is the default value
	bool relatedToDynamicNode = false;
	if(startingNodeId.isNull() || pReferenceId == NULL || pBrowseName == NULL || pResultNodeId == NULL)
	{
		if(pResultNodeId)
		{
			*pResultNodeId = resultNodeId;
		}
		return relatedToDynamicNode;
	}

	lock();
	{
		for(size_t nodeCount=0; !relatedToDynamicNode && nodeCount<m_virtualNodes.size(); nodeCount++)
		{
			const INodeId& dynamicNodeId = m_virtualNodes[nodeCount].getNodeId();
			_ASSERTION(!dynamicNodeId.isNull(), _T("The nodeId of the dynamic node shouldn't be NULL."));
			const std::vector<SoftingOPCToolbox5::ReferenceDescription>& dynamicReferences = m_virtualNodes[nodeCount].getReferences();
			EnumBrowseDirection effectiveBrowseDirection = browseDir;

			// search on the cache always
			// search for references from the dynamic node or to it?
			if(startingNodeId.compare(dynamicNodeId) == 0)
			{
				relatedToDynamicNode = true;

				for(size_t i = 0; i < dynamicReferences.size(); i++)
				{
					EnumBrowseDirection direction = dynamicReferences[i].getIsForward() ? EnumBrowseDirection_Forward : EnumBrowseDirection_Inverse;
					if(effectiveBrowseDirection != EnumBrowseDirection_Both && effectiveBrowseDirection != direction)
					{
						continue;
					}
					// does the reference type match?
					Server::BaseNodePtr referenceTypeNode = Server::AddressSpaceRoot::instance()->getNode(pReferenceId);
					if(referenceTypeNode.isNull() || !referenceTypeNode->isOfUaType(dynamicReferences[i].getReferenceTypeId(), includeSubtypes))
					{
						continue;
					}

					// does the browse name match?
					if(dynamicReferences[i].getBrowseName()->compare(pBrowseName) != 0)
					{
						continue;
					}
					// this is the desired node
					resultNodeId = dynamicReferences[i].getNodeId();
					break;
				}
			}
			else
			{
				// invert the browse direction when the dynamic Node isn't the starting node
				// (e.g. when searching forward to the dynamic node, it must have a backward reference to the starting node)
				if(browseDir == EnumBrowseDirection_Forward)
				{
					effectiveBrowseDirection = EnumBrowseDirection_Inverse;
				}
				else if(browseDir == EnumBrowseDirection_Inverse)
				{
					effectiveBrowseDirection = EnumBrowseDirection_Forward;
				}

				for(size_t i = 0; i < dynamicReferences.size(); i++)
				{
					EnumBrowseDirection direction = dynamicReferences[i].getIsForward() ? EnumBrowseDirection_Forward : EnumBrowseDirection_Inverse;
					if(effectiveBrowseDirection != EnumBrowseDirection_Both && effectiveBrowseDirection != direction)
					{
						continue;
					}
					// does the reference type match?
					Server::BaseNodePtr referenceTypeNode = Server::AddressSpaceRoot::instance()->getNode(pReferenceId);
					if(referenceTypeNode.isNull() || !referenceTypeNode->isOfUaType(dynamicReferences[i].getReferenceTypeId(), includeSubtypes))
					{
						continue;
					}

					// the target of the reference must be the starting node

					// if we search the dynamic node from another node, check whether the starting node references the dynamic node
					// (only the dynamic node knows the references to existing nodes, but not vice versa)
					if(dynamicReferences[i].getNodeId()->compare(startingNodeId) != 0)
					{
						continue;
					}

					// does the browse name match?
					// use the own browse name to compare for inverse browse direction
					if(m_virtualNodes[nodeCount].getBrowseName().compare(pBrowseName) != 0)
					{
						continue;
					}

					resultNodeId = dynamicNodeId;
					relatedToDynamicNode = true;
					break;
				}
			}
		}
	}
	unlock();
	pResultNodeId->set(resultNodeId);
	return relatedToDynamicNode;
}

bool DynamicNodeCache::isNodeIdRelatedToVirtualNode(const SoftingOPCToolbox5::NodeId& nodeId)
{
	bool bHandleRequest = false;

	for(size_t nodeCount=0; !bHandleRequest && nodeCount<m_virtualNodes.size(); nodeCount++)
	{
		bHandleRequest = (m_virtualNodes[nodeCount].getNodeId().compare(nodeId) == 0);
		if(!bHandleRequest)
		{
			const std::vector<ReferenceDescription>& refDesc = m_virtualNodes[nodeCount].getReferences();
			for(size_t i=0; i<refDesc.size(); i++)
			{
				bHandleRequest = NodeId::areEqual(nodeId, refDesc[i].getNodeId());
				if(bHandleRequest)
				{
					break;
				}
			}
		}
	}
	return bHandleRequest;
}

bool DynamicNodeCache::addVirtualReferences(const SoftingOPCToolbox5::NodeId& nodeIdToBrowse, const SoftingOPCToolbox5::NodeId& referenceTypeId, bool includeSubtypes, EnumBrowseDirection browseDirection, EnumNodeClass nodeClassMask,
	EnumResultMask resultMask, std::vector<SoftingOPCToolbox5::ReferenceDescription> &allReferences)
{
	for(size_t nodeIndex = 0; nodeIndex < m_virtualNodes.size(); nodeIndex++)
	{
		//const std::vector<ReferenceDescription>& cacheReferences = m_virtualNodes[nodeCount].getReferences();
		std::vector<ReferenceDescription> foundReferences = m_virtualNodes[nodeIndex].searchReferences(nodeIdToBrowse, referenceTypeId, includeSubtypes, browseDirection, nodeClassMask);
		for(size_t refIndex = 0; refIndex < foundReferences.size(); refIndex++)
		{
			// copy only information according to result mask
			ReferenceDescription refToAdd;
			refToAdd.setNodeId(foundReferences[refIndex].getNodeId());
			if((resultMask & EnumResultMask_NodeClass) == EnumResultMask_NodeClass)
			{
				// hard coded in dynamic node cache
				refToAdd.setNodeClass(foundReferences[refIndex].getNodeClass());
			}
			if((resultMask & EnumResultMask_ReferenceType) == EnumResultMask_ReferenceType)
			{
				refToAdd.setReferenceTypeId(foundReferences[refIndex].getReferenceTypeId());
			}
			if(resultMask & EnumResultMask_IsForward)
			{
				refToAdd.setIsForward(foundReferences[refIndex].getIsForward());
			}
			if((resultMask & EnumResultMask_BrowseName) == EnumResultMask_BrowseName)
			{
				refToAdd.setBrowseName(foundReferences[refIndex].getBrowseName());
			}
			if((resultMask & EnumResultMask_DisplayName) == EnumResultMask_DisplayName)
			{
				refToAdd.setDisplayName(foundReferences[refIndex].getDisplayName());
			}
			if((resultMask & EnumResultMask_TypeDefinition) == EnumResultMask_TypeDefinition)
			{
				refToAdd.setTypeDefinition(foundReferences[refIndex].getTypeDefinition());
			}

			bool alreadyExisting = false;
			for(size_t j = 0; j < allReferences.size(); j++)
			{
				if(allReferences[j] == refToAdd)
				{
					alreadyExisting = true;
					break;
				}
			}
			if(!alreadyExisting)
			{
				allReferences.push_back(refToAdd);
			}
		}
	}
	return true;
}

EnumModelDesignerStatus DynamicNodeCache::exportVirtualNodes(SoftingOPCToolbox5::Server::NodeSet2Export::ExportCustomNodesTransactionPtr transaction)
{
	std::vector<XmlElement> extensions;
	EnumModelDesignerStatus mcResult;

	const std::vector<tstring> namespacesToExport =  transaction->getNamespacesToExport();
	StringTableConstPtr namespaces = AddressSpaceRoot::instance()->getNamespaces();
	for(size_t i=0; i<m_virtualNodes.size(); i++)
	{
		tstring namespaceUriOfNode;
		if(!StatusCode::isGood(namespaces->getEntry(m_virtualNodes[i].getNodeId().getNamespaceIndex(), namespaceUriOfNode)))
		{
			// should not happen, but if => ignore this node
			continue;
		}
		// check whether virtual node shall be exported
		for(size_t index = 0; index < namespacesToExport.size(); index++)
		{
			if(namespacesToExport[index] == namespaceUriOfNode)
			{
				// this node shall be exported
				NodeId parentNodeId;
				switch(m_virtualNodes[i].getNodeClass())
				{
				case EnumNodeClass_Object:
					mcResult = transaction->exportCustomObject(m_virtualNodes[i].getNodeId(), parentNodeId, m_virtualNodes[i].getBrowseName(), m_virtualNodes[i].getDisplayName(), m_virtualNodes[i].getDescription(),
						m_virtualNodes[i].getWriteMask(), m_virtualNodes[i].getEventNotifier(), m_virtualNodes[i].getReferences(), extensions);
					break;
				case EnumNodeClass_Variable:
					mcResult = transaction->exportCustomVariable(m_virtualNodes[i].getNodeId(), parentNodeId, m_virtualNodes[i].getBrowseName(), m_virtualNodes[i].getDisplayName(), m_virtualNodes[i].getDescription(),
						m_virtualNodes[i].getWriteMask(), m_virtualNodes[i].getDataTypeId(), m_virtualNodes[i].getValueRank(), m_virtualNodes[i].getArrayDimensions(), m_virtualNodes[i].getDataValue().getValue(),
						m_virtualNodes[i].getAccessLevel(), m_virtualNodes[i].getMinimumSamplingInterval(), m_virtualNodes[i].getHistorizing(),
						m_virtualNodes[i].getReferences(), extensions);
					break;
				case EnumNodeClass_Method:
					mcResult = transaction->exportCustomUaMethod(m_virtualNodes[i].getNodeId(), parentNodeId, m_virtualNodes[i].getBrowseName(), m_virtualNodes[i].getDisplayName(), m_virtualNodes[i].getDescription(),
						m_virtualNodes[i].getWriteMask(), m_virtualNodes[i].getExecutable(), m_virtualNodes[i].getReferences(), extensions);
					break;
				case EnumNodeClass_ObjectType:
					mcResult = transaction->exportCustomObjectType(m_virtualNodes[i].getNodeId(), m_virtualNodes[i].getBrowseName(), m_virtualNodes[i].getDisplayName(), m_virtualNodes[i].getDescription(),
						m_virtualNodes[i].getWriteMask(), m_virtualNodes[i].getIsAbstract(), m_virtualNodes[i].getReferences(), extensions);
					break;
				case EnumNodeClass_VariableType:
					mcResult = transaction->exportCustomVariableType(m_virtualNodes[i].getNodeId(), m_virtualNodes[i].getBrowseName(), m_virtualNodes[i].getDisplayName(), m_virtualNodes[i].getDescription(),
						m_virtualNodes[i].getWriteMask(), m_virtualNodes[i].getDataTypeId(), m_virtualNodes[i].getValueRank(), m_virtualNodes[i].getArrayDimensions(), m_virtualNodes[i].getDataValue().getValue(),
						m_virtualNodes[i].getIsAbstract(), m_virtualNodes[i].getReferences(), extensions);
					break;
				case EnumNodeClass_ReferenceType:
					mcResult = transaction->exportCustomReferenceType(m_virtualNodes[i].getNodeId(), m_virtualNodes[i].getBrowseName(), m_virtualNodes[i].getDisplayName(), m_virtualNodes[i].getDescription(),
						m_virtualNodes[i].getWriteMask(), m_virtualNodes[i].getIsAbstract(), m_virtualNodes[i].getIsSymmetric(), m_virtualNodes[i].getInverseName(), m_virtualNodes[i].getReferences(), extensions);
					break;
				case EnumNodeClass_DataType:
					mcResult = transaction->exportCustomDataType(m_virtualNodes[i].getNodeId(), m_virtualNodes[i].getBrowseName(), m_virtualNodes[i].getDisplayName(), m_virtualNodes[i].getDescription(),
						m_virtualNodes[i].getWriteMask(), m_virtualNodes[i].getIsAbstract(), m_virtualNodes[i].getDataTypeDescription(), m_virtualNodes[i].getReferences(), extensions);
					break;
				case EnumNodeClass_View:
					mcResult = transaction->exportCustomUaView(m_virtualNodes[i].getNodeId(), parentNodeId, m_virtualNodes[i].getBrowseName(), m_virtualNodes[i].getDisplayName(), m_virtualNodes[i].getDescription(),
						m_virtualNodes[i].getWriteMask(), m_virtualNodes[i].getEventNotifier(), m_virtualNodes[i].getContainsNoLoops(), m_virtualNodes[i].getReferences(), extensions);
					break;
				default:
					mcResult = EnumModelDesignerStatus_BadNotImplemented;
					break;
				}

				if(mcResult != EnumModelDesignerStatus_Good)
				{
					Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("Export returned error: "), MCGetEnumModelDesignerStatusString(mcResult));
				}
			}
		}
		
	}
	return EnumModelDesignerStatus_Good;
}

void DynamicNodeCache::initializeReferenceDescription(ReferenceDescription &referenceDescription, const VirtualNodeInfo& virtualNode, const NodeId& referenceTypeId, bool isForward)
{
	_ASSERTION(!virtualNode.getNodeId().isNull(), _T("The referenced virtual node info seems not to be initialized yet"));
	referenceDescription.clear();
	referenceDescription.setNodeId(ExpandedNodeId(virtualNode.getNodeId()));
	referenceDescription.setBrowseName(virtualNode.getBrowseName());
	referenceDescription.setDisplayName(virtualNode.getDisplayName());
	referenceDescription.setNodeClass(virtualNode.getNodeClass());
	referenceDescription.setReferenceTypeId(referenceTypeId);
	referenceDescription.setIsForward(isForward);

	NodeId hasTypeDefinitionId(EnumNumericNodeId_HasTypeDefinition);
	const std::vector<ReferenceDescription>& references = virtualNode.getReferences();
	for (size_t i = 0; i < references.size() ; i++)
	{
		Server::BaseNodePtr referenceType = Server::AddressSpaceRoot::instance()->getNode(references[i].getReferenceTypeId());
		if(referenceType.isNotNull() &&
			referenceType->isOfUaType(&hasTypeDefinitionId, true))
		{
			referenceDescription.setTypeDefinition(references[i].getNodeId());
			break;
		}
	}
}

void DynamicNodeCache::initializeReferenceDescription(ReferenceDescription& referenceDescription, Server::BaseNodePtr existingNode, const NodeId& referenceTypeId, bool isForward)
{
	referenceDescription.clear();
	referenceDescription.setNodeId(ExpandedNodeId(existingNode->getNodeId()));
	referenceDescription.setBrowseName(existingNode->getBrowseName());
	referenceDescription.setDisplayName(existingNode->getDisplayName());
	referenceDescription.setNodeClass(existingNode->getNodeClass());
	referenceDescription.setReferenceTypeId(referenceTypeId);
	referenceDescription.setIsForward(isForward);

	NodeId hasTypeDefinitionId(EnumNumericNodeId_HasTypeDefinition);
	// use NULL session (all nodes) to find type definition
	Server::ReferenceIteratorPtr it = existingNode->getReferenceIterator(EnumBrowseDirection_Forward, &hasTypeDefinitionId, true, NULL);
	if(!it->isEnd())
	{
		referenceDescription.setTypeDefinition(it->getReferenceDescription()->getNodeId());
	}
}

bool DynamicNodeCache::isOfUaType(const INodeId* sourceNodeId, const SoftingOPCToolbox5::INodeId* parentTypeId, bool includeSubtypes)
{
	if(sourceNodeId == NULL || parentTypeId == NULL)
	{
		return false;
	}
	bool isOfType = false;

	Server::BaseNodePtr node = Server::AddressSpaceRoot::instance()->getNode(sourceNodeId);
	if(node.isNotNull())
	{
		isOfType = node->isOfUaType(parentTypeId, includeSubtypes);
	}
	else
	{
		// maybe a dynamic node

		VirtualNodeInfo* pNodeInfo = NULL;
		for(size_t i = 0; i < m_virtualNodes.size(); i++)
		{
			if(m_virtualNodes[i].getNodeId().compare(sourceNodeId) == 0)
			{
				pNodeInfo = &m_virtualNodes[i];
				break;
			}
		}

		if(pNodeInfo)
		{
			switch(pNodeInfo->getNodeClass())
			{
			case EnumNodeClass_ObjectType:
			case EnumNodeClass_VariableType:
			case EnumNodeClass_ReferenceType:
			case EnumNodeClass_DataType:
				if(pNodeInfo->getNodeId().compare(parentTypeId) == 0)
				{
					isOfType = true;
				}
				if(!isOfType && includeSubtypes)
				{
					Server::BaseNodePtr parentType = Server::AddressSpaceRoot::instance()->getNode(parentTypeId);
					if(parentType.isNotNull())
					{
						isOfType = parentType->isOfUaType(parentTypeId, includeSubtypes);
					}
					else
					{
						// next parent might be a dynamic node
						Server::ReferenceTypePtr referenceType;
						const std::vector<ReferenceDescription>& references = pNodeInfo->getReferences();
						// search the hasSubtype reference
						for(size_t i = 0; i < references.size(); i++)
						{
							bool continueSearch = true;
							// there must be a IsSubtypeOf reference to the next parent
							if(!references[i].getIsForward())
							{
								referenceType = Server::AddressSpaceRoot::instance()->getReferenceType(references[i].getReferenceTypeId());
								if(referenceType.isNotNull())
								{
									isOfType = referenceType->isOfUaType(Statics::ReferenceTypeId_HasSubtype, includeSubtypes);
								}
								else
								{
									// maybe a dynamic node as well
									for(size_t virtualIndex = 0; virtualIndex < m_virtualNodes.size(); virtualIndex++)
									{
										if(m_virtualNodes[virtualIndex].getNodeId().compare(references[i].getReferenceTypeId()) == 0)
										{
											isOfType = isOfUaType(&m_virtualNodes[virtualIndex].getNodeId(), parentTypeId, includeSubtypes);
											continueSearch = false;
											break;	// for virtual nodes
										}
									}
								}
								if(!continueSearch)
								{
									break;	// for references
								}
							}
						}
					}
				}
				break;
			default:
				break;
			}
		}
		else
		{
			// node neither known in address space, nor in node cache
		}
	}
	return isOfType;
}

VirtualNodeInfo::VirtualNodeInfo(EnumNodeClass nodeClass)
{
	m_nodeClass = nodeClass;
	m_writeMask = 0;
	m_accessLevel = EnumAccessLevel_CurrentRead;
	m_valueRank = ValueRank_Scalar;
	m_historizing = false;
	m_minimumSamplingInterval = 0.0;
	m_executable = false;
	m_eventNotifier = 0;
	m_containsNoLoops = false;
	m_isAbstract = false;
	m_isSymmetric = false;
	m_pNodeInfoStore = NULL;
	m_monitoredItemsWithoutNode = false;

	interlockedSet(&m_monitoredItemCount, 0);
}

VirtualNodeInfo::~VirtualNodeInfo()
{

}

EnumNodeClass VirtualNodeInfo::getNodeClass() const
{
	return m_nodeClass;
}

const SoftingOPCToolbox5::INodeId& VirtualNodeInfo::getNodeId() const
{
	return m_nodeId;
}

void VirtualNodeInfo::setNodeId(const SoftingOPCToolbox5::INodeId& nodeId)
{
	m_nodeId = nodeId;
}

void VirtualNodeInfo::setMonitoredItemsWithoutNode(bool flag)
{
	m_monitoredItemsWithoutNode = flag;
}

bool VirtualNodeInfo::getMonitoredItemsWithoutNode() const
{
	return m_monitoredItemsWithoutNode;
}

void VirtualNodeInfo::updateMonitoredItems(EnumAttributeId attributeId /* = EnumAttributeId_Invalid */)
{
	DataValue dataValue;
	dataValue.setServerTimestamp(DateTime::getUtcNow());
	dataValue.setSourceTimestamp(DateTime::getUtcNow());
	if(m_monitoredItemsWithoutNode)
	{
		for(std::set<SoftingOPCToolbox5::Server::MonitoredItemPtr>::iterator it = m_monitoredItems.begin();
			it != m_monitoredItems.end(); ++it)
		{
			// update one specific attribute or all
			if((attributeId == EnumAttributeId_Invalid) || ((*it)->getAttributeId() == attributeId))
			{
				getAttribute((*it)->getAttributeId(), dataValue);
				(*it)->reportDataChange(dataValue);
			}
		}
	}
	else
	{
		Server::BaseNodePtr dynNode;
		// the node should exist
		dynNode = AddressSpaceRoot::instance()->getNode(&m_nodeId);
		if(dynNode)
		{
			dynNode->setAttribute(attributeId, &dataValue, true, NULL);
		}
		else
		{
			Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("Node does not exist: "), m_nodeId.toString());
		}
	}
}

void VirtualNodeInfo::getAttribute(EnumAttributeId attributeId, DataValue& dataValue)
{
	EnumStatusCode result = EnumStatusCode_Good;
	Value val;
	switch(attributeId)
	{
	case EnumAttributeId_NodeId:
		result = val.setNodeId(&getNodeId());
		break;
	case EnumAttributeId_NodeClass:
		val.setInt32(getNodeClass());
		break;
	case EnumAttributeId_BrowseName:
		result = val.setQualifiedName(&getBrowseName());
		break;
	case EnumAttributeId_DisplayName:
		result = val.setLocalizedText(&getDisplayName());
		break;
	case EnumAttributeId_Description:
		result = val.setLocalizedText(&getDescription());
		break;
	case EnumAttributeId_WriteMask:
	case EnumAttributeId_UserWriteMask:
		val.setUInt32(getWriteMask());
		break;
	case EnumAttributeId_IsAbstract:
		switch(getNodeClass())
		{
		case EnumNodeClass_ObjectType:
		case EnumNodeClass_VariableType:
		case EnumNodeClass_ReferenceType:
		case EnumNodeClass_DataType:
			val.setBoolean(getIsAbstract());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_Symmetric:
		switch(getNodeClass())
		{
		case EnumNodeClass_ReferenceType:
			val.setBoolean(getIsSymmetric());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_InverseName:
		switch(getNodeClass())
		{
		case EnumNodeClass_ReferenceType:
			val.setLocalizedText(&getInverseName());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_ContainsNoLoops:
		switch(getNodeClass())
		{
		case EnumNodeClass_View:
			val.setBoolean(getContainsNoLoops());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_EventNotifier:
		switch(getNodeClass())
		{
		case EnumNodeClass_Object:
		case EnumNodeClass_View:
			val.setUInt8(getEventNotifier());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_Value:
		switch(getNodeClass())
		{
		case EnumNodeClass_Variable:
		case EnumNodeClass_VariableType:
			if(getAccessLevel() & EnumAccessLevel_CurrentRead)
			{
				result = val.set(getDataValue().getValue());
				if(StatusCode::isGood(result))
				{
					result = getDataValue().getStatusCode();
				}
			}
			else
			{
				result = EnumStatusCode_BadNotReadable;
			}
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_DataType:
		switch(getNodeClass())
		{
		case EnumNodeClass_Variable:
		case EnumNodeClass_VariableType:
			result = val.setNodeId(&getDataTypeId());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_ValueRank:
		switch(getNodeClass())
		{
		case EnumNodeClass_Variable:
		case EnumNodeClass_VariableType:
			val.setInt32(getValueRank());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_ArrayDimensions:
		switch(getNodeClass())
		{
		case EnumNodeClass_Variable:
		case EnumNodeClass_VariableType:
			result = val.setUInt32Array(getArrayDimensions());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_AccessLevel:
	case EnumAttributeId_UserAccessLevel:
		switch(getNodeClass())
		{
		case EnumNodeClass_Variable:
			val.setUInt8(getAccessLevel());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_MinimumSamplingInterval:
		switch(getNodeClass())
		{
		case EnumNodeClass_Variable:
			val.setDouble(getMinimumSamplingInterval());	// indeterminate
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_Historizing:
		switch(getNodeClass())
		{
		case EnumNodeClass_Variable:
			val.setBoolean(getHistorizing());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_Executable:
	case EnumAttributeId_UserExecutable:
		switch(getNodeClass())
		{
		case EnumNodeClass_Method:
			val.setBoolean(getExecutable());
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_DataTypeDefinition:
		// TODO: support data type definition for virtual nodes
		switch(getNodeClass())
		{
		case EnumNodeClass_DataType:
			result = EnumStatusCode_BadAttributeIdInvalid;
			// val.setStructureDefinition();
			// val.setEnumDefinition();
			break;
		default:
			result = EnumStatusCode_BadAttributeIdInvalid;
			break;
		}
		break;
	case EnumAttributeId_RolePermissions:
	case EnumAttributeId_UserRolePermissions:
	case EnumAttributeId_AccessRestrictions:
	case EnumAttributeId_AccessLevelEx:
		// not yet supported attributes
		result = EnumStatusCode_BadAttributeIdInvalid;
		break;
	case EnumAttributeId_Invalid:
	default:
		_REPORT_ASSERTION(_T("Unknown Attribute"));
		result = EnumStatusCode_BadAttributeIdInvalid;
		break;
	}
	if(StatusCode::isGood(result))
	{
		dataValue.setValue(val);
		dataValue.setSourceTimestamp(DateTime::getUtcNow());
		dataValue.setServerTimestamp(DateTime::getUtcNow());
	}
	dataValue.setStatusCode(result);
}

EnumStatusCode VirtualNodeInfo::setAttribute(EnumAttributeId attributeId, const IDataValue* dataValue)
{
	EnumStatusCode result = EnumStatusCode_Good;
	const IValue* val = dataValue->getValue();
	OTUInt32 writeMask = getWriteMask();
	// set the right attribute
	switch(attributeId)
	{
	case EnumAttributeId_NodeId:
		if(writeMask & EnumWriteMask_NodeId)
		{
			NodeId id;
			result = val->getNodeId(id);
			if(StatusCode::isGood(result))
			{
				setNodeId(id);
			}
		}
		else
		{
			result = EnumStatusCode_BadNotWritable;
		}
		break;
	case EnumAttributeId_NodeClass:
		result = EnumStatusCode_BadNotWritable;
		break;
	case EnumAttributeId_BrowseName:
		if(writeMask & EnumWriteMask_BrowseName)
		{
			QualifiedName name;
			result = val->getQualifiedName(name);
			if(StatusCode::isGood(result))
			{
				setBrowseName(name);
			}
		}
		else
		{
			result = EnumStatusCode_BadNotWritable;
		}
		break;
	case EnumAttributeId_DisplayName:
		if(writeMask & EnumWriteMask_DisplayName)
		{
			LocalizedText text;
			result = val->getLocalizedText(text);
			if(StatusCode::isGood(result))
			{
				setDisplayName(text);;
			}
		}
		else
		{
			result = EnumStatusCode_BadNotWritable;
		}
		break;
	case EnumAttributeId_Description:
		if(writeMask & EnumWriteMask_Description)
		{
			LocalizedText text;
			result = val->getLocalizedText(text);
			if(StatusCode::isGood(result))
			{
				setDescription(text);;
			}
		}
		else
		{
			result = EnumStatusCode_BadNotWritable;
		}
		break;
	case EnumAttributeId_WriteMask:
	case EnumAttributeId_UserWriteMask:
		if(writeMask & EnumWriteMask_WriteMask || writeMask & EnumWriteMask_UserWriteMask)
		{
			OTUInt32 mask;
			result = val->getUInt32(&mask);
			if(StatusCode::isGood(result))
			{
				setWriteMask(mask);;
			}
		}
		else
		{
			result = EnumStatusCode_BadNotWritable;
		}
		break;
	case EnumAttributeId_Value:
		if(getAccessLevel() & EnumAccessLevel_CurrentWrite)
		{
			NodeId dataTypeId(0, val->getDataType());
			Server::BaseNodePtr dataType = Server::AddressSpaceRoot::instance()->getNode(&dataTypeId);
			if(dataType && dataType->isOfUaType(&getDataTypeId(), true))
			{
				setDataValue(*dataValue);
			}
		}
		else
		{
			result = EnumStatusCode_BadNotWritable;
		}
		break;
	case EnumAttributeId_DataType:
		if(writeMask & EnumWriteMask_DataType)
		{
			NodeId id;
			result = val->getNodeId(id);
			if(StatusCode::isGood(result))
			{
				setDataTypeId(id);
			}
		}
		else
		{
			result = EnumStatusCode_BadNotWritable;
		}
		break;
	case EnumAttributeId_ValueRank:
		if(writeMask & EnumWriteMask_ValueRank)
		{
			OTInt32 rank;
			result = val->getInt32(&rank);
			if(StatusCode::isGood(result))
			{
				setValueRank(rank);
			}
		}
		else
		{
			result = EnumStatusCode_BadNotWritable;
		}
		break;
	case EnumAttributeId_ArrayDimensions:
		if(writeMask & EnumWriteMask_ArrayDimensions)
		{
			std::vector<OTUInt32> dimensions;
			result = val->getUInt32Array(dimensions);
			if(StatusCode::isGood(result))
			{
				setArrayDimensions(dimensions);
			}
		}
		else
		{
			result = EnumStatusCode_BadNotWritable;
		}
		break;
	case EnumAttributeId_AccessLevel:
	case EnumAttributeId_UserAccessLevel:
		if(writeMask & EnumWriteMask_AccessLevel || writeMask & EnumWriteMask_UserAccessLevel)
		{
			OTUInt8 accessLevel;
			result = val->getUInt8(&accessLevel);
			if(StatusCode::isGood(result))
			{
				setAccessLevel(accessLevel);
			}
		}
		else
		{
			result = EnumStatusCode_BadNotWritable;
		}
		break;
	case EnumAttributeId_MinimumSamplingInterval:
		result = EnumStatusCode_BadNotWritable;
		break;
	case EnumAttributeId_Historizing:
		result = EnumStatusCode_BadNotWritable;
		break;
		// not supported attributes for a variable
	case EnumAttributeId_IsAbstract:
	case EnumAttributeId_Symmetric:
	case EnumAttributeId_InverseName:
	case EnumAttributeId_ContainsNoLoops:
	case EnumAttributeId_EventNotifier:
	case EnumAttributeId_Executable:
	case EnumAttributeId_UserExecutable:
		result = EnumStatusCode_BadAttributeIdInvalid;
		break;
	default:
		_REPORT_ASSERTION(_T("Unknown Attribute"));
		result = EnumStatusCode_BadAttributeIdInvalid;
		break;
	}
	return result;
}

const SoftingOPCToolbox5::IQualifiedName& VirtualNodeInfo::getBrowseName() const
{
	return m_browseName;
}

void VirtualNodeInfo::setBrowseName(const SoftingOPCToolbox5::IQualifiedName& browseName)
{
	m_browseName = browseName;
}

const SoftingOPCToolbox5::ILocalizedText& VirtualNodeInfo::getDisplayName() const
{
	return m_displayName;
}

void VirtualNodeInfo::setDisplayName(const SoftingOPCToolbox5::ILocalizedText& displayName)
{
	m_displayName = displayName;
}

const SoftingOPCToolbox5::ILocalizedText& VirtualNodeInfo::getDescription() const
{
	return m_description;
}

void VirtualNodeInfo::setDescription(const SoftingOPCToolbox5::ILocalizedText& description)
{
	m_description = description;
}

OTUInt32 VirtualNodeInfo::getWriteMask() const
{
	return m_writeMask;
}

void VirtualNodeInfo::setWriteMask(OTUInt32 writeMask)
{
	m_writeMask = writeMask;
}

const std::vector<SoftingOPCToolbox5::ReferenceDescription>& VirtualNodeInfo::getReferences() const
{
	return m_references;
}

void VirtualNodeInfo::setReferences(const std::vector<SoftingOPCToolbox5::ReferenceDescription>& references)
{
	m_references = references;
}

const SoftingOPCToolbox5::IDataValue& VirtualNodeInfo::getDataValue() const
{
	return m_dataValue;
}

void VirtualNodeInfo::setDataValue(const SoftingOPCToolbox5::IDataValue& value)
{
	m_dataValue = value;
}

OTUInt8 VirtualNodeInfo::getAccessLevel() const
{
	return m_accessLevel;
}

void VirtualNodeInfo::setAccessLevel(OTUInt8 accessLevel)
{
	m_accessLevel = accessLevel;
}

const SoftingOPCToolbox5::INodeId& VirtualNodeInfo::getDataTypeId() const
{
	return m_dataTypeId;
}

void VirtualNodeInfo::setDataTypeId(const SoftingOPCToolbox5::INodeId& dataTypeId)
{
	m_dataTypeId = dataTypeId;
}

OTInt32 VirtualNodeInfo::getValueRank() const
{
	return m_valueRank;
}

void VirtualNodeInfo::setValueRank(OTInt32 valueRank)
{
	m_valueRank = valueRank;
}

const std::vector<OTUInt32>& VirtualNodeInfo::getArrayDimensions() const
{
	return m_arrayDimensions;
}

void VirtualNodeInfo::setArrayDimensions(const std::vector<OTUInt32>& arrayDimensions)
{
	m_arrayDimensions = arrayDimensions;
}

bool VirtualNodeInfo::getExecutable() const
{
	return m_executable;
}

void VirtualNodeInfo::setExecutable(bool executable)
{
	m_executable = executable;
}

bool VirtualNodeInfo::getHistorizing() const
{
	return m_historizing;
}

void VirtualNodeInfo::setHistorizing(bool historizing)
{
	m_historizing = historizing;
}

double VirtualNodeInfo::getMinimumSamplingInterval() const
{
	return m_minimumSamplingInterval;
}

void VirtualNodeInfo::setMinimumSamplingInterval(double minimumSamplingInterval)
{
	m_minimumSamplingInterval = minimumSamplingInterval;
}

EnumEventNotifier VirtualNodeInfo::getEventNotifier() const
{
	return m_eventNotifier;
}

void VirtualNodeInfo::setEventNotifier(EnumEventNotifier eventNotifier)
{
	m_eventNotifier = eventNotifier;
}

bool VirtualNodeInfo::getContainsNoLoops() const
{
	return m_containsNoLoops;
}

void VirtualNodeInfo::setContainsNoLoops(bool containsNoLoops)
{
	m_containsNoLoops = containsNoLoops;
}

const SoftingOPCToolbox5::INodeId& VirtualNodeInfo::getMonitoredItemCountNodeId() const
{
	return m_monitoredItemCountNodeId;
}

void VirtualNodeInfo::setMonitoredItemCountNodeId(const SoftingOPCToolbox5::INodeId& nodeId)
{
	m_monitoredItemCountNodeId = nodeId;
}

EnumStatusCode VirtualNodeInfo::handleMonitoredItemCreation(SoftingOPCToolbox5::Server::MonitoredItemPtr monitoredItem)
{
	EnumStatusCode result = EnumStatusCode_Good;

	// before possibly creating this node, create the linked nodes
	// (e.g. create the ReferenceType before creating a reference of that type)
	std::vector<VirtualNodeInfo*> linkedNodeInfos = getLinkedNodeInfos();
	for(size_t i = 0; i < linkedNodeInfos.size(); i++)
	{
		if(linkedNodeInfos[i])
		{
			result = linkedNodeInfos[i]->handleMonitoredItemCreation(monitoredItem);
		}
		else
		{
			_REPORT_ASSERTION(_T("Unexpected NULL pointer"));
			result = EnumStatusCode_BadConfigurationError;
		}
	}

	if(StatusCode::isGood(result))
	{
		OTAtomic newCount = incrementMiCount();
		if((newCount == 1) && !m_monitoredItemsWithoutNode)
		{
			Server::BaseNodePtr dynNode;
			// the node should not exist, but some tests create the node accidentally, which would cause problems
			dynNode = AddressSpaceRoot::instance()->getNode(&m_nodeId);
			if(dynNode.isNotNull())
			{
				// if the node already exists (which normally shouldn't happen) the node class must match to be able to take over this node
				if(dynNode->getNodeClass() != m_nodeClass)
				{
					_REPORT_ASSERTION(_T("The already existing dynamic node should have the same node class!"));
					result = EnumStatusCode_BadUnexpectedError;
				}
			}

			if(StatusCode::isGood(result))
			{
				switch(m_nodeClass)
				{
				case EnumNodeClass_Object:
					{
						Server::ObjectPtr dynObj;
						if(dynNode.isNull())
						{
							// the first MI - create the node
							dynObj = Server::Object::create();
						}
						else
						{
							// take over the existing node
							dynObj = static_pointer_cast<Server::Object>(dynNode);
						}
						dynObj->setEventNotifier(m_eventNotifier);
						dynNode = dynObj;
						break;
					}
				case EnumNodeClass_Variable:
					{
						Server::VariablePtr dynVar;
						if(dynNode.isNull())
						{
							// the first MI - create the node
							dynVar = Server::Variable::create();
						}
						else
						{
							// take over the existing node
							dynVar = static_pointer_cast<Server::Variable>(dynNode);
						}
						dynVar->setAccessLevel(m_accessLevel);
						dynVar->setDataType(m_dataTypeId);
						dynVar->setValueRank(m_valueRank);
						dynVar->setArrayDimensions(m_arrayDimensions);
						dynVar->setDataValue(m_dataValue);
						dynNode = dynVar;
						break;
					}
				case EnumNodeClass_Method:
					{
						Server::MethodPtr dynMeth;
						if(dynNode.isNull())
						{
							// the first MI - create the node
							dynMeth = Server::Method::create();
						}
						else
						{
							// take over the existing node
							dynMeth = static_pointer_cast<Server::Method>(dynNode);
						}
						dynMeth->setExecutable(m_executable);
						dynNode = dynMeth;
						break;
					}
				case EnumNodeClass_ObjectType:
					{
						Server::ObjectTypePtr dynObjType;
						if(dynNode.isNull())
						{
							// the first MI - create the node
							dynObjType = Server::ObjectType::create();
						}
						else
						{
							// take over the existing node
							dynObjType = static_pointer_cast<Server::ObjectType>(dynNode);
						}
						dynObjType->setIsAbstract(m_isAbstract);
						dynNode = dynObjType;
						break;
					}
				case EnumNodeClass_VariableType:
					{
						Server::VariableTypePtr dynVarType;
						if(dynNode.isNull())
						{
							// the first MI - create the node
							dynVarType = Server::VariableType::create();
						}
						else
						{
							// take over the existing node
							dynVarType = static_pointer_cast<Server::VariableType>(dynNode);
						}
						dynVarType->setIsAbstract(m_isAbstract);
						dynVarType->setDataType(m_dataTypeId);
						dynVarType->setValueRank(m_valueRank);
						dynVarType->setArrayDimensions(m_arrayDimensions);
						dynVarType->setDataValue(m_dataValue);
						dynNode = dynVarType;
						break;
					}
				case EnumNodeClass_ReferenceType:
					{
						Server::ReferenceTypePtr dynRefType;
						if(dynNode.isNull())
						{
							// the first MI - create the node
							dynRefType = Server::ReferenceType::create();
						}
						else
						{
							// take over the existing node
							dynRefType = static_pointer_cast<Server::ReferenceType>(dynNode);
						}
						dynRefType->setIsAbstract(m_isAbstract);
						dynRefType->setInverseName(m_inverseName);
						dynRefType->setIsSymmetric(m_isSymmetric);
						dynNode = dynRefType;
						break;
					}
				case EnumNodeClass_DataType:
					{
						Server::DataTypePtr dynDataType;
						if(dynNode.isNull())
						{
							// the first MI - create the node
							dynDataType = Server::DataType::create();
						}
						else
						{
							// take over the existing node
							dynDataType = static_pointer_cast<Server::DataType>(dynNode);
						}
						dynDataType->setIsAbstract(m_isAbstract);
						dynNode = dynDataType;
						break;
					}
				case EnumNodeClass_View:
					{
						Server::ViewPtr dynView;
						if(dynNode.isNull())
						{
							// the first MI - create the node
							dynView = Server::View::create();
						}
						else
						{
							// take over the existing node
							dynView = static_pointer_cast<Server::View>(dynNode);
						}
						dynView->setEventNotifier(m_eventNotifier);
						dynView->setContainsNoLoops(m_containsNoLoops);
						dynNode = dynView;
						break;
					}
				default:
					_REPORT_ASSERTION(_T("Unsupported node class"));
					break;
				}

				if(dynNode.isNotNull())
				{
					// if the node is taken over, the NodeId already exists and matches
					// but overwrite the other attributes
					if(dynNode->getNodeId()->isNull())
					{
						dynNode->setNodeId(m_nodeId);
					}
					dynNode->setBrowseName(m_browseName);
					dynNode->setDescription(m_description);
					dynNode->setDisplayName(m_displayName);
					dynNode->setWriteMask(m_writeMask);
					result = dynNode->insertTree();
				}
				else
				{
					result = EnumStatusCode_BadNotSupported;
				}
			}
		}
		else
		{
			m_monitoredItems.insert(monitoredItem);
			DataValue dv = getDataValue();
			// update all MI monitoring all attributes
			updateMonitoredItems();
		}

		// update the counter variable if exists
		Server::VariablePtr monitoredItemCountVar = Server::AddressSpaceRoot::instance()->getVariable(&m_monitoredItemCountNodeId);
		if(monitoredItemCountVar)
		{
			DataValue dv;
			Value val;
			dv = monitoredItemCountVar->getDataValue();
			val = dv.getValue();
			val.setUInt32((OTUInt32)newCount);
			dv.setValue(val);
			monitoredItemCountVar->setDataValue(dv);
		}
	}
	return result;
}

EnumStatusCode VirtualNodeInfo::handleMonitoredItemDeletion(SoftingOPCToolbox5::Server::MonitoredItemPtr monitoredItem)
{
	EnumStatusCode result = EnumStatusCode_Good;

	OTAtomic newCount = decrementMiCount();

	Server::BaseNodePtr dynNode = Server::AddressSpaceRoot::instance()->getNode(&m_nodeId);
	if(dynNode)
	{
		if((newCount == 0) && !m_monitoredItemsWithoutNode)
		{
			// the last MI - remove the node
			// copy data from node to cache
			m_description = *dynNode->getDescription();
			m_browseName = *dynNode->getBrowseName();
			m_displayName = *dynNode->getDisplayName();
			m_writeMask = dynNode->getWriteMask();

			switch(m_nodeClass)
			{
			case EnumNodeClass_Variable:
				{
					Server::VariablePtr dynVar = (Server::Variable*)dynNode.p();
					m_accessLevel = dynVar->getAccessLevel();
					m_arrayDimensions = dynVar->getArrayDimensions();
					m_dataValue = *dynVar->getDataValue();
					m_dataTypeId = *dynVar->getDataType();
					break;
				}
			case EnumNodeClass_Object:
				{
					Server::ObjectPtr dynObj = (Server::Object*)dynNode.p();
					m_eventNotifier = dynObj->getEventNotifier();
					break;
				}
			case EnumNodeClass_Method:
				{
					Server::MethodPtr dynObj = (Server::Method*)dynNode.p();
					m_executable = dynObj->getExecutable();
					break;
				}
			case EnumNodeClass_View:
				{
					Server::ViewPtr dynView = (Server::View*)dynNode.p();
					m_containsNoLoops = dynView->getContainsNoLoops();
					m_eventNotifier = dynView->getEventNotifier();
					break;
				}
			default:
				break;
			}

			dynNode->removeTree();
		}
		// release the linked nodes after the direct node
		std::vector<VirtualNodeInfo*> linkedNodeInfos = getLinkedNodeInfos();
		for(size_t i = 0; i < linkedNodeInfos.size(); i++)
		{
			linkedNodeInfos[i]->handleMonitoredItemDeletion(monitoredItem);
		}
	}
	else
	{
		if(m_monitoredItemsWithoutNode)
		{
			m_monitoredItems.erase(monitoredItem);
		}
		else
		{
			// during shutdown it can happen - otherwise it should be asserted
			if(!getApplicationModule()->getShutDownFlag())
			{
				_REPORT_ASSERTION(_T("Dynamic node does not exists while deleting monitored item"));
				result = EnumStatusCode_BadInternalError;
			}
		}
	}

	Server::VariablePtr monitoredItemCountVar = Server::AddressSpaceRoot::instance()->getVariable(&m_monitoredItemCountNodeId);
	if(monitoredItemCountVar)
	{
		DataValue dv;
		Value val;
		val.setUInt32((OTUInt32)newCount);
		dv.setValue(val);
		monitoredItemCountVar->setDataValue(dv);
	}
	return result;
}


OTAtomic VirtualNodeInfo::incrementMiCount()
{
	return interlockedIncrement(&m_monitoredItemCount);
}

OTAtomic VirtualNodeInfo::decrementMiCount()
{
	return interlockedDecrement(&m_monitoredItemCount);
}

bool VirtualNodeInfo::getIsAbstract() const
{
	return m_isAbstract;
}

void VirtualNodeInfo::setIsAbstract(bool isAbstract)
{
	m_isAbstract = isAbstract;
}

const SoftingOPCToolbox5::ILocalizedText& VirtualNodeInfo::getInverseName() const
{
	return m_inverseName;
}

void VirtualNodeInfo::setInverseName(const SoftingOPCToolbox5::ILocalizedText& inverseName)
{
	m_inverseName = inverseName;
}

SoftingOPCToolbox5::DataTypeDescriptionConstPtr VirtualNodeInfo::getDataTypeDescription() const
{
	return m_dataTypeDescription;
}

void VirtualNodeInfo::setDataTypeDescription(SoftingOPCToolbox5::DataTypeDescriptionConstPtr dataTypeDescription)
{
	m_dataTypeDescription = dataTypeDescription;
}

bool VirtualNodeInfo::getIsSymmetric() const
{
	return m_isSymmetric;
}

void VirtualNodeInfo::setIsSymmetric(bool isSymmetric)
{
	m_isSymmetric = isSymmetric;
}

std::vector<VirtualNodeInfo*> VirtualNodeInfo::getLinkedNodeInfos() const
{
	std::vector<VirtualNodeInfo*> nodeInfos;
	if(m_pNodeInfoStore)
	{
		nodeInfos.reserve(m_linkedNodeInfoIndexes.size());
		for(size_t i = 0; i < m_linkedNodeInfoIndexes.size(); i++)
		{
			if(m_linkedNodeInfoIndexes[i] < (*m_pNodeInfoStore).size())
			{
				nodeInfos.push_back(&(*m_pNodeInfoStore)[m_linkedNodeInfoIndexes[i]]);
			}
			else
			{
				_REPORT_ASSERTION("Invalid index in store");
			}
		}
	}
	return nodeInfos;
}

void VirtualNodeInfo::addLinkedNodeInfo(size_t indexInStore)
{
	m_linkedNodeInfoIndexes.push_back(indexInStore);
}

void VirtualNodeInfo::setPointerToStore(std::vector<VirtualNodeInfo>* pNodeInfoStore)
{
	_ASSERTION(m_pNodeInfoStore == NULL, _T("Cannot change the store of a node info."));
	if(m_pNodeInfoStore == NULL)
	{
		m_pNodeInfoStore = pNodeInfoStore;
	}
}

std::vector<SoftingOPCToolbox5::ReferenceDescription> VirtualNodeInfo::searchReferences(const SoftingOPCToolbox5::NodeId& nodeIdToBrowse,
	const SoftingOPCToolbox5::NodeId& referenceTypeId,
	bool includeSubtypes,
	EnumBrowseDirection browseDirection,
	EnumNodeClass nodeClassMask)
{
	std::vector<SoftingOPCToolbox5::ReferenceDescription> foundReferences;
	for(size_t i = 0; i < m_references.size(); i++)
	{
		bool referenceValid = true;
		ReferenceDescription currentRef = m_references[i];

		// test the reference type
		if(!DynamicNodeCache::instance()->isOfUaType(currentRef.getReferenceTypeId(), &referenceTypeId, includeSubtypes))
		{
			referenceValid = false;
		}
		if(referenceValid)
		{
			// check NodeId, NodeClass and direction
			if(NodeId::areEqual(nodeIdToBrowse, m_nodeId))
			{
				// browse from dynamic node
				EnumNodeClass targetNodeClass = currentRef.getNodeClass();
				if((nodeClassMask != EnumNodeClass_Node) &&
					((targetNodeClass & nodeClassMask) != targetNodeClass))
				{
					referenceValid = false;
				}
				if(referenceValid)
				{
					switch(browseDirection)
					{
					case EnumBrowseDirection_Forward:
						if(!currentRef.getIsForward())
						{
							referenceValid = false;
						}
						break;
					case EnumBrowseDirection_Inverse:
						if(currentRef.getIsForward())
						{
							referenceValid = false;
						}
						break;
					case EnumBrowseDirection_Both:
						break;
					case EnumBrowseDirection_Invalid:
					default:
						_REPORT_ASSERTION(_T("Unknown Browse Direction"));
						referenceValid = false;
					}
				}
				if(referenceValid)
				{
					// set output parameter
					foundReferences.push_back(currentRef);
				}
			}
			else
			{
				// browse to dynamic node?
				if((nodeClassMask != EnumNodeClass_Node) &&
					((m_nodeClass & nodeClassMask) != m_nodeClass))
				{
					referenceValid = false;
				}
				if(referenceValid && !NodeId::areEqual(nodeIdToBrowse, currentRef.getNodeId()))
				{
					referenceValid = false;
				}

				if(referenceValid)
				{
					switch(browseDirection)
					{
					case EnumBrowseDirection_Forward:
						// inverse logic, the references are from dynamic node, not to dynamic node
						if(currentRef.getIsForward())
						{
							referenceValid = false;
						}
						break;
					case EnumBrowseDirection_Inverse:
						// inverse logic, the references are from dynamic node, not to dynamic node
						if(!currentRef.getIsForward())
						{
							referenceValid = false;
						}
						break;
					case EnumBrowseDirection_Both:
						break;
					case EnumBrowseDirection_Invalid:
					default:
						_REPORT_ASSERTION(_T("Unknown Browse Direction"));
					}
				}
				if(referenceValid)
				{
					// set output parameter
					foundReferences.push_back(currentRef);
					
					ExpandedNodeId dynamicNodeTypeDefinition;
					for(size_t refIndex = 0; refIndex < m_references.size(); refIndex++)
					{
						if(!m_references[refIndex].getIsForward())
						{
							continue;
						}
						if(DynamicNodeCache::instance()->isOfUaType(m_references[refIndex].getReferenceTypeId(), Statics::ReferenceTypeId_HasTypeDefinition, true))
						{
							dynamicNodeTypeDefinition = *(m_references[refIndex].getNodeId());
							break;
						}
					}

					// invert reference
					foundReferences.back().setIsForward(!currentRef.getIsForward());
					foundReferences.back().setNodeId(ExpandedNodeId(m_nodeId));
					foundReferences.back().setBrowseName(m_browseName);
					foundReferences.back().setDisplayName(m_displayName);
					foundReferences.back().setNodeClass(m_nodeClass);
					foundReferences.back().setTypeDefinition(dynamicNodeTypeDefinition);
				}
			}
		}
	}
	return foundReferences;
}

DynamicExportWorkItem::DynamicExportWorkItem(const SoftingOPCToolbox5::Server::NodeSet2Export::ExportCustomNodesTransactionPtr transaction)
{
	m_tranaction = transaction;
}

DynamicExportWorkItem::DynamicExportWorkItem()
{
}

DynamicExportWorkItem::~DynamicExportWorkItem()
{
}

void DynamicExportWorkItem::execute()
{
	EnumModelDesignerStatus ret = EnumModelDesignerStatus_Good;
	OTUInt16 testModuleNamspaceIndex = 0;
	StringTableConstPtr namespaces;
	namespaces = Server::AddressSpaceRoot::instance()->getNamespaces();
	Server::VariablePtr errorVariable;
	EnumStatusCode result = namespaces->getIndex(_T("http://softing.com/OPCToolbox/Console"), testModuleNamspaceIndex);

	NodeId errorsNodeId(testModuleNamspaceIndex, _T("NextExportError"));

	errorVariable = Server::AddressSpaceRoot::instance()->getVariable(&errorsNodeId);
	if(StatusCode::isGood(result) && errorVariable)
	{
		DataValue errorValue = errorVariable->getDataValue();
		OTInt32 errVal = 0;
		result = errorValue.getValue()->getInt32(&errVal);
		if(StatusCode::isGood(result) &&
			((EnumModelDesignerStatus)errVal != EnumModelDesignerStatus_Good))
		{
			ret = (EnumModelDesignerStatus)errVal;
		}
	}

	if(ret == EnumModelDesignerStatus_Good)
	{
		ret = DynamicNodeCache::instance()->exportVirtualNodes(m_tranaction);
	}
	m_tranaction->complete(ret);
}
