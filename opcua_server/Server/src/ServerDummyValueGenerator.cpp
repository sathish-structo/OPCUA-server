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
#include "ServerDummyValueGenerator.h"

#include "ExpandedNodeId.h"
#include "LocalizedText.h"
#include "NumericRange.h"
#include "Argument.h"
#include "UserIdentityToken.h"
#include "BuildInfo.h"
#include "SignedSoftwareCertificate.h"
#include "ServerDiagnosticsSummaryDataType.h"
#include "ServerStatusDataType.h"
#include "ModelChangeStructureDataType.h"
#include "Range.h"
#include "EUInformation.h"
#include "SemanticChangeStructureDataType.h"
#include "EnumValueType.h"
#include "ApplicationDescription.h"
#include "ReferenceDescription.h"
#include "EventFilter.h"
#include "SessionDiagnosticsDataType.h"
#include "SessionSecurityDiagnosticsDataType.h"
#include "SubscriptionDiagnosticsDataType.h"
#include "DataValue.h"

using namespace SoftingOPCToolbox5;


bool DummyValueGenerator::supportsDataType(EnumDataTypeId dataType)
{
	switch(dataType)
	{
	case EnumDataTypeId_Null:
	case EnumDataTypeId_Boolean:
	case EnumDataTypeId_SByte:
	case EnumDataTypeId_Byte:
	case EnumDataTypeId_Int16:
	case EnumDataTypeId_UInt16:
	case EnumDataTypeId_Int32:
	case EnumDataTypeId_UInt32:
	case EnumDataTypeId_Int64:
	case EnumDataTypeId_UInt64:
	case EnumDataTypeId_Float:
	case EnumDataTypeId_Double:
	case EnumDataTypeId_String:
	case EnumDataTypeId_DateTime:
	case EnumDataTypeId_Guid:
	case EnumDataTypeId_ByteString:
	case EnumDataTypeId_XmlElement:
	case EnumDataTypeId_NodeId:
	case EnumDataTypeId_ExpandedNodeId:
	case EnumDataTypeId_StatusCode:
	case EnumDataTypeId_QualifiedName:
	case EnumDataTypeId_LocalizedText:
	case EnumDataTypeId_Structure:
	case EnumDataTypeId_BaseDataType:
	case EnumDataTypeId_Number:
	case EnumDataTypeId_Integer:
	case EnumDataTypeId_UInteger:
		//case EnumDataTypeId_Enumeration:
		//case EnumDataTypeId_Image:
		//case EnumDataTypeId_NamingRuleType:
	case EnumDataTypeId_Duration:
		//case EnumDataTypeId_NumericRange:
	case EnumDataTypeId_UtcTime:
		//case EnumDataTypeId_LocaleId:
	case EnumDataTypeId_Argument:
	case EnumDataTypeId_ApplicationDescription:
	case EnumDataTypeId_UserIdentityToken:
	case EnumDataTypeId_AnonymousIdentityToken:
	case EnumDataTypeId_UserNameIdentityToken:
	case EnumDataTypeId_X509IdentityToken:
	case EnumDataTypeId_IssuedIdentityToken:
	case EnumDataTypeId_BuildInfo:
	case EnumDataTypeId_SignedSoftwareCertificate:
	case EnumDataTypeId_ReferenceDescription:
	case EnumDataTypeId_EventFilter:
	case EnumDataTypeId_ServerDiagnosticsSummaryDataType:
	case EnumDataTypeId_ServerStatusDataType:
		//case EnumDataTypeId_SessionDiagnosticsDataType:
		//case EnumDataTypeId_SessionSecurityDiagnosticsDataType:
		//case EnumDataTypeId_ServiceCounterDataType:
		//case EnumDataTypeId_SubscriptionDiagnosticsDataType:
	case EnumDataTypeId_ModelChangeStructureDataType:
	case EnumDataTypeId_Range:
	case EnumDataTypeId_EUInformation:
	case EnumDataTypeId_SemanticChangeStructureDataType:
		//case EnumDataTypeId_ImageBMP:
		//case EnumDataTypeId_ImageGIF:
		//case EnumDataTypeId_ImageJPG:
		//case EnumDataTypeId_ImagePNG:
	case EnumDataTypeId_EnumValueType:
		//case EnumDataTypeId_TimeZoneDataType:
		return true;
	default:
		return false;
	}
}

