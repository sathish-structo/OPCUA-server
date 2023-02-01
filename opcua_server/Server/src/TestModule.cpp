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
#include "TestMethod.h"
#include "Argument.h"
#include <limits>
#include "TestModule.h"
#include "ApplicationModule.h"
#include "Guid.h"
#include "EUInformation.h"
#include "Range.h"
#include "Statics.h"
#include "ServerMethod.h"
#include "SimVarMethods.h"
#include "ChangeAddressSpaceMethod.h"
#include "ServerAnalogItem.h"
#include "ServerTwoStateItem.h"
#include "ServerMultiStateItem.h"
#include "HistorizingVariable.h"
#include "ServerAggregateConfiguration.h"
#include "ServerAggregateFunction.h"
#include "ServerVariableType.h"
#include "ServerDataType.h"
#include "ServerView.h"
#include "TestModelChangeEventMethod.h"
#include "TestSemanticChangeEventMethod.h"
#include "Constants.h"
#include "DynamicNodeCache.h"
#include "NodeId.h"
#include "CustomCallbackEvent.h"
#include "EndpointManipulationMethods.h"
#include "Nodeset2ExportImport.h"
#include "ServerObjectType.h"
#include "ModelCompiler/ErrorEnums.h"
#include "TestModuleAccessPattern.h"
#include "TestModuleFile.h"
#include "StructuredDataTypeModule.h"
#include "PubSubModule.h"
#include "TestModuleRolePermission.h"
#include "OperationLimitMethods.h"

using namespace SoftingOPCToolbox5;

// a very strange class which deleted another node within destructor
// such an operation has caused formerly a crash
class VarWithLinked : public Server::Variable
{
public:
	VarWithLinked() {};
	~VarWithLinked()
	{
		if(m_linkedVar)
		{
			m_linkedVar->removeTree();
			m_linkedVar = NULL;
		}
	};

	Server::VariablePtr m_linkedVar;
};

/* @brief Constructor */
TestModule::TestModule()
{
}

/* @brief Destructor */
TestModule::~TestModule()
{
}

EnumStatusCode TestModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	// Name= StructoAutoMold,
	// namespaceIndex  // server name space
	// Object Folder =AddressSpaceRoot::instance()->getObjectsFolder();

	m_nameSpaceIndex = namespaceIndex;

	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

