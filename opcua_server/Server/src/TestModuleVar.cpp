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
#include "ShutdownMethod.h"
#include "ChangeAddressSpaceMethod.h"
#include "ServerAnalogItem.h"
#include "ServerTwoStateItem.h"
#include "ServerMultiStateItem.h"
#include "HistorizingVariable.h"
#include "ServerDataType.h"
#include "Decimal.h"
#include "Constants.h"

using namespace SoftingOPCToolbox5;

#define MATRIX_LEN(x) (sizeof(x) / sizeof(**x))

EnumStatusCode StaticsModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	createAllScalarVariables(namespaceIndex);
	createAllArrayVariables(namespaceIndex);
	return result;
}

void StaticsModule::createAllScalarVariables(OTUInt16 namespaceIndex)
{
	(void)createAndInitVariable(_T("UInt8"),     namespaceIndex, Statics::DataTypeId_Byte);
	(void)createAndInitVariable(_T("UInt8_RO"),  namespaceIndex, Statics::DataTypeId_Byte,   false);
	(void)createAndInitVariable(_T("Int8"),      namespaceIndex, Statics::DataTypeId_SByte);
	(void)createAndInitVariable(_T("Int8_RO"),   namespaceIndex, Statics::DataTypeId_SByte,  false);
	(void)createAndInitVariable(_T("UInt16"),    namespaceIndex, Statics::DataTypeId_UInt16);
	(void)createAndInitVariable(_T("UInt16_RO"), namespaceIndex, Statics::DataTypeId_UInt16, false);
	(void)createAndInitVariable(_T("Int16"),     namespaceIndex, Statics::DataTypeId_Int16);
	(void)createAndInitVariable(_T("Int16_RO"),  namespaceIndex, Statics::DataTypeId_Int16,  false);
	(void)createAndInitVariable(_T("UInt32"),    namespaceIndex, Statics::DataTypeId_UInt32);
	(void)createAndInitVariable(_T("UInt32_RO"), namespaceIndex, Statics::DataTypeId_UInt32, false);
	(void)createAndInitVariable(_T("Int32"),     namespaceIndex, Statics::DataTypeId_Int32);
	(void)createAndInitVariable(_T("Int32_RO"),  namespaceIndex, Statics::DataTypeId_Int32,  false);
	(void)createAndInitVariable(_T("UInt64"),    namespaceIndex, Statics::DataTypeId_UInt64);
	(void)createAndInitVariable(_T("UInt64_RO"), namespaceIndex, Statics::DataTypeId_UInt64, false);
	(void)createAndInitVariable(_T("Int64"),     namespaceIndex, Statics::DataTypeId_Int64);
	(void)createAndInitVariable(_T("Int64_RO"),  namespaceIndex, Statics::DataTypeId_Int64,  false);
	(void)createAndInitVariable(_T("Double"),    namespaceIndex, Statics::DataTypeId_Double);
	(void)createAndInitVariable(_T("Double_RO"), namespaceIndex, Statics::DataTypeId_Double, false);
	(void)createAndInitVariable(_T("Float"),     namespaceIndex, Statics::DataTypeId_Float);
	(void)createAndInitVariable(_T("Float_RO"),  namespaceIndex, Statics::DataTypeId_Float,  false);

	VariablePtr testVariableBoolean = createAndInitVariable(_T("Boolean"), namespaceIndex, Statics::DataTypeId_Boolean);
	testVariableBoolean->setWriteMask(EnumWriteMask_Description | EnumWriteMask_DisplayName);

	testVariableBoolean = createAndInitVariable(_T("Boolean_RO"), namespaceIndex, Statics::DataTypeId_Boolean, false);
	testVariableBoolean->setWriteMask(EnumWriteMask_Description | EnumWriteMask_DisplayName);

	testVariableBoolean = createAndInitVariable(_T("Boolean_WO"), namespaceIndex, Statics::DataTypeId_Boolean, false);
	// clear the read bit and set write bit to make variable write-only
	testVariableBoolean->setAccessLevel((testVariableBoolean->getAccessLevel() & ~EnumAccessLevel_CurrentRead) | EnumAccessLevel_CurrentWrite);
	testVariableBoolean->setWriteMask(EnumWriteMask_Description | EnumWriteMask_DisplayName);

	(void)createAndInitVariable(_T("String"),            namespaceIndex, Statics::DataTypeId_String);
	(void)createAndInitVariable(_T("String_RO"),         namespaceIndex, Statics::DataTypeId_String, false);
	(void)createAndInitVariable(_T("ByteString"),        namespaceIndex, Statics::DataTypeId_ByteString);
	(void)createAndInitVariable(_T("ByteString_RO"),     namespaceIndex, Statics::DataTypeId_ByteString, false);
	(void)createAndInitVariable(_T("XmlElement"),        namespaceIndex, Statics::DataTypeId_XmlElement);
	(void)createAndInitVariable(_T("XmlElement_RO"),     namespaceIndex, Statics::DataTypeId_XmlElement, false);
	(void)createAndInitVariable(_T("NodeId"),            namespaceIndex, Statics::DataTypeId_NodeId);
	(void)createAndInitVariable(_T("NodeId_RO"),         namespaceIndex, Statics::DataTypeId_NodeId, false);
	(void)createAndInitVariable(_T("ExpandedNodeId"),    namespaceIndex, Statics::DataTypeId_ExpandedNodeId);
	(void)createAndInitVariable(_T("ExpandedNodeId_RO"), namespaceIndex, Statics::DataTypeId_ExpandedNodeId, false);
	(void)createAndInitVariable(_T("Guid"),              namespaceIndex, Statics::DataTypeId_Guid);
	(void)createAndInitVariable(_T("Guid_RO"),           namespaceIndex, Statics::DataTypeId_Guid, false);
	(void)createAndInitVariable(_T("DateTime"),          namespaceIndex, Statics::DataTypeId_DateTime);
	(void)createAndInitVariable(_T("DateTime_RO"),       namespaceIndex, Statics::DataTypeId_DateTime, false);
	(void)createAndInitVariable(_T("StatusCode"),        namespaceIndex, Statics::DataTypeId_StatusCode);
	(void)createAndInitVariable(_T("StatusCode_RO"),     namespaceIndex, Statics::DataTypeId_StatusCode, false);
	(void)createAndInitVariable(_T("LocalizedText"),     namespaceIndex, Statics::DataTypeId_LocalizedText);
	(void)createAndInitVariable(_T("QualifiedName"),     namespaceIndex, Statics::DataTypeId_QualifiedName);
	(void)createAndInitVariable(_T("Integer"),           namespaceIndex, Statics::DataTypeId_Integer);
	(void)createAndInitVariable(_T("UnsignedInteger"),   namespaceIndex, Statics::DataTypeId_UInteger);
	(void)createAndInitVariable(_T("Enumeration"),       namespaceIndex, Statics::DataTypeId_ServerState);
	(void)createAndInitVariable(_T("EUInformation"),     namespaceIndex, Statics::DataTypeId_EUInformation);
	(void)createAndInitVariable(_T("Range"),             namespaceIndex, Statics::DataTypeId_Range);
	(void)createAndInitVariable(_T("Argument"),          namespaceIndex, Statics::DataTypeId_Argument);
	(void)createAndInitVariable(_T("Duration"),          namespaceIndex, Statics::DataTypeId_Duration);
	(void)createAndInitVariable(_T("LocaleId"),          namespaceIndex, Statics::DataTypeId_LocaleId);
	(void)createAndInitVariable(_T("Number"),            namespaceIndex, Statics::DataTypeId_Number);
	(void)createAndInitVariable(_T("UtcTime"),           namespaceIndex, Statics::DataTypeId_UtcTime);
	(void)createAndInitVariable(_T("Variant"),           namespaceIndex, Statics::DataTypeId_BaseDataType);
	(void)createAndInitVariable(_T("Image"),             namespaceIndex, Statics::DataTypeId_Image);
	(void)createAndInitVariable(_T("ImageBMP"),          namespaceIndex, Statics::DataTypeId_ImageBMP);
	(void)createAndInitVariable(_T("ImageGIF"),          namespaceIndex, Statics::DataTypeId_ImageGIF);
	(void)createAndInitVariable(_T("ImageJPG"),          namespaceIndex, Statics::DataTypeId_ImageJPG);
	(void)createAndInitVariable(_T("ImagePNG"),          namespaceIndex, Statics::DataTypeId_ImagePNG);
	(void)createAndInitVariable(_T("Decimal"),			 namespaceIndex, Statics::DataTypeId_Decimal);

	// create error test variables
	TestVariablePtr testVariableError = createVariable(_T("ErrorTestVariable"), namespaceIndex, Statics::DataTypeId_String);
	DataValue initialValue;
	tstring initialName(_T("AnErrorTestVariable"));
	initialValue.getValue()->setString(initialName);
	initialValue.setServerTimestamp(DateTime::getUtcNow());
	initialValue.setSourceTimestamp(DateTime::getUtcNow());
	testVariableError->setDataValue(initialValue);
}

