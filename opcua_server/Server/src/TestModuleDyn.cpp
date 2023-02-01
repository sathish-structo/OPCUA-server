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

using namespace SoftingOPCToolbox5;

EnumStatusCode DynamicsModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	m_namespaceIndex = namespaceIndex;
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	createAllDynamicVariables();

	//array dynamic types
	createDynamicArrayTestVariablesPart1();
	createDynamicArrayTestVariablesPart2();
	createDynamicArrayTestVariablesPart3();

	getApplicationModule()->addAction(&m_dynamicVariableChangeAction);
	return result;
}

void DynamicsModule::endModule()
{
	getApplicationModule()->removeAction(&m_dynamicVariableChangeAction);
	ModuleFolder::endModule();
}

void DynamicsModule::createAllDynamicVariables()
{
	unsigned char uint8Values[] = {225, 234, 57, 0, 1, 45, 77, 123};
	DynamicTestVariablePtr testVariableUInt8 = createDynamicVariable(_T("Dynamic.UInt8"), uint8Values, sizeof(uint8Values) / sizeof(uint8Values[0]), Statics::DataTypeId_Byte, false);
	signed char int8Values[] = {(signed char)-127, (signed char)-123, (signed char)-57, 0, 1, 45, 77, 123};
	DynamicTestVariablePtr testVariableInt8 = createDynamicVariable(_T("Dynamic.Int8"), int8Values, sizeof(int8Values) / sizeof(int8Values[0]), Statics::DataTypeId_SByte, false);
	unsigned short uint16Values[] = {127, 133, 57, 0, 1, 45, 77, 123};
	DynamicTestVariablePtr testVariableUInt16 = createDynamicVariable(_T("Dynamic.UInt16"), uint16Values, sizeof(uint16Values) / sizeof(uint16Values[0]), Statics::DataTypeId_UInt16, false);
	short int16Values[] = {-127, - 133, - 57, 0, 1, 45, 77, 123};
	DynamicTestVariablePtr testVariableInt16 = createDynamicVariable(_T("Dynamic.Int16"), int16Values, sizeof(int16Values) / sizeof(int16Values[0]), Statics::DataTypeId_Int16, false);
	unsigned int uint32Values[] = {127, 133, 57, 0, 1, 45, 77, 123};
	DynamicTestVariablePtr testVariableUInt32 = createDynamicVariable(_T("Dynamic.UInt32"), uint32Values, sizeof(uint32Values) / sizeof(uint32Values[0]), Statics::DataTypeId_UInt32, false);
	int int32Values[] = {-127, - 133, - 57, 0, 1, 45, 77, 123};
	DynamicTestVariablePtr testVariableInt32 = createDynamicVariable(_T("Dynamic.Int32"), int32Values, sizeof(int32Values) / sizeof(int32Values[0]), Statics::DataTypeId_Int32, false);
	OTUInt64 uint64Values[] = {127, 133, 57, 0, 1, 45, 77, 123};
	DynamicTestVariablePtr testVariableUInt64 = createDynamicVariable(_T("Dynamic.UInt64"), uint64Values, sizeof(uint64Values) / sizeof(uint64Values[0]), Statics::DataTypeId_UInt64, false);
	OTInt64 int64Values[] = {-127, - 133, - 57, 0, 1, 45, 77, 123};
	DynamicTestVariablePtr testVariableInt64 = createDynamicVariable(_T("Dynamic.Int64"), int64Values, sizeof(int64Values) / sizeof(int64Values[0]), Statics::DataTypeId_Int64, false);
	double doubleValues[] = {-2303.3453, -133.24, -57, 0, 0.1, 45.234874, 7723435.34, 12234863845378.3};
	DynamicTestVariablePtr testVariableDouble = createDynamicVariable(_T("Dynamic.Double"), doubleValues, sizeof(doubleValues) / sizeof(doubleValues[0]), Statics::DataTypeId_Double, false);
	float floatValues[] = {(float)-2303.3453, (float)-133.24, (float)-57, (float)0.0, (float)0.1, (float)45.234874, (float)73435.34, (float)12235378.3};
	DynamicTestVariablePtr testVariableFloat = createDynamicVariable(_T("Dynamic.Float"), floatValues, sizeof(floatValues) / sizeof(floatValues[0]), Statics::DataTypeId_Float, false);
	bool boolValues[] = {true, false};
	DynamicTestVariablePtr testVariableBoolean = createDynamicVariable(_T("Dynamic.Boolean"), boolValues, sizeof(boolValues) / sizeof(boolValues[0]), Statics::DataTypeId_Boolean, false);
	tstring stringValues[5];
	stringValues[0] = _T("Apples");
	stringValues[1] = _T("Pears");
	stringValues[2] = _T("Plums");
	stringValues[3] = _T("Bananas");
	stringValues[4] = _T("Mangos");
	DynamicTestVariablePtr testVariableString = createDynamicVariable(_T("Dynamic.String"), stringValues, sizeof(stringValues) / sizeof(stringValues[0]), Statics::DataTypeId_String, false);
	ByteString byteStringValues[5];
	byteStringValues[0].init(6, (unsigned char*)"Apples");
	byteStringValues[1].init(4, (unsigned char*)"Pears");
	byteStringValues[2].init(5, (unsigned char*)"Plums");
	byteStringValues[3].init(7, (unsigned char*)"Bananas");
	byteStringValues[4].init(6, (unsigned char*)"Mangos");
	DynamicTestVariablePtr testVariableByteString = createDynamicVariableComplex(_T("Dynamic.ByteString"), byteStringValues, sizeof(byteStringValues) / sizeof(byteStringValues[0]), Statics::DataTypeId_ByteString);
	//TestVariablePtr testVariableXmlElement = createOneVariable(_T("Dynamic.XmlElement"), Statics::DataTypeId_XmlElement);
	//m_testVariables.push_back(testVariableXmlElement);
	NodeId nodeIdValues[5];

	OTUInt8 data4[8];
	memset(data4, 4, sizeof(data4));
	Guid guid(111, 222, 333, data4);

	tstring byteStringStr;
	byteStringStr = _T("byte string");
	ByteString byteString(byteStringStr);
	nodeIdValues[0].setString(m_namespaceIndex, _T("DummyNodeId"));
	nodeIdValues[1].setNumeric(m_namespaceIndex, 12345);
	nodeIdValues[2].setString(m_namespaceIndex, _T("Apple Pie"));
	nodeIdValues[3].setGuid(m_namespaceIndex, &guid);
	nodeIdValues[4].setOpaque(m_namespaceIndex, byteString);
	DynamicTestVariablePtr testVariableNodeId = createDynamicVariableComplex(_T("Dynamic.NodeId"), nodeIdValues,  sizeof(nodeIdValues) / sizeof(nodeIdValues[0]), Statics::DataTypeId_NodeId);
	//m_testVariables.push_back(testVariableNodeId);
	//TestVariablePtr testVariableExpandedNodeId = createOneVariable(_T("Dynamic.ExpandedNodeId"), Statics::DataTypeId_ExpandedNodeId);
	//m_testVariables.push_back(testVariableExpandedNodeId);
	//TestVariablePtr testVariableGuid = createOneVariable(_T("Dynamic.Guid"), Statics::DataTypeId_Guid);
	//m_testVariables.push_back(testVariableGuid);
	//TestVariablePtr testVariableExpandedDateTime = createOneVariable(_T("Dynamic.DateTime"), Statics::DataTypeId_DateTime);
	//m_testVariables.push_back(testVariableExpandedDateTime);
	//TestVariablePtr testVariableStatusCode = createOneVariable(_T("Dynamic.StatusCode"), Statics::DataTypeId_StatusCode);
	//m_testVariables.push_back(testVariableStatusCode);
	////	TestVariablePtr testVariableDiagnosticInfo = createOneVariable(_T("Dynamic.DiagnosticInfo"), Statics::
	////	m_testVariables.push_back(testVariableDiagnosticInfo);
	//TestVariablePtr testVariableLocalizedText = createOneVariable(_T("Dynamic.LocalizedText"), Statics::DataTypeId_LocalizedText);
	//m_testVariables.push_back(testVariableLocalizedText);
	//TestVariablePtr testVariableQualifiedName = createOneVariable(_T("Dynamic.QualifiedName"), Statics::DataTypeId_QualifiedName);
	//m_testVariables.push_back(testVariableQualifiedName);
	//TestVariablePtr testVariableInteger = createOneVariable(_T("Dynamic.Integer"), Statics::DataTypeId_Integer);
	//m_testVariables.push_back(testVariableInteger);
	//TestVariablePtr testVariableUnsignedInteger = createOneVariable(_T("Dynamic.UnsignedInteger"), Statics::DataTypeId_UInteger);
	//m_testVariables.push_back(testVariableUnsignedInteger);
	//TestVariablePtr testVariableEnumeration = createOneVariable(_T("Dynamic.Enumeration"), Statics::DataTypeId_Enumeration);
	//m_testVariables.push_back(testVariableEnumeration);
	//TestVariablePtr testVariableEUInfo = createOneVariable(_T("Dynamic.EUInformation"), Statics::DataTypeId_EUInformation);
	//m_testVariables.push_back(testVariableEUInfo);
	//TestVariablePtr testVariableRange = createOneVariable(_T("Dynamic.Range"), Statics::DataTypeId_Range);
	//m_testVariables.push_back(testVariableRange);
}