#ifdef TESTING

	TestVariable::s_init();

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr staticVariablesFolder = new StaticsModule();
		result = staticVariablesFolder->initModule(_T("Static"), namespaceIndex, this);
		m_modules.push_back(staticVariablesFolder);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr dynamicVariablesFolder = new DynamicsModule();
		result = dynamicVariablesFolder->initModule(_T("Dynamic"), namespaceIndex, this);
		m_modules.push_back(dynamicVariablesFolder);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr matrixVariablesFolder = new MatrixModule();
		result = matrixVariablesFolder->initModule(_T("Matrix"), namespaceIndex, this);
		m_modules.push_back(matrixVariablesFolder);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr changingContentFolder = new ChangingContentModule();
		result = changingContentFolder->initModule(_T("ChangingContent"), namespaceIndex, this);
		m_modules.push_back(changingContentFolder);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr userAccessFolder = new UserAccessModule();
		result = userAccessFolder->initModule(_T("UserAccess"), namespaceIndex, this);
		m_modules.push_back(userAccessFolder);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new BrowseTestModule();
		result = module->initModule(_T("BrowseTest"), namespaceIndex, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new CustomDataTypeTestModule();
		result = module->initModule(_T("CustomDataTypeTest"), namespaceIndex, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new ExportImportNodesModule();
		result = module->initModule(_T("ExportImport"), namespaceIndex, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new AnalogItemsModule();
		// The CTT is configured to expects this node in namespace 1
		result = module->initModule(_T("AnalogItems"), 1, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new DiscreteItemsModule();
		// The TestClient expects these nodes in namespace 1
		result = module->initModule(_T("DiscreteItems"), 1, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new AttributeItemsModule();
		result = module->initModule(_T("AttributeItems"), namespaceIndex, this);
		m_modules.push_back(module);
	}

	defineSupportedAggregateFunctions();

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new DynamicHistoricalItemsModule();
		result = module->initModule(_T("DynamicHistoricalDataItems"), namespaceIndex, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new StaticHistoricalItemsModule();
		result = module->initModule(_T("StaticHistoricalDataItems"), namespaceIndex, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new HistoricalEventsModule();
		result = module->initModule(_T("HistoricalEventNodes"), namespaceIndex, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new StructuredDataTypeModule();
		// use the refrigerator namespace
		OTUInt16 typeNamespaceIndex = getApplicationModule()->getTypesNameSpaceIndex();
		result = module->initModule(_T("CustomStructuredDataTypeTest"), typeNamespaceIndex, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new PubSubModule();
		result = module->initModule(_T("PubSub"), namespaceIndex, this);
		m_modules.push_back(module);
	}

	// Create the methods used for simulation variables
	createSimVarMethods();

	//few nodes for the nasty tests.
	createNodeIdTestVariables();

	// Create nodes for testing query services
	ObjectPtr queryTestArea = createQueryTestArea();
	this->addReference(Statics::ReferenceTypeId_Organizes, queryTestArea);

	// Create nodes for delayed service responses
	if(StatusCode::isGood(result))
	{
		m_serviceDelayModule = new ServiceDelayModule();
		result = m_serviceDelayModule->initModule(_T("ServiceDelays"), namespaceIndex, this);
	}

	testValueConstructors();

	SetServiceLevelMethod::create(NodeId(2, _T("SetServiceLevel")), this);

#if TB5_ALARMS
	TestModelChangeEventMethod::create(NodeId(2, _T("ModelChangeMethod")), this);

	TestSemanticChangeEventMethod::create(NodeId(2, _T("SemanticChangeMethod")), this);
#endif

	// add a method to enable / disable tracing
	{
		EnableTracingMethodPtr method = new EnableTracingMethod();
		tstring strMethodName(_T("EnableTracing"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		method->setDescription(LocalizedText(_T("Enables / disables tracing in server"), _T("en")));
		tstring strMethodId(_T("EnableTracing"));

		method->setNodeId(NodeId(m_nameSpaceIndex, strMethodId));
		method->setExecutable(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);

		{
			// test changing nodeId
			NodeId origNodeId = *this->getNodeId();
			setNodeId(NodeId(m_nameSpaceIndex, _T("RenamedNodeId")));
			setNodeId(origNodeId);
		}


		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_Boolean));
		param.setName(_T("enable"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_String));
		param.setName(_T("TraceFileName"));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("MaxFileSize"));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("MaxBackups"));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("TraceGroup"));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("TraceLevel"));
		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("EnableTrace_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		pInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);

		// no output args for the method ......


		// test changing nodeId
		{
			NodeId origNodeId = *method->getNodeId();
			_ASSERTION(StatusCode::isGood(method->setNodeId(NodeId(m_nameSpaceIndex, _T("RenamedNodeId")))), _T("Rename of method shall work"));
			// rename back
			method->setNodeId(origNodeId);

			// test changing nodeId of new created data type
			NodeId testDataTypeNodeId;
			testDataTypeNodeId.setString(m_nameSpaceIndex, _T("NewDataTypeNodeId"));
			DataTypePtr testDataType = DataType::create();
			testDataType->setNodeId(testDataTypeNodeId);
			testDataType->setBrowseName(QualifiedName(_T("NewDataTypeNodeId"), m_nameSpaceIndex));
			testDataType->setDisplayName(LocalizedText(_T("NewDataTypeNodeId"), _T("en")));
			testDataType->insertTree();
			AddressSpaceRoot::instance()->getDataType(Statics::DataTypeId_UInt32)->addReference(Statics::ReferenceTypeId_HasSubtype, testDataType);

			// no node using this data type exists
			_ASSERTION(StatusCode::isGood(testDataType->setNodeId(NodeId(m_nameSpaceIndex, _T("RenamedNodeId")))), _T("Rename of unused data type shall work"));
			// rename back
			testDataType->setNodeId(testDataTypeNodeId);

			// use the data type in an argument
			DataValue testValue;
			param.setDataType(testDataTypeNodeId);
			param.setName(_T("TestParam"));
			inputArgs.push_back(param);
			containedValue.setArgumentArray(inputArgs);
			testValue.setValue(containedValue);
			pInputArgs->setDataValue(testValue);
			// change node ID of data type shall be refused (used data type)
			_ASSERTION(StatusCode::isBad(testDataType->setNodeId(NodeId(m_nameSpaceIndex, _T("RenamedNodeId")))), _T("Rename of used data type shall not succeed"));

			Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_Api, _T("The following single error trace is done by intention"), _T(""));
			_ASSERTION(StatusCode::isBad(testDataType->removeTree()), _T("Deletion of used data shall not succeed"));
			// restore input arguments
			pInputArgs->setDataValue(value);

			// changing node ID should work again
			_ASSERTION(StatusCode::isGood(testDataType->setNodeId(NodeId(m_nameSpaceIndex, _T("RenamedNodeId")))), _T("Rename of data type does not work"));
			// deletion of unused data type should work again
			_ASSERTION(StatusCode::isGood(testDataType->removeTree()), _T("Deletion of used data shall not succeed"));
		}
	}

	{
		CreateEndpointMethodPtr createEndpointMethod = new CreateEndpointMethod();
		createEndpointMethod->initialize(this, m_nameSpaceIndex);
		RemoveEndpointMethodPtr removeEndpointMethod = new RemoveEndpointMethod();
		removeEndpointMethod->initialize(this, m_nameSpaceIndex);
		DisableEndpointMethodPtr disableEndpointMethod = new DisableEndpointMethod();
		disableEndpointMethod->initialize(this, m_nameSpaceIndex);
		AddRemoveReverseConnectorMethodPtr addRemoveReverseConnectorMethod = new AddRemoveReverseConnectorMethod();
		addRemoveReverseConnectorMethod->initialize(this, m_nameSpaceIndex);
		DenySecureChannelOpenMethodPtr denySecureChannelOpenMethod = new DenySecureChannelOpenMethod();
		denySecureChannelOpenMethod->initialize(this, m_nameSpaceIndex);
		ChangeWatcherMethodPtr changeWatcherMethod = new ChangeWatcherMethod();
		changeWatcherMethod->initialize(this, m_nameSpaceIndex);
	}

	// configure the dynamic node cache
	DynamicNodeCache::initialize(this);

#if TB5_ALARMS
	CustomCallbackEvent::initialize(m_nameSpaceIndex);
#endif

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new AccessPatternModule();
		result = module->initModule(_T("AccessPattern"), namespaceIndex, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new RolePermissionModule();
		result = module->initModule(_T("RolePermissionTest"), namespaceIndex, this);
		m_modules.push_back(module);
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new FileModule();
		result = module->initModule(_T("FileSystem"), namespaceIndex, this);
		if(StatusCode::isSUCCEEDED(result))
		{
			m_modules.push_back(module);
		}
		else
		{
			// FileModule initialization will fail if the path to server files is not valid.
			// We issue a warning and let the server continue, of course 'FileSystem' will not be available.
			tstring message = tstring(_T("File Module initialization failed, 'FileSystem' will not be available. Reason: ")) +
				tstring(getEnumStatusCodeString(result));
			Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User1, message, _T("StructoAutomoldServer"));
			_tprintf(_T("%s\n"), message.c_str());
			result = EnumStatusCode_Good;
		}
	}

	if(StatusCode::isGood(result))
	{
        result = createParrallelAccessNodes();
	}

	if(StatusCode::isGood(result))
	{
		ModuleFolderPtr module = new OperationLimitsModule();
		result = module->initModule(_T("OperationLimits"), namespaceIndex, this);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to initialize OperationLimits module\n"));
		}
		m_modules.push_back(module);
	}

	{
		// a test that deletion of other nodes within destructor works
		ObjectPtr objectWithNested = Object::create();
		objectWithNested->setNodeId(NodeId(1, _T("ObjectWithNested")));
		objectWithNested->setBrowseName(QualifiedName(_T("ObjectWithNested"), 1));
		objectWithNested->setDisplayName(LocalizedText(_T("ObjectWithNested"), _T("")));
		AddressSpaceRoot::instance()->getObjectsFolder()->addReference(Statics::ReferenceTypeId_Organizes, objectWithNested);
		{
			// use a separate block to ensure that the variables are not held alive by a ref count

			// create a simple var 
			VariablePtr nestedVar = Variable::create();
			nestedVar->setNodeId(NodeId(1, _T("NestedVar")));
			nestedVar->setBrowseName(QualifiedName(_T("NestedVar"), 1));
			nestedVar->setDisplayName(LocalizedText(_T("NestedVar"), _T("")));
			nestedVar->setDataType(Statics::DataTypeId_UInt32);
			AddressSpaceRoot::instance()->getObjectsFolder()->addReference(Statics::ReferenceTypeId_Organizes, nestedVar);

			// create the variable which deletes 
			ObjectPointer<VarWithLinked> varWithNested = new VarWithLinked();
			varWithNested->setNodeId(NodeId(1, _T("VarWithNested")));
			varWithNested->setBrowseName(QualifiedName(_T("VarWithNested"), 1));
			varWithNested->setDisplayName(LocalizedText(_T("VarWithNested"), _T("")));
			varWithNested->setDataType(Statics::DataTypeId_UInt32);
			varWithNested->m_linkedVar = nestedVar;
			objectWithNested->addReference(Statics::ReferenceTypeId_Organizes, varWithNested);
		}

		// initially the nodes "objectWithNested" and "varWithNested" will be removed
		// during deletion of "varWithNested" another node "nestedVar" will be removed
		objectWithNested->removeTree();

	}
#endif 
	
	return result;
}

void TestModule::endModule()
{
	TestVariable::s_end();

	if(m_serviceDelayModule)
	{
		m_serviceDelayModule->endModule();
		m_serviceDelayModule.reset();
	}

	// cleanup in reverse order
	for(OTInt32 i = (OTInt32)m_modules.size() - 1; i > 0; i--)
	{
		m_modules[i]->endModule();
	}
	m_modules.clear();

	ModuleFolder::endModule();
}