Value DummyValueGenerator::generateDummyValue(EnumDataTypeId typeId, size_t randomizationNumber /* = 0 */, const std::vector<OTUInt32>* pArrayDimensions /* = NULL */)
{
	size_t effectiveSize = 1;	// default for scalar
	if(pArrayDimensions != NULL)
	{
		for(size_t i = 0; i < pArrayDimensions->size(); i++)
		{
			if((*pArrayDimensions)[i] != 0)
			{
				effectiveSize *= (size_t)((*pArrayDimensions)[i]);
			}
		}
	}
	Value resultValue;

#define LOCAL_MACRO_CASE_REFERENCE(stackType, toolkitType, toolkitName)\
	case EnumDataTypeId_##stackType:\
	{\
	std::vector<toolkitType> data;\
	data.reserve(effectiveSize);\
	for(size_t i = 0; i < effectiveSize; i++)\
	{\
	data.push_back(generate##stackType(randomizationNumber + i));\
	}\
	if(pArrayDimensions == NULL || pArrayDimensions->empty())\
	{\
	resultValue.set##toolkitName(data[0]);\
	}\
	else if(pArrayDimensions->size() == 1)\
	{\
	resultValue.set##toolkitName##Array(data);\
	}\
	else\
	{\
	resultValue.set##toolkitName##Matrix(*pArrayDimensions, data);\
	}\
	}\
	break

#define LOCAL_MACRO_CASE_POINTER(stackType, toolkitType, toolkitName)\
	case EnumDataTypeId_##stackType:\
	{\
	std::vector<toolkitType> data;\
	data.reserve(effectiveSize);\
	for(size_t i = 0; i < effectiveSize; i++)\
	{\
	data.push_back(generate##stackType(randomizationNumber + i));\
	}\
	if(pArrayDimensions == NULL || pArrayDimensions->empty())\
	{\
	resultValue.set##toolkitName(&data[0]);\
	}\
	else if(pArrayDimensions->size() == 1)\
	{\
	resultValue.set##toolkitName##Array(data);\
	}\
	else\
	{\
	resultValue.set##toolkitName##Matrix(*pArrayDimensions, data);\
	}\
	}\
	break

#define LOCAL_MACRO_CASE_POINTER_TWOSETTER(stackType, toolkitType, toolkitNameScalar, toolkitNameArray)\
	case EnumDataTypeId_##stackType:\
	{\
	std::vector<toolkitType> data;\
	data.reserve(effectiveSize);\
	for(size_t i = 0; i < effectiveSize; i++)\
	{\
	data.push_back(generate##stackType(randomizationNumber + i));\
	}\
	if(pArrayDimensions == NULL || pArrayDimensions->empty())\
	{\
	resultValue.set##toolkitNameScalar(data[0]);\
	}\
	else if(pArrayDimensions->size() == 1)\
	{\
	resultValue.set##toolkitNameArray##Array(data);\
	}\
	else\
	{\
	resultValue.set##toolkitNameArray##Matrix(*pArrayDimensions, data);\
	}\
	}\
	break

#define LOCAL_MACRO_CASE_NULL(stackType)\
	case EnumDataTypeId_##stackType:\
	break;

	switch(typeId)
	{
		LOCAL_MACRO_CASE_NULL(Null);
		LOCAL_MACRO_CASE_REFERENCE(Boolean, OTBoolean, Boolean);
		LOCAL_MACRO_CASE_REFERENCE(SByte, OTInt8, Int8);
		LOCAL_MACRO_CASE_REFERENCE(Byte, OTUInt8, UInt8);
		LOCAL_MACRO_CASE_REFERENCE(Int16, OTInt16, Int16);
		LOCAL_MACRO_CASE_REFERENCE(UInt16, OTUInt16, UInt16);
		LOCAL_MACRO_CASE_REFERENCE(Int32, OTInt32, Int32);
		LOCAL_MACRO_CASE_REFERENCE(UInt32, OTUInt32, UInt32);
		LOCAL_MACRO_CASE_REFERENCE(Int64, OTInt64, Int64);
		LOCAL_MACRO_CASE_REFERENCE(UInt64, OTUInt64, UInt64);
		LOCAL_MACRO_CASE_REFERENCE(Float, OTFloat, Float);
		LOCAL_MACRO_CASE_REFERENCE(Double, OTDouble, Double);
		LOCAL_MACRO_CASE_REFERENCE(String, tstring, String);
		LOCAL_MACRO_CASE_POINTER(DateTime, DateTime, DateTime);
		LOCAL_MACRO_CASE_POINTER(Guid, Guid, Guid);
		LOCAL_MACRO_CASE_POINTER(ByteString, ByteString, ByteString);
		LOCAL_MACRO_CASE_POINTER(XmlElement, XmlElement, XmlElement);
		LOCAL_MACRO_CASE_POINTER(NodeId, NodeId, NodeId);
		LOCAL_MACRO_CASE_POINTER(ExpandedNodeId, ExpandedNodeId, ExpandedNodeId);
		LOCAL_MACRO_CASE_REFERENCE(StatusCode, EnumStatusCode, StatusCode);
		LOCAL_MACRO_CASE_POINTER(QualifiedName, QualifiedName, QualifiedName);
		LOCAL_MACRO_CASE_POINTER(LocalizedText, LocalizedText, LocalizedText);
		LOCAL_MACRO_CASE_POINTER_TWOSETTER(Structure, Value,, Value);	// 3. parameter generates "set(&data)"
		LOCAL_MACRO_CASE_POINTER_TWOSETTER(BaseDataType, Value,, Value);	// 3. parameter generates "set(&data)"
		LOCAL_MACRO_CASE_POINTER_TWOSETTER(Number, Value,, Value);	// 3. parameter generates "set(&data)"
		LOCAL_MACRO_CASE_POINTER_TWOSETTER(Integer, Value,, Value);	// 3. parameter generates "set(&data)"
		LOCAL_MACRO_CASE_POINTER_TWOSETTER(UInteger, Value,, Value);	// 3. parameter generates "set(&data)"
		//LOCAL_MACRO_CASE_POINTER(Enumeration, Enumeration, Enumeration);
		//LOCAL_MACRO_CASE_POINTER(Image, Image, Image);
		//LOCAL_MACRO_CASE_POINTER(NamingRuleType, NamingRuleType, NamingRuleType);
		LOCAL_MACRO_CASE_REFERENCE(Duration, OTDouble, Double);
		//LOCAL_MACRO_CASE_POINTER(NumericRange, NumericRange, NumericRange);
		LOCAL_MACRO_CASE_POINTER(UtcTime, DateTime, DateTime);
		//LOCAL_MACRO_CASE_POINTER(LocaleId, LocaleId, LocaleId);
		LOCAL_MACRO_CASE_POINTER(Argument, Argument, Argument);
		LOCAL_MACRO_CASE_POINTER(ApplicationDescription, ApplicationDescription, ApplicationDescription);
		LOCAL_MACRO_CASE_POINTER(BuildInfo, BuildInfo, BuildInfo);
		LOCAL_MACRO_CASE_POINTER(SignedSoftwareCertificate, SignedSoftwareCertificate, SignedSoftwareCertificate);
		LOCAL_MACRO_CASE_POINTER(ReferenceDescription, ReferenceDescription, ReferenceDescription);
#if TB5_ALARMS
		LOCAL_MACRO_CASE_POINTER(EventFilter, EventFilter, EventFilter);
#endif
		LOCAL_MACRO_CASE_POINTER(ServerDiagnosticsSummaryDataType, ServerDiagnosticsSummaryDataType, ServerDiagnosticsSummaryDataType);
		LOCAL_MACRO_CASE_POINTER(ServerStatusDataType, ServerStatusDataType, ServerStatusDataType);
		LOCAL_MACRO_CASE_POINTER(SessionDiagnosticsDataType, SessionDiagnosticsDataType, SessionDiagnosticsDataType);
		LOCAL_MACRO_CASE_POINTER(SessionSecurityDiagnosticsDataType, SessionSecurityDiagnosticsDataType, SessionSecurityDiagnosticsDataType);
		//LOCAL_MACRO_CASE_POINTER(ServiceCounterDataType, ServiceCounterDataType, ServiceCounterDataType);
		LOCAL_MACRO_CASE_POINTER(SubscriptionDiagnosticsDataType, SubscriptionDiagnosticsDataType, SubscriptionDiagnosticsDataType);
#if TB5_ALARMS
		LOCAL_MACRO_CASE_POINTER(ModelChangeStructureDataType, ModelChangeStructureDataType, ModelChangeStructureDataType);
#endif
		LOCAL_MACRO_CASE_POINTER(Range, Range, Range);
		LOCAL_MACRO_CASE_POINTER(EUInformation, EUInformation, EUInformation);
#if TB5_ALARMS
		LOCAL_MACRO_CASE_POINTER(SemanticChangeStructureDataType, SemanticChangeStructureDataType, SemanticChangeStructureDataType);
#endif
		//LOCAL_MACRO_CASE_POINTER(ImageBMP, ImageBMP, ImageBMP);
		//LOCAL_MACRO_CASE_POINTER(ImageGIF, ImageGIF, ImageGIF);
		//LOCAL_MACRO_CASE_POINTER(ImageJPG, ImageJPG, ImageJPG);
		//LOCAL_MACRO_CASE_POINTER(ImagePNG, ImagePNG, ImagePNG);
		LOCAL_MACRO_CASE_POINTER(EnumValueType, EnumValueType, EnumValueType);
		//LOCAL_MACRO_CASE_POINTER(TimeZoneDataType, TimeZoneDataType, TimeZoneDataType);
	default:
		_REPORT_ASSERTION(_T("This Data type is not supported for a Value"));
	}
#undef LOCAL_MACRO_CASE_REFERENCE
#undef LOCAL_MACRO_CASE_POINTER
#undef LOCAL_MACRO_CASE_POINTER_TWOSETTER
#undef LOCAL_MACRO_CASE_NULL

	return resultValue;
}

bool DummyValueGenerator::generateBoolean(size_t randomizationNumber)
{
	return randomizationNumber %2 ? true : false;
}

OTInt8 DummyValueGenerator::generateSByte(size_t randomizationNumber)
{
	return (OTInt8)randomizationNumber;
}

OTUInt8 DummyValueGenerator::generateByte(size_t randomizationNumber)
{
	return (OTInt8)randomizationNumber;
}

OTInt16 DummyValueGenerator::generateInt16(size_t randomizationNumber)
{
	return (OTInt16)randomizationNumber;
}

OTUInt16 DummyValueGenerator::generateUInt16(size_t randomizationNumber)
{
	return (OTUInt16)randomizationNumber;
}

OTInt32 DummyValueGenerator::generateInt32(size_t randomizationNumber)
{
	return (OTInt32)randomizationNumber;
}

OTUInt32 DummyValueGenerator::generateUInt32(size_t randomizationNumber)
{
	return (OTUInt32)randomizationNumber;
}

OTInt64 DummyValueGenerator::generateInt64(size_t randomizationNumber)
{
	return (OTInt64)randomizationNumber;
}

OTUInt64 DummyValueGenerator::generateUInt64(size_t randomizationNumber)
{
	return (OTUInt64)randomizationNumber;
}

OTFloat DummyValueGenerator::generateFloat(size_t randomizationNumber)
{
	return (OTFloat)randomizationNumber;
}

OTDouble DummyValueGenerator::generateDouble(size_t randomizationNumber)
{
	return (OTDouble)randomizationNumber;
}

tstring DummyValueGenerator::generateString(size_t randomizationNumber)
{
	tstringstream ss;
	ss << randomizationNumber;
	return ss.str();
}

DateTime DummyValueGenerator::generateDateTime(size_t randomizationNumber)
{
	DateTime data;
	data = DateTime::getUtcNow();
	data.addMilliseconds((randomizationNumber + 1) * 1000);
	return data;
}

Guid DummyValueGenerator::generateGuid(size_t randomizationNumber)
{
	Guid data;
	OTUInt64 data4 = (OTUInt64)randomizationNumber;
	data = Guid((OTUInt32)randomizationNumber, (OTUInt16)randomizationNumber, (OTUInt16)randomizationNumber, (OTUInt8*)&data4);
	return data;
}

ByteString DummyValueGenerator::generateByteString(size_t randomizationNumber)
{
	tstringstream ss;
	ss << _T("Data ") << randomizationNumber;
	return ByteString(ss.str());
}

XmlElement DummyValueGenerator::generateXmlElement(size_t randomizationNumber)
{
	tstringstream ss;
	ss << _T("Data ") << randomizationNumber;
	return ByteString(ss.str());
}

NodeId DummyValueGenerator::generateNodeId(size_t randomizationNumber)
{
	return NodeId(1, (OTUInt32)randomizationNumber);
}

ExpandedNodeId DummyValueGenerator::generateExpandedNodeId(size_t randomizationNumber)
{
	return ExpandedNodeId(1, (OTUInt32)randomizationNumber);
}

EnumStatusCode DummyValueGenerator::generateStatusCode(size_t randomizationNumber)
{
	return randomizationNumber %2 ? EnumStatusCode_Bad : EnumStatusCode_Good;
}

QualifiedName DummyValueGenerator::generateQualifiedName(size_t randomizationNumber)
{
	tstringstream ss;
	ss << _T("QualifiedName") << randomizationNumber;
	return QualifiedName(ss.str(), 1);
}

LocalizedText DummyValueGenerator::generateLocalizedText(size_t randomizationNumber)
{
	tstringstream ss;
	ss << _T("LocalizedText") << randomizationNumber;
	return LocalizedText(ss.str(), _T("en"));
}

Value DummyValueGenerator::generateStructure(size_t randomizationNumber)
{
	Value data;
	data.setComplexValueRaw(NodeId(1, _T("Unknown")), generateDummyValue(EnumDataTypeId_ByteString, randomizationNumber).getByteString());
	return data;
}

Value DummyValueGenerator::generateBaseDataType(size_t randomizationNumber)
{
	return Value((OTUInt32)randomizationNumber);
}

Value DummyValueGenerator::generateNumber(size_t randomizationNumber)
{
	return Value((OTUInt32)randomizationNumber);
}

Value DummyValueGenerator::generateInteger(size_t randomizationNumber)
{
	return Value((OTInt32)randomizationNumber);
}

Value DummyValueGenerator::generateUInteger(size_t randomizationNumber)
{
	return Value((OTUInt32)randomizationNumber);
}

OTDouble DummyValueGenerator::generateDuration(size_t randomizationNumber)
{
	return (double)randomizationNumber;
}

DateTime DummyValueGenerator::generateUtcTime(size_t randomizationNumber)
{
	return generateDateTime(randomizationNumber);
}

Argument DummyValueGenerator::generateArgument(size_t randomizationNumber)
{
	Argument data;
	tstringstream ss;
	ss << _T("Argument") << randomizationNumber;
	data.setDataType(NodeId(EnumNumericNodeId_UInt32));
	data.setName(ss.str());
	return data;
}

ApplicationDescription DummyValueGenerator::generateApplicationDescription(size_t randomizationNumber)
{
	ApplicationDescription data;
	data.setApplicationName(generateDummyValue(EnumDataTypeId_LocalizedText, randomizationNumber).getLocalizedText());
	data.setApplicationType(EnumApplicationType_Client);
	data.setApplicationUri(generateDummyValue(EnumDataTypeId_String, randomizationNumber).getString());
	return data;
}

BuildInfo DummyValueGenerator::generateBuildInfo(size_t randomizationNumber)
{
	BuildInfo data;
	data.setBuildDate(generateDummyValue(EnumDataTypeId_DateTime, randomizationNumber).getDateTime());
	data.setBuildNumber(generateDummyValue(EnumDataTypeId_String, randomizationNumber).getString());
	data.setManufacturerName(generateDummyValue(EnumDataTypeId_String, randomizationNumber).getString());
	data.setProductName(generateDummyValue(EnumDataTypeId_String, randomizationNumber).getString());
	data.setProductUri(generateDummyValue(EnumDataTypeId_String, randomizationNumber).getString());
	return data;
}

SignedSoftwareCertificate DummyValueGenerator::generateSignedSoftwareCertificate(size_t randomizationNumber)
{
	SignedSoftwareCertificate data;
	data.setCertificateData(generateDummyValue(EnumDataTypeId_ByteString, randomizationNumber).getByteString());
	data.setSignature(generateDummyValue(EnumDataTypeId_ByteString, randomizationNumber).getByteString());
	return data;
}

ReferenceDescription DummyValueGenerator::generateReferenceDescription(size_t randomizationNumber)
{
	ReferenceDescription data;
	data.setBrowseName(generateDummyValue(EnumDataTypeId_QualifiedName, randomizationNumber).getQualifiedName());
	data.setDisplayName(generateDummyValue(EnumDataTypeId_LocalizedText, randomizationNumber).getLocalizedText());
	data.setIsForward(generateDummyValue(EnumDataTypeId_Boolean, randomizationNumber).getBoolean());
	data.setNodeClass(EnumNodeClass_Node);
	data.setNodeId(generateDummyValue(EnumDataTypeId_ExpandedNodeId, randomizationNumber).getExpandedNodeId());
	data.setReferenceTypeId(generateDummyValue(EnumDataTypeId_NodeId, randomizationNumber).getNodeId());
	data.setTypeDefinition(generateDummyValue(EnumDataTypeId_ExpandedNodeId, randomizationNumber).getExpandedNodeId());
	return data;
}

#if TB5_ALARMS
EventFilter DummyValueGenerator::generateEventFilter(size_t randomizationNumber)
{
	EventFilter data;
	SimpleAttributeOperand atop;
	atop.setAttributeId(EnumAttributeId_DisplayName);
	data.addSelectClause(&atop);
	ContentFilter cofi;
	ContentFilterElement cofiel;
	cofiel.setFilterOperator(EnumFilterOperator_Equals);
	FilterOperand fiop;
	LiteralOperand litop;
	litop.setValue(Value((OTUInt32)randomizationNumber));
	fiop.setLiteralOperand(&litop);
	cofiel.addFilterOperand(&fiop);
	cofi.addElement(&cofiel);
	data.setWhereClause(cofi);
	return data;
}
#endif

ServerDiagnosticsSummaryDataType DummyValueGenerator::generateServerDiagnosticsSummaryDataType(size_t randomizationNumber)
{
	ServerDiagnosticsSummaryDataType data;
	data.setCumulatedSessionCount((OTUInt32)randomizationNumber);
	data.setCumulatedSubscriptionCount((OTUInt32)randomizationNumber);
	data.setCurrentSessionCount((OTUInt32)randomizationNumber);
	data.setCurrentSubscriptionCount((OTUInt32)randomizationNumber);
	data.setPublishingIntervalCount((OTUInt32)randomizationNumber);
	data.setRejectedRequestsCount((OTUInt32)randomizationNumber);
	data.setRejectedSessionCount((OTUInt32)randomizationNumber);
	data.setSecurityRejectedRequestsCount((OTUInt32)randomizationNumber);
	data.setSecurityRejectedSessionCount((OTUInt32)randomizationNumber);
	data.setServerViewCount((OTUInt32)randomizationNumber);
	data.setSessionAbortCount((OTUInt32)randomizationNumber);
	data.setSessionTimeoutCount((OTUInt32)randomizationNumber);
	return data;
}

ServerStatusDataType DummyValueGenerator::generateServerStatusDataType(size_t randomizationNumber)
{
	ServerStatusDataType data;
	data.setBuildInfo(generateDummyValue(EnumDataTypeId_BuildInfo, randomizationNumber).getBuildInfo());
	data.setCurrentTime(generateDummyValue(EnumDataTypeId_DateTime, randomizationNumber).getDateTime());
	data.setSecondsTillShutdown((OTUInt32)randomizationNumber);
	data.setShutdownReason(generateDummyValue(EnumDataTypeId_LocalizedText, randomizationNumber).getLocalizedText());
	data.setStartTime(generateDummyValue(EnumDataTypeId_DateTime, randomizationNumber).getDateTime());
	data.setState(EnumServerState_Shutdown);
	return data;
}

SessionDiagnosticsDataType DummyValueGenerator::generateSessionDiagnosticsDataType(size_t randomizationNumber)
{
	_UNUSED(randomizationNumber);
	_REPORT_ASSERTION(_T("Not Implemented"));
	return SessionDiagnosticsDataType();
}

SessionSecurityDiagnosticsDataType DummyValueGenerator::generateSessionSecurityDiagnosticsDataType(size_t randomizationNumber)
{
	_UNUSED(randomizationNumber);
	_REPORT_ASSERTION(_T("Not Implemented"));
	return SessionSecurityDiagnosticsDataType();
}

SubscriptionDiagnosticsDataType DummyValueGenerator::generateSubscriptionDiagnosticsDataType(size_t randomizationNumber)
{
	_UNUSED(randomizationNumber);
	_REPORT_ASSERTION(_T("Not Implemented"));
	return SubscriptionDiagnosticsDataType();
}

#if TB5_ALARMS
ModelChangeStructureDataType DummyValueGenerator::generateModelChangeStructureDataType(size_t randomizationNumber)
{
	ModelChangeStructureDataType data;
	data.setAffected(generateDummyValue(EnumDataTypeId_NodeId, randomizationNumber).getNodeId());
	data.setAffectedType(generateDummyValue(EnumDataTypeId_NodeId, randomizationNumber).getNodeId());
	data.setVerb(EnumModelChangeStructureVerbMask_NodeAdded);
	return data;
}
#endif

Range DummyValueGenerator::generateRange(size_t randomizationNumber)
{
	return Range((OTDouble)randomizationNumber, (OTDouble)randomizationNumber + 100);
}

EUInformation DummyValueGenerator::generateEUInformation(size_t randomizationNumber)
{
	EUInformation data;
	data.setDescription(generateDummyValue(EnumDataTypeId_LocalizedText, randomizationNumber).getLocalizedText());
	data.setDisplayName(generateDummyValue(EnumDataTypeId_LocalizedText, randomizationNumber).getLocalizedText());
	data.setNamespaceUri(generateDummyValue(EnumDataTypeId_String, randomizationNumber).getString());
	return data;
}

#if TB5_ALARMS
SemanticChangeStructureDataType DummyValueGenerator::generateSemanticChangeStructureDataType(size_t randomizationNumber)
{
	SemanticChangeStructureDataType data;
	data.setAffected(generateDummyValue(EnumDataTypeId_NodeId, randomizationNumber).getNodeId());
	data.setAffectedType(generateDummyValue(EnumDataTypeId_NodeId, randomizationNumber).getNodeId());
	return data;
}
#endif

EnumValueType DummyValueGenerator::generateEnumValueType(size_t randomizationNumber)
{
	EnumValueType data;
	data.setDescription(generateDummyValue(EnumDataTypeId_LocalizedText, randomizationNumber).getLocalizedText());
	data.setDisplayName(generateDummyValue(EnumDataTypeId_LocalizedText, randomizationNumber).getLocalizedText());
	data.setValue((OTUInt64)randomizationNumber);
	return data;
}