#define COMPILE_11
#define COMPILE_12
#define COMPILE_13
#define COMPILE_14
/**
* @brief Creates all the test array variables into TestModule folder
* @return void
*/
void DynamicsModule::createDynamicArrayTestVariablesPart1()
{
	Value temp;

#ifdef COMPILE_11
	std::vector<unsigned char> uint8ArrayValues[5];
	uint8ArrayValues[0].push_back((unsigned char)225);
	uint8ArrayValues[0].push_back((unsigned char)234);
	uint8ArrayValues[0].push_back((unsigned char)57);
	uint8ArrayValues[0].push_back((unsigned char)0);
	uint8ArrayValues[0].push_back((unsigned char)1);
	uint8ArrayValues[0].push_back((unsigned char)45);
	uint8ArrayValues[1].push_back((unsigned char)57);
	uint8ArrayValues[1].push_back((unsigned char)0);
	uint8ArrayValues[1].push_back((unsigned char)1);
	uint8ArrayValues[1].push_back((unsigned char)45);
	uint8ArrayValues[1].push_back((unsigned char)225);
	uint8ArrayValues[1].push_back((unsigned char)234);
	uint8ArrayValues[2].push_back((unsigned char)0);
	uint8ArrayValues[2].push_back((unsigned char)1);
	uint8ArrayValues[2].push_back((unsigned char)45);
	uint8ArrayValues[2].push_back((unsigned char)225);
	uint8ArrayValues[2].push_back((unsigned char)234);
	uint8ArrayValues[2].push_back((unsigned char)57);
	uint8ArrayValues[3].push_back((unsigned char)1);
	uint8ArrayValues[3].push_back((unsigned char)45);
	uint8ArrayValues[3].push_back((unsigned char)225);
	uint8ArrayValues[3].push_back((unsigned char)234);
	uint8ArrayValues[3].push_back((unsigned char)57);
	uint8ArrayValues[3].push_back((unsigned char)0);
	uint8ArrayValues[4].push_back((unsigned char)45);
	uint8ArrayValues[4].push_back((unsigned char)225);
	uint8ArrayValues[4].push_back((unsigned char)234);
	uint8ArrayValues[4].push_back((unsigned char)57);
	uint8ArrayValues[4].push_back((unsigned char)0);
	uint8ArrayValues[4].push_back((unsigned char)1);
	DynamicTestVariablePtr testVariableUInt8 = createDynamicVariable(_T("Dynamic.UInt8Array"), uint8ArrayValues, sizeof(uint8ArrayValues) / sizeof(uint8ArrayValues[0]), Statics::DataTypeId_Byte, true);
#endif

#ifdef COMPILE_12
	std::vector<signed char> int8ArrayValues[5];
	int8ArrayValues[0].push_back((signed char)-127);
	int8ArrayValues[0].push_back((signed char)-123);
	int8ArrayValues[0].push_back((signed char)-57);
	int8ArrayValues[0].push_back((signed char)0);
	int8ArrayValues[0].push_back((signed char)1);
	int8ArrayValues[0].push_back((signed char)45);
	int8ArrayValues[1].push_back((signed char)77);
	int8ArrayValues[1].push_back((signed char)123);
	int8ArrayValues[1].push_back((signed char)-127);
	int8ArrayValues[1].push_back((signed char)-123);
	int8ArrayValues[1].push_back((signed char)-57);
	int8ArrayValues[1].push_back((signed char)0);
	int8ArrayValues[2].push_back((signed char)1);
	int8ArrayValues[2].push_back((signed char)45);
	int8ArrayValues[2].push_back((signed char)77);
	int8ArrayValues[2].push_back((signed char)123);
	int8ArrayValues[2].push_back((signed char)-127);
	int8ArrayValues[2].push_back((signed char)-123);
	int8ArrayValues[3].push_back((signed char)-57);
	int8ArrayValues[3].push_back((signed char)0);
	int8ArrayValues[3].push_back((signed char)1);
	int8ArrayValues[3].push_back((signed char)45);
	int8ArrayValues[3].push_back((signed char)77);
	int8ArrayValues[3].push_back((signed char)123);
	int8ArrayValues[4].push_back((signed char)-57);
	int8ArrayValues[4].push_back((signed char)0);
	int8ArrayValues[4].push_back((signed char)1);
	int8ArrayValues[4].push_back((signed char)45);
	int8ArrayValues[4].push_back((signed char)77);
	int8ArrayValues[4].push_back((signed char)123);
	DynamicTestVariablePtr testVariableInt8 = createDynamicVariable(_T("Dynamic.Int8Array"), int8ArrayValues, sizeof(int8ArrayValues) / sizeof(int8ArrayValues[0]), Statics::DataTypeId_SByte, true);
#endif

#ifdef COMPILE_13
	std::vector<unsigned short> uint16ArrayValues[5];
	uint16ArrayValues[0].push_back(127);
	uint16ArrayValues[0].push_back(133);
	uint16ArrayValues[0].push_back(57);
	uint16ArrayValues[0].push_back(0);
	uint16ArrayValues[0].push_back(1);
	uint16ArrayValues[0].push_back(45);
	uint16ArrayValues[1].push_back(77);
	uint16ArrayValues[1].push_back(123);
	uint16ArrayValues[1].push_back(127);
	uint16ArrayValues[1].push_back(133);
	uint16ArrayValues[1].push_back(57);
	uint16ArrayValues[1].push_back(0);
	uint16ArrayValues[2].push_back(1);
	uint16ArrayValues[2].push_back(45);
	uint16ArrayValues[2].push_back(77);
	uint16ArrayValues[2].push_back(123);
	uint16ArrayValues[2].push_back(127);
	uint16ArrayValues[2].push_back(133);
	uint16ArrayValues[3].push_back(57);
	uint16ArrayValues[3].push_back(0);
	uint16ArrayValues[3].push_back(1);
	uint16ArrayValues[3].push_back(45);
	uint16ArrayValues[3].push_back(77);
	uint16ArrayValues[3].push_back(123);
	uint16ArrayValues[4].push_back(127);
	uint16ArrayValues[4].push_back(133);
	uint16ArrayValues[4].push_back(57);
	uint16ArrayValues[4].push_back(0);
	uint16ArrayValues[4].push_back(1);
	uint16ArrayValues[4].push_back(45);
	DynamicTestVariablePtr testVariableUInt16 = createDynamicVariable(_T("Dynamic.UInt16Array"), uint16ArrayValues, sizeof(uint16ArrayValues) / sizeof(uint16ArrayValues[0]), Statics::DataTypeId_UInt16, true);
#endif

#ifdef COMPILE_14
	std::vector<short> int16ArrayValues[5];
	int16ArrayValues[0].push_back(-127);
	int16ArrayValues[0].push_back(-133);
	int16ArrayValues[0].push_back(-57);
	int16ArrayValues[0].push_back(0);
	int16ArrayValues[0].push_back(1);
	int16ArrayValues[0].push_back(45);
	int16ArrayValues[1].push_back(77);
	int16ArrayValues[1].push_back(133);
	int16ArrayValues[1].push_back(-127);
	int16ArrayValues[1].push_back(-133);
	int16ArrayValues[1].push_back(-57);
	int16ArrayValues[1].push_back(0);
	int16ArrayValues[2].push_back(1);
	int16ArrayValues[2].push_back(45);
	int16ArrayValues[2].push_back(77);
	int16ArrayValues[2].push_back(133);
	int16ArrayValues[2].push_back(-127);
	int16ArrayValues[2].push_back(-133);
	int16ArrayValues[3].push_back(-57);
	int16ArrayValues[3].push_back(0);
	int16ArrayValues[3].push_back(1);
	int16ArrayValues[3].push_back(45);
	int16ArrayValues[3].push_back(77);
	int16ArrayValues[3].push_back(133);
	int16ArrayValues[4].push_back(-127);
	int16ArrayValues[4].push_back(-133);
	int16ArrayValues[4].push_back(-57);
	int16ArrayValues[4].push_back(0);
	int16ArrayValues[4].push_back(1);
	int16ArrayValues[4].push_back(45);
	DynamicTestVariablePtr testVariableInt16 = createDynamicVariable(_T("Dynamic.Int16Array"), int16ArrayValues, sizeof(int16ArrayValues) / sizeof(int16ArrayValues[0]), Statics::DataTypeId_Int16, true);
#endif
}