void TestModule::testValueConstructors()
{
	Value vVoid;
	Value vBool(true);
	Value vChar(3);
	Value vUChar((unsigned char)4);
	Value vShort((short)5);
	Value vUShort((unsigned short)6);
	Value vInt32((OTInt32)7);
	Value vUInt32((OTUInt32)8);
	Value vInt64((OTInt64)9);
	Value vUInt64((OTUInt64)10);
	Value vFloat((float)11.11);
	Value vDouble((double)12.12);
	Value vTCHAR(_T("13"));
	tstring str = _T("14");
	Value vTstring(str);
	DateTime dTime = DateTime::getUtcNow();
	Value vDataTime(&dTime);
	unsigned char buff[4] = {1, 2, 3, 4};
	Value vByteString1(buff, 4);
	ByteString bStr(4, buff);
	Value vByteString2(&bStr);
	Guid newGuid = Guid::generateGuid();
	Value bGuid(&newGuid);
	NodeId nId(10, 10);
	Value vNodeId(&nId);
	ExpandedNodeId enId(10, 10);
	Value vExpandedNodeId(&enId);
	Value vEnumStatusCode(EnumStatusCode_Good);
	QualifiedName qName(_T("Name"), 10);
	Value vQualifiedName(&qName);
	LocalizedText locTxt(_T("Text"), _T("en"));
	Value vLocalizedText(&locTxt);
	DataValue dVal;
	dVal.setValue(vQualifiedName);
	Range rng;
	rng.setHigh(10);
	rng.setLow(-10);
	Value vrange(&rng);
	Argument arg;
	arg.setName(_T("Name"));
	arg.setDataType(nId);
	Value vArgument(&arg);

	std::vector<bool> boolArray;
	std::vector<unsigned char> uCharArray;
	std::vector<signed char> charArray;
	std::vector<unsigned short> uShortArray;
	std::vector<short> shortArray;
	std::vector<OTUInt32> uInt32Array;
	std::vector<OTInt32> int32Array;
	std::vector<OTUInt64> uInt64Array;
	std::vector<OTInt64> int64Array;
	std::vector<float> floatArray;
	std::vector<double> doubleArray;
	std::vector<DateTime> dateTimeArray;
	std::vector<tstring> tstringArray;
	std::vector<Guid> guidArray;
	std::vector<NodeId> nodeIdArray;
	std::vector<ExpandedNodeId> exNodeIdArray;
	std::vector<EnumStatusCode> statusCodeArray;
	std::vector<QualifiedName> qualifiedNameArray;
	std::vector<LocalizedText> locTextArray;
	std::vector<Argument> argArray;
	std::vector<Value> valArray;
	std::vector<ByteString> byteStringArray;
	std::vector<DataValue> dataValueArray;
	std::vector<Range> rangeArray;

	Value	vboolArray				(boolArray);
	Value	vuCharArray				(uCharArray);
	Value	vcharArray				(charArray);
	Value	vuShortArray			(uShortArray);
	Value	vshortArray				(shortArray);
	Value	vuInt32Array			(uInt32Array);
	Value	vint32Array				(int32Array);
	Value	vuInt64Array			(uInt64Array);
	Value	vint64Array				(int64Array);
	Value	vfloatArray				(floatArray);
	Value	vdoubleArray			(doubleArray);
	Value	vdateTimeArray			(dateTimeArray);
	Value	vtstringArray			(tstringArray);
	Value	vguidArray				(guidArray);
	Value	vnodeIdArray			(nodeIdArray);
	Value	vexNodeIdArray			(exNodeIdArray);
	Value	vstatusCodeArray		(statusCodeArray);
	Value	vqualifiedNameArray		(qualifiedNameArray);
	Value	vlocTextArray			(locTextArray);
	Value	vargArray				(argArray);
	Value	vvalArray				(valArray);
	Value	vbyteStringArray		(byteStringArray);
	Value	vrangeArray				(rangeArray);
}