/*! @brief create some nodes for the NodeId TestCase. */
void TestModule::createNodeIdTestVariables(void)
{
	static const unsigned int namespaceIdx = 3;

	{ // add a test method in the test server
		MethodPtr method = new TestMethod(3, 3);
		tstring strMethodName(_T("Method"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("methodId"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);

		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("Input_1"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("Input_2"));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_Double));
		param.setName(_T("Input_3"));
		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("methodId_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		pInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);

		// define output args for the method ......
		VariablePtr pOutputArgs = Variable::create();
		vector<Argument> outputArgs;
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("Output_1"));
		param.setValueRank(ValueRank_Scalar);
		outputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("Output_2"));
		outputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_Double));
		param.setName(_T("Output_3"));
		outputArgs.push_back(param);

		containedValue.setArgumentArray(outputArgs);

		value.setValue(containedValue);
		pOutputArgs->setDataValue(value);
		pOutputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("methodId_OutputArguments")));
		pOutputArgs->setBrowseName(QualifiedName(BrowseName_OutputArguments, 0));
		pOutputArgs->setDisplayName(LocalizedText(BrowseName_OutputArguments, _T("en")));
		pOutputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pOutputArgs->insert();
		pOutputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pOutputArgs);
	}

	{ // add a second test method in the test server
		TestMethodPtr method = new TestMethod(3, 3);
		tstring strMethodName(_T("MethodAsync"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		method->setDescription(LocalizedText(_T("Method waits 2000 ms till processing"), _T("en")));
		tstring strMethodId(_T("methodIdAsync"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->setAsync(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);


		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("Input_1"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("Input_2"));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_Double));
		param.setName(_T("Input_3"));
		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("methodIdAsync_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		pInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);

		// define output args for the method ......
		VariablePtr pOutputArgs = Variable::create();
		vector<Argument> outputArgs;
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("Output_1"));
		param.setValueRank(ValueRank_Scalar);
		outputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("Output_2"));
		outputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_Double));
		param.setName(_T("Output_3"));
		outputArgs.push_back(param);

		containedValue.setArgumentArray(outputArgs);

		value.setValue(containedValue);
		pOutputArgs->setDataValue(value);
		pOutputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("methodIdAsync_OutputArguments")));
		pOutputArgs->setBrowseName(QualifiedName(BrowseName_OutputArguments, 0));
		pOutputArgs->setDisplayName(LocalizedText(BrowseName_OutputArguments, _T("en")));
		pOutputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pOutputArgs->insert();
		pOutputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pOutputArgs);
	}
	{ // add a method to measure performance
		TestPerfomanceMethodPtr method = new TestPerfomanceMethod();
		tstring strMethodName(_T("PerformanceMeasure"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		method->setDescription(LocalizedText(_T("Measures performance of system calls"), _T("en")));
		tstring strMethodId(_T("PerformanceMeasure"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);


		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("Time"));
		param.setDescription(LocalizedText(_T("Time for a single measurement (ms)"), _T("en")));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("PerformanceMeasure_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		pInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);

		// define output args for the method ......
		VariablePtr pOutputArgs = Variable::create();
		vector<Argument> outputArgs;
		param.setDataType(NodeId(0, EnumDataTypeId_Double));
		param.setValueRank(ValueRank_Scalar);
		param.setName(_T("getTickCount"));
		param.setDescription(LocalizedText(_T("Time for a single getTickCount (ms)"), _T("en")));
		outputArgs.push_back(param);

		param.setName(_T("getTickCount64"));
		param.setDescription(LocalizedText(_T("Time for a single getTickCount64 (ms)"), _T("en")));
		outputArgs.push_back(param);

		param.setName(_T("UtcNow"));
		param.setDescription(LocalizedText(_T("Time for a single UtcNow (ms)"), _T("en")));
		outputArgs.push_back(param);

		param.setName(_T("InterlockedIncrement"));
		param.setDescription(LocalizedText(_T("Time for a single InterlockedIncrement/decrement (ms)"), _T("en")));
		outputArgs.push_back(param);

		param.setName(_T("Lock"));
		param.setDescription(LocalizedText(_T("Time for a single simple Lock-Unlock (ms)"), _T("en")));
		outputArgs.push_back(param);

		param.setName(_T("AdderssSpaceLock"));
		param.setDescription(LocalizedText(_T("Time for a single Address space Lock-Unlock (ms)"), _T("en")));
		outputArgs.push_back(param);

		containedValue.setArgumentArray(outputArgs);

		value.setValue(containedValue);
		pOutputArgs->setDataValue(value);
		pOutputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("PerformanceMeasure_OutputArguments")));
		pOutputArgs->setBrowseName(QualifiedName(BrowseName_OutputArguments, 0));
		pOutputArgs->setDisplayName(LocalizedText(BrowseName_OutputArguments, _T("en")));
		pOutputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pOutputArgs->insert();
		pOutputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pOutputArgs);
	}

	{ // add the shutDown Method in the test server
		ObjectPointer< ShutdownMethod> method = new ShutdownMethod();
		method->initialize(this, namespaceIdx);
	}

	{ // add a second test method in the test server
		MethodPtr method = new TestMethod(0, 0);
		tstring strMethodName(_T("Method NoReference"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("methodId_NoReference"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->insert();
		// use a wrong reference type by intention, thus calling this method should fail
		this->addReference(Statics::ReferenceTypeId_Organizes, method);	}

	{ // add a second test method in the test server
		MethodPtr method = new TestMethod(0, 0);
		tstring strMethodName(_T("Method SubType"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("methodId_SubType"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasOrderedComponent, method);
	}

	{ // add a method which is not executable
		MethodPtr method = new TestMethod(0, 0);
		tstring strMethodName(_T("Method Not Executable"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("methodId_NotExecutable"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(false);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);
	}

	{
		// add a node directly providing data change notifications
		TestVariablePtr testIntertDataNotificationVar = new TestVariable();
		DataValue dataValue;
		dataValue.getValue()->setUInt32(1);

		tstring strVarName(_T("InsertDataNotificationVar"));
		testIntertDataNotificationVar->setBrowseName(QualifiedName(strVarName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strVarName);
		ltStr.setLocale(_T("en"));
		testIntertDataNotificationVar->setDisplayName(&ltStr);
		testIntertDataNotificationVar->setNodeId(TestVariable::getDirectHandledVarNodeId());
		testIntertDataNotificationVar->setDataType(Statics::DataTypeId_UInt32);
		testIntertDataNotificationVar->setVariableType(Statics::VariableTypeId_DataItemType);
		testIntertDataNotificationVar->setAccessLevel(testIntertDataNotificationVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		testIntertDataNotificationVar->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		testIntertDataNotificationVar->insert();
		// do not set any value for this variable - it handles all read / write / MI requests by itself without SDK cache
		//testIntertDataNotificationVar->setDataValue(dataValue);
		this->addReference(Statics::ReferenceTypeId_HasComponent, testIntertDataNotificationVar);
	}

	{ // add the method to insert data changes into a server MI
		MethodPtr method = new InsertDataNotificationMethod();
		tstring strMethodName(_T("InsertDataNotification"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("InsertDataNotificationId"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->setDescription(LocalizedText(_T("Method to inject a data change notification into a monitored item (only for testing)"), _T("en")));
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);

		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("SubscriptionId"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("MonitoredItemId"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumDataTypeId_DataValue));
		param.setName(_T("ValueToReport"));
		param.setValueRank(1);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumDataTypeId_Boolean));
		param.setName(_T("UseFilter"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumDataTypeId_Boolean));
		param.setName(_T("UseSleep"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("InsertDataNotification_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		pInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);
	}

#if TB5_ALARMS
	{ // add the method to insert event into a server MI
		MethodPtr method = new InsertEventNotificationMethod();
		tstring strMethodName(_T("InsertEventNotification"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("InsertEventNotificationId"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->setDescription(LocalizedText(_T("Method to inject a event notification into a monitored item (only for testing)"), _T("en")));
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);

		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("SubscriptionId"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("MonitoredItemId"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("option how to inject event"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumDataTypeId_NodeId));
		param.setName(_T("EventTypeId"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumDataTypeId_QualifiedName));
		param.setName(_T("Properties"));
		param.setValueRank(1);
		param.setDataType(NodeId(0, EnumDataTypeId_BaseDataType));
		param.setName(_T("Property values"));
		param.setValueRank(1);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumDataTypeId_Boolean));
		param.setName(_T("UseFilter"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("InsertEventNotification_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		pInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);
	}
#endif

	{ // add the method to insert data changes into a server MI
		MethodPtr method = new SetAttributeByTypeMethod();
		tstring strMethodName(_T("SetAttributeByType"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("SetAttributeByTypeId"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->setDescription(LocalizedText(_T("Method to set attributes directly by method not by setAttribute (only for testing)"), _T("en")));
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);

		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_NodeId));
		param.setName(_T("NodeId"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("AttributId"));
		//param.setValueRank(1);
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);
		param.setDataType(NodeId(0, EnumNumericNodeId_BaseDataType));
		param.setName(_T("Value"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("SetAttributeByType_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		pInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);
	}

	{ // add the method to test HasArgumentDescription references
		ArgumentDescriptionMethodPtr method = new ArgumentDescriptionMethod();
		method->initialize(this, m_nameSpaceIndex);
	}

	TestVariablePtr testVariableStrId = new TestVariable();
	DataValue dataValue;
	unsigned long aLong = 12345;
	dataValue.getValue()->setUInt32(aLong);
	NodeId* varType = Statics::DataTypeId_UInt32;

	tstring strVarName(_T("VarWithStrNodeID"));
	testVariableStrId->setBrowseName(QualifiedName(strVarName, m_nameSpaceIndex));
	LocalizedText ltStr;
	ltStr.setText(strVarName);
	ltStr.setLocale(_T("en"));
	testVariableStrId->setDisplayName(&ltStr);
	tstring nodeIdStr(_T("stringNodeId"));
	testVariableStrId->setNodeId(NodeId(namespaceIdx, nodeIdStr));
	testVariableStrId->setDataType(varType);
	testVariableStrId->setVariableType(Statics::VariableTypeId_DataItemType);
	testVariableStrId->setAccessLevel(testVariableStrId->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	testVariableStrId->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	testVariableStrId->insert();
	testVariableStrId->setDataValue(dataValue);
	this->addReference(Statics::ReferenceTypeId_HasComponent, testVariableStrId);

	TestVariablePtr testVariableNumId = new TestVariable();
	tstring numVarName(_T("VarWithNumNodeID"));
	testVariableNumId->setBrowseName(QualifiedName(numVarName, m_nameSpaceIndex));
	LocalizedText ltNum;
	ltNum.setText(numVarName);
	ltNum.setLocale(_T("en"));
	testVariableNumId->setDisplayName(&ltNum);
	testVariableNumId->setNodeId(NodeId(namespaceIdx, 6783));
	testVariableNumId->setDataType(varType);
	testVariableNumId->setVariableType(Statics::VariableTypeId_DataItemType);
	testVariableNumId->setAccessLevel(testVariableNumId->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	testVariableNumId->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	testVariableNumId->insert();
	testVariableNumId->setDataValue(dataValue);
	this->addReference(Statics::ReferenceTypeId_HasComponent, testVariableNumId);

	TestVariablePtr testVariableGuidId = new TestVariable();
	tstring guidVarName(_T("VarWithGuidNodeID"));
	testVariableGuidId->setBrowseName(QualifiedName(guidVarName, m_nameSpaceIndex));
	LocalizedText ltGuid;
	ltGuid.setText(guidVarName);
	ltGuid.setLocale(_T("en"));
	testVariableGuidId->setDisplayName(&ltGuid);

	Guid guid;
	GuidStruct* pGuid = (GuidStruct*)guid.getInternHandle();
	pGuid->data1 = 11;
	pGuid->data2 = 22;
	pGuid->data3 = 33;
	OTUInt8  data4[8] =  {1, 2, 3, 4, 5, 6, 7, 8};
	memcpy(pGuid->data4, data4, 8);
	NodeId nodeIDGuid;
	nodeIDGuid.setGuid(namespaceIdx, &guid);

	testVariableGuidId->setNodeId(nodeIDGuid);
	testVariableGuidId->setDataType(varType);
	testVariableGuidId->setVariableType(Statics::VariableTypeId_DataItemType);
	testVariableGuidId->setAccessLevel(testVariableGuidId->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	testVariableGuidId->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	testVariableGuidId->insert();
	testVariableGuidId->setDataValue(dataValue);
	this->addReference(Statics::ReferenceTypeId_HasComponent, testVariableGuidId);

	TestVariablePtr testVariableBS = new TestVariable();
	tstring bsVarName(_T("VarWithByteStringNodeID"));
	testVariableBS->setBrowseName(QualifiedName(bsVarName, m_nameSpaceIndex));
	LocalizedText ltBS;
	ltBS.setText(bsVarName);
	ltBS.setLocale(_T("en"));
	testVariableBS->setDisplayName(&ltBS);

	NodeId bsNodeID;
	unsigned char initData1[] = {0xAB, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 };
	ByteString bs;
	bs.init(10, initData1);
	bsNodeID.setOpaque(namespaceIdx, bs);

	testVariableBS->setNodeId(bsNodeID);
	testVariableBS->setDataType(varType);
	testVariableBS->setVariableType(Statics::VariableTypeId_DataItemType);
	testVariableBS->setAccessLevel(testVariableBS->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	testVariableBS->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	testVariableBS->insert();
	testVariableBS->setDataValue(dataValue);
	this->addReference(Statics::ReferenceTypeId_HasComponent, testVariableBS);

}

TestVariablePtr StaticsModule::createVariable(const tstring& varName, OTUInt16 namespaceIndex, const NodeId* pDataTypeId, bool writeable)
{
	TestVariablePtr testVariable = new TestVariable();

	testVariable->setBrowseName(QualifiedName(varName, namespaceIndex));
	LocalizedText lt;
	lt.setText(varName);
	lt.setLocale(_T("en"));
	testVariable->setDisplayName(&lt);
	testVariable->setNodeId(NodeId(namespaceIndex, varName));
	testVariable->setDataType(pDataTypeId);
	if(writeable)
	{
		// read / write access
		testVariable->setAccessLevel(testVariable->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	}
	else
	{
		// read-only access
		testVariable->setAccessLevel(EnumAccessLevel_CurrentRead);
	}

	testVariable->setWriteMask(EnumWriteMask_Description);
	testVariable->setVariableType(Statics::VariableTypeId_BaseDataVariableType);

	testVariable->insert();
	this->addReference(Statics::ReferenceTypeId_HasComponent, testVariable);
	return testVariable;
}

TestVariablePtr StaticsModule::createAndInitVariable(const tstring& varName, OTUInt16 namespaceIndex, const NodeId* pDataTypeId, bool writeable /*= true*/)
{
	TestVariablePtr variable = createVariable(varName, namespaceIndex, pDataTypeId, writeable);
	initVariableValue(variable, pDataTypeId);
	return variable;
}

TestVariablePtr MatrixModule::createMatrixVariable(const tstring& varName, OTUInt16 namespaceIndex, const NodeId* pDataTypeId, const std::vector<OTUInt32> &arrayDimensions, bool writeable)
{
	EnumStatusCode result;
	TestVariablePtr testVariable = new TestVariable();

	testVariable->setBrowseName(QualifiedName(varName, namespaceIndex));
	LocalizedText lt;
	lt.setText(varName);
	lt.setLocale(_T("en"));
	testVariable->setDisplayName(&lt);
	testVariable->setNodeId(NodeId(namespaceIndex, varName));
	testVariable->setDataType(pDataTypeId);
	//NodeId dataType = testVariable->getDataType();
	testVariable->setVariableType(Statics::VariableTypeId_DataItemType);
	if(writeable)
	{
		// read / write access
		testVariable->setAccessLevel(testVariable->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	}
	else
	{
		// read-only access
		testVariable->setAccessLevel(EnumAccessLevel_CurrentRead);
	}

	testVariable->setWriteMask(EnumWriteMask_Description | EnumWriteMask_ArrayDimensions | EnumWriteMask_ValueRank);
	testVariable->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	testVariable->insert();
	result = testVariable->setValueRank((OTUInt32)arrayDimensions.size());
	if(!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setValueRank failed: "), getEnumStatusCodeString(result));
	}
	testVariable->setArrayDimensions(arrayDimensions);
	if(!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setArrayDimensions failed: "), getEnumStatusCodeString(result));
	}
	this->addReference(Statics::ReferenceTypeId_HasComponent, testVariable);
	return testVariable;
}

void StaticsModule::initVariableValue(TestVariablePtr variable, const NodeId* dataTypeId)
{
	DataValue initialValue;

	if(dataTypeId->getNamespaceIndex() == 0 &&
		dataTypeId->getIdentifierType() == EnumIdentifierType_Numeric)
	{
		OTUInt32 numericId = 0;
		dataTypeId->getNumericIdentifier(&numericId);
		switch(numericId)
		{
		case EnumDataTypeId_EUInformation:
			{
				EUInformation euInfo;
				euInfo.setDescription(LocalizedText(_T("Hello EU Information world"), _T("en")));
				euInfo.setDisplayName(LocalizedText(_T("km/h"), _T("en")));
				initialValue.getValue()->setEUInformation(&euInfo);
			}
			break;
		case EnumDataTypeId_Boolean:
			initialValue.getValue()->setBoolean(false);
			break;
		case EnumDataTypeId_SByte:
			initialValue.getValue()->setInt8(numeric_limits<signed char>::max());
			break;
		case EnumDataTypeId_Byte:
			initialValue.getValue()->setUInt8(numeric_limits<unsigned char>::max());
			break;
		case EnumDataTypeId_Int16:
			initialValue.getValue()->setInt16(numeric_limits<short>::max());
			break;
		case EnumDataTypeId_UInt16:
			initialValue.getValue()->setUInt16(numeric_limits<unsigned short>::max());
			break;
		case EnumDataTypeId_Int32:
			initialValue.getValue()->setInt32(numeric_limits<int>::max());
			break;
		case EnumDataTypeId_UInt32:
			initialValue.getValue()->setUInt32(numeric_limits<unsigned int>::max());
			break;
		case EnumDataTypeId_Int64:
			initialValue.getValue()->setInt64(numeric_limits<long long>::max());
			break;
		case EnumDataTypeId_UInt64:
			initialValue.getValue()->setUInt64(numeric_limits<unsigned long long>::max());
			break;
		case EnumDataTypeId_Float:
			initialValue.getValue()->setFloat(numeric_limits<float>::max());
			break;
		case EnumDataTypeId_Double:
			initialValue.getValue()->setDouble(numeric_limits<double>::max());
			break;
		case EnumDataTypeId_String:
			initialValue.getValue()->setString(_T("DEMO"));
			break;
		case EnumDataTypeId_DateTime:
			{
				DateTime dateTime;
				dateTime.utcNow();
				initialValue.getValue()->setDateTime(&dateTime);
			}
			break;
		case EnumDataTypeId_ByteString:
			{
				unsigned char initData[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 };
				initialValue.getValue()->setByteString(initData, 10);
			}
			break;
		case EnumDataTypeId_Guid:
			{
				Guid guid = Guid::generateGuid();
				initialValue.getValue()->setGuid(&guid);
			}
			break;
		case EnumDataTypeId_XmlElement:
			{
				tstring initData = _T("<xml value=\"test\"/>");
				initialValue.getValue()->setXmlElement(XmlElement(initData));
			}
			break;
		case EnumDataTypeId_NodeId:
			{
				NodeId nodeId;
				nodeId.setNumeric(1, 6783);
				initialValue.getValue()->setNodeId(&nodeId);
			}
			break;
		case EnumDataTypeId_ExpandedNodeId:
			{
				ExpandedNodeId nodeId;
				nodeId.setNumeric(1, 67836666);
				initialValue.getValue()->setExpandedNodeId(&nodeId);
			}
			break;
		case EnumDataTypeId_StatusCode:
			initialValue.getValue()->setStatusCode(EnumStatusCode_BadOutOfMemory);
			break;
		case EnumDataTypeId_QualifiedName:
			{
				QualifiedName qn;
				qn.setName(_T("qualifiedTextName"));
				qn.setNamespaceIndex(1);
				initialValue.getValue()->setQualifiedName(&qn);
			}
			break;
		case EnumDataTypeId_LocalizedText:
			{
				LocalizedText lt;
				lt.setLocale(_T("en"));
				lt.setText(_T("localizedText"));
				initialValue.getValue()->setLocalizedText(&lt);
			}
			break;
		case EnumDataTypeId_Number:
			initialValue.getValue()->setUInt16(34);
			break;
		case EnumDataTypeId_Integer:
			initialValue.getValue()->setInt16(34);
			break;
		case EnumDataTypeId_UInteger:
			initialValue.getValue()->setUInt16(34);
			break;
		case EnumDataTypeId_ServerState:
			initialValue.getValue()->setInt32(0); // running
			break;
		case EnumDataTypeId_Range:
			{
				Range rng;
				rng.setHigh(10.0);
				rng.setLow(-10.0);
				initialValue.getValue()->setRange(&rng);
			}
			break;
		case EnumDataTypeId_Argument:
			{
				Argument arg;
				arg.setDataType(Statics::DataTypeId_LocalizedText);
				arg.setDescription(LocalizedText(_T("What is the ultimare answer?!"), _T("en")));
				arg.setName(_T("Sollution"));
				arg.setValueRank(ValueRank_OneOrMoreDimensions);
				std::vector<OTUInt32> dimensions;
				dimensions.push_back(10);
				dimensions.push_back(20);
				arg.setArrayDimensions(dimensions);

				initialValue.getValue()->setArgument(&arg);
			}
			break;
		case EnumDataTypeId_Decimal:
			{
				Decimal dec;
				dec.set(numeric_limits<OTInt32>::max(), numeric_limits<OTInt16>::max());
				initialValue.getValue()->setDecimal(&dec);
			}
			break;
		case EnumDataTypeId_Duration:
			initialValue.getValue()->setDouble(17.25);
			break;
		case EnumDataTypeId_LocaleId:
			initialValue.getValue()->setString(_T("en"));
			break;
		case EnumDataTypeId_UtcTime:
			{
				DateTime dateTime;
				dateTime.utcNow();
				initialValue.getValue()->setDateTime(&dateTime);
			}
			break;
		case EnumDataTypeId_BaseDataType:
			initialValue.getValue()->setBoolean(true);
			break;
		case EnumDataTypeId_Image:
			{
				unsigned char initData[] = {
					0x42, 0x4d, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0x00,
					0x00, 0x00, 0x24, 0x1c, 0xed, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x00, 0xe8, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x00, 0x00, 0x00,
					0x00, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x00};
					initialValue.getValue()->setByteString(initData, 134);
			}
			break;
		case EnumDataTypeId_ImageBMP:
			{
				unsigned char initData[] = {
					0x42, 0x4d, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0x00,
					0x00, 0x00, 0x24, 0x1c, 0xed, 0x00, 0xe8, 0xa2, 0x00, 0xe8, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x00, 0xe8, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x00, 0x00, 0x00,
					0x00, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x24, 0x1c, 0xed, 0x00};
					initialValue.getValue()->setByteString(initData, 134);
			}
			break;
		case EnumDataTypeId_ImageGIF:
			{
				unsigned char initData[] = {
					0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x05, 0x00, 0x05, 0x00, 0xf7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x66, 0x00, 0x00, 0x99, 0x00, 0x00, 0xcc, 0x00, 0x00, 0xff, 0x00, 0x2b, 0x00, 0x00, 0x2b, 0x33, 0x00, 0x2b, 0x66,
					0x00, 0x2b, 0x99, 0x00, 0x2b, 0xcc, 0x00, 0x2b, 0xff, 0x00, 0x55, 0x00, 0x00, 0x55, 0x33, 0x00, 0x55, 0x66, 0x00, 0x55, 0x99, 0x00, 0x55, 0xcc, 0x00, 0x55, 0xff, 0x00, 0x80, 0x00, 0x00, 0x80, 0x33, 0x00, 0x80, 0x66, 0x00, 0x80, 0x99, 0x00,
					0x80, 0xcc, 0x00, 0x80, 0xff, 0x00, 0xaa, 0x00, 0x00, 0xaa, 0x33, 0x00, 0xaa, 0x66, 0x00, 0xaa, 0x99, 0x00, 0xaa, 0xcc, 0x00, 0xaa, 0xff, 0x00, 0xd5, 0x00, 0x00, 0xd5, 0x33, 0x00, 0xd5, 0x66, 0x00, 0xd5, 0x99, 0x00, 0xd5, 0xcc, 0x00, 0xd5,
					0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x33, 0x00, 0xff, 0x66, 0x00, 0xff, 0x99, 0x00, 0xff, 0xcc, 0x00, 0xff, 0xff, 0x33, 0x00, 0x00, 0x33, 0x00, 0x33, 0x33, 0x00, 0x66, 0x33, 0x00, 0x99, 0x33, 0x00, 0xcc, 0x33, 0x00, 0xff, 0x33, 0x2b, 0x00,
					0x33, 0x2b, 0x33, 0x33, 0x2b, 0x66, 0x33, 0x2b, 0x99, 0x33, 0x2b, 0xcc, 0x33, 0x2b, 0xff, 0x33, 0x55, 0x00, 0x33, 0x55, 0x33, 0x33, 0x55, 0x66, 0x33, 0x55, 0x99, 0x33, 0x55, 0xcc, 0x33, 0x55, 0xff, 0x33, 0x80, 0x00, 0x33, 0x80, 0x33, 0x33,
					0x80, 0x66, 0x33, 0x80, 0x99, 0x33, 0x80, 0xcc, 0x33, 0x80, 0xff, 0x33, 0xaa, 0x00, 0x33, 0xaa, 0x33, 0x33, 0xaa, 0x66, 0x33, 0xaa, 0x99, 0x33, 0xaa, 0xcc, 0x33, 0xaa, 0xff, 0x33, 0xd5, 0x00, 0x33, 0xd5, 0x33, 0x33, 0xd5, 0x66, 0x33, 0xd5,
					0x99, 0x33, 0xd5, 0xcc, 0x33, 0xd5, 0xff, 0x33, 0xff, 0x00, 0x33, 0xff, 0x33, 0x33, 0xff, 0x66, 0x33, 0xff, 0x99, 0x33, 0xff, 0xcc, 0x33, 0xff, 0xff, 0x66, 0x00, 0x00, 0x66, 0x00, 0x33, 0x66, 0x00, 0x66, 0x66, 0x00, 0x99, 0x66, 0x00, 0xcc,
					0x66, 0x00, 0xff, 0x66, 0x2b, 0x00, 0x66, 0x2b, 0x33, 0x66, 0x2b, 0x66, 0x66, 0x2b, 0x99, 0x66, 0x2b, 0xcc, 0x66, 0x2b, 0xff, 0x66, 0x55, 0x00, 0x66, 0x55, 0x33, 0x66, 0x55, 0x66, 0x66, 0x55, 0x99, 0x66, 0x55, 0xcc, 0x66, 0x55, 0xff, 0x66,
					0x80, 0x00, 0x66, 0x80, 0x33, 0x66, 0x80, 0x66, 0x66, 0x80, 0x99, 0x66, 0x80, 0xcc, 0x66, 0x80, 0xff, 0x66, 0xaa, 0x00, 0x66, 0xaa, 0x33, 0x66, 0xaa, 0x66, 0x66, 0xaa, 0x99, 0x66, 0xaa, 0xcc, 0x66, 0xaa, 0xff, 0x66, 0xd5, 0x00, 0x66, 0xd5,
					0x33, 0x66, 0xd5, 0x66, 0x66, 0xd5, 0x99, 0x66, 0xd5, 0xcc, 0x66, 0xd5, 0xff, 0x66, 0xff, 0x00, 0x66, 0xff, 0x33, 0x66, 0xff, 0x66, 0x66, 0xff, 0x99, 0x66, 0xff, 0xcc, 0x66, 0xff, 0xff, 0x99, 0x00, 0x00, 0x99, 0x00, 0x33, 0x99, 0x00, 0x66,
					0x99, 0x00, 0x99, 0x99, 0x00, 0xcc, 0x99, 0x00, 0xff, 0x99, 0x2b, 0x00, 0x99, 0x2b, 0x33, 0x99, 0x2b, 0x66, 0x99, 0x2b, 0x99, 0x99, 0x2b, 0xcc, 0x99, 0x2b, 0xff, 0x99, 0x55, 0x00, 0x99, 0x55, 0x33, 0x99, 0x55, 0x66, 0x99, 0x55, 0x99, 0x99,
					0x55, 0xcc, 0x99, 0x55, 0xff, 0x99, 0x80, 0x00, 0x99, 0x80, 0x33, 0x99, 0x80, 0x66, 0x99, 0x80, 0x99, 0x99, 0x80, 0xcc, 0x99, 0x80, 0xff, 0x99, 0xaa, 0x00, 0x99, 0xaa, 0x33, 0x99, 0xaa, 0x66, 0x99, 0xaa, 0x99, 0x99, 0xaa, 0xcc, 0x99, 0xaa,
					0xff, 0x99, 0xd5, 0x00, 0x99, 0xd5, 0x33, 0x99, 0xd5, 0x66, 0x99, 0xd5, 0x99, 0x99, 0xd5, 0xcc, 0x99, 0xd5, 0xff, 0x99, 0xff, 0x00, 0x99, 0xff, 0x33, 0x99, 0xff, 0x66, 0x99, 0xff, 0x99, 0x99, 0xff, 0xcc, 0x99, 0xff, 0xff, 0xcc, 0x00, 0x00,
					0xcc, 0x00, 0x33, 0xcc, 0x00, 0x66, 0xcc, 0x00, 0x99, 0xcc, 0x00, 0xcc, 0xcc, 0x00, 0xff, 0xcc, 0x2b, 0x00, 0xcc, 0x2b, 0x33, 0xcc, 0x2b, 0x66, 0xcc, 0x2b, 0x99, 0xcc, 0x2b, 0xcc, 0xcc, 0x2b, 0xff, 0xcc, 0x55, 0x00, 0xcc, 0x55, 0x33, 0xcc,
					0x55, 0x66, 0xcc, 0x55, 0x99, 0xcc, 0x55, 0xcc, 0xcc, 0x55, 0xff, 0xcc, 0x80, 0x00, 0xcc, 0x80, 0x33, 0xcc, 0x80, 0x66, 0xcc, 0x80, 0x99, 0xcc, 0x80, 0xcc, 0xcc, 0x80, 0xff, 0xcc, 0xaa, 0x00, 0xcc, 0xaa, 0x33, 0xcc, 0xaa, 0x66, 0xcc, 0xaa,
					0x99, 0xcc, 0xaa, 0xcc, 0xcc, 0xaa, 0xff, 0xcc, 0xd5, 0x00, 0xcc, 0xd5, 0x33, 0xcc, 0xd5, 0x66, 0xcc, 0xd5, 0x99, 0xcc, 0xd5, 0xcc, 0xcc, 0xd5, 0xff, 0xcc, 0xff, 0x00, 0xcc, 0xff, 0x33, 0xcc, 0xff, 0x66, 0xcc, 0xff, 0x99, 0xcc, 0xff, 0xcc,
					0xcc, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x33, 0xff, 0x00, 0x66, 0xff, 0x00, 0x99, 0xff, 0x00, 0xcc, 0xff, 0x00, 0xff, 0xff, 0x2b, 0x00, 0xff, 0x2b, 0x33, 0xff, 0x2b, 0x66, 0xff, 0x2b, 0x99, 0xff, 0x2b, 0xcc, 0xff, 0x2b, 0xff, 0xff,
					0x55, 0x00, 0xff, 0x55, 0x33, 0xff, 0x55, 0x66, 0xff, 0x55, 0x99, 0xff, 0x55, 0xcc, 0xff, 0x55, 0xff, 0xff, 0x80, 0x00, 0xff, 0x80, 0x33, 0xff, 0x80, 0x66, 0xff, 0x80, 0x99, 0xff, 0x80, 0xcc, 0xff, 0x80, 0xff, 0xff, 0xaa, 0x00, 0xff, 0xaa,
					0x33, 0xff, 0xaa, 0x66, 0xff, 0xaa, 0x99, 0xff, 0xaa, 0xcc, 0xff, 0xaa, 0xff, 0xff, 0xd5, 0x00, 0xff, 0xd5, 0x33, 0xff, 0xd5, 0x66, 0xff, 0xd5, 0x99, 0xff, 0xd5, 0xcc, 0xff, 0xd5, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x33, 0xff, 0xff, 0x66,
					0xff, 0xff, 0x99, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0xf9, 0x04, 0x01, 0x00, 0x00, 0xfc, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x00, 0x08,
					0x1e, 0x00, 0x01, 0x30, 0x7a, 0x85, 0x6d, 0x5a, 0x1a, 0x00, 0x86, 0x5e, 0xbd, 0x02, 0x62, 0x04, 0x40, 0xa1, 0x57, 0x1d, 0x38, 0xa8, 0x01, 0x00, 0x8b, 0x43, 0x44, 0x20, 0x00, 0x02, 0x02, 0x00, 0x3b};
					initialValue.getValue()->setByteString(initData, 833);
			}
			break;
		case EnumDataTypeId_ImageJPG:
			{
				unsigned char initData[] = {
					0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x01, 0x00, 0x60, 0x00, 0x60, 0x00, 0x00, 0xff, 0xe1, 0x00, 0x5a, 0x45, 0x78, 0x69, 0x66, 0x00, 0x00, 0x4d, 0x4d, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x08, 0x00, 0x05,
					0x03, 0x01, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x4a, 0x03, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x51, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x51, 0x11, 0x00, 0x04,
					0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x0e, 0xc3, 0x51, 0x12, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x0e, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0xa0, 0x00, 0x00, 0xb1, 0x8f, 0xff, 0xdb, 0x00, 0x43, 0x00, 0x02, 0x01, 0x01,
					0x02, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x05, 0x03, 0x03, 0x03, 0x03, 0x03, 0x06, 0x04, 0x04, 0x03, 0x05, 0x07, 0x06, 0x07, 0x07, 0x07, 0x06, 0x07, 0x07, 0x08, 0x09, 0x0b, 0x09, 0x08, 0x08, 0x0a, 0x08, 0x07,
					0x07, 0x0a, 0x0d, 0x0a, 0x0a, 0x0b, 0x0c, 0x0c, 0x0c, 0x0c, 0x07, 0x09, 0x0e, 0x0f, 0x0d, 0x0c, 0x0e, 0x0b, 0x0c, 0x0c, 0x0c, 0xff, 0xdb, 0x00, 0x43, 0x01, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x06, 0x03, 0x03, 0x06, 0x0c, 0x08, 0x07, 0x08,
					0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
					0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xff, 0xc0, 0x00, 0x11, 0x08, 0x00, 0x05, 0x00, 0x05, 0x03, 0x01, 0x22, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xff, 0xc4, 0x00, 0x1f, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0xff, 0xc4, 0x00, 0xb5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00,
					0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82,
					0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
					0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3,
					0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5,
					0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xff, 0xc4, 0x00, 0x1f, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0xff, 0xc4,
					0x00, 0xb5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08,
					0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45,
					0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93,
					0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
					0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xff, 0xda, 0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f, 0x00, 0xf8, 0x93, 0xf6, 0xd0, 0xf8,
					0xa3, 0x6d, 0xfb, 0x0d, 0xea, 0x3a, 0x12, 0x69, 0x9a, 0x4f, 0xf6, 0xb6, 0x89, 0xe2, 0x7f, 0xb4, 0x35, 0xa6, 0x9d, 0xf6, 0x9f, 0xb3, 0x2e, 0x8c, 0x60, 0xf2, 0xb7, 0xc7, 0x13, 0x6c, 0x7c, 0xc0, 0xde, 0x72, 0x94, 0x8f, 0x03, 0xca, 0xda, 0xca,
					0xa4, 0xc6, 0x63, 0x8e, 0x22, 0x8a, 0x2b, 0xda, 0xf1, 0x2b, 0x84, 0x32, 0x7a, 0x5c, 0x4b, 0x8a, 0x8d, 0x2a, 0x0a, 0x29, 0xb8, 0xbb, 0x26, 0xd2, 0xbc, 0xa1, 0x19, 0x3b, 0x24, 0xec, 0xae, 0xdb, 0x7a, 0x68, 0x77, 0x78, 0x17, 0xf4, 0x83, 0xf1,
					0x06, 0x1c, 0x0b, 0x97, 0xd2, 0x79, 0x94, 0xa4, 0xa1, 0x19, 0x41, 0x39, 0xc2, 0x94, 0xe5, 0xcb, 0x0a, 0x93, 0x8c, 0x53, 0x94, 0xe1, 0x29, 0x3b, 0x45, 0x24, 0x9b, 0x6d, 0xd9, 0x23, 0xff, 0xd9};
					initialValue.getValue()->setByteString(initData, 832);
			}
			break;
		case EnumDataTypeId_ImagePNG:
			{
				unsigned char initData[] = {
					0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x05,
					0x00, 0x00, 0x00, 0x05, 0x08, 0x02, 0x00, 0x00, 0x00, 0x02, 0x0d, 0xb1, 0xb2, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47,
					0x42, 0x00, 0xae, 0xce, 0x1c, 0xe9, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4d, 0x41, 0x00, 0x00, 0xb1, 0x8f, 0x0b, 0xfc,
					0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0e, 0xc3, 0x00, 0x00, 0x0e, 0xc3, 0x01, 0xc7,
					0x6f, 0xa8, 0x64, 0x00, 0x00, 0x00, 0x22, 0x49, 0x44, 0x41, 0x54, 0x18, 0x57, 0x63, 0x60, 0x60, 0x60, 0x78, 0x2b, 0xa3,
					0x02, 0x47, 0x0c, 0x0c, 0x8b, 0x5e, 0x20, 0x0b, 0x81, 0xf9, 0x48, 0x42, 0x30, 0x3e, 0x4c, 0x08, 0x89, 0x0f, 0x16, 0x02,
					0x00, 0x0e, 0x0c, 0x1b, 0x27, 0xde, 0x68, 0xea, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60,
					0x82};
					initialValue.getValue()->setByteString(initData, 141);
			}
			break;
		default:
			_REPORT_ASSERTION(_T("Unexpected DataType"));
			// no initialization
			break;
		}
	}	// namespace 0 and numeric

	initialValue.setServerTimestamp(DateTime::getUtcNow());
	initialValue.setSourceTimestamp(DateTime::getUtcNow());

	variable->setDataValue(initialValue);
}


EnumStatusCode MatrixModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	// some matrix types
	TestVariablePtr matrixVar;
	Value matrixVal;
	std::vector<OTUInt32> arrayDimensions;
	DataValue dv;

	arrayDimensions.clear();
	arrayDimensions.push_back(2);
	arrayDimensions.push_back(3);

	// Boolean matrix value
	matrixVar = createMatrixVariable(_T("BooleanMatrix"), namespaceIndex, Statics::DataTypeId_Boolean, arrayDimensions, true);
	bool booleanTestVal[2][3] = {
		{ false, true, false },
		{ true, false, true }
	};
	result = matrixVal.setBooleanMatrix(arrayDimensions, MATRIX_LEN(booleanTestVal), &booleanTestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setBooleanMatrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// Byte matrix value
	matrixVar = createMatrixVariable(_T("ByteMatrix"), namespaceIndex, Statics::DataTypeId_Byte, arrayDimensions, true);
	OTUInt8 byteTestVal[2][3] = {
		{ 1, 2, 3 },
		{ 4, 5, 6 }
	};
	result = matrixVal.setUInt8Matrix(arrayDimensions, MATRIX_LEN(byteTestVal), &byteTestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setUInt8Matrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// SByte matrix value
	matrixVar = createMatrixVariable(_T("SByteMatrix"), namespaceIndex, Statics::DataTypeId_SByte, arrayDimensions, true);
	OTInt8 sByteTestVal[2][3] = {
		{  1,  2,  3 },
		{ -1, -2, -3 }
	};
	result = matrixVal.setInt8Matrix(arrayDimensions, MATRIX_LEN(sByteTestVal), &sByteTestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setInt8Matrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// Int16 matrix value
	matrixVar = createMatrixVariable(_T("Int16Matrix"), namespaceIndex, Statics::DataTypeId_Int16, arrayDimensions, true);
	OTInt16 int16TestVal[2][3] = {
		{  1,  2,  3 },
		{ -1, -2, -3 }
	};
	result = matrixVal.setInt16Matrix(arrayDimensions, MATRIX_LEN(int16TestVal), &int16TestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setInt16Matrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// UInt16 matrix value
	matrixVar = createMatrixVariable(_T("UInt16Matrix"), namespaceIndex, Statics::DataTypeId_UInt16, arrayDimensions, true);
	OTUInt16 uInt16TestVal[2][3] = {
		{ 1, 2, 3 },
		{ 4, 5, 6 }
	};
	result = matrixVal.setUInt16Matrix(arrayDimensions, MATRIX_LEN(uInt16TestVal), &uInt16TestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setUInt16Matrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// Int32 matrix value
	matrixVar = createMatrixVariable(_T("Int32Matrix"), namespaceIndex, Statics::DataTypeId_Int32, arrayDimensions, true);
	OTInt32 int32TestVal[2][3] = {
		{  1,  2,  3 },
		{ -1, -2, -3 }
	};
	result = matrixVal.setInt32Matrix(arrayDimensions, MATRIX_LEN(int32TestVal), &int32TestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setInt32Matrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// UInt32 matrix value
	matrixVar = createMatrixVariable(_T("UInt32Matrix"), namespaceIndex, Statics::DataTypeId_UInt32, arrayDimensions, true);
	OTUInt32 uInt32TestVal[2][3] = {
		{ 1, 2, 3 },
		{ 4, 5, 6 }
	};
	result = matrixVal.setUInt32Matrix(arrayDimensions, MATRIX_LEN(uInt32TestVal), &uInt32TestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setUInt32Matrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// Int64 matrix value
	matrixVar = createMatrixVariable(_T("Int64Matrix"), namespaceIndex, Statics::DataTypeId_Int64, arrayDimensions, true);
	OTInt64 int64TestVal[2][3] = {
		{  1,  2,  3 },
		{ -1, -2, -3 }
	};
	result = matrixVal.setInt64Matrix(arrayDimensions, MATRIX_LEN(int64TestVal), &int64TestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setInt64Matrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// UInt64 matrix value
	matrixVar = createMatrixVariable(_T("UInt64Matrix"), namespaceIndex, Statics::DataTypeId_UInt64, arrayDimensions, true);
	OTUInt64 uInt64TestVal[2][3] = {
		{ 1, 2, 3 },
		{ 4, 5, 6 }
	};
	result = matrixVal.setUInt64Matrix(arrayDimensions, MATRIX_LEN(uInt64TestVal), &uInt64TestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setUInt64Matrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// Float matrix value
	matrixVar = createMatrixVariable(_T("FloatMatrix"), namespaceIndex, Statics::DataTypeId_Float, arrayDimensions, true);
	OTFloat floatTestVal[2][3] = {
		{  1.0F,  2.71828F,  3.14159F },
		{ -1.0F, -2.71828F, -3.14159F }
	};
	result = matrixVal.setFloatMatrix(arrayDimensions, MATRIX_LEN(floatTestVal), &floatTestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setFloatMatrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// Double matrix value
	matrixVar = createMatrixVariable(_T("DoubleMatrix"), namespaceIndex, Statics::DataTypeId_Double, arrayDimensions, true);
	OTDouble doubleTestVal[2][3] = {
		{  1.0,  2.71828,  3.14159 },
		{ -1.0, -2.71828, -3.14159 }
	};
	result = matrixVal.setDoubleMatrix(arrayDimensions, MATRIX_LEN(doubleTestVal), &doubleTestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setDoubleMatrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// DateTime matrix value
	matrixVar = createMatrixVariable(_T("DateTimeMatrix"), namespaceIndex, Statics::DataTypeId_DateTime, arrayDimensions, true);
	DateTime dateTimeTestVal[2][3] = {
		{ DateTime(), DateTime(), DateTime() },
		{ DateTime(), DateTime(), DateTime() }
	};

	dateTimeTestVal[0][0].parse(tstring(_T("2017-01-01T00:00:00.000Z")));
	dateTimeTestVal[0][1].parse(tstring(_T("2017-02-01T00:00:00.000Z")));
	dateTimeTestVal[0][2].parse(tstring(_T("2017-03-01T00:00:00.000Z")));
	dateTimeTestVal[0][0].parse(tstring(_T("2017-04-01T00:00:00.000Z")));
	dateTimeTestVal[1][1].parse(tstring(_T("2017-05-01T00:00:00.000Z")));
	dateTimeTestVal[1][2].parse(tstring(_T("2017-06-01T00:00:00.000Z")));

	result = matrixVal.setDateTimeMatrix(arrayDimensions, MATRIX_LEN(dateTimeTestVal), &dateTimeTestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setDateTimeMatrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// String matrix value
	matrixVar = createMatrixVariable(_T("StringMatrix"), namespaceIndex, Statics::DataTypeId_String, arrayDimensions, true);
	tstring stringTestVal[2][3] = {
		{ tstring(_T("OPC")), tstring(_T("UA")), tstring(_T("is")) },
		{ tstring(_T("a")), tstring(_T("cool")), tstring(_T("thing")) }
	};


	result = matrixVal.setStringMatrix(arrayDimensions, MATRIX_LEN(stringTestVal), &stringTestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setStringMatrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// GUID matrix value
	matrixVar = createMatrixVariable(_T("GuidMatrix"), namespaceIndex, Statics::DataTypeId_Guid, arrayDimensions, true);
	Guid guidTestVal [2][3];
	OTUInt8 guid_testData4[] = {0,1,2,3,4,5,6,7};
	for(OTUInt32 i = 0; i<2; i++)
	{
		for(OTUInt32 j=0; j<3; j++)
		{
			guidTestVal[i][j].setData(i+1, (OTUInt16)j+1, (OTUInt16)((i+1)*(j+1)), guid_testData4);
		}
	}
	result = matrixVal.setGuidMatrix(arrayDimensions, MATRIX_LEN(guidTestVal), &guidTestVal[0][0]);
	if(!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setGuidMatrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if(!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// StatusCode matrix value
	matrixVar = createMatrixVariable(_T("StatusCodeMatrix"), namespaceIndex, Statics::DataTypeId_StatusCode, arrayDimensions, true);
	EnumStatusCode statusCodeTestVal[2][3] = {
		{ EnumStatusCode_Good,           EnumStatusCode_Uncertain,  EnumStatusCode_Bad },
		{ EnumStatusCode_BadOutOfMemory, EnumStatusCode_BadTimeout, EnumStatusCode_BadNothingToDo }

	};

	result = matrixVal.setStatusCodeMatrix(arrayDimensions, MATRIX_LEN(statusCodeTestVal), &statusCodeTestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setStatusCodeMatrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// ByteString matrix value
	matrixVar = createMatrixVariable(_T("ByteStringMatrix"), namespaceIndex, Statics::DataTypeId_ByteString, arrayDimensions, true);
	ByteString byteStringTestVal[2][3] = {
		{ ByteString(_T("OPC")), ByteString(_T("UA")), ByteString(_T("is")) },
		{ ByteString(_T("a")), ByteString(_T("cool")), ByteString(_T("thing")) }

	};

	result = matrixVal.setByteStringMatrix(arrayDimensions, MATRIX_LEN(byteStringTestVal), &byteStringTestVal[0][0]);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setByteStringMatrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if (!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// Range matrix value
	matrixVar = createMatrixVariable(_T("RangeMatrix"), namespaceIndex, Statics::DataTypeId_Range, arrayDimensions, true);
	Range rangeTestVal [2][3] =
	{
		{Range(0.0, 100.0), Range(0.0, 110.0), Range(0.0, 120.0)},
		{Range(10.0, 100.0), Range(10.0, 110.0), Range(10.0, 120.0)}
	};
	result = matrixVal.setRangeMatrix(arrayDimensions, MATRIX_LEN(rangeTestVal), &rangeTestVal[0][0]);
	if(!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setRangeMatrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if(!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	// Decimal matrix value
	matrixVar = createMatrixVariable(_T("DecimalMatrix"), namespaceIndex, Statics::DataTypeId_Decimal, arrayDimensions, true);
	Decimal decimalTestVal[2][3];

	for(OTUInt32 i = 0; i < 2; i++)
	{
		for(OTUInt32 j = 0; j < 3; j++)
		{
			decimalTestVal[i][j].set((OTUInt16)((i + 1) * 100 + j), 0);
		}
	}

	result = matrixVal.setDecimalMatrix(arrayDimensions, MATRIX_LEN(decimalTestVal), &decimalTestVal[0][0]);
	if(!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVal.setDecimalMatrix failed: "), getEnumStatusCodeString(result));
	}
	dv.setValue(matrixVal);
	result = matrixVar->setDataValue(dv);
	if(!StatusCode::isGood(result))
	{
		Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setDataValue failed: "), getEnumStatusCodeString(result));
	}

	return result;
}