#define COMPILE_21
#define COMPILE_22
#define COMPILE_23
#define COMPILE_24
void DynamicsModule::createDynamicArrayTestVariablesPart2()
{
	Value temp;

#ifdef COMPILE_21
	std::vector<unsigned int> uint32ArrayValues[5];
	uint32ArrayValues[0].push_back(127);
	uint32ArrayValues[0].push_back(133);
	uint32ArrayValues[0].push_back(57);
	uint32ArrayValues[0].push_back(0);
	uint32ArrayValues[0].push_back(1);
	uint32ArrayValues[0].push_back(45);
	uint32ArrayValues[1].push_back(77);
	uint32ArrayValues[1].push_back(123);
	uint32ArrayValues[1].push_back(127);
	uint32ArrayValues[1].push_back(133);
	uint32ArrayValues[1].push_back(57);
	uint32ArrayValues[1].push_back(0);
	uint32ArrayValues[2].push_back(1);
	uint32ArrayValues[2].push_back(45);
	uint32ArrayValues[2].push_back(77);
	uint32ArrayValues[2].push_back(123);
	uint32ArrayValues[2].push_back(127);
	uint32ArrayValues[2].push_back(133);
	uint32ArrayValues[3].push_back(57);
	uint32ArrayValues[3].push_back(0);
	uint32ArrayValues[3].push_back(1);
	uint32ArrayValues[3].push_back(45);
	uint32ArrayValues[3].push_back(77);
	uint32ArrayValues[3].push_back(123);
	uint32ArrayValues[4].push_back(127);
	uint32ArrayValues[4].push_back(133);
	uint32ArrayValues[4].push_back(57);
	uint32ArrayValues[4].push_back(0);
	uint32ArrayValues[4].push_back(1);
	uint32ArrayValues[4].push_back(45);
	DynamicTestVariablePtr testVariableUInt32 = createDynamicVariable(_T("Dynamic.UInt32Array"), uint32ArrayValues, sizeof(uint32ArrayValues) / sizeof(uint32ArrayValues[0]), Statics::DataTypeId_UInt32, true);
#endif

#ifdef COMPILE_22
	std::vector<int> int32ArrayValues[5];
	int32ArrayValues[0].push_back(-127);
	int32ArrayValues[0].push_back(-133);
	int32ArrayValues[0].push_back(-57);
	int32ArrayValues[0].push_back(0);
	int32ArrayValues[0].push_back(1);
	int32ArrayValues[0].push_back(45);
	int32ArrayValues[1].push_back(77);
	int32ArrayValues[1].push_back(133);
	int32ArrayValues[1].push_back(-127);
	int32ArrayValues[1].push_back(-133);
	int32ArrayValues[1].push_back(-57);
	int32ArrayValues[1].push_back(0);
	int32ArrayValues[2].push_back(1);
	int32ArrayValues[2].push_back(45);
	int32ArrayValues[2].push_back(77);
	int32ArrayValues[2].push_back(133);
	int32ArrayValues[2].push_back(-127);
	int32ArrayValues[2].push_back(-133);
	int32ArrayValues[3].push_back(-57);
	int32ArrayValues[3].push_back(0);
	int32ArrayValues[3].push_back(1);
	int32ArrayValues[3].push_back(45);
	int32ArrayValues[3].push_back(77);
	int32ArrayValues[3].push_back(133);
	int32ArrayValues[4].push_back(-127);
	int32ArrayValues[4].push_back(-133);
	int32ArrayValues[4].push_back(-57);
	int32ArrayValues[4].push_back(0);
	int32ArrayValues[4].push_back(1);
	int32ArrayValues[4].push_back(45);
	DynamicTestVariablePtr testVariableInt32 = createDynamicVariable(_T("Dynamic.Int32Array"), int32ArrayValues, sizeof(int32ArrayValues) / sizeof(int32ArrayValues[0]), Statics::DataTypeId_Int32, true);
#endif

#ifdef COMPILE_23
	std::vector<OTUInt64> uint64ArrayValues[5];
	uint64ArrayValues[0].push_back(127);
	uint64ArrayValues[0].push_back(133);
	uint64ArrayValues[0].push_back(57);
	uint64ArrayValues[0].push_back(0);
	uint64ArrayValues[0].push_back(1);
	uint64ArrayValues[0].push_back(45);
	uint64ArrayValues[1].push_back(77);
	uint64ArrayValues[1].push_back(123);
	uint64ArrayValues[1].push_back(127);
	uint64ArrayValues[1].push_back(133);
	uint64ArrayValues[1].push_back(57);
	uint64ArrayValues[1].push_back(0);
	uint64ArrayValues[2].push_back(1);
	uint64ArrayValues[2].push_back(45);
	uint64ArrayValues[2].push_back(77);
	uint64ArrayValues[2].push_back(123);
	uint64ArrayValues[2].push_back(127);
	uint64ArrayValues[2].push_back(133);
	uint64ArrayValues[3].push_back(57);
	uint64ArrayValues[3].push_back(0);
	uint64ArrayValues[3].push_back(1);
	uint64ArrayValues[3].push_back(45);
	uint64ArrayValues[3].push_back(77);
	uint64ArrayValues[3].push_back(123);
	uint64ArrayValues[4].push_back(127);
	uint64ArrayValues[4].push_back(133);
	uint64ArrayValues[4].push_back(57);
	uint64ArrayValues[4].push_back(0);
	uint64ArrayValues[4].push_back(1);
	uint64ArrayValues[4].push_back(45);
	DynamicTestVariablePtr testVariableUInt64 = createDynamicVariable(_T("Dynamic.UInt64Array"), uint64ArrayValues, sizeof(uint64ArrayValues) / sizeof(uint64ArrayValues[0]), Statics::DataTypeId_UInt64, true);
#endif

#ifdef COMPILE_24
	std::vector<OTInt64> int64ArrayValues[5];
	int64ArrayValues[0].push_back(-127);
	int64ArrayValues[0].push_back(-133);
	int64ArrayValues[0].push_back(-57);
	int64ArrayValues[0].push_back(0);
	int64ArrayValues[0].push_back(1);
	int64ArrayValues[0].push_back(45);
	int64ArrayValues[1].push_back(77);
	int64ArrayValues[1].push_back(133);
	int64ArrayValues[1].push_back(-127);
	int64ArrayValues[1].push_back(-133);
	int64ArrayValues[1].push_back(-57);
	int64ArrayValues[1].push_back(0);
	int64ArrayValues[2].push_back(1);
	int64ArrayValues[2].push_back(45);
	int64ArrayValues[2].push_back(77);
	int64ArrayValues[2].push_back(133);
	int64ArrayValues[2].push_back(-127);
	int64ArrayValues[2].push_back(-133);
	int64ArrayValues[3].push_back(-57);
	int64ArrayValues[3].push_back(0);
	int64ArrayValues[3].push_back(1);
	int64ArrayValues[3].push_back(45);
	int64ArrayValues[3].push_back(77);
	int64ArrayValues[3].push_back(133);
	int64ArrayValues[4].push_back(-127);
	int64ArrayValues[4].push_back(-133);
	int64ArrayValues[4].push_back(-57);
	int64ArrayValues[4].push_back(0);
	int64ArrayValues[4].push_back(1);
	int64ArrayValues[4].push_back(45);
	DynamicTestVariablePtr testVariableInt64 = createDynamicVariable(_T("Dynamic.Int64Array"), int64ArrayValues, sizeof(int64ArrayValues) / sizeof(int64ArrayValues[0]), Statics::DataTypeId_Int64, true);
#endif

}
#define COMPILE_31
#define COMPILE_32
#define COMPILE_33
#define COMPILE_34
void DynamicsModule::createDynamicArrayTestVariablesPart3()
{
	Value temp;

#ifdef COMPILE_31
	std::vector<double> doubleArrayValues[5];
	doubleArrayValues[0].push_back(-2303.3453);
	doubleArrayValues[0].push_back(-133.24);
	doubleArrayValues[0].push_back(-57.0);
	doubleArrayValues[0].push_back(0);
	doubleArrayValues[0].push_back(0.1);
	doubleArrayValues[0].push_back(45.234874);
	doubleArrayValues[1].push_back(7723435.34);
	doubleArrayValues[1].push_back(133.24);
	doubleArrayValues[1].push_back(-127.6587);
	doubleArrayValues[1].push_back(-133.57);
	doubleArrayValues[1].push_back(-57.54545);
	doubleArrayValues[1].push_back(0.99999);
	doubleArrayValues[2].push_back(1.3456);
	doubleArrayValues[2].push_back(45.8756);
	doubleArrayValues[2].push_back(77963.5458);
	doubleArrayValues[2].push_back(133.212);
	doubleArrayValues[2].push_back(-127.898745);
	doubleArrayValues[2].push_back(-133.89865);
	doubleArrayValues[3].push_back(-57.4826);
	doubleArrayValues[3].push_back(0.111111);
	doubleArrayValues[3].push_back(1.0009);
	doubleArrayValues[3].push_back(45.45488);
	doubleArrayValues[3].push_back(77.98654);
	doubleArrayValues[3].push_back(133.21586);
	doubleArrayValues[4].push_back(-127.56467);
	doubleArrayValues[4].push_back(-133.004657);
	doubleArrayValues[4].push_back(-57.8973);
	doubleArrayValues[4].push_back(10.9634);
	doubleArrayValues[4].push_back(1.345256);
	doubleArrayValues[4].push_back(45.9879641);

	DynamicTestVariablePtr testVariableDouble = createDynamicVariable(_T("Dynamic.DoubleArray"), doubleArrayValues, sizeof(doubleArrayValues) / sizeof(doubleArrayValues[0]), Statics::DataTypeId_Double, true);
#endif

#ifdef COMPILE_32
	std::vector<float> floatArrayValues[5];
	floatArrayValues[0].push_back((float)-2303.3453);
	floatArrayValues[0].push_back((float)-133.24);
	floatArrayValues[0].push_back((float)-57.0);
	floatArrayValues[0].push_back((float)0);
	floatArrayValues[0].push_back((float)0.1);
	floatArrayValues[0].push_back((float)45.234874);
	floatArrayValues[1].push_back((float)7723435.34);
	floatArrayValues[1].push_back((float)133.24);
	floatArrayValues[1].push_back((float)-127.6587);
	floatArrayValues[1].push_back((float)-133.57);
	floatArrayValues[1].push_back((float)-57.54545);
	floatArrayValues[1].push_back((float)0.99999);
	floatArrayValues[2].push_back((float)1.3456);
	floatArrayValues[2].push_back((float)45.8756);
	floatArrayValues[2].push_back((float)77963.5458);
	floatArrayValues[2].push_back((float)133.212);
	floatArrayValues[2].push_back((float)-127.898745);
	floatArrayValues[2].push_back((float)-133.89865);
	floatArrayValues[3].push_back((float)-57.4826);
	floatArrayValues[3].push_back((float)0.111111);
	floatArrayValues[3].push_back((float)1.0009);
	floatArrayValues[3].push_back((float)45.45488);
	floatArrayValues[3].push_back((float)77.98654);
	floatArrayValues[3].push_back((float)133.21586);
	floatArrayValues[4].push_back((float)-127.56467);
	floatArrayValues[4].push_back((float)-133.004657);
	floatArrayValues[4].push_back((float)-57.8973);
	floatArrayValues[4].push_back((float)10.9634);
	floatArrayValues[4].push_back((float)1.345256);
	floatArrayValues[4].push_back((float)45.9879641);

	DynamicTestVariablePtr testVariableFloat = createDynamicVariable(_T("Dynamic.FloatArray"), floatArrayValues, sizeof(floatArrayValues) / sizeof(floatArrayValues[0]), Statics::DataTypeId_Float, true);
#endif

#ifdef COMPILE_33
	std::vector<bool> booleanArrayValues[5];
	booleanArrayValues[0].push_back(true);
	booleanArrayValues[0].push_back(false);
	booleanArrayValues[1].push_back(false);
	booleanArrayValues[1].push_back(true);
	booleanArrayValues[2].push_back(true);
	booleanArrayValues[2].push_back(false);
	booleanArrayValues[3].push_back(false);
	booleanArrayValues[3].push_back(true);
	booleanArrayValues[4].push_back(true);
	booleanArrayValues[4].push_back(false);

	DynamicTestVariablePtr testVariableBoolean = createDynamicVariable(_T("Dynamic.BooleanArray"), booleanArrayValues, sizeof(booleanArrayValues) / sizeof(booleanArrayValues[0]), Statics::DataTypeId_Boolean, true);
#endif

#ifdef COMPILE_34
	std::vector<Value> valueArrayValues[5];
	temp.setDouble(0.345);
	valueArrayValues[0].push_back(temp);
	temp.setBoolean(true);
	valueArrayValues[0].push_back(temp);
	temp.setInt16(3);
	valueArrayValues[0].push_back(temp);
	temp.setDouble(99.6548);
	valueArrayValues[1].push_back(temp);
	temp.setBoolean(false);
	valueArrayValues[1].push_back(temp);
	temp.setInt16(5);
	valueArrayValues[1].push_back(temp);
	temp.setDouble(1.875665);
	valueArrayValues[2].push_back(temp);
	temp.setBoolean(true);
	valueArrayValues[2].push_back(temp);
	temp.setInt16(32767);
	valueArrayValues[2].push_back(temp);
	temp.setDouble(767565.111117);
	valueArrayValues[3].push_back(temp);
	temp.setBoolean(false);
	valueArrayValues[3].push_back(temp);
	temp.setInt16(-32768);
	valueArrayValues[3].push_back(temp);
	temp.setDouble(0.999);
	valueArrayValues[4].push_back(temp);
	temp.setBoolean(true);
	valueArrayValues[4].push_back(temp);
	temp.setInt16(-5);
	valueArrayValues[4].push_back(temp);
	DynamicTestVariablePtr testVariableValue = createDynamicVariable(_T("Dynamic.ValueArray"), valueArrayValues, sizeof(valueArrayValues) / sizeof(valueArrayValues[0]), Statics::DataTypeId_BaseDataType, true);
#endif

}