ObjectPtr TestModule::createQueryTestArea()
{
	ObjectTypePtr queryTestAreaType = ObjectType::create();
	ObjectPtr queryTestArea = Object::create();
	//
	// QueryTestAreaType
	//
	queryTestAreaType->setBrowseName(QualifiedName(_T("QueryTestAreaType"), m_nameSpaceIndex));
	queryTestAreaType->setDisplayName(LocalizedText(_T("QueryTestAreaType"), _T("")));
	queryTestAreaType->setIsAbstract(false);
	queryTestAreaType->setNodeId(NodeId(m_nameSpaceIndex, _T("QueryTestAreaType")));
	queryTestAreaType->init(NULL);

	BaseNodePtr baseObjectType = AddressSpaceRoot::instance()->getNode(Statics::ObjectTypeId_BaseObjectType);
	baseObjectType->addReference(Statics::ReferenceTypeId_HasSubtype, queryTestAreaType);

	//
	// QueryTestArea
	//
	NodeId queryTestAreaTypeId(m_nameSpaceIndex, _T("QueryTestAreaType"));

	queryTestArea->setNodeId(NodeId(m_nameSpaceIndex, _T("QueryTestArea")));
	queryTestArea->setBrowseName(QualifiedName(_T("QueryTestArea"), m_nameSpaceIndex));
	queryTestArea->setDisplayName(LocalizedText(_T("QueryTestArea"), _T("en")));
	queryTestArea->init(&queryTestAreaTypeId);

	// ScalarTestVariables
	ObjectPtr scalarTestVariablesFolder = Object::create();
	scalarTestVariablesFolder->setNodeId(NodeId(m_nameSpaceIndex, _T("QueryTestArea.ScalarTestVariables")));
	scalarTestVariablesFolder->setDisplayName(LocalizedText(_T("ScalarTestVariables"), _T("")));
	scalarTestVariablesFolder->setBrowseName(QualifiedName(_T("QueryTestArea.TestVariables"), m_nameSpaceIndex));
	scalarTestVariablesFolder->init(Statics::ObjectTypeId_FolderType);

	queryTestArea->addReference(Statics::ReferenceTypeId_HasComponent, scalarTestVariablesFolder);

	// ArrayTestVariables
	ObjectPtr arrayTestVariablesFolder = Object::create();
	arrayTestVariablesFolder->setNodeId(NodeId(m_nameSpaceIndex, _T("QueryTestArea.ArrayTestVariables")));
	arrayTestVariablesFolder->setDisplayName(LocalizedText(_T("ArrayTestVariables"), _T("")));
	arrayTestVariablesFolder->setBrowseName(QualifiedName(_T("QueryTestArea.TestVariables"), m_nameSpaceIndex));
	arrayTestVariablesFolder->init(Statics::ObjectTypeId_FolderType);

	queryTestArea->addReference(Statics::ReferenceTypeId_HasComponent, arrayTestVariablesFolder);

	// MyTestVariables
	ObjectPtr myTestVariablesFolder = Object::create();
	myTestVariablesFolder->setNodeId(NodeId(m_nameSpaceIndex, _T("QueryTestArea.MyTestVariables")));
	myTestVariablesFolder->setDisplayName(LocalizedText(_T("MyTestVariables"), _T("")));
	myTestVariablesFolder->setBrowseName(QualifiedName(_T("QueryTestArea.MyTestVariables"), m_nameSpaceIndex));
	myTestVariablesFolder->init(Statics::ObjectTypeId_FolderType);

	queryTestArea->addReference(Statics::ReferenceTypeId_HasComponent, myTestVariablesFolder);

	// Int32 (Scalar)
	DataValue int32ScalarValue;
	int32ScalarValue.setValue(Value((OTInt32)-1000));

	VariablePtr int32ScalarVariable = Variable::create();
	int32ScalarVariable->setNodeId(NodeId(m_nameSpaceIndex, _T("QueryTestArea.Int32Scalar")));
	int32ScalarVariable->setDisplayName(LocalizedText(_T("Int32Scalar"), _T("")));
	int32ScalarVariable->setBrowseName(QualifiedName(_T("Int32"), m_nameSpaceIndex));
	int32ScalarVariable->setValueRank(ValueRank_Scalar);
	int32ScalarVariable->setDataValue(int32ScalarValue);
	int32ScalarVariable->setDataType(Statics::DataTypeId_Int32);
	int32ScalarVariable->setAccessLevel(EnumAccessLevel_CurrentRead);
	int32ScalarVariable->init(Statics::VariableTypeId_BaseDataVariableType);
	// Do not grant read access for "query" user group
	ApplicationModule::assignAccessPatternAndRolePermissions(int32ScalarVariable, ApplicationModule::s_AccessKey_ReadHistory_Read_Read_None_None);

	scalarTestVariablesFolder->addReference(Statics::ReferenceTypeId_HasComponent, int32ScalarVariable);
	myTestVariablesFolder->addReference(Statics::ReferenceTypeId_HasComponent, int32ScalarVariable);

	// UInt32 (Scalar)
	DataValue uint32ScalarValue;
	uint32ScalarValue.setValue(Value((OTUInt32)1000));

	VariablePtr uint32ScalarVariable = Variable::create();
	uint32ScalarVariable->setNodeId(NodeId(m_nameSpaceIndex, _T("QueryTestArea.UInt32Scalar")));
	uint32ScalarVariable->setDisplayName(LocalizedText(_T("UInt32Scalar"), _T("")));
	uint32ScalarVariable->setBrowseName(QualifiedName(_T("UInt32"), m_nameSpaceIndex));
	uint32ScalarVariable->setValueRank(ValueRank_Scalar);
	uint32ScalarVariable->setDataValue(uint32ScalarValue);
	uint32ScalarVariable->setDataType(Statics::DataTypeId_UInt32);
	uint32ScalarVariable->setAccessLevel(EnumAccessLevel_CurrentRead);
	uint32ScalarVariable->init(Statics::VariableTypeId_BaseDataVariableType);

	scalarTestVariablesFolder->addReference(Statics::ReferenceTypeId_HasComponent, uint32ScalarVariable);

	// Int32 (Array)
	DataValue int32ArrayValue;
	{
		std::vector<OTInt32> values;
		values.push_back(-1000);
		values.push_back(-500);
		values.push_back(0);
		values.push_back(500);
		values.push_back(1000);

		Value value;
		value.setInt32Array(values);

		int32ArrayValue.setValue(value);
	}

	VariablePtr int32ArrayVariable = Variable::create();
	int32ArrayVariable->setNodeId(NodeId(m_nameSpaceIndex, _T("QueryTestArea.Int32Array")));
	int32ArrayVariable->setDisplayName(LocalizedText(_T("Int32Array"), _T("")));
	int32ArrayVariable->setBrowseName(QualifiedName(_T("Int32"), m_nameSpaceIndex));
	int32ArrayVariable->setValueRank(1);
	int32ArrayVariable->setDataValue(int32ArrayValue);
	int32ArrayVariable->setDataType(Statics::DataTypeId_Int32);
	int32ArrayVariable->setAccessLevel(EnumAccessLevel_CurrentRead);
	int32ArrayVariable->init(Statics::VariableTypeId_BaseDataVariableType);

	arrayTestVariablesFolder->addReference(Statics::ReferenceTypeId_HasComponent, int32ArrayVariable);

	// UInt32 (Array)
	DataValue uint32ArrayValue;
	{
		std::vector<OTUInt32> values;
		values.push_back(1000);
		values.push_back(500);
		values.push_back(0);
		values.push_back(500);
		values.push_back(1000);

		Value value;
		value.setUInt32Array(values);

		uint32ArrayValue.setValue(value);
	}

	VariablePtr uint32ArrayVariable = Variable::create();
	uint32ArrayVariable->setNodeId(NodeId(m_nameSpaceIndex, _T("QueryTestArea.UInt32Array")));
	uint32ArrayVariable->setDisplayName(LocalizedText(_T("UInt32Array"), _T("")));
	uint32ArrayVariable->setBrowseName(QualifiedName(_T("UInt32"), m_nameSpaceIndex));
	uint32ArrayVariable->setValueRank(1);
	uint32ArrayVariable->setDataValue(uint32ArrayValue);
	uint32ArrayVariable->setDataType(Statics::DataTypeId_UInt32);
	uint32ArrayVariable->setAccessLevel(EnumAccessLevel_CurrentRead);
	uint32ArrayVariable->init(Statics::VariableTypeId_BaseDataVariableType);

	arrayTestVariablesFolder->addReference(Statics::ReferenceTypeId_HasComponent, uint32ArrayVariable);
	myTestVariablesFolder->addReference(Statics::ReferenceTypeId_HasComponent, uint32ArrayVariable);

	// this variable can be created only of role permissions are enabled
		// UInt16 - only browsable / readable for Anonymous users
	DataValue uint16Value;
	uint16Value.setValue(Value((OTUInt16)333));

	VariablePtr onlyVisiblyForQueryVar = Variable::create();
	onlyVisiblyForQueryVar->setNodeId(NodeId(m_nameSpaceIndex, _T("QueryTestArea.OnlyVisibleForAnonymous")));
	onlyVisiblyForQueryVar->setDisplayName(LocalizedText(_T("OnlyVisibleForAnonymous"), _T("")));
	onlyVisiblyForQueryVar->setBrowseName(QualifiedName(_T("ForAnonymous"), m_nameSpaceIndex));
	onlyVisiblyForQueryVar->setDataValue(uint16Value);
	onlyVisiblyForQueryVar->setDataType(Statics::DataTypeId_UInt16);
	onlyVisiblyForQueryVar->setAccessLevel(EnumAccessLevel_CurrentRead);
	onlyVisiblyForQueryVar->init(Statics::VariableTypeId_BaseDataVariableType);
	if(!ApplicationModule::s_accessPatternEnabled)	// for AccessPattern, keep it visible and accessible for all users
	{
		std::vector<RolePermissionType> rolePermissions;
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
		rolePermissions.back().setPermissions(EnumPermissionType_Browse | EnumPermissionType_Read);
		onlyVisiblyForQueryVar->setRolePermissions(rolePermissions);
	}

	arrayTestVariablesFolder->addReference(Statics::ReferenceTypeId_HasComponent, onlyVisiblyForQueryVar);
	myTestVariablesFolder->addReference(Statics::ReferenceTypeId_HasComponent, onlyVisiblyForQueryVar);
	return queryTestArea;
}

void TestModule::defineSupportedAggregateFunctions()
{
#if TB5_HISTORY
	NodeId serverCapabilitiesAggregateFunctionsId(EnumNumericNodeId_Server_ServerCapabilities_AggregateFunctions);
	ObjectPtr serverCapabilitiesAggregateFunctions((Object*)(BaseNode*)AddressSpaceRoot::instance()->getNode(&serverCapabilitiesAggregateFunctionsId));

	NodeId historyServerCapabilitiesAggregateFunctionsId(EnumNumericNodeId_HistoryServerCapabilities_AggregateFunctions);
	ObjectPtr historyServerCapabilitiesAggregateFunctions((Object*)(BaseNode*)AddressSpaceRoot::instance()->getNode(&historyServerCapabilitiesAggregateFunctionsId));

	if(historyServerCapabilitiesAggregateFunctions.isNotNull() && serverCapabilitiesAggregateFunctions.isNotNull())
	{
		AggregateFunctionPtr averageAggregate = AggregateFunction::createAverageFunctionObject();
		if(averageAggregate.isNotNull())
		{
			// Add 'Average' aggregate to server capabilities
			serverCapabilitiesAggregateFunctions->addReference(Statics::ReferenceTypeId_Organizes, averageAggregate);
			historyServerCapabilitiesAggregateFunctions->addReference(Statics::ReferenceTypeId_Organizes, averageAggregate);
		}
	}
	else
	{
		_REPORT_ASSERTION(_T("Could not find the objects that reference the aggregations the server supports"));
	}
#endif
}

