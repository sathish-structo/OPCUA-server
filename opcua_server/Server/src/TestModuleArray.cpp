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
#include "Decimal.h"

using namespace SoftingOPCToolbox5;

TestVariablePtr StaticsModule::createArrayVariable(const tstring& varName, OTUInt16 namespaceIndex, const NodeId* dataTypeId)
{
	TestVariablePtr variable = createVariable(varName, namespaceIndex, dataTypeId);
	variable->setValueRank(1);
	return variable;
}

TestVariablePtr StaticsModule::createAndInitArrayVariable(const tstring& varName, OTUInt16 namespaceIndex, const NodeId* dataTypeId)
{
	TestVariablePtr variable = createArrayVariable(varName, namespaceIndex, dataTypeId);
	initArrayVariableValue(variable, dataTypeId);
	return variable;
}

void StaticsModule::createAllArrayVariables(OTUInt16 namespaceIndex)
{
	createAndInitArrayVariable(_T("UInt8Array"),          namespaceIndex, Statics::DataTypeId_Byte);
	createAndInitArrayVariable(_T("Int8Array"),           namespaceIndex, Statics::DataTypeId_SByte);
	createAndInitArrayVariable(_T("UInt16Array"),         namespaceIndex, Statics::DataTypeId_UInt16);
	createAndInitArrayVariable(_T("Int16Array"),          namespaceIndex, Statics::DataTypeId_Int16);
	createAndInitArrayVariable(_T("UInt32Array"),         namespaceIndex, Statics::DataTypeId_UInt32);
	createAndInitArrayVariable(_T("Int32Array"),          namespaceIndex, Statics::DataTypeId_Int32);
	createAndInitArrayVariable(_T("UInt64Array"),         namespaceIndex, Statics::DataTypeId_UInt64);
	createAndInitArrayVariable(_T("Int64Array"),          namespaceIndex, Statics::DataTypeId_Int64);
	createAndInitArrayVariable(_T("DoubleArray"),         namespaceIndex, Statics::DataTypeId_Double);
	createAndInitArrayVariable(_T("FloatArray"),          namespaceIndex, Statics::DataTypeId_Float);
	createAndInitArrayVariable(_T("BooleanArray"),        namespaceIndex, Statics::DataTypeId_Boolean);
	createAndInitArrayVariable(_T("StringArray"),         namespaceIndex, Statics::DataTypeId_String);
	createAndInitArrayVariable(_T("ByteStringArray"),     namespaceIndex, Statics::DataTypeId_ByteString);
	createAndInitArrayVariable(_T("XmlElementArray"),     namespaceIndex, Statics::DataTypeId_XmlElement);
	createAndInitArrayVariable(_T("NodeIdArray"),         namespaceIndex, Statics::DataTypeId_NodeId);
	createAndInitArrayVariable(_T("ExpandedNodeIdArray"), namespaceIndex, Statics::DataTypeId_ExpandedNodeId);
	createAndInitArrayVariable(_T("GuidArray"),           namespaceIndex, Statics::DataTypeId_Guid);
	createAndInitArrayVariable(_T("DateTimeArray"),       namespaceIndex, Statics::DataTypeId_DateTime);
	createAndInitArrayVariable(_T("StatusCodeArray"),     namespaceIndex, Statics::DataTypeId_StatusCode);
	createAndInitArrayVariable(_T("LocalizedTextArray"),  namespaceIndex, Statics::DataTypeId_LocalizedText);
	createAndInitArrayVariable(_T("QualifiedNameArray"),  namespaceIndex, Statics::DataTypeId_QualifiedName);
	createAndInitArrayVariable(_T("EUInformationArray"),  namespaceIndex, Statics::DataTypeId_EUInformation);
	createAndInitArrayVariable(_T("RangeArray"),          namespaceIndex, Statics::DataTypeId_Range);
	createAndInitArrayVariable(_T("ValueArray"),          namespaceIndex, Statics::DataTypeId_BaseDataType);
	createAndInitArrayVariable(_T("ArgumentArray"),       namespaceIndex, Statics::DataTypeId_Argument);
	createAndInitArrayVariable(_T("DecimalArray"),		  namespaceIndex, Statics::DataTypeId_Decimal);

	VariablePtr variantsArray = TestVariableForAuditEvents::create();

	Value temp;
	Value value;
	std::vector<Value> arr;

	temp.setInt16(123);
	arr.push_back(temp);

	temp.setDouble(123);
	arr.push_back(temp);
	temp.setBoolean(true);
	arr.push_back(temp);
	temp.setString(_T("Marry, Sally and John"));
	arr.push_back(temp);
	temp.setInt8(123);
	arr.push_back(temp);
	temp.setUInt16(123);
	arr.push_back(temp);
	temp.setUInt32(123);
	arr.push_back(temp);

	value.setValueArray(arr);

	DataValue variantsArrayDataValue;
	variantsArrayDataValue.setValue(value);
	variantsArray->setDataValue(&variantsArrayDataValue);
	variantsArray->setBrowseName(QualifiedName(_T("VariantsArray"), namespaceIndex));
	LocalizedText lt;
	lt.setText(_T("VariantsArray"));
	lt.setLocale(_T("en"));
	variantsArray->setDisplayName(&lt);
	variantsArray->setNodeId(NodeId(namespaceIndex, _T("VariantsArray")));
	variantsArray->setDataType(Statics::DataTypeId_BaseDataType);
	variantsArray->setValueRank(1);
	//NodeId dataType = testVariable->getDataType();
	variantsArray->setAccessLevel(variantsArray->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	variantsArray->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	variantsArray->insert();
	this->addReference(Statics::ReferenceTypeId_HasComponent, variantsArray);
}

void StaticsModule::initArrayVariableValue(TestVariablePtr variable, const NodeId* dataTypeId)
{
	DataValue initialValue;

	if(dataTypeId->getNamespaceIndex() == 0 &&
		dataTypeId->getIdentifierType() == EnumIdentifierType_Numeric)
	{
		OTUInt32 numericId = 0;
		dataTypeId->getNumericIdentifier(&numericId);
		switch(numericId)
		{
		case EnumNumericNodeId_Boolean:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<bool> boolArray;
				boolArray.push_back(true);
				boolArray.push_back(false);
				boolArray.push_back(true);
				boolArray.push_back(true);
				boolArray.push_back(false);
				boolArray.push_back(true);
				initialValue.getValue()->setBooleanArray(boolArray);
			}
			break;
		case EnumNumericNodeId_SByte:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<signed char> sbyteArray;
				sbyteArray.push_back((signed char)-8);
				sbyteArray.push_back((signed char)17);
				sbyteArray.push_back(numeric_limits<signed char>::max());
				sbyteArray.push_back(numeric_limits<signed char>::min());
				sbyteArray.push_back((signed char)-23);
				initialValue.getValue()->setInt8Array(sbyteArray);
			}
			break;
		case EnumNumericNodeId_Byte:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<unsigned char> byteArray;
				byteArray.push_back(8);
				byteArray.push_back(17);
				byteArray.push_back(numeric_limits<unsigned char>::max());
				byteArray.push_back(numeric_limits<unsigned char>::min());
				byteArray.push_back(23);
				initialValue.getValue()->setUInt8Array(byteArray);
			}
			break;
		case EnumNumericNodeId_Int16:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<short> int16Array;
				int16Array.push_back(-16);
				int16Array.push_back(-32);
				int16Array.push_back(numeric_limits<short>::max());
				int16Array.push_back(numeric_limits<short>::min());
				int16Array.push_back(32);
				int16Array.push_back(16);
				initialValue.getValue()->setInt16Array(int16Array);
			}
			break;
		case EnumNumericNodeId_UInt16:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<unsigned short> uint16Array;
				uint16Array.push_back(16);
				uint16Array.push_back(32);
				uint16Array.push_back(numeric_limits<unsigned short>::max());
				uint16Array.push_back(numeric_limits<unsigned short>::min());
				uint16Array.push_back(64);
				uint16Array.push_back(128);
				initialValue.getValue()->setUInt16Array(uint16Array);
			}
			break;
		case EnumNumericNodeId_Int32:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<OTInt32> int32Array;
				int32Array.push_back(-32);
				int32Array.push_back(-64);
				int32Array.push_back(numeric_limits<OTInt32>::max());
				int32Array.push_back(numeric_limits<OTInt32>::min());
				int32Array.push_back(64);
				int32Array.push_back(32);
				initialValue.getValue()->setInt32Array(int32Array);
			}
			break;
		case EnumNumericNodeId_UInt32:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<OTUInt32> uint32Array;
				uint32Array.push_back(32);
				uint32Array.push_back(64);
				uint32Array.push_back(numeric_limits<OTUInt32>::max());
				uint32Array.push_back(numeric_limits<OTUInt32>::min());
				uint32Array.push_back(128);
				uint32Array.push_back(256);
				initialValue.getValue()->setUInt32Array(uint32Array);
			}
			break;
		case EnumNumericNodeId_Int64:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<OTInt64> int64Array;
				int64Array.push_back(-64);
				int64Array.push_back(-128);
				int64Array.push_back(numeric_limits<OTInt64>::max());
				int64Array.push_back(numeric_limits<OTInt64>::min());
				int64Array.push_back(128);
				int64Array.push_back(64);
				initialValue.getValue()->setInt64Array(int64Array);
			}
			break;
		case EnumNumericNodeId_UInt64:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<unsigned long long> int64Array;
				int64Array.push_back(64);
				int64Array.push_back(128);
				int64Array.push_back(numeric_limits<unsigned long long>::max());
				int64Array.push_back(numeric_limits<unsigned long long>::min());
				int64Array.push_back(256);
				int64Array.push_back(512);
				initialValue.getValue()->setUInt64Array(int64Array);
			}
			break;
		case EnumNumericNodeId_Float:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<float> floatArray;
				floatArray.push_back((float)-6.28);
				floatArray.push_back((float)-3.14);
				floatArray.push_back(numeric_limits<float>::max());
				floatArray.push_back(numeric_limits<float>::min());
				floatArray.push_back((float)42.42);
				floatArray.push_back((float)123.45);
				initialValue.getValue()->setFloatArray(floatArray);
			}
			break;
		case EnumNumericNodeId_Double:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<double> doubleArray;
				doubleArray.push_back((double)-6.28);
				doubleArray.push_back((double)-3.14);
				doubleArray.push_back(numeric_limits<double>::max());
				doubleArray.push_back(numeric_limits<double>::min());
				doubleArray.push_back((double)42.42);
				doubleArray.push_back((double)123.45);
				initialValue.getValue()->setDoubleArray(doubleArray);
			}
			break;
		case EnumNumericNodeId_String:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<tstring> stringArray;
				stringArray.push_back(_T("string 1"));
				stringArray.push_back(_T("string 2"));
				stringArray.push_back(_T("string 3"));
				stringArray.push_back(_T("string 4"));
				stringArray.push_back(_T("string 5"));
				stringArray.push_back(_T("string 6"));
				initialValue.getValue()->setStringArray(stringArray);
			}
			break;
		case EnumNumericNodeId_DateTime:
			{
				std::vector<DateTime> dateTimeArray;
				dateTimeArray.push_back(DateTime::getUtcNow());
				dateTimeArray.push_back(DateTime::getUtcNow());
				dateTimeArray.push_back(DateTime::getUtcNow());
				dateTimeArray.push_back(DateTime::getUtcNow());
				dateTimeArray.push_back(DateTime::getUtcNow());
				dateTimeArray.push_back(DateTime::getUtcNow());
				initialValue.getValue()->setDateTimeArray(dateTimeArray);
			}
			break;
		case EnumNumericNodeId_ByteString:
			{
				// initial values copied to TestClient test cases ReadAllArrayValues and WriteAllArrayValues
				// any changes here have to be updated there also!
				unsigned char initData1[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99 };
				unsigned char initData2[] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xFF,0x23 };
				unsigned char initData3[] = {0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x89,0x01 };
				unsigned char initData4[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99 };
				unsigned char initData5[] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90 };
				unsigned char initData6[] = {0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x89,0x01 };
				std::vector<ByteString> byteStringArray;
				byteStringArray.push_back(ByteString(sizeof(initData1)/sizeof(initData1[0]), initData1));
				byteStringArray.push_back(ByteString(sizeof(initData2)/sizeof(initData2[0]), initData2));
				byteStringArray.push_back(ByteString(sizeof(initData3)/sizeof(initData3[0]), initData3));
				byteStringArray.push_back(ByteString(sizeof(initData4)/sizeof(initData4[0]), initData4));
				byteStringArray.push_back(ByteString(sizeof(initData5)/sizeof(initData5[0]), initData5));
				byteStringArray.push_back(ByteString(sizeof(initData6)/sizeof(initData6[0]), initData6));
				initialValue.getValue()->setByteStringArray(byteStringArray);
			}
			break;
		case EnumNumericNodeId_Guid:
			{
				std::vector<Guid> guidArray;
				guidArray.push_back(Guid::generateGuid());
				guidArray.push_back(Guid::generateGuid());
				guidArray.push_back(Guid::generateGuid());
				guidArray.push_back(Guid::generateGuid());
				guidArray.push_back(Guid::generateGuid());
				guidArray.push_back(Guid::generateGuid());
				initialValue.getValue()->setGuidArray(guidArray);
			}
			break;
		case EnumNumericNodeId_XmlElement:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				tstring initData1 = _T("<xml value=\"test1\"/>");
				tstring initData2 = _T("<xml value=\"test2\"/>");
				tstring initData3 = _T("<xml value=\"test3\"/>");
				tstring initData4 = _T("<xml value=\"test4\"/>");
				tstring initData5 = _T("<xml value=\"test5\"/>");
				std::vector<XmlElement> xmlElementArray;
				xmlElementArray.push_back(XmlElement(initData1));
				xmlElementArray.push_back(XmlElement(initData2));
				xmlElementArray.push_back(XmlElement(initData3));
				xmlElementArray.push_back(XmlElement(initData4));
				xmlElementArray.push_back(XmlElement(initData5));
				initialValue.getValue()->setXmlElementArray(xmlElementArray);
			}
			break;
		case EnumNumericNodeId_NodeId:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<NodeId> nodeIdArray;
				nodeIdArray.push_back(NodeId(1,6783));
				nodeIdArray.push_back(NodeId(2,1234));
				nodeIdArray.push_back(NodeId(3,5678));
				initialValue.getValue()->setNodeIdArray(nodeIdArray);
			}
			break;
		case EnumNumericNodeId_ExpandedNodeId:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<ExpandedNodeId> nodeIdArray;
				nodeIdArray.push_back(ExpandedNodeId(1,6783));
				nodeIdArray.push_back(ExpandedNodeId(2,1234));
				nodeIdArray.push_back(ExpandedNodeId(3,5678));
				initialValue.getValue()->setExpandedNodeIdArray(nodeIdArray);
			}
			break;
		case EnumNumericNodeId_StatusCode:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<EnumStatusCode> statusCodeArray;
				statusCodeArray.push_back(EnumStatusCode_Good);
				statusCodeArray.push_back(EnumStatusCode_BadInvalidArgument);
				statusCodeArray.push_back(EnumStatusCode_BadOutOfMemory);
				initialValue.getValue()->setStatusCodeArray(statusCodeArray);
			}
			break;
		case EnumNumericNodeId_QualifiedName:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<QualifiedName> qualifiedNameArray;
				qualifiedNameArray.push_back(QualifiedName(_T("qualifiedTextName1"), 1));
				qualifiedNameArray.push_back(QualifiedName(_T("qualifiedTextName2"), 2));
				qualifiedNameArray.push_back(QualifiedName(_T("qualifiedTextName3"), 3));
				qualifiedNameArray.push_back(QualifiedName(_T("qualifiedTextName4"), 3));
				qualifiedNameArray.push_back(QualifiedName(_T("qualifiedTextName5"), 3));
				initialValue.getValue()->setQualifiedNameArray(qualifiedNameArray);
			}
			break;
		case EnumNumericNodeId_LocalizedText:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				std::vector<LocalizedText> locTextArray;
				locTextArray.push_back(LocalizedText(_T("localizedText1"), _T("en")));
				locTextArray.push_back(LocalizedText(_T("localizedText2"), _T("de")));
				locTextArray.push_back(LocalizedText(_T("localizedText3"), _T("ro")));
				locTextArray.push_back(LocalizedText(_T("localizedText4"), _T("en-US")));
				locTextArray.push_back(LocalizedText(_T("localizedText5"), _T("de-DE")));
				initialValue.getValue()->setLocalizedTextArray(locTextArray);
			}
			break;
		case EnumNumericNodeId_EUInformation:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				EUInformation lt1,lt2,lt3;
				std::vector<EUInformation> EUInformationArray;
				lt1.setDescription(LocalizedText(_T("1. Hello EU Information world"), _T("en")));
				lt1.setDisplayName(LocalizedText(_T("1. Hello EU Information world"), _T("en")));
				lt1.setNamespaceUri(_T("1. Namespace uri"));
				lt1.setUnitId(1);
				lt2.setDescription(LocalizedText(_T("2. Hello EU Information world"), _T("en")));
				lt2.setDisplayName(LocalizedText(_T("2. Hello EU Information world"), _T("en")));
				lt2.setNamespaceUri(_T("2. Namespace uri"));
				lt2.setUnitId(2);
				lt3.setDescription(LocalizedText(_T("3. Hello EU Information world"), _T("en")));
				lt3.setDisplayName(LocalizedText(_T("3. Hello EU Information world"), _T("en")));
				lt3.setNamespaceUri(_T("3. Namespace uri"));
				lt3.setUnitId(3);
				EUInformationArray.push_back(lt1);
				EUInformationArray.push_back(lt2);
				EUInformationArray.push_back(lt3);
				initialValue.getValue()->setEUInformationArray(EUInformationArray);
			}
			break;
		case EnumNumericNodeId_Range:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				Range r1,r2,r3;
				r1.setHigh(100.0);
				r1.setLow(-100.0);
				r2.setHigh(1234.0);
				r2.setLow(-1234.0);
				r3.setHigh(99992.0);
				r3.setLow(-28771.0);
				std::vector<Range> rngArray;
				rngArray.push_back(r1);
				rngArray.push_back(r2);
				rngArray.push_back(r3);
				initialValue.getValue()->setRangeArray(rngArray);
			}
			break;
		case EnumNumericNodeId_BaseDataType:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				Value v1, v2, v3, v4, v5;
				v1.setBoolean(true);
				v2.setDouble(2.3);
				std::vector<unsigned char> arrUInt8;
				arrUInt8.push_back(1);
				arrUInt8.push_back(2);
				arrUInt8.push_back(4);
				v3.setUInt8Array(arrUInt8);
				std::vector<OTInt64> arrInt64;
				arrInt64.push_back(0x11);
				arrInt64.push_back(0x22);
				arrInt64.push_back(0x33);
				arrInt64.push_back(0x44);
				v4.setInt64Array(arrInt64);
				v5.setString(_T("Value5"));
				std::vector<Value> valArray;
				valArray.push_back(v1);
				valArray.push_back(v2);
				valArray.push_back(v3);
				valArray.push_back(v4);
				valArray.push_back(v5);
				initialValue.getValue()->setValueArray(valArray);
			}
			break;
		case EnumNumericNodeId_Argument:
			{
				// initial values copied to TestClient test case ReadAllArrayValues
				// any changes here have to be updated there also!
				Argument arg1, arg2, arg3;
				arg1.setDataType(NodeId(EnumNumericNodeId_LocalizedText));
				arg1.setDescription(LocalizedText(_T("What is the ultimare answer?!"), _T("en")));
				arg1.setName(_T("Sollution"));
				arg1.setValueRank(ValueRank_Scalar);

				arg2.setDataType(NodeId(EnumNumericNodeId_Boolean));
				arg2.setDescription(LocalizedText(_T("Is the answer true?!"), _T("en")));
				arg2.setName(_T("Check it"));
				arg2.setValueRank(ValueRank_Scalar);

				arg3.setDataType(NodeId(EnumNumericNodeId_UInt64));
				arg3.setDescription(LocalizedText(_T("The answer:"), _T("en")));
				arg3.setName(_T("The End"));
				arg3.setValueRank(ValueRank_Scalar);

				std::vector<Argument> argArray;
				argArray.push_back(arg1);
				argArray.push_back(arg2);
				argArray.push_back(arg3);

				initialValue.getValue()->setArgumentArray(argArray);
			}
			break;
		case EnumNumericNodeId_Decimal:
			{
				std::vector<Decimal> decArray;
				decArray.push_back(Decimal());
				decArray.back().set((OTInt32)127, 0);
				decArray.push_back(Decimal());
				decArray.back().set((OTInt32)-33, 1);
				decArray.push_back(Decimal());
				decArray.back().set((OTInt32)5600, -2);
				initialValue.getValue()->setDecimalArray(decArray);
			}
			break;
		default:
			_REPORT_ASSERTION(_T("Unknown data type"));
			// no initialization
			break;
		}
	}

	initialValue.setServerTimestamp(DateTime::getUtcNow());
	initialValue.setSourceTimestamp(DateTime::getUtcNow());

	variable->setDataValue(initialValue);
}
