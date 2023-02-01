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
#ifndef DUMMY_VALUE_GENERATOR_H
#define DUMMY_VALUE_GENERATOR_H

#include "Value.h"

class DummyValueGenerator
{
public:

	static bool supportsDataType(EnumDataTypeId dataType);

	/*! Creates Value with filled content of any data type, which is allowed within a value.
	* The not supported types will result in a Null-value.
	* @param typeId					The data type to use for the generated value
	* @param randomizationNumber	An optional number to create different value contents
	* @param arraySize				Optional parameter to specify arrays. Negative values means scalar. */
	static SoftingOPCToolbox5::Value generateDummyValue(EnumDataTypeId typeId, size_t randomizationNumber = 0, const std::vector<OTUInt32>* pArrayDimensions = NULL);

	static bool generateBoolean(size_t randomizationNumber);
	static OTInt8 generateSByte(size_t randomizationNumber);
	static OTUInt8 generateByte(size_t randomizationNumber);
	static OTInt16 generateInt16(size_t randomizationNumber);
	static OTUInt16 generateUInt16(size_t randomizationNumber);
	static OTInt32 generateInt32(size_t randomizationNumber);
	static OTUInt32 generateUInt32(size_t randomizationNumber);
	static OTInt64 generateInt64(size_t randomizationNumber);
	static OTUInt64 generateUInt64(size_t randomizationNumber);
	static OTFloat generateFloat(size_t randomizationNumber);
	static OTDouble generateDouble(size_t randomizationNumber);
	static tstring generateString(size_t randomizationNumber);
	static SoftingOPCToolbox5::DateTime generateDateTime(size_t randomizationNumber);
	static SoftingOPCToolbox5::Guid generateGuid(size_t randomizationNumber);
	static SoftingOPCToolbox5::ByteString generateByteString(size_t randomizationNumber);
	static SoftingOPCToolbox5::XmlElement generateXmlElement(size_t randomizationNumber);
	static SoftingOPCToolbox5::NodeId generateNodeId(size_t randomizationNumber);
	static SoftingOPCToolbox5::ExpandedNodeId generateExpandedNodeId(size_t randomizationNumber);
	static EnumStatusCode generateStatusCode(size_t randomizationNumber);
	static SoftingOPCToolbox5::QualifiedName generateQualifiedName(size_t randomizationNumber);
	static SoftingOPCToolbox5::LocalizedText generateLocalizedText(size_t randomizationNumber);
	static SoftingOPCToolbox5::Value generateStructure(size_t randomizationNumber);
	static SoftingOPCToolbox5::Value generateBaseDataType(size_t randomizationNumber);
	static SoftingOPCToolbox5::Value generateNumber(size_t randomizationNumber);
	static SoftingOPCToolbox5::Value generateInteger(size_t randomizationNumber);
	static SoftingOPCToolbox5::Value generateUInteger(size_t randomizationNumber);
	static OTDouble generateDuration(size_t randomizationNumber);
	static SoftingOPCToolbox5::DateTime generateUtcTime(size_t randomizationNumber);
	static SoftingOPCToolbox5::Argument generateArgument(size_t randomizationNumber);
	static SoftingOPCToolbox5::ApplicationDescription generateApplicationDescription(size_t randomizationNumber);
	static SoftingOPCToolbox5::UserIdentityToken generateUserIdentityToken(size_t randomizationNumber);
	static SoftingOPCToolbox5::AnonymousIdentityToken generateAnonymousIdentityToken(size_t randomizationNumber);
	static SoftingOPCToolbox5::UserNameIdentityToken generateUserNameIdentityToken(size_t randomizationNumber);
	static SoftingOPCToolbox5::X509IdentityToken generateX509IdentityToken(size_t randomizationNumber);
	static SoftingOPCToolbox5::IssuedIdentityToken generateIssuedIdentityToken(size_t randomizationNumber);
	static SoftingOPCToolbox5::BuildInfo generateBuildInfo(size_t randomizationNumber);
	static SoftingOPCToolbox5::SignedSoftwareCertificate generateSignedSoftwareCertificate(size_t randomizationNumber);
	static SoftingOPCToolbox5::ReferenceDescription generateReferenceDescription(size_t randomizationNumber);
#if TB5_ALARMS
	static SoftingOPCToolbox5::EventFilter generateEventFilter(size_t randomizationNumber);
#endif
	static SoftingOPCToolbox5::ServerDiagnosticsSummaryDataType generateServerDiagnosticsSummaryDataType(size_t randomizationNumber);
	static SoftingOPCToolbox5::ServerStatusDataType generateServerStatusDataType(size_t randomizationNumber);
	static SoftingOPCToolbox5::SessionDiagnosticsDataType generateSessionDiagnosticsDataType(size_t randomizationNumber);
	static SoftingOPCToolbox5::SessionSecurityDiagnosticsDataType generateSessionSecurityDiagnosticsDataType(size_t randomizationNumber);
	static SoftingOPCToolbox5::SubscriptionDiagnosticsDataType generateSubscriptionDiagnosticsDataType(size_t randomizationNumber);
#if TB5_ALARMS
	static SoftingOPCToolbox5::ModelChangeStructureDataType generateModelChangeStructureDataType(size_t randomizationNumber);
	static SoftingOPCToolbox5::SemanticChangeStructureDataType generateSemanticChangeStructureDataType(size_t randomizationNumber);
#endif
	static SoftingOPCToolbox5::Range generateRange(size_t randomizationNumber);
	static SoftingOPCToolbox5::EUInformation generateEUInformation(size_t randomizationNumber);
	static SoftingOPCToolbox5::EnumValueType generateEnumValueType(size_t randomizationNumber);
};

#endif	// DUMMY_VALUE_GENERATOR_H