TestModule::SetServiceLevelMethod::SetServiceLevelMethod()
{
}

TestModule::SetServiceLevelMethod::~SetServiceLevelMethod()
{
}

ObjectPointer<TestModule::SetServiceLevelMethod> TestModule::SetServiceLevelMethod::create(const NodeId& methodNodeId, Server::BaseNodePtr parent)
{
	ObjectPointer<SetServiceLevelMethod> pMethod = new SetServiceLevelMethod();
	if(!pMethod)
	{
		return NULL;
	}

	pMethod->setNodeId(methodNodeId);
	pMethod->setBrowseName(QualifiedName(_T("SetServiceLevel"), 2));
	pMethod->setDisplayName(LocalizedText(_T("SetServiceLevel"), _T("en")));
	pMethod->init(NULL);
	pMethod->setDescription(LocalizedText(_T("Method to change the service level manually."), _T("en")));

	parent->addReference(Statics::ReferenceTypeId_HasComponent, pMethod);

	pMethod->setExecutable(true);

	std::vector<Argument> arguments;
	// define the input arguments 'service level' for the method
	Argument param;
	param.setDataType(Statics::DataTypeId_Byte);
	param.setName(_T("Service level"));
	param.setValueRank(ValueRank_Scalar);
	arguments.push_back(param);
	NodeId argNodeId(1, _T("SetServiceLevelInputArg"));
	pMethod->setInputArguments(&argNodeId, arguments);
	// no output arguments
	
	return pMethod;
}

EnumStatusCode TestModule::SetServiceLevelMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	EnumStatusCode result;
	std::vector<Value> inputValues = request->getInputArguments();
	if(inputValues.size() == 1)
	{
		OTUInt8 serviceLevel;
		result = inputValues[0].getUInt8(&serviceLevel);
		if(StatusCode::isGood(result))
		{
			result = AddressSpaceRoot::instance()->setServiceLevel(serviceLevel);
		}
	}
	else
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	request->setStatusCode(result);
	request->complete();
	return result;
}

EnumStatusCode TestModule::createParrallelAccessNodes()
{
    // ParallelAccess method
    TestParallelAccessMethodPtr paralleAccessMethod = TestParallelAccessMethod::initialize(m_nameSpaceIndex, _T("ParallelAccess"));
    EnumStatusCode result = this->addReference(Statics::ReferenceTypeId_HasComponent, paralleAccessMethod);

    if(StatusCode::isGood(result))
    {
        // ParallelAccessStressVar variable
        TestVariablePtr testVariablePll = new TestVariable();
        DataValue dataValue;
        EventFilter eventFilter;
        SimpleAttributeOperand operand1;
        SimpleAttributeOperand operand2;
        SimpleAttributeOperand operand3;
        SimpleAttributeOperand operand4;

        QualifiedName qNameBrowsePath1;
        QualifiedName qNameBrowsePath2;
        QualifiedName qNameBrowsePath3;
        QualifiedName qNameBrowsePath4;

        qNameBrowsePath1.setNameAndIndex(_T("EventType"), 0);
        operand1.addBrowsePath(&qNameBrowsePath1);
        operand1.setAttributeId(EnumAttributeId_Value);
        operand1.setTypeDefinitionId(NodeId(0, EnumNumericNodeId_BaseEventType));

        qNameBrowsePath2.setNameAndIndex(_T("Message"), 0);
        operand2.addBrowsePath(&qNameBrowsePath2);
        operand2.setAttributeId(EnumAttributeId_Value);
        operand2.setTypeDefinitionId(NodeId(0, EnumNumericNodeId_BaseEventType));

        qNameBrowsePath3.setNameAndIndex(_T("Severity"), 0);
        operand3.addBrowsePath(&qNameBrowsePath3);
        operand3.setAttributeId(EnumAttributeId_Value);
        operand3.setTypeDefinitionId(NodeId(0, EnumNumericNodeId_BaseEventType));

        qNameBrowsePath4.setNameAndIndex(_T("SourceName"), 0);
        operand4.addBrowsePath(&qNameBrowsePath4);
        operand4.setAttributeId(EnumAttributeId_Value);
        operand4.setTypeDefinitionId(NodeId(0, EnumNumericNodeId_BaseEventType));

        eventFilter.addSelectClause(&operand1);
        eventFilter.addSelectClause(&operand2);
        eventFilter.addSelectClause(&operand3);
        eventFilter.addSelectClause(&operand4);
        dataValue.getValue()->setEventFilter(&eventFilter);

        tstring strVarName(_T("ParallelAccessStressVar"));
        testVariablePll->setBrowseName(QualifiedName(strVarName, m_nameSpaceIndex));
        LocalizedText ltStr;
        ltStr.setText(strVarName);
        ltStr.setLocale(_T("en"));
        testVariablePll->setDisplayName(&ltStr);
        testVariablePll->setNodeId(NodeId(m_nameSpaceIndex, strVarName));
        testVariablePll->setDataType(Statics::DataTypeId_EventFilter);
        testVariablePll->setVariableType(Statics::VariableTypeId_BaseVariableType);
        testVariablePll->setAccessLevel(testVariablePll->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
        result = testVariablePll->insert();
        if(StatusCode::isGood(result))
        {
            result = testVariablePll->setDataValue(dataValue);
        }
        if(StatusCode::isGood(result))
        {
            result = this->addReference(Statics::ReferenceTypeId_HasComponent, testVariablePll);
        }
    }

    return result;
}

EnumStatusCode BrowseTestModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	//////////////////////////////////////////////////////////////////////////
	// Create folder 'NodeClasses'
	//////////////////////////////////////////////////////////////////////////
	ObjectPtr nodeClassesFolder = Object::create();
	tstring nodeClassesFolderId(_T("NodeClasses"));
	nodeClassesFolder->setBrowseName(QualifiedName(nodeClassesFolderId, namespaceIndex));
	nodeClassesFolder->setDisplayName(LocalizedText(nodeClassesFolderId, _T("")));
	nodeClassesFolder->setNodeId(NodeId(namespaceIndex, nodeClassesFolderId));
	nodeClassesFolder->init(Statics::ObjectTypeId_FolderType);

	//////////////////////////////////////////////////////////////////////////
	// Create variable 'NodeClass_Variable'
	//////////////////////////////////////////////////////////////////////////
	VariablePtr nodeClassVariable = Variable::create();
	tstring nodeClassVariableId(_T("NodeClass_Variable"));
	nodeClassVariable->setBrowseName(QualifiedName(nodeClassVariableId, namespaceIndex));
	nodeClassVariable->setDisplayName(LocalizedText(nodeClassVariableId, _T("")));
	nodeClassVariable->setNodeId(NodeId(namespaceIndex, nodeClassVariableId));
	nodeClassVariable->init(Statics::VariableTypeId_BaseDataVariableType);

	//////////////////////////////////////////////////////////////////////////
	// Create variable type 'NodeClass_VariableType'
	//////////////////////////////////////////////////////////////////////////
	VariableTypePtr nodeClassVariableType = VariableType::create();
	tstring nodeClassVariableTypeId(_T("NodeClass_VariableType"));
	nodeClassVariableType->setBrowseName(QualifiedName(nodeClassVariableTypeId, namespaceIndex));
	nodeClassVariableType->setDisplayName(LocalizedText(nodeClassVariableTypeId, _T("")));
	nodeClassVariableType->setNodeId(NodeId(namespaceIndex, nodeClassVariableTypeId));
	nodeClassVariableType->init(NULL);

	//////////////////////////////////////////////////////////////////////////
	// Create object 'NodeClass_Object'
	//////////////////////////////////////////////////////////////////////////
	ObjectPtr nodeClassObject = Object::create();
	tstring nodeClassObjectId(_T("NodeClass_Object"));
	nodeClassObject->setBrowseName(QualifiedName(nodeClassObjectId, namespaceIndex));
	nodeClassObject->setDisplayName(LocalizedText(nodeClassObjectId, _T("")));
	nodeClassObject->setNodeId(NodeId(namespaceIndex, nodeClassObjectId));
	nodeClassObject->init(Statics::ObjectTypeId_BaseObjectType);

	//////////////////////////////////////////////////////////////////////////
	// Create object type 'NodeClass_ObjectType'
	//////////////////////////////////////////////////////////////////////////
	ObjectTypePtr nodeClassObjectType = ObjectType::create();
	tstring nodeClassObjectTypeId(_T("NodeClass_ObjectType"));
	nodeClassObjectType->setBrowseName(QualifiedName(nodeClassObjectTypeId, namespaceIndex));
	nodeClassObjectType->setDisplayName(LocalizedText(nodeClassObjectTypeId, _T("")));
	nodeClassObjectType->setNodeId(NodeId(namespaceIndex, nodeClassObjectTypeId));
	nodeClassObjectType->init(NULL);

	//////////////////////////////////////////////////////////////////////////
	// Create method 'NodeClass_Method'
	//////////////////////////////////////////////////////////////////////////
	MethodPtr nodeClassMethod = Method::create();
	tstring nodeClassMethodId(_T("NodeClass_Method"));
	nodeClassMethod->setBrowseName(QualifiedName(nodeClassMethodId, namespaceIndex));
	nodeClassMethod->setDisplayName(LocalizedText(nodeClassMethodId, _T("")));
	nodeClassMethod->setNodeId(NodeId(namespaceIndex, nodeClassMethodId));
	nodeClassMethod->init(NULL);

	//////////////////////////////////////////////////////////////////////////
	// Create data type 'NodeClass_DataType'
	//////////////////////////////////////////////////////////////////////////
	DataTypePtr nodeClassDataType = DataType::create();
	tstring nodeClassDataTypeId(_T("NodeClass_DataType"));
	nodeClassDataType->setBrowseName(QualifiedName(nodeClassDataTypeId, namespaceIndex));
	nodeClassDataType->setDisplayName(LocalizedText(nodeClassDataTypeId, _T("")));
	nodeClassDataType->setNodeId(NodeId(namespaceIndex, nodeClassDataTypeId));
	nodeClassDataType->init(NULL);

	//////////////////////////////////////////////////////////////////////////
	// Create view 'NodeClass_View'
	//////////////////////////////////////////////////////////////////////////
	ViewPtr nodeClassView = View::create();
	tstring nodeClassViewId(_T("NodeClass_View"));
	nodeClassView->setBrowseName(QualifiedName(nodeClassViewId, namespaceIndex));
	nodeClassView->setDisplayName(LocalizedText(nodeClassViewId, _T("")));
	nodeClassView->setNodeId(NodeId(namespaceIndex, nodeClassViewId));
	nodeClassView->init(NULL);

	ViewPtr nodeClassView2 = View::create();
	nodeClassViewId = _T("NodeClass_View2");
	nodeClassView2->setBrowseName(QualifiedName(nodeClassViewId, namespaceIndex));
	nodeClassView2->setDisplayName(LocalizedText(nodeClassViewId, _T("")));
	nodeClassView2->setNodeId(NodeId(namespaceIndex, nodeClassViewId));
	nodeClassView2->setContainsNoLoops(true);
	nodeClassView2->setEventNotifier(EnumEventNotifier_SubscribeToEvents);
	nodeClassView2->init(NULL);

	//////////////////////////////////////////////////////////////////////////
	// Create references
	//////////////////////////////////////////////////////////////////////////
	this->addReference(Statics::ReferenceTypeId_Organizes, nodeClassesFolder);
	nodeClassesFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassVariable);
	nodeClassesFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassVariableType);
	nodeClassesFolder->addReference(Statics::ReferenceTypeId_Organizes, nodeClassObject);
	nodeClassesFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassObjectType);
	nodeClassesFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassMethod);
	nodeClassesFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassDataType);
	nodeClassesFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassView);
	nodeClassesFolder->addReference(Statics::ReferenceTypeId_HasComponent, nodeClassView2);

	this->insertTree();

	return result;
}


EnumStatusCode CustomDataTypeTestModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	//////////////////////////////////////////////////////////////////////////
	// Create data type with for syntactic information
	//////////////////////////////////////////////////////////////////////////
	m_myInt32 = DataType::create();

	tstring nodeClassDataTypeId(_T("MyInt32"));
	m_myInt32->setBrowseName(QualifiedName(nodeClassDataTypeId, namespaceIndex));
	m_myInt32->setDisplayName(LocalizedText(nodeClassDataTypeId, _T("")));
	m_myInt32->setNodeId(NodeId(namespaceIndex, nodeClassDataTypeId));
	m_myInt32->init(NULL);

	//////////////////////////////////////////////////////////////////////////
	// Insert type into type tree
	//////////////////////////////////////////////////////////////////////////
	AddressSpaceRoot::instance()->getDataType(Statics::DataTypeId_Int32)->addReference(Statics::ReferenceTypeId_HasSubtype, m_myInt32);

	//////////////////////////////////////////////////////////////////////////
	// Create variable 'MyInt32_Variable'
	//////////////////////////////////////////////////////////////////////////
	m_myInt32Variable = TestVariableForAuditEvents::create();
	tstring myInt32VariableId(_T("MyInt32_Variable"));
	m_myInt32Variable->setBrowseName(QualifiedName(myInt32VariableId, namespaceIndex));
	m_myInt32Variable->setDisplayName(LocalizedText(myInt32VariableId, _T("")));
	m_myInt32Variable->setNodeId(NodeId(namespaceIndex, myInt32VariableId));
	m_myInt32Variable->setDataType(m_myInt32->getNodeId());
	DataValue initialValue;
	initialValue.setValue(Value((OTInt32)1234));
	m_myInt32Variable->setDataValue(initialValue);
	m_myInt32Variable->setAccessLevel(m_myInt32Variable->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	m_myInt32Variable->init(Statics::VariableTypeId_BaseDataVariableType);

	//////////////////////////////////////////////////////////////////////////
	// Create references
	//////////////////////////////////////////////////////////////////////////
	addReference(Statics::ReferenceTypeId_HasComponent, m_myInt32Variable);
	
	this->insertTree();

	return result;
}

void CustomDataTypeTestModule::endModule()
{
	// remove the variable before the data type, otherwise the data type is still in use
	// and will produce an error log
	if(m_myInt32Variable)
	{
		m_myInt32Variable->remove();
		m_myInt32Variable.reset();
	}
	if(m_myInt32)
	{
		m_myInt32->remove();
		m_myInt32.reset();
	}
	ModuleFolder::endModule();
}


EnumStatusCode ExportImportNodesModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	// xml Export functionality

	NodeSet2SetNodeExtensionsMethodPtr nodeSet2SetExtensionsMethod = new NodeSet2SetNodeExtensionsMethod();
	nodeSet2SetExtensionsMethod->initialize(this, namespaceIndex);
	NodeSet2GetNodeExtensionsMethodPtr nodeSet2GetExtensionsMethod = new NodeSet2GetNodeExtensionsMethod();
	nodeSet2GetExtensionsMethod->initialize(this, namespaceIndex);

	ObjectPtr exportEscapeCharObject = Object::create();
	tstring exportEscapeCharId(_T("EscapeTest<>\"'&():"));
	exportEscapeCharObject->setBrowseName(QualifiedName(exportEscapeCharId, namespaceIndex));
	exportEscapeCharObject->setDisplayName(LocalizedText(exportEscapeCharId, _T("")));
	exportEscapeCharObject->setNodeId(NodeId(namespaceIndex, exportEscapeCharId));
	exportEscapeCharObject->init(Statics::ObjectTypeId_BaseObjectType);
	exportEscapeCharObject->setWriteMask(EnumWriteMask_Description | EnumWriteMask_DisplayName);
	addReference(Statics::ReferenceTypeId_Organizes, exportEscapeCharObject);

#if TB5_ALARMS
	// create an event type for the duplicate node, event if it's not referenced by this folder
	ObjectTypePtr importDuplicateNodeEventType = Server::ObjectType::create();
	importDuplicateNodeEventType->setNodeId(NodeId(namespaceIndex, _T("ImportDuplicateNodeEventType")));
	importDuplicateNodeEventType->setBrowseName(QualifiedName(_T("ImportDuplicateNodeEventType"), namespaceIndex));
	importDuplicateNodeEventType->setDescription(LocalizedText(_T("Is raised when the NodeSet2Import tries to import an already existing node"), _T("en")));
	importDuplicateNodeEventType->setDisplayName(LocalizedText(_T("ImportDuplicateNodeEventType"), _T("en")));
	Server::AddressSpaceRoot::instance()->getNode(Statics::ObjectTypeId_BaseEventType)->addReference(Statics::ReferenceTypeId_HasSubtype, importDuplicateNodeEventType);
	importDuplicateNodeEventType->init(NULL);
	// add the NodeId property
	VariablePtr nodeIdProperty = Variable::create();
	nodeIdProperty->setNodeId(NodeId(namespaceIndex, _T("ImportDuplicateNodeEventType_NodeId")));
	nodeIdProperty->setBrowseName(QualifiedName(_T("NodeId"), namespaceIndex));
	nodeIdProperty->setDisplayName(LocalizedText(_T("NodeId"), _T("en")));
	nodeIdProperty->setDataType(Statics::DataTypeId_NodeId);
	nodeIdProperty->setAccessLevel(EnumAccessLevel_CurrentRead);
	importDuplicateNodeEventType->addReference(Statics::ReferenceTypeId_HasProperty, nodeIdProperty);
	nodeIdProperty->init(Statics::VariableTypeId_PropertyType);
	// add the XmlTag property
	VariablePtr xmlTagProperty = Variable::create();
	xmlTagProperty->setNodeId(NodeId(namespaceIndex, _T("ImportDuplicateNodeEventType_XmlTag")));
	xmlTagProperty->setBrowseName(QualifiedName(_T("XmlTag"), namespaceIndex));
	xmlTagProperty->setDisplayName(LocalizedText(_T("XmlTag"), _T("en")));
	xmlTagProperty->setDataType(Statics::DataTypeId_String);
	xmlTagProperty->setAccessLevel(EnumAccessLevel_CurrentRead);
	importDuplicateNodeEventType->addReference(Statics::ReferenceTypeId_HasProperty, xmlTagProperty);
	xmlTagProperty->init(Statics::VariableTypeId_PropertyType);
#endif

	//set the required event related properties and references to raise the event on the ExportImportFolder
	setEventNotifier(EnumEventNotifier_SubscribeToEvents);
	parentFolder->addReference(Statics::ReferenceTypeId_HasNotifier, this);

	//////////////////////////////////////////////////////////////////////////
	// Create string array for export/import errors
	//////////////////////////////////////////////////////////////////////////

	VariablePtr exportImportErrors = Variable::create();
	tstring exportImportErrorsId(_T("ExportImportErrors"));
	exportImportErrors->setBrowseName(QualifiedName(exportImportErrorsId, namespaceIndex));
	exportImportErrors->setDisplayName(LocalizedText(exportImportErrorsId, _T("")));
	exportImportErrors->setNodeId(NodeId(namespaceIndex, exportImportErrorsId));
	exportImportErrors->setDataType(Statics::DataTypeId_String);
	exportImportErrors->setAccessLevel(EnumAccessLevel_CurrentRead);
	exportImportErrors->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	exportImportErrors->setValueRank(1);
	exportImportErrors->insert();
	addReference(Statics::ReferenceTypeId_HasComponent, exportImportErrors);

	DataValue initialValue;
	initialValue.setValue(Value(std::vector<tstring>()));
	exportImportErrors->setDataValue(initialValue);

	//////////////////////////////////////////////////////////////////////////
	// Create string array for export/import warnings
	//////////////////////////////////////////////////////////////////////////

	VariablePtr exportImportWarnings = Variable::create();
	tstring exportImportWarningsId(_T("ExportImportWarnings"));
	exportImportWarnings->setBrowseName(QualifiedName(exportImportWarningsId, namespaceIndex));
	exportImportWarnings->setDisplayName(LocalizedText(exportImportWarningsId, _T("")));
	exportImportWarnings->setNodeId(NodeId(namespaceIndex, exportImportWarningsId));
	exportImportWarnings->setDataType(Statics::DataTypeId_String);
	exportImportWarnings->setAccessLevel(EnumAccessLevel_CurrentRead);
	exportImportWarnings->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	exportImportWarnings->setValueRank(1);
	exportImportWarnings->insert();
	addReference(Statics::ReferenceTypeId_HasComponent, exportImportWarnings);

	//DataValue initialWarningsValue;
	//std::vector<tstring> initialWarningsStringVector;
	//initialWarningsStringVector.push_back(_T("no warnings"));
	//initialWarningsValue.setValue(Value(initialWarningsStringVector));
	//m_ExportImportWarnings->setDataValue(initialWarningsValue);

	//////////////////////////////////////////////////////////////////////////
	// Create string array for export/import global extensions
	//////////////////////////////////////////////////////////////////////////
	
	VariablePtr exportImportGlobalExtensions = TestVariableForAuditEvents::create();
	tstring exportImportGlobalExtensionsId(_T("ExportImportGlobalExtensions"));
	exportImportGlobalExtensions->setBrowseName(QualifiedName(exportImportGlobalExtensionsId, namespaceIndex));
	exportImportGlobalExtensions->setDisplayName(LocalizedText(exportImportGlobalExtensionsId, _T("")));
	exportImportGlobalExtensions->setNodeId(NodeId(namespaceIndex, exportImportGlobalExtensionsId));
	exportImportGlobalExtensions->setDataType(Statics::DataTypeId_String);
	exportImportGlobalExtensions->setAccessLevel(exportImportGlobalExtensions->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	exportImportGlobalExtensions->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	exportImportGlobalExtensions->setValueRank(1);
	exportImportGlobalExtensions->insert();
	addReference(Statics::ReferenceTypeId_HasComponent, exportImportGlobalExtensions);


	//////////////////////////////////////////////////////////////////////////
	// Create string array for export errors
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 

	NodeId nextExportErrorNodeId(namespaceIndex, _T("NextExportError"));
	BaseNodePtr nextExportErrorNode = AddressSpaceRoot::instance()->getNode(&nextExportErrorNodeId);
	if(nextExportErrorNode.isNull())
	{
		OTUInt16 namespaceId;
		AddressSpaceRootPtr addressSpace = AddressSpaceRoot::instance();
		addressSpace->getToolkitNamespaceIndex(&namespaceId);

		SoftingOPCToolbox5::Server::VariablePtr nextExportError;
		nextExportError = TestVariableForAuditEvents::create();
		tstring nextExportErrorId(_T("NextExportError"));
		nextExportError->setBrowseName(QualifiedName(nextExportErrorId, namespaceIndex));
		nextExportError->setDisplayName(LocalizedText(nextExportErrorId, _T("")));
		nextExportError->setNodeId(NodeId(namespaceIndex, nextExportErrorId));
		//nextExportError->setDataType(Statics::DataTypeId_String);
		nextExportError->setDataType(NodeId(namespaceId, _T("enumModelDesignerStatusType")));
		nextExportError->setAccessLevel(nextExportError->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		nextExportError->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		nextExportError->setValueRank(ValueRank_Scalar);
		nextExportError->insert();
		addReference(Statics::ReferenceTypeId_HasComponent, nextExportError);

		//DataValue initialValue;
		DataValue initialNextExportDataError;
		Value initialNextExportError = EnumModelDesignerStatus_Good;
		initialNextExportDataError.setValue(initialNextExportError);
		nextExportError->setDataValue(initialNextExportDataError);
	}

	return EnumStatusCode_Good;
}


EnumStatusCode ServiceDelayModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	createDelayVariable(_T("CloseSessionDelay"),         namespaceIndex, EnumNumericNodeId_CloseSessionRequest,         100);
	createDelayVariable(_T("HistoryReadDelay"),          namespaceIndex, EnumNumericNodeId_HistoryReadRequest,          0);
	createDelayVariable(_T("DeleteSubscriptionDelay"),   namespaceIndex, EnumNumericNodeId_DeleteSubscriptionsRequest,  100);
	createDelayVariable(_T("CreateMonitoredItemsDelay"), namespaceIndex, EnumNumericNodeId_CreateMonitoredItemsRequest, 100);
	createDelayVariable(_T("ModifyMonitoredItemsDelay"), namespaceIndex, EnumNumericNodeId_ModifyMonitoredItemsRequest, 100);
	createDelayVariable(_T("DeleteMonitoredItemsDelay"), namespaceIndex, EnumNumericNodeId_DeleteMonitoredItemsRequest, 100);

	this->insertTree();

	return result;
}

void ServiceDelayModule::endModule()
{
	m_delayVariables.clear();
	ModuleFolder::endModule();
}

void ServiceDelayModule::createDelayVariable(const tstring& name, OTUInt16 namespaceIndex, EnumNumericNodeId serviceId, OTUInt32 initialValue)
{
	DataValue dv;
	Value val;
	Server::VariablePtr delayVar = TestVariableForAuditEvents::create();
	delayVar->setBrowseName(QualifiedName(name, namespaceIndex));
	delayVar->setDisplayName(LocalizedText(name, _T("")));
	delayVar->setNodeId(NodeId(namespaceIndex, name));
	delayVar->setDataType(Statics::DataTypeId_UInt32);
	delayVar->setAccessLevel(delayVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	delayVar->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	delayVar->setValueRank(ValueRank_Scalar);
	val.setUInt32(initialValue);
	dv.setValue(val);
	delayVar->setDataValue(dv);
	addReference(Statics::ReferenceTypeId_HasComponent, delayVar);

	m_delayVariables.insert(make_pair(serviceId, delayVar));
}

OTUInt32 ServiceDelayModule::getServiceDelay(EnumNumericNodeId serviceId)
{
	OTUInt32 delay = 0;
	std::map<EnumNumericNodeId, Server::VariablePtr>::iterator findIter = m_delayVariables.find(serviceId);
	if(findIter != m_delayVariables.end())
	{
		Server::VariablePtr delayVar = findIter->second;
		if(delayVar)
		{
			if(StatusCode::isBad(delayVar->getDataValue()->getValue()->getUInt32(&delay)))
			{
				delay = 0;
			}
		}
	}
	return delay;
}

EnumStatusCode ServiceDelayModule::setServiceDelay(EnumNumericNodeId serviceId, OTUInt32 delay)
{
	EnumStatusCode result = EnumStatusCode_Good;

	std::map<EnumNumericNodeId, Server::VariablePtr>::iterator findIter = m_delayVariables.find(serviceId);
	if(findIter != m_delayVariables.end())
	{
		Server::VariablePtr delayVar = findIter->second;
		if(delayVar)
		{
			DataValue dv;
			Value val;
			val.setUInt32(delay);
			dv.setValue(val);
			result = delayVar->setDataValue(dv);
		}
	}
	else
	{
		result = EnumStatusCode_BadNotSupported;
	}
	return result;
}

OTUInt32 TestModule::getServiceDelay(EnumNumericNodeId serviceId)
{
	if(m_serviceDelayModule)
	{
		return m_serviceDelayModule->getServiceDelay(serviceId);
	}
	return 0;
}

EnumStatusCode TestModule::setServiceDelay(EnumNumericNodeId serviceId, OTUInt32 delay)
{
	if(m_serviceDelayModule)
	{
		return m_serviceDelayModule->setServiceDelay(serviceId, delay);
	}
	return EnumStatusCode_BadInvalidState;
}

EnumStatusCode ChangingContentModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isGood(result))
	{
		result = this->setDescription(LocalizedText(_T("Object performing dynamic address changes"), _T("")));
	}
	if(StatusCode::isGood(result))
	{
		ChangeAddressSpaceMethodPtr changeMeth = new ChangeAddressSpaceMethod(this);

			NodeId diffNodeId;
		diffNodeId.setNumeric(0, EnumNumericNodeId_ServerType);

		/* TestVariablePtr testVariableDiffNodeId = createOneVariable(_T("Different var"), &diffNodeId);
		this->addReference(Statics::ReferenceTypeId_HasComponent, testVariableDiffNodeId); */

		AddressSpaceRoot* addrSpace = AddressSpaceRoot::instance();
		ObjectPointer<Server::BaseNode> typeNode = addrSpace->getNode(&diffNodeId);
		_ASSERTION(typeNode.isNotNull(), _T("Invalid type node!!"));
	}
	return result;
}

EnumStatusCode OperationLimitsModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isGood(result))
	{
		GetOperationLimitMethodPtr getOperationLimit = new GetOperationLimitMethod();
		result = getOperationLimit->initialize(this, namespaceIndex);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to initialize GetOperationLimitMethod\n"));
		}
	}
	if(StatusCode::isGood(result))
	{
		SetOperationLimitMethodPtr setOperationLimit = new SetOperationLimitMethod();
		result = setOperationLimit->initialize(this, namespaceIndex);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to initialize SetOperationLimitMethod\n"));
		}
	}
	if(StatusCode::isGood(result))
	{
		// reference the server's OperationLimits folder as a shortcut
		NodeId serverOperationLimitsId(EnumNumericNodeId_Server_ServerCapabilities_OperationLimits);
		ObjectPtr serverOperationLimits = AddressSpaceRoot::instance()->getObject(&serverOperationLimitsId);
		this->addReference(Statics::ReferenceTypeId_Organizes, serverOperationLimits);
	}


	return result;
}

