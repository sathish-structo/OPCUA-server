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
#include "StructuredDataTypeModule.h"
#include "ServerDataType.h"
#include "ServerDictionaryVariable.h"
#include "StructuredDataTypeDescription.h"
#include "StructureField.h"
#include "EnumField.h"
#include "EnumerationDataTypeDescription.h"
#include "ServerAddressSpaceRoot.h"
#include "Statics.h"
#include "ServerObject.h"
#include <limits.h>
#include "Constants.h"
#include "ServerObjectType.h"
#include "MyRefrigeratorStateVariable.h"
// includes for well known structured types
#include "ExpandedNodeId.h"
#include "Argument.h"
#include "ApplicationDescription.h"
#include "UserIdentityToken.h"
#include "AnonymousIdentityToken.h"
#include "UserNameIdentityToken.h"
#include "X509IdentityToken.h"
#include "IssuedIdentityToken.h"
#include "BuildInfo.h"
#include "SignedSoftwareCertificate.h"
#include "ReferenceDescription.h"
#include "ServerDiagnosticsSummaryDataType.h"
#include "ServerStatusDataType.h"
#include "SessionDiagnosticsDataType.h"
#include "SessionSecurityDiagnosticsDataType.h"
#include "SubscriptionDiagnosticsDataType.h"
#include "ModelChangeStructureDataType.h"
#include "Range.h"
#include "EUInformation.h"
#include "SemanticChangeStructureDataType.h"
#include "EnumValueType.h"
#include "TimeZone.h"
#include "ToolkitTrace.h"
#include "ServerWriteRequest.h"
#include "TestVariable.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;
// #define TEST_SERVER_ADD_ENUM_TO_STRUCT 1

class StructVar : public TestVariableForAuditEvents
{
public:
	static ObjectPointer<StructVar> create();
protected:
	StructVar();
public:
	StructVar(void* otbHandle);
	virtual ~StructVar();
	virtual EnumStatusCode handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest);
};

typedef ObjectPointer<StructVar> StructVarPtr;
typedef ObjectPointer<const StructVar> StructVarConstPtr;


StructVar::StructVar() : TestVariableForAuditEvents()
{

}

StructVar::StructVar(void* otbHandle) : TestVariableForAuditEvents(otbHandle)
{

}

StructVar::~StructVar()
{

}

EnumStatusCode StructVar::handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest)
{
	if(pRequest->getWriteValue()->getDataValue()->getValue()->getDataType() == EnumDataTypeId_Structure)
	{
		DataTypeDescriptionConstPtr dataTypeDesc;
		AddressSpaceRoot::instance()->getDataTypeDescription(pRequest->getWriteValue()->getDataValue()->getValue(), dataTypeDesc);
		if(dataTypeDesc && (dataTypeDesc->getDescriptionType() == EnumDataTypeDescriptionType_Structure))
		{
			StructuredDataTypeDescriptionConstPtr structDesc;
			tstring message;
			std::vector<Value> fieldValues;
			structDesc = static_cast<const StructuredDataTypeDescription*>(const_cast<DataTypeDescription*>(dataTypeDesc.p()));
			message = _T("Value (");
			message += structDesc->getName().getName();
			message += _T("):");
			pRequest->getWriteValue()->getDataValue()->getValue()->getStructuredValue(structDesc, fieldValues);
			for(size_t i=0; i<fieldValues.size(); i++)
			{
				message += _T("\n   ");
				message += fieldValues[i].toString();
			}
			Trace::instance()->writeLine(EnumTraceLevel_Debug, EnumTraceGroup_User1, message, _T(""));
		}
	}
	return TestVariableForAuditEvents::handleWriteRequest(pTransaction, pRequest);
}

ObjectPointer<StructVar> StructVar::create()
{
	return new StructVar();
}


StructuredDataTypeModule::StructuredDataTypeModule()
{
}

StructuredDataTypeModule::~StructuredDataTypeModule()
{
}

EnumStatusCode StructuredDataTypeModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isGood(result))
	{
		//set the required event related properties and references to raise events
		result = setEventNotifier(EnumEventNotifier_SubscribeToEvents);
	}
	if(StatusCode::isGood(result))
	{
		result = parentFolder->addReference(Statics::ReferenceTypeId_HasNotifier, this);
	}

	// these attributes should match to the .Net Demo Server
	m_refrigeratorStatusDataTypeId = NodeId(namespaceIndex, 435);
	m_dictionaryId = NodeId(namespaceIndex, 445);
	NodeId namespaceUriId(namespaceIndex, 447);
	NodeId dataTypeDescriptionId = NodeId(namespaceIndex, 448);
	NodeId encodingId = NodeId(namespaceIndex, 444);

	tstring typeName = _T("RefrigeratorStatusType");
	NodeId parentDataTypeId(EnumNumericNodeId_Structure);
	tstring dictionaryName = _T("Refrigerator");
	tstring dataTypeDescriptionCustomAttributeName = _T("BaseType");
	tstring dataTypeDescriptionCustomAttributeValue = _T("ua:ExtensionObject");
	tstring field1Name = _T("CondensorMotorRunning");
	NodeId field1TypeId(EnumNumericNodeId_Boolean);
	tstring field2Name = _T("PreasureBeforePump");
	NodeId field2TypeId(EnumNumericNodeId_Double);
	tstring field3Name = _T("PreasureAfterPump");
	NodeId field3TypeId(EnumNumericNodeId_Double);
	NodeId refrigeratorStatusId(namespaceIndex, 13);
	QualifiedName refrigeratorStatusBrowseName(_T("RefrigeratorStatus"), namespaceIndex);
	LocalizedText refrigeratorStatusDisplayName(_T("RefrigeratorStatus"), _T(""));
	NodeId refrigeratorStatusId2(namespaceIndex, 14);
	QualifiedName refrigeratorStatusBrowseName2(_T("RefrigeratorStatus2"), namespaceIndex);
	LocalizedText refrigeratorStatusDisplayName2(_T("RefrigeratorStatus2"), _T(""));
	OTInt32 refrigeratorStatusValueRank = ValueRank_Scalar;

	StructuredDataTypeDescriptionPtr outerDataTypeDescription;
	NodeId outerStructVarId(namespaceIndex, 15);
	QualifiedName outerStructVarBrowseName(_T("OuterStructVar"), namespaceIndex);
	LocalizedText outerStructVarDisplayName(_T("OuterStructVar"), _T(""));
	NodeId outerStructArrayVarId(namespaceIndex, 16);
	QualifiedName outerStructArrayVarBrowseName(_T("OuterStructArrayVar"), namespaceIndex);
	LocalizedText outerStructArrayVarDisplayName(_T("OuterStructArrayVar"), _T(""));
	NodeId outerStructMatrixVarId(namespaceIndex, 17);
	QualifiedName outerStructMatrixVarBrowseName(_T("OuterStructMatrixVar"), namespaceIndex);
	LocalizedText outerStructMatrixVarDisplayName(_T("OuterStructMatrixVar"), _T(""));

	///////////////////////////////
	// Data Type, Dictionary ... //
	///////////////////////////////

	DictionaryVariablePtr dictionary;
	StructuredDataTypeDescriptionPtr refStatusDataTypeDescription;
	DataTypePtr dataType;
	DataTypePtr refStatusDataType;

	// create dictionary
	dictionary = DictionaryVariable::create();
	(void)dictionary->setNodeId(m_dictionaryId);
	(void)dictionary->setBrowseName(QualifiedName(dictionaryName, namespaceIndex));
	(void)dictionary->setDisplayName(LocalizedText(dictionaryName, _T("")));
	// empty description
	result = dictionary->registerDictionary();
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to register the data type dictionary: %s\n"), getEnumStatusCodeString(result));
	}
	else
	{
		result = dictionary->addNamespaceUri(namespaceUriId);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to add the namespaceUri property to the dictionary.\n"));
		}
	}

#ifdef MULTIPLE_DICTIONARIES_IN_SAME_UA_NAMSPEACE
	{
		// create a additional dictionaries in same namespace (without data types - for testing namespace URI property)
		NodeId dictNodeId2, namespaceUriId2;
		
		dictNodeId2.setNumeric(namespaceIndex, 10000);
		namespaceUriId2.setNumeric(namespaceIndex, 10001);
		dictionary = DictionaryVariable::create();
		(void)dictionary->setNodeId(dictNodeId2);
		(void)dictionary->setBrowseName(QualifiedName(dictionaryName + _T("_1"), namespaceIndex));
		(void)dictionary->setDisplayName(LocalizedText(dictionaryName + _T("_1"), _T("")));
		// register first, then add namespace uri
		result = dictionary->registerDictionary();
		result = dictionary->addNamespaceUri(namespaceUriId2);


		dictNodeId2.setNumeric(namespaceIndex, 10003);
		namespaceUriId2.setNumeric(namespaceIndex, 10004);
		dictionary = DictionaryVariable::create();
		(void)dictionary->setNodeId(dictNodeId2);
		(void)dictionary->setBrowseName(QualifiedName(dictionaryName + _T("_2"), namespaceIndex));
		(void)dictionary->setDisplayName(LocalizedText(dictionaryName + _T("_2"), _T("")));
		// add namespace uri first, then register
		result = dictionary->addNamespaceUri(namespaceUriId2);
		result = dictionary->registerDictionary();
	}
#endif

	// big barrel type and variables
	initBigBarrel(namespaceIndex, this);
	// refrigerator state type and variables
	initRefrigeratorStateEnumeration(namespaceIndex, this);

	// structure with optional fields
	initStructureWithOptional(namespaceIndex, this);

	if(StatusCode::isGood(result))
	{
		// create the field descriptions
		std::vector<StructureField> fields;
		fields.push_back(StructureField());
		fields.back().setName(field1Name);
		fields.back().setDataType(field1TypeId);

		fields.push_back(StructureField());
		fields.back().setName(field2Name);
		fields.back().setDataType(field2TypeId);

		fields.push_back(StructureField());
		fields.back().setName(field3Name);
		fields.back().setDataType(field3TypeId);
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
		fields.push_back(StructureField());
		fields.back().setName(_T("StateEnum"));
		fields.back().setDataType(m_enumRefrigeratorStateTypeId);
#endif

	// create the data type description
		refStatusDataTypeDescription = StructuredDataTypeDescription::create();
		result = refStatusDataTypeDescription->setFields(fields);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the field descriptions\n"));
		}
	}
	if(StatusCode::isGood(result))
	{
		// create data type
		dataType = DataType::create();
		(void)dataType->setNodeId(m_refrigeratorStatusDataTypeId);
		(void)dataType->setBrowseName(QualifiedName(typeName, namespaceIndex));
		(void)dataType->setDisplayName(LocalizedText(typeName, _T("")));
		// empty description
		// default write mask
		result = dataType->setDataTypeDescription(refStatusDataTypeDescription, dataTypeDescriptionId, encodingId, m_dictionaryId);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the data type description.\n"));
		}
	}
	if(StatusCode::isGood(result))
	{
		DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
		(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);
		result = dataType->init(NULL);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to initialize the data type node.\n"));
		}
		refStatusDataType = dataType;
	}

	// Struct with fixed array field
	{
		StructuredDataTypeDescriptionPtr strucDesc;
		std::vector<StructureField> structDescFields;
		structDescFields.push_back(StructureField());;
		structDescFields.back().setName(_T("SclalarString"));
		structDescFields.back().setDataType(*Statics::DataTypeId_String);

		structDescFields.push_back(StructureField());;
		structDescFields.back().setName(_T("FixedArray"));
		structDescFields.back().setDataType(*Statics::DataTypeId_Double);
		std::vector<OTUInt32> arrayDimensions;
		arrayDimensions.push_back(3);
		structDescFields.back().setArrayDimensions(arrayDimensions);

		structDescFields.push_back(StructureField());;
		structDescFields.back().setName(_T("SclalarDouble"));
		structDescFields.back().setDataType(*Statics::DataTypeId_Double);


		strucDesc = StructuredDataTypeDescription::create();
		result = strucDesc->setFields(structDescFields);

		dataType = DataType::create();
		(void)dataType->setNodeId(NodeId(namespaceIndex, 9999));
		(void)dataType->setBrowseName(QualifiedName(_T("TestFixedArrayType"), namespaceIndex));
		(void)dataType->setDisplayName(LocalizedText(_T("TestFixedArrayType"), _T("")));

		result = dataType->setDataTypeDescription(strucDesc, NodeId(namespaceIndex, 9998), NodeId(namespaceIndex, 9997), m_dictionaryId);
		result = dataType->init(NULL);

		DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
		(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);

		VariablePtr testFixedArrayVar;
		if(StatusCode::isGood(result))
		{
			// RefrigeratorStatus variable
			testFixedArrayVar = TestVariableForAuditEvents::create();
			testFixedArrayVar->setNodeId(NodeId(namespaceIndex, _T("TestFixedArrayVar")));
			testFixedArrayVar->setBrowseName(QualifiedName(_T("TestFixedArrayVar"), namespaceIndex));
			testFixedArrayVar->setDisplayName(LocalizedText(_T("TestFixedArrayVar"), _T("")));
			testFixedArrayVar->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			testFixedArrayVar->setValueRank(ValueRank_Scalar);
			testFixedArrayVar->setDataType(NodeId(namespaceIndex, 9999));

			testFixedArrayVar->init(NULL);
			addReference(Statics::ReferenceTypeId_HasComponent, testFixedArrayVar);

			std::vector<Value> testFixedValues;
			std::vector<OTDouble> doubleArray;
			testFixedValues.push_back(_T("TestString"));
			doubleArray.push_back(1.1);
			doubleArray.push_back(2.2);
			doubleArray.push_back(3.3);
			Value arrayVal;
			arrayVal.setDoubleArray(doubleArray);
			testFixedValues.push_back(arrayVal);
			testFixedValues.push_back(Value((OTDouble)55.75));

			Value val;
			val.setStructuredValue(strucDesc, testFixedValues);
			DataValue dval;
			dval.setValue(val);
			testFixedArrayVar->setDataValue(dval);
		}

	}

	// Struct with matrix field
	{
		StructuredDataTypeDescriptionPtr strucDesc;
		std::vector<StructureField> structDescFields;
		structDescFields.push_back(StructureField());;
		structDescFields.back().setName(_T("SclalarString"));
		structDescFields.back().setDataType(*Statics::DataTypeId_String);

		structDescFields.push_back(StructureField());;
		structDescFields.back().setName(_T("MatrixField"));
		structDescFields.back().setDataType(*Statics::DataTypeId_Double);
		std::vector<OTUInt32> arrayDimensions;
		arrayDimensions.push_back(2);
		arrayDimensions.push_back(3);
		structDescFields.back().setArrayDimensions(arrayDimensions);

		structDescFields.push_back(StructureField());;
		structDescFields.back().setName(_T("SclalarDouble"));
		structDescFields.back().setDataType(*Statics::DataTypeId_Double);


		strucDesc = StructuredDataTypeDescription::create();
		result = strucDesc->setFields(structDescFields);

		dataType = DataType::create();
		(void)dataType->setNodeId(NodeId(namespaceIndex, 5678));
		(void)dataType->setBrowseName(QualifiedName(_T("TestMatrixFieldType"), namespaceIndex));
		(void)dataType->setDisplayName(LocalizedText(_T("TestMatrixFieldType"), _T("")));

		result = dataType->setDataTypeDescription(strucDesc, NodeId(namespaceIndex, 5679), NodeId(namespaceIndex, 5680), m_dictionaryId);
		result = dataType->init(NULL);

		DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
		(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);

		VariablePtr testMatrixFieldVar;
		if(StatusCode::isGood(result))
		{
			// RefrigeratorStatus variable
			testMatrixFieldVar = TestVariableForAuditEvents::create();
			testMatrixFieldVar->setNodeId(NodeId(namespaceIndex, _T("TestMatrixFieldVar")));
			testMatrixFieldVar->setBrowseName(QualifiedName(_T("TestMatrixFieldVar"), namespaceIndex));
			testMatrixFieldVar->setDisplayName(LocalizedText(_T("TestMatrixFieldVar"), _T("")));
			testMatrixFieldVar->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			testMatrixFieldVar->setValueRank(ValueRank_Scalar);
			testMatrixFieldVar->setDataType(NodeId(namespaceIndex, 5678));

			testMatrixFieldVar->init(NULL);
			addReference(Statics::ReferenceTypeId_HasComponent, testMatrixFieldVar);

			std::vector<Value> testFixedValues;
			std::vector<OTDouble> doubleArray;
			testFixedValues.push_back(_T("TestString"));
			doubleArray.push_back(1.1);
			doubleArray.push_back(2.2);
			doubleArray.push_back(3.3);
			doubleArray.push_back(4.4);
			doubleArray.push_back(5.5);
			doubleArray.push_back(6.6);
			Value arrayVal;
			arrayVal.setDoubleMatrix(arrayDimensions, doubleArray);
			testFixedValues.push_back(arrayVal);
			testFixedValues.push_back(Value((OTDouble)55.75));

			Value val;
			val.setStructuredValue(strucDesc, testFixedValues);
			DataValue dval;
			dval.setValue(val);
			testMatrixFieldVar->setDataValue(dval);
		}

	}

	// Struct with matrix field (user-defined)
	{
		StructuredDataTypeDescriptionPtr strucDesc;
		std::vector<StructureField> structDescFields;
		structDescFields.push_back(StructureField());;
		structDescFields.back().setName(_T("SclalarString"));
		structDescFields.back().setDataType(*Statics::DataTypeId_String);

		structDescFields.push_back(StructureField());;
		structDescFields.back().setName(_T("MatrixRefStatus"));
		structDescFields.back().setDataType(m_refrigeratorStatusDataTypeId);
		std::vector<OTUInt32> arrayDimensions;
		arrayDimensions.push_back(2);
		arrayDimensions.push_back(3);
		structDescFields.back().setArrayDimensions(arrayDimensions);

		structDescFields.push_back(StructureField());;
		structDescFields.back().setName(_T("SclalarDouble"));
		structDescFields.back().setDataType(*Statics::DataTypeId_Double);


		strucDesc = StructuredDataTypeDescription::create();
		result = strucDesc->setFields(structDescFields);

		dataType = DataType::create();
		(void)dataType->setNodeId(NodeId(namespaceIndex, 5681));
		(void)dataType->setBrowseName(QualifiedName(_T("TestMatrixRefStatusType"), namespaceIndex));
		(void)dataType->setDisplayName(LocalizedText(_T("TestMatrixRefStatusType"), _T("")));

		result = dataType->setDataTypeDescription(strucDesc, NodeId(namespaceIndex, 5682), NodeId(namespaceIndex, 5683), m_dictionaryId);
		result = dataType->init(NULL);

		DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
		(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);

		VariablePtr testMatrixFieldVar;
		if(StatusCode::isGood(result))
		{
			// Struct with matrix field (user-defined) variable
			testMatrixFieldVar = TestVariableForAuditEvents::create();
			testMatrixFieldVar->setNodeId(NodeId(namespaceIndex, _T("TestMatrixRefStatusVar")));
			testMatrixFieldVar->setBrowseName(QualifiedName(_T("TestMatrixRefStatusVar"), namespaceIndex));
			testMatrixFieldVar->setDisplayName(LocalizedText(_T("TestMatrixRefStatusVar"), _T("")));
			testMatrixFieldVar->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			testMatrixFieldVar->setValueRank(ValueRank_Scalar);
			testMatrixFieldVar->setDataType(NodeId(namespaceIndex, 5681));

			testMatrixFieldVar->init(NULL);
			addReference(Statics::ReferenceTypeId_HasComponent, testMatrixFieldVar);

			std::vector<Value> testFixedValues;
			std::vector<Value> refStatusArray;
			testFixedValues.push_back(_T("TestString"));
			for(size_t i = 0; i < 6; i++)
			{
				std::vector<Value> singleRefStatus;

					singleRefStatus.push_back(Value(i % 2 == 0));
					singleRefStatus.push_back(Value((double)(i * 10)));
					singleRefStatus.push_back(Value((double)(i * 20)));
					refStatusArray.push_back(Value());
					refStatusArray.back().setValueArray(singleRefStatus);
			}

			testFixedValues.push_back(Value());
			// The field is a matrix of RefStatus - so the field value is a matrix of value arrays
			// Each element (value array) is defining the value of one RefStatus.
			// A better interface would also accept a matrix of structured values, but this is not (yet) implemented.
			testFixedValues.back().setValueMatrix(arrayDimensions, refStatusArray);
			testFixedValues.push_back(Value((OTDouble)55.75));

			Value val;
			val.setStructuredValue(strucDesc, testFixedValues);
			DataValue dval;
			dval.setValue(val);
			testMatrixFieldVar->setDataValue(dval);
		}

	}

	{
		// create another structure with nested refrigerator status structure
		tstring outerTypeName = _T("OuterStructureType");

		DataTypePtr outerDataType;
		m_outerStructureDataTypeId = NodeId(namespaceIndex, 500);
		NodeId outerDataTypeDescriptionId = NodeId(namespaceIndex, 501);
		NodeId outerEncodingId = NodeId(namespaceIndex, 502);

		std::vector<StructureField> outerFields;

		outerFields.push_back(StructureField());
		outerFields.back().setName(_T("OuterStringValue"));
		outerFields.back().setDataType(*Statics::DataTypeId_String);
		
		outerFields.push_back(StructureField());
		outerFields.back().setName(_T("InnerStruct"));
		outerFields.back().setDataType(m_refrigeratorStatusDataTypeId);

		outerFields.push_back(StructureField());
		outerFields.back().setName(_T("OuterDoubleValue"));
		outerFields.back().setDataType(*Statics::DataTypeId_Double);

		outerFields.push_back(StructureField());
		outerFields.back().setName(_T("OuterRangeValue"));
		outerFields.back().setDataType(*Statics::DataTypeId_Range);

		outerDataTypeDescription = StructuredDataTypeDescription::create();
		result = outerDataTypeDescription->setFields(outerFields);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the field descriptions\n"));
		}

		if(StatusCode::isGood(result))
		{
			// create data type
			outerDataType = DataType::create();
			(void)outerDataType->setNodeId(m_outerStructureDataTypeId);
			(void)outerDataType->setBrowseName(QualifiedName(outerTypeName, namespaceIndex));
			(void)outerDataType->setDisplayName(LocalizedText(outerTypeName, _T("")));
			// empty description
			// default write mask
			result = outerDataType->setDataTypeDescription(outerDataTypeDescription, outerDataTypeDescriptionId, outerEncodingId, m_dictionaryId);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to set the data type description.\n"));
			}
		}
		if(StatusCode::isGood(result))
		{
			DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
			(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, outerDataType);
			result = outerDataType->init(NULL);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to initialize the data type node.\n"));
			}
		}
	}

	// child structure
	if(StatusCode::isGood(result))
	{
		result = initChildStructure(namespaceIndex, this);
	}

	// union (must call after child structure init)
	if(StatusCode::isGood(result))
	{
		result = initUnion(namespaceIndex, this);
	}

	_UNUSED(result);

	///////////////////
	// Variables ... //
	///////////////////
	{
		Value structVal, structVal2;
		VariablePtr refrigeratorStatusVar, refrigeratorStatusVar2;
		if(StatusCode::isGood(result))
		{
			// RefrigeratorStatus variable
			refrigeratorStatusVar = TestVariableForAuditEvents::create();
			refrigeratorStatusVar->setNodeId(refrigeratorStatusId);
			refrigeratorStatusVar->setBrowseName(refrigeratorStatusBrowseName);
			refrigeratorStatusVar->setDisplayName(refrigeratorStatusDisplayName);
			refrigeratorStatusVar->setAccessLevel(refrigeratorStatusVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			refrigeratorStatusVar->setValueRank(refrigeratorStatusValueRank);
			refrigeratorStatusVar->setDataType(m_refrigeratorStatusDataTypeId);

			refrigeratorStatusVar->init(NULL);
			addReference(Statics::ReferenceTypeId_HasComponent, refrigeratorStatusVar);
		}
		if(StatusCode::isGood(result))
		{
			DataValue dval;
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value(true));
			fieldValues.push_back(Value((double)110.0));
			fieldValues.push_back(Value((double)70.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			fieldValues.push_back(Value((OTInt32)2));
#endif
			{
				EnumStatusCode setResult = structVal.setStructuredValue(refStatusDataTypeDescription, fieldValues);
				if(StatusCode::isBad(setResult))
				{
					_tprintf(_T("Failed to set the structured value.\n"));
				}
			}

			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			refrigeratorStatusVar->setDataValue(dval);

			// second RefrigeratorStatus variable
			refrigeratorStatusVar2 = TestVariableForAuditEvents::create();
			refrigeratorStatusVar2->setNodeId(refrigeratorStatusId2);
			refrigeratorStatusVar2->setBrowseName(refrigeratorStatusBrowseName2);
			refrigeratorStatusVar2->setDisplayName(refrigeratorStatusDisplayName2);
			refrigeratorStatusVar2->setAccessLevel(refrigeratorStatusVar2->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			refrigeratorStatusVar2->setValueRank(refrigeratorStatusValueRank);
			refrigeratorStatusVar2->setDataType(m_refrigeratorStatusDataTypeId);

			refrigeratorStatusVar2->init(NULL);
			addReference(Statics::ReferenceTypeId_HasComponent, refrigeratorStatusVar2);

			std::vector<Value> fieldValues2;
			fieldValues2.push_back(Value(true));
			fieldValues2.push_back(Value((double)220.0));
			fieldValues2.push_back(Value((double)140.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			fieldValues2.push_back(Value((OTInt32)1));
#endif
			{
				EnumStatusCode setResult2 = structVal2.setStructuredValue(refStatusDataTypeDescription, fieldValues2);
				if(StatusCode::isBad(setResult2))
				{
					_tprintf(_T("Failed to set the structured value.\n"));
				}
			}
		}
		if(StatusCode::isGood(result))
		{
			Value structValMatrix;
			VariablePtr refrigeratorStatusMatrix;
			DataValue dval;
			tstring varName = _T("RefrigeratorStatusMatrix");

			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			refrigeratorStatusVar->setDataValue(dval);

			// second RefrigeratorStatus variable
			refrigeratorStatusMatrix = TestVariableForAuditEvents::create();
			refrigeratorStatusMatrix->setNodeId(NodeId(namespaceIndex, varName));
			refrigeratorStatusMatrix->setBrowseName(QualifiedName(varName, namespaceIndex));
			refrigeratorStatusMatrix->setDisplayName(LocalizedText(varName, _T("")));
			refrigeratorStatusMatrix->setAccessLevel(refrigeratorStatusMatrix->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

			std::vector<OTUInt32> arrayDimensions;
			arrayDimensions.clear();
			arrayDimensions.push_back(2);
			arrayDimensions.push_back(3);

			result = refrigeratorStatusMatrix->setValueRank((OTUInt32)arrayDimensions.size());
			if(!StatusCode::isGood(result))
			{
				Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setValueRank failed: "), getEnumStatusCodeString(result));
			}
			refrigeratorStatusMatrix->setArrayDimensions(arrayDimensions);
			if(!StatusCode::isGood(result))
			{
				Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("matrixVar->setArrayDimensions failed: "), getEnumStatusCodeString(result));
			}
			refrigeratorStatusMatrix->setDataType(m_refrigeratorStatusDataTypeId);

			refrigeratorStatusMatrix->init(NULL);
			addReference(Statics::ReferenceTypeId_HasComponent, refrigeratorStatusMatrix);

			std::vector<std::vector<Value> > fieldValuesMatrix;
			for(OTUInt32 i=0; i<6; i++)
			{
				std::vector<Value> simpleFieldValues;
				simpleFieldValues.push_back(Value(i%2 == 0));
				simpleFieldValues.push_back(Value((double)(i*10)));
				simpleFieldValues.push_back(Value((double)(i*20)));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
				simpleFieldValues.push_back(Value((OTInt32)((i%3)+1)));
#endif

				fieldValuesMatrix.push_back(simpleFieldValues);
			}

			result = structValMatrix.setStructuredValueMatrix(refStatusDataTypeDescription, arrayDimensions, fieldValuesMatrix);
			if(!StatusCode::isGood(result))
			{
				_tprintf(_T("Failed to set the structured value.\n"));
			}
			dval.setValue(structValMatrix);
			refrigeratorStatusMatrix->setDataValue(dval);

			std::vector<std::vector<Value> > fieldValuesMatrixReturned;
			std::vector<OTUInt32> arrayDimensionsReturned;
			result = structValMatrix.getStructuredValueMatrix(refStatusDataTypeDescription, arrayDimensionsReturned, fieldValuesMatrixReturned);
			if(!StatusCode::isGood(result))
			{
				_tprintf(_T("Failed to get the structured value.\n"));
			}

		}
		if(StatusCode::isGood(result))
		{
			Value structValArray;
			VariablePtr refrigeratorStatusArray;
			DataValue dval;
			tstring varName = _T("RefrigeratorStatusArray");

			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			refrigeratorStatusVar->setDataValue(dval);

			// second RefrigeratorStatus variable
			refrigeratorStatusArray = TestVariableForAuditEvents::create();
			refrigeratorStatusArray->setNodeId(NodeId(namespaceIndex, varName));
			refrigeratorStatusArray->setBrowseName(QualifiedName(varName, namespaceIndex));
			refrigeratorStatusArray->setDisplayName(LocalizedText(varName, _T("")));
			refrigeratorStatusArray->setAccessLevel(refrigeratorStatusArray->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

			std::vector<OTUInt32> arrayDimensions;
			arrayDimensions.clear();
			arrayDimensions.push_back(0);

			result = refrigeratorStatusArray->setValueRank((OTUInt32)arrayDimensions.size());
			if(!StatusCode::isGood(result))
			{
				Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("ArrayVar->setValueRank failed: "), getEnumStatusCodeString(result));
			}
			result = refrigeratorStatusArray->setArrayDimensions(arrayDimensions);
			if(!StatusCode::isGood(result))
			{
				Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("ArrayVar->setArrayDimensions failed: "), getEnumStatusCodeString(result));
			}
			refrigeratorStatusArray->setDataType(m_refrigeratorStatusDataTypeId);

			refrigeratorStatusArray->init(NULL);
			addReference(Statics::ReferenceTypeId_HasComponent, refrigeratorStatusArray);

			std::vector<std::vector<Value> > fieldValuesArray;
			for(OTUInt32 i=0; i<3; i++)
			{
				std::vector<Value> simpleFieldValues;
				simpleFieldValues.push_back(Value(i%2 == 0));
				simpleFieldValues.push_back(Value((double)(i*10)));
				simpleFieldValues.push_back(Value((double)(i*20)));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
				simpleFieldValues.push_back(Value((OTInt32)((i%3)+1)));
#endif

				fieldValuesArray.push_back(simpleFieldValues);
			}

			result = structValArray.setStructuredValueArray(refStatusDataTypeDescription, fieldValuesArray);
			if(!StatusCode::isGood(result))
			{
				_tprintf(_T("Failed to set the structured value.\n"));
			}
			dval.setValue(structValArray);
			refrigeratorStatusArray->setDataValue(dval);

			std::vector<std::vector<Value> > fieldValuesArrayReturned;
			std::vector<OTUInt32> arrayDimensionsReturned;
			result = structValArray.getStructuredValueArray(refStatusDataTypeDescription, fieldValuesArrayReturned);
			if(!StatusCode::isGood(result))
			{
				_tprintf(_T("Failed to get the structured value.\n"));
			}

		}
		if(StatusCode::isGood(result))
		{
			DataValue dval2;
			dval2.setValue(structVal2);
			dval2.setSourceTimestamp(DateTime::getUtcNow());
			dval2.setServerTimestamp(DateTime::getUtcNow());
			refrigeratorStatusVar2->setDataValue(dval2);

		}
	}
	// nested structure variables
	{
		tstring varName;
		Value structVal;

		{
			// OuterStructVar (scalar) variable
			varName = _T("OuterStructVar");
			VariablePtr outerStructVar;
			outerStructVar = TestVariableForAuditEvents::create();
			outerStructVar->setNodeId(NodeId(namespaceIndex, varName));
			outerStructVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			outerStructVar->setDisplayName(LocalizedText(varName, _T("")));
			outerStructVar->setAccessLevel(outerStructVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			outerStructVar->setValueRank(ValueRank_Scalar);
			outerStructVar->setDataType(m_outerStructureDataTypeId);

			outerStructVar->init(NULL);
			addReference(Statics::ReferenceTypeId_HasComponent, outerStructVar);

			// set field values
			DataValue dval;
			std::vector<Value> fieldValues;

			fieldValues.push_back(Value(_T("String member")));

			std::vector<Value> innerFieldValues;
			Value innerRefrigeratorStatusValue;
			innerFieldValues.push_back(Value(false));
			innerFieldValues.push_back(Value((double)123.5));
			innerFieldValues.push_back(Value((double)178.25));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			innerFieldValues.push_back(Value((OTInt32)2));
#endif
			innerRefrigeratorStatusValue.setValueArray(innerFieldValues);
			fieldValues.push_back(innerRefrigeratorStatusValue);

			fieldValues.push_back(Value((double)2.5));
			Value rangeValue;
			Range range(12.0, 20.0);
			rangeValue.setRange(&range);
			fieldValues.push_back(rangeValue);

			EnumStatusCode setResult = structVal.setStructuredValue(outerDataTypeDescription, fieldValues);
			if(StatusCode::isBad(setResult))
			{
				_tprintf(_T("Failed to set the structured value.\n"));
			}
			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			outerStructVar->setDataValue(dval);
		}

		{
			// OuterStructVar (array) variable
			varName = _T("OuterStructArrayVar");
			VariablePtr outerStructVar;
			outerStructVar = TestVariableForAuditEvents::create();
			outerStructVar->setNodeId(NodeId(namespaceIndex, varName));
			outerStructVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			outerStructVar->setDisplayName(LocalizedText(varName, _T("")));
			outerStructVar->setAccessLevel(outerStructVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			outerStructVar->setValueRank(1);
			std::vector<OTUInt32> arrayDim;
			arrayDim.push_back(3);
			outerStructVar->setArrayDimensions(arrayDim);
			outerStructVar->setDataType(m_outerStructureDataTypeId);

			outerStructVar->init(NULL);
			addReference(Statics::ReferenceTypeId_HasComponent, outerStructVar);

			// set field values
			DataValue dval;
			std::vector<std::vector<Value> > structValArray;

			for(int i=0; i<3; i++)
			{
				Value arrayElementStructVal;
				std::vector<Value> fieldValues;
				OTChar stringVal[20];
				_sntprintf(stringVal, 20, _T("String member %d"), i);

				fieldValues.push_back(Value(stringVal));

				std::vector<Value> innerFieldValues;
				Value innerRefrigeratorStatusValue;
				innerFieldValues.push_back(Value(false));
				innerFieldValues.push_back(Value((double)(100.0*i)+23.5));
				innerFieldValues.push_back(Value((double)(100.0*i)+78.25));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
				innerFieldValues.push_back(Value((OTInt32)2));
#endif
				innerRefrigeratorStatusValue.setValueArray(innerFieldValues);
				fieldValues.push_back(innerRefrigeratorStatusValue);

				fieldValues.push_back(Value((double)(10.0*i)+0.5));

				Value rangeValue;
				Range range(12.0 * (i+1), 20.0 * (i+1));
				rangeValue.setRange(&range);
				fieldValues.push_back(rangeValue);

				structValArray.push_back(fieldValues);
			}

			structVal.setStructuredValueArray(outerDataTypeDescription, structValArray);
			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			outerStructVar->setDataValue(dval);
		}

		{
			// OuterStructVar (matrix) variable
			varName = _T("OuterStructMatrixVar");
			VariablePtr outerStructVar;
			outerStructVar = TestVariableForAuditEvents::create();
			outerStructVar->setNodeId(NodeId(namespaceIndex, varName));
			outerStructVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			outerStructVar->setDisplayName(LocalizedText(varName, _T("")));
			outerStructVar->setAccessLevel(outerStructVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			std::vector<OTUInt32> variableArrayDimensions;
			variableArrayDimensions.push_back(0);
			variableArrayDimensions.push_back(0);

			outerStructVar->setValueRank(2);
			outerStructVar->setArrayDimensions(variableArrayDimensions);
			outerStructVar->setDataType(m_outerStructureDataTypeId);

			outerStructVar->init(NULL);
			addReference(Statics::ReferenceTypeId_HasComponent, outerStructVar);

			// set field values
			DataValue dval;
			std::vector<std::vector<Value> > structValArray;

			std::vector<OTUInt32> valueArrayDimensions;
			valueArrayDimensions.push_back(2);
			valueArrayDimensions.push_back(3);

			for(OTUInt32 i = 0; i < valueArrayDimensions[0]; i++)
				for(OTUInt32 j = 0; j < valueArrayDimensions[1]; j++)
			{
				Value arrayElementStructVal;
				std::vector<Value> fieldValues;
				OTChar stringVal[20];
				_sntprintf(stringVal, 20, _T("String member %u/%u"), i, j);

				fieldValues.push_back(Value(stringVal));

				std::vector<Value> innerFieldValues;
				Value innerRefrigeratorStatusValue;
				innerFieldValues.push_back(Value(false));	
				innerFieldValues.push_back(Value((double)(100.0*(i+1)*(j+1))+23.5));
				innerFieldValues.push_back(Value((double)(100.0*(i+1)*(j+1))+78.25));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
				innerFieldValues.push_back(Value((OTInt32)2));
#endif
				innerRefrigeratorStatusValue.setValueArray(innerFieldValues);
				fieldValues.push_back(innerRefrigeratorStatusValue);

				fieldValues.push_back(Value((double)(10.0*(i+1)*(j+1))+0.5));

				Value rangeValue;
				Range range(12.0 * (i+1)*(j+1), 20.0 * (i+1)*(j+1));
				rangeValue.setRange(&range);
				fieldValues.push_back(rangeValue);

				structValArray.push_back(fieldValues);
			}

			structVal.setStructuredValueMatrix(outerDataTypeDescription, valueArrayDimensions, structValArray);
			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			outerStructVar->setDataValue(dval);
		}
	}

	// create a optionset
	{
		tstring optionSetTypeName = _T("OptionSetType");
		NodeId optionSetDescriptionId(namespaceIndex, 536);
		NodeId optionSetValues(namespaceIndex, 537);
		m_optionSetDataTypeId = NodeId(namespaceIndex, 535);

		// We do not know whether it is mandatory to specify the bits as a sequence
		// It is not explicitly states in specification, but some clients may expect this
		OTUInt64 option1Bit = 0;
		OTUInt64 option2Bit = 1;
		OTUInt64 option3Bit = 2;
		OTUInt64 option5Bit = 4;

		EnumerationDataTypeDescriptionPtr optionSetDescription = EnumerationDataTypeDescription::create();
		std::vector<EnumField> enumerationValues;
		enumerationValues.push_back(EnumField());
		enumerationValues.back().setName(_T("Option1"));
		enumerationValues.back().setValue(option1Bit);
		enumerationValues.push_back(EnumField());
		enumerationValues.back().setName(_T("Option2"));
		enumerationValues.back().setValue(option2Bit);
		enumerationValues.push_back(EnumField());
		enumerationValues.back().setName(_T("Option3"));
		enumerationValues.back().setValue(option3Bit);
		enumerationValues.push_back(EnumField());
		enumerationValues.back().setName(_T("Option5"));
		enumerationValues.back().setValue(option5Bit);

		optionSetDescription->setEnumeratedValues(enumerationValues);
		// optionSetDescription->setTransportDataType(EnumDataTypeId_UInt16);
		
		DataTypePtr optionSetDataType = DataType::create();
		(void)optionSetDataType->setNodeId(m_optionSetDataTypeId);
		(void)optionSetDataType->setBrowseName(QualifiedName(optionSetTypeName, namespaceIndex));
		(void)optionSetDataType->setDisplayName(LocalizedText(optionSetTypeName, _T("")));
		result = optionSetDataType->setDataTypeDescription(optionSetDescription, optionSetDescriptionId, NodeId(), m_dictionaryId, optionSetValues);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the data type description.\n"));
		}

		if(StatusCode::isGood(result))
		{
			DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(Statics::DataTypeId_UInt16);
			(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, optionSetDataType);
			result = optionSetDataType->init(NULL);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to initialize the data type node.\n"));
			}
		}

		// create a variable
		tstring optionSetVarName = _T("OptionSetVar");
		VariablePtr optionSetVar = TestVariableForAuditEvents::create();
		optionSetVar->setNodeId(NodeId(namespaceIndex, optionSetVarName));
		optionSetVar->setBrowseName(QualifiedName(optionSetVarName, namespaceIndex));
		optionSetVar->setDisplayName(LocalizedText(optionSetVarName, _T("")));
		optionSetVar->setDataType(m_optionSetDataTypeId);
		optionSetVar->setAccessLevel(optionSetVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		optionSetVar->init(NULL);
		addReference(Statics::ReferenceTypeId_HasComponent, optionSetVar);

		// provide value (Option2 and Opiton5)
		DataValue dval;
		dval.setValue(Value((OTUInt16)((0x1 << option2Bit) | (0x1 << option5Bit))));
		optionSetVar->setDataValue(dval);
	}


	return result;
}

void StructuredDataTypeModule::endModule()
{
	ModuleFolder::endModule();
}

NodeId StructuredDataTypeModule::getDataTypeNodeId()
{
	return m_refrigeratorStatusDataTypeId;
}

void StructuredDataTypeModule::initRefrigeratorStateEnumeration(OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = EnumStatusCode_Good;
	m_enumRefrigeratorStateTypeId = NodeId(namespaceIndex, 451);	// same identifier as on .Net StructoAutomoldServer
	NodeId enumRefrigeratorEnumValuesId(namespaceIndex, 454);
	NodeId enumRefrigeratorStateDescriptionId(namespaceIndex, 457);
	NodeId parentDataTypeId(EnumNumericNodeId_Enumeration);

	NodeId refrigeratorStateId(namespaceIndex, 16);
	NodeId enumRefrigeratorValueAsTextId(namespaceIndex, 17);
	QualifiedName refrigeratorStateTypeBrowseName(_T("EnumRefrigeratorState"), namespaceIndex);
	LocalizedText refrigeratorStateTypeDisplayName(_T("EnumRefrigeratorState"), _T(""));
	OTInt32 refrigeratorStateValueRank = ValueRank_Scalar;

	DataTypePtr dataType;

	tstring typeName = _T("EnumRefrigeratorState");
	tstring refrigeratorStateStopName = _T("stop");
	tstring refrigeratorStateRunningName = _T("running");
	tstring refrigeratorStateFaultName = _T("fault");

	std::vector<EnumField> enumerationValues;
	EnumField enumValue1;
	enumValue1.setName(refrigeratorStateStopName);
	enumValue1.setValue(1);
	enumerationValues.push_back(enumValue1);

	EnumField enumValue2;
	enumValue2.setName(refrigeratorStateRunningName);
	enumValue2.setValue(2);
	enumerationValues.push_back(enumValue2);

	EnumField enumValue3;
	enumValue3.setName(refrigeratorStateFaultName);
	enumValue3.setValue(3);
	enumerationValues.push_back(enumValue3);

	EnumerationDataTypeDescriptionPtr enumerationDataTypeDescription = EnumerationDataTypeDescription::create();
	//Set Properties
	enumerationDataTypeDescription->setEnumeratedValues(enumerationValues);

	dataType = DataType::create();
	(void)dataType->setNodeId(m_enumRefrigeratorStateTypeId);
	(void)dataType->setBrowseName(QualifiedName(typeName, namespaceIndex));
	(void)dataType->setDisplayName(LocalizedText(typeName, _T("")));
	result = dataType->setDataTypeDescription(enumerationDataTypeDescription, enumRefrigeratorStateDescriptionId, NodeId(), m_dictionaryId, enumRefrigeratorEnumValuesId);

	if(StatusCode::isGood(result))
	{
		DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
		(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);
		result = dataType->init(NULL);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to initialize the data type node.\n"));
		}
	}


	DataValue dval;
#if TB5_ALARMS
	//////////////////////
	//EventType
	//////////////////////
	// create an event type for the duplicate node, event if it's not referenced by this folder
	ObjectTypePtr refrigeratorStatusEventType = Server::ObjectType::create();
	tstring eventTypeName = _T("RefrigeratorStateEventType");
	m_enumRefrigeratorStateEventTypeId = NodeId(namespaceIndex, eventTypeName);
	refrigeratorStatusEventType->setNodeId(m_enumRefrigeratorStateEventTypeId);
	refrigeratorStatusEventType->setBrowseName(QualifiedName(eventTypeName, namespaceIndex));
	refrigeratorStatusEventType->setDisplayName(LocalizedText(eventTypeName, _T("")));
	refrigeratorStatusEventType->setDescription(LocalizedText(_T("Event containing the enumeration data type RefrigeratorStatus."), _T("en")));
	Server::AddressSpaceRoot::instance()->getNode(Statics::ObjectTypeId_BaseEventType)->addReference(Statics::ReferenceTypeId_HasSubtype, refrigeratorStatusEventType);
	refrigeratorStatusEventType->init(NULL);
	// add the RefrigeratorStatus property
	VariablePtr refrigeratorStatusProperty = TestVariableForAuditEvents::create();
	tstring propertyName = _T("RefrigeratorStatus");
	refrigeratorStatusProperty->setNodeId(NodeId(namespaceIndex, eventTypeName + _T("_") + propertyName));
	refrigeratorStatusProperty->setBrowseName(QualifiedName(propertyName, namespaceIndex));
	refrigeratorStatusProperty->setDisplayName(LocalizedText(propertyName, _T("")));
	refrigeratorStatusProperty->setDataType(m_enumRefrigeratorStateTypeId);
	dval.setValue(Value((OTInt32)1));
	refrigeratorStatusProperty->setDataValue(dval);
	refrigeratorStatusProperty->setAccessLevel(EnumAccessLevel_CurrentRead);
	NodeId mandatoryModellingRuleId(EnumNumericNodeId_ModellingRule_Mandatory);
	refrigeratorStatusProperty->addReference(Statics::ReferenceTypeId_HasModellingRule, AddressSpaceRoot::instance()->getNode(&mandatoryModellingRuleId));
	refrigeratorStatusEventType->addReference(Statics::ReferenceTypeId_HasProperty, refrigeratorStatusProperty);
	refrigeratorStatusProperty->init(Statics::VariableTypeId_PropertyType);
#endif


	//////////////////////
	//Variable
	//////////////////////
	Value enumVal;
	//VariablePtr refrigeratorStateVar;
	MyRefrigeratorStateVariablePtr refrigeratorStateVar;

	QualifiedName refrigeratorStateBrowseName(_T("State"), namespaceIndex);
	LocalizedText refrigeratorStateDisplayName(_T("State"), _T(""));

	// RefrigeratorStatus variable
	refrigeratorStateVar = MyRefrigeratorStateVariable::create();
	refrigeratorStateVar->setNodeId(refrigeratorStateId);
	refrigeratorStateVar->setBrowseName(refrigeratorStateBrowseName);
	refrigeratorStateVar->setDisplayName(refrigeratorStateDisplayName);
	refrigeratorStateVar->setAccessLevel(refrigeratorStateVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	refrigeratorStateVar->setValueRank(refrigeratorStateValueRank);
	refrigeratorStateVar->setDataType(m_enumRefrigeratorStateTypeId);
	dval.clear();
	dval.setValue(Value((OTInt32)1));
	refrigeratorStateVar->setDataValue(dval);
	refrigeratorStateVar->provideValueAsText(enumRefrigeratorValueAsTextId);
	// custom methods
	refrigeratorStateVar->setReportingNode(*parentFolder->getNodeId());
	refrigeratorStateVar->setEventTypeToReport(m_enumRefrigeratorStateEventTypeId);

	refrigeratorStateVar->init(NULL);
	parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, refrigeratorStateVar);
}

EnumStatusCode StructuredDataTypeModule::initStructureWithOptional(OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = EnumStatusCode_Good;

	// defined structure with optional fields (incl. variable)
	{
		m_structureDataWithOptionalsTypeId = NodeId(namespaceIndex, 600);
		NodeId parentDataTypeId(EnumNumericNodeId_Structure);
		NodeId dataTypeDescriptionId = NodeId(namespaceIndex, 601);
		NodeId encodingId = NodeId(namespaceIndex, 602);

		DataTypePtr dataType;

		tstring typeName = _T("StructureWithOptional");

		std::vector<StructureField> fields;
		fields.push_back(StructureField());
		fields.back().setName(_T("MandatoryDouble"));
		fields.back().setDataType(*Statics::DataTypeId_Double);

		fields.push_back(StructureField());
		fields.back().setName(_T("OptionalString"));
		fields.back().setDataType(*Statics::DataTypeId_String);
		fields.back().setIsOptional(true);

		fields.push_back(StructureField());
		fields.back().setName(_T("MandatoryInt16"));
		fields.back().setDataType(*Statics::DataTypeId_Int16);

		fields.push_back(StructureField());
		fields.back().setName(_T("OptionalUInt32"));
		fields.back().setDataType(*Statics::DataTypeId_UInt32);
		fields.back().setIsOptional(true);

		StructuredDataTypeDescriptionPtr dataTypeDescription = StructuredDataTypeDescription::create();
		result = dataTypeDescription->setFields(fields);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the field descriptions\n"));
		}

		dataType = DataType::create();
		(void)dataType->setNodeId(m_structureDataWithOptionalsTypeId);
		(void)dataType->setBrowseName(QualifiedName(typeName, namespaceIndex));
		(void)dataType->setDisplayName(LocalizedText(typeName, _T("")));
		// empty description
		// default write mask
		result = dataType->setDataTypeDescription(dataTypeDescription, dataTypeDescriptionId, encodingId, m_dictionaryId);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the data type description.\n"));
		}

		if(StatusCode::isGood(result))
		{
			DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
			(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);
			result = dataType->init(NULL);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to initialize the data type node.\n"));
			}
		}


		DataValue dval;
		//////////////////////
		//Variables
		//////////////////////
		// scalar
		{
			Value structVal;
			VariablePtr structWithOptionalVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructWithOptional");

			structWithOptionalVar->setNodeId(NodeId(namespaceIndex, varName));
			structWithOptionalVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structWithOptionalVar->setDisplayName(LocalizedText(varName, _T("en")));
			structWithOptionalVar->setAccessLevel(structWithOptionalVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			structWithOptionalVar->setValueRank(ValueRank_Scalar);
			structWithOptionalVar->setDataType(m_structureDataWithOptionalsTypeId);

			// provide mandatory fields only
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues[0].setDouble(123.45);
			fieldValues[2].setInt16(123);
			result = structVal.setStructuredValue(dataTypeDescription, fieldValues);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to set structured value.\n"));
			}

			dval.setValue(structVal);
			structWithOptionalVar->setDataValue(dval);

			structWithOptionalVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structWithOptionalVar);
		}
		// array
		{
			Value structVal;
			VariablePtr structWithOptionalVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructWithOptionalArray");

			structWithOptionalVar->setNodeId(NodeId(namespaceIndex, varName));
			structWithOptionalVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structWithOptionalVar->setDisplayName(LocalizedText(varName, _T("en")));
			structWithOptionalVar->setAccessLevel(structWithOptionalVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			structWithOptionalVar->setValueRank(1); // one-dimensional array
			std::vector<OTUInt32> arrayDimansions;
			arrayDimansions.push_back(2); // exact 2 elements
			structWithOptionalVar->setArrayDimensions(arrayDimansions);
			structWithOptionalVar->setDataType(m_structureDataWithOptionalsTypeId);

			std::vector<std::vector<Value> > arrayValues;
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());

			// value of element 1
			fieldValues[0].setDouble(3.14);
			fieldValues[1].setString(_T("StringVal[0]"));
			fieldValues[2].setInt16(16);
			fieldValues[3].setUInt32(45);
			arrayValues.push_back(fieldValues);

			// value of element 2
			fieldValues[0].setDouble(17.23);
			fieldValues[1].setString(_T("StringVal[1]"));
			fieldValues[2].setInt16(1166);
			fieldValues[3].setUInt32(456);
			arrayValues.push_back(fieldValues);

			result = structVal.setStructuredValueArray(dataTypeDescription, arrayValues);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to set structured value.\n"));
			}

			dval.setValue(structVal);
			structWithOptionalVar->setDataValue(dval);

			structWithOptionalVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structWithOptionalVar);
		}
		// matrix
		{
			Value structVal;
			VariablePtr structWithOptionalVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructWithOptionalMatrix");

			structWithOptionalVar->setNodeId(NodeId(namespaceIndex, varName));
			structWithOptionalVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structWithOptionalVar->setDisplayName(LocalizedText(varName, _T("en")));
			structWithOptionalVar->setAccessLevel(structWithOptionalVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			structWithOptionalVar->setValueRank(2); // two-dimensional matrix
			std::vector<OTUInt32> arrayDimansions;
			arrayDimansions.push_back(2); // exact 2 elements
			arrayDimansions.push_back(2); // exact 2 elements
			structWithOptionalVar->setArrayDimensions(arrayDimansions);
			structWithOptionalVar->setDataType(m_structureDataWithOptionalsTypeId);

			std::vector<std::vector<Value> > arrayValues;
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());

			// value of element 1
			fieldValues[0].setDouble(1.11);
			fieldValues[1].setString(_T("StringVal[0][0]"));
			fieldValues[2].setInt16(11);
			fieldValues[3].setUInt32(1111);
			arrayValues.push_back(fieldValues);

			// value of element 2
			fieldValues[0].setDouble(1.12);
			fieldValues[1].setString(_T("StringVal[0][1]"));
			fieldValues[2].setInt16(22);
			fieldValues[3].setUInt32(2222);
			arrayValues.push_back(fieldValues);

			// value of element 1
			fieldValues[0].setDouble(1.21);
			fieldValues[1].setString(_T("StringVal[1][0]"));
			fieldValues[2].setInt16(21);
			fieldValues[3].setUInt32(2121);
			arrayValues.push_back(fieldValues);

			// value of element 2
			fieldValues[0].setDouble(1.22);
			fieldValues[1].setString(_T("StringVal[1][1]"));
			fieldValues[2].setInt16(22);
			fieldValues[3].setUInt32(2222);
			arrayValues.push_back(fieldValues);

			result = structVal.setStructuredValueMatrix(dataTypeDescription, arrayDimansions, arrayValues);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to set structured value.\n"));
			}

			dval.setValue(structVal);
			structWithOptionalVar->setDataValue(dval);

			structWithOptionalVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structWithOptionalVar);
		}
	}

	// defined structure with optional fields, including nested struct (incl. variable)
	{
		m_structureDataWithOptionalsNestedTypeId = NodeId(namespaceIndex, 610);
		NodeId parentDataTypeId(EnumNumericNodeId_Structure);
		NodeId dataTypeDescriptionId = NodeId(namespaceIndex, 611);
		NodeId encodingId = NodeId(namespaceIndex, 612);

		DataTypePtr dataType;

		tstring typeName = _T("StructureWithNestedOptional");

		std::vector<StructureField> fields;
		fields.push_back(StructureField());
		fields.back().setName(_T("MandatoryInt32"));
		fields.back().setDataType(*Statics::DataTypeId_Int32);

		fields.push_back(StructureField());
		fields.back().setName(_T("OptionalNested"));
		fields.back().setDataType(m_structureDataWithOptionalsTypeId);
		fields.back().setIsOptional(true);

		fields.push_back(StructureField());
		fields.back().setName(_T("OptionalDouble"));
		fields.back().setDataType(*Statics::DataTypeId_Double);
		fields.back().setIsOptional(true);

		StructuredDataTypeDescriptionPtr dataTypeDescription = StructuredDataTypeDescription::create();
		result = dataTypeDescription->setFields(fields);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the field descriptions\n"));
		}

		dataType = DataType::create();
		(void)dataType->setNodeId(m_structureDataWithOptionalsNestedTypeId);
		(void)dataType->setBrowseName(QualifiedName(typeName, namespaceIndex));
		(void)dataType->setDisplayName(LocalizedText(typeName, _T("")));
		// empty description
		// default write mask
		result = dataType->setDataTypeDescription(dataTypeDescription, dataTypeDescriptionId, encodingId, m_dictionaryId);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the data type description.\n"));
		}

		if(StatusCode::isGood(result))
		{
			DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
			(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);
			result = dataType->init(NULL);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to initialize the data type node.\n"));
			}
		}


		DataValue dval;
		//////////////////////
		//Variables
		//////////////////////
		// scalar
		{
			Value structVal;
			VariablePtr structWithOptionalNestedVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructWithNestedOptional");

			structWithOptionalNestedVar->setNodeId(NodeId(namespaceIndex, varName));
			structWithOptionalNestedVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structWithOptionalNestedVar->setDisplayName(LocalizedText(varName, _T("en")));
			structWithOptionalNestedVar->setAccessLevel(structWithOptionalNestedVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			structWithOptionalNestedVar->setValueRank(ValueRank_Scalar);
			structWithOptionalNestedVar->setDataType(m_structureDataWithOptionalsNestedTypeId);

			// provide mandatory fields only in nested struct
			std::vector<Value> innerFieldValues;
			innerFieldValues.push_back(Value());
			innerFieldValues.push_back(Value());
			innerFieldValues.push_back(Value());
			innerFieldValues.push_back(Value());
			innerFieldValues[0].setDouble(123.45);
			innerFieldValues[2].setInt16(123);

			// provide all fields, including optional
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues[0].setInt32(32);
			fieldValues[1].setValueArray(innerFieldValues);
			fieldValues[2].setDouble(123.45);
			result = structVal.setStructuredValue(dataTypeDescription, fieldValues);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to set structured value.\n"));
			}

			dval.setValue(structVal);
			structWithOptionalNestedVar->setDataValue(dval);

			structWithOptionalNestedVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structWithOptionalNestedVar);
		}
		// array
		{
			Value structVal;
			VariablePtr structWithOptionalNestedVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructWithNestedOptionalArray");

			structWithOptionalNestedVar->setNodeId(NodeId(namespaceIndex, varName));
			structWithOptionalNestedVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structWithOptionalNestedVar->setDisplayName(LocalizedText(varName, _T("en")));
			structWithOptionalNestedVar->setAccessLevel(structWithOptionalNestedVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			std::vector<OTUInt32> arrayDimansions;
			arrayDimansions.push_back(2); // exact 2 elements
			structWithOptionalNestedVar->setValueRank(1);
			structWithOptionalNestedVar->setDataType(m_structureDataWithOptionalsNestedTypeId);

			std::vector<std::vector<Value> > arrayValues;

			std::vector<Value> innerFieldValues;
			innerFieldValues.push_back(Value());
			innerFieldValues.push_back(Value());
			innerFieldValues.push_back(Value());
			innerFieldValues.push_back(Value());

			std::vector<Value> fieldValues;
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());

			// first element
			innerFieldValues[0].setDouble(1.11);
			innerFieldValues[1].setString(_T("String[0]"));
			innerFieldValues[2].setInt16(11);
			fieldValues[0].setInt32(511);
			fieldValues[1].setValueArray(innerFieldValues);
			fieldValues[2].setDouble(511.11);
			arrayValues.push_back(fieldValues);

			// second element
			innerFieldValues[0].setDouble(2.22);
			innerFieldValues[1].clear(); // do not provide this optional
			innerFieldValues[2].setInt16(22);
			fieldValues[0].setInt32(522);
			fieldValues[1].setValueArray(innerFieldValues);
			fieldValues[2].setDouble(123.45);
			arrayValues.push_back(fieldValues);

			result = structVal.setStructuredValueArray(dataTypeDescription, arrayValues);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to set structured value.\n"));
			}

			dval.setValue(structVal);
			structWithOptionalNestedVar->setDataValue(dval);

			structWithOptionalNestedVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structWithOptionalNestedVar);
		}
		// matrix
		{
			Value structVal;
			VariablePtr structWithOptionalNestedVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructWithNestedOptionalMatrix");

			structWithOptionalNestedVar->setNodeId(NodeId(namespaceIndex, varName));
			structWithOptionalNestedVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structWithOptionalNestedVar->setDisplayName(LocalizedText(varName, _T("en")));
			structWithOptionalNestedVar->setAccessLevel(structWithOptionalNestedVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			std::vector<OTUInt32> arrayDimansions;
			arrayDimansions.push_back(2); // exact 2*2 elements
			arrayDimansions.push_back(2); // exact 2*2 elements
			structWithOptionalNestedVar->setValueRank(2);
			structWithOptionalNestedVar->setDataType(m_structureDataWithOptionalsNestedTypeId);

			std::vector<std::vector<Value> > arrayValues;

			std::vector<Value> innerFieldValues;
			innerFieldValues.push_back(Value());
			innerFieldValues.push_back(Value());
			innerFieldValues.push_back(Value());
			innerFieldValues.push_back(Value());

			std::vector<Value> fieldValues;
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());
			fieldValues.push_back(Value());

			// element [0][0]
			innerFieldValues[0].setDouble(111.11);
			innerFieldValues[1].setString(_T("String[0][0]"));
			innerFieldValues[2].setInt16(111);
			fieldValues[0].setInt32(5111);
			fieldValues[1].setValueArray(innerFieldValues);
			fieldValues[2].setDouble(5111.11);
			arrayValues.push_back(fieldValues);

			// element [0][1]
			innerFieldValues[0].setDouble(12.1212);
			innerFieldValues[1].clear(); // do not provide this optional
			innerFieldValues[2].setInt16(1212);
			fieldValues[0].setInt32(51212);
			fieldValues[1].setValueArray(innerFieldValues);
			fieldValues[2].setDouble(5112.12);
			arrayValues.push_back(fieldValues);

			// element [1][0]
			innerFieldValues[0].setDouble(21.21);
			innerFieldValues[1].clear(); // do not provide this optional
			innerFieldValues[2].setInt16(21);
			fieldValues[0].setInt32(5221);
			fieldValues[1].setValueArray(innerFieldValues);
			fieldValues[2].setDouble(5121.21);
			arrayValues.push_back(fieldValues);

			// element [1][1]
			innerFieldValues[0].setDouble(22.2222);
			innerFieldValues[1].setString(_T("String[1][1]"));
			innerFieldValues[2].setInt16(2222);
			fieldValues[0].setInt32(52222);
			fieldValues[1].setValueArray(innerFieldValues);
			fieldValues[2].setDouble(5122.22);
			arrayValues.push_back(fieldValues);

			result = structVal.setStructuredValueMatrix(dataTypeDescription, arrayDimansions, arrayValues);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to set structured value.\n"));
			}

			dval.setValue(structVal);
			structWithOptionalNestedVar->setDataValue(dval);

			structWithOptionalNestedVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structWithOptionalNestedVar);
		}
	}
	return result;
}

EnumStatusCode StructuredDataTypeModule::initChildStructure(OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = EnumStatusCode_Good;

	// defined structure derived from RefrigeratorStatusType (incl. variable)
	{
		m_childStructureDataTypeId = NodeId(namespaceIndex, 620);
		NodeId dataTypeDescriptionId = NodeId(namespaceIndex, 621);
		NodeId encodingId = NodeId(namespaceIndex, 622);

		DataTypePtr dataType;

		tstring typeName = _T("ExtendedStatusType");

		std::vector<StructureField> fields;
		fields.push_back(StructureField());
		fields.back().setName(_T("ExtraString"));
		fields.back().setDataType(*Statics::DataTypeId_String);

		fields.push_back(StructureField());
		fields.back().setName(_T("ExtraInt16"));
		fields.back().setDataType(*Statics::DataTypeId_Int16);

		StructuredDataTypeDescriptionPtr dataTypeDescription = StructuredDataTypeDescription::create();
		result = dataTypeDescription->setFields(fields);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the field descriptions\n"));
		}

		dataType = DataType::create();
		(void)dataType->setNodeId(m_childStructureDataTypeId);
		(void)dataType->setBrowseName(QualifiedName(typeName, namespaceIndex));
		(void)dataType->setDisplayName(LocalizedText(typeName, _T("")));
		// empty description
		// default write mask

		if(StatusCode::isGood(result))
		{
			DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&m_refrigeratorStatusDataTypeId);
			(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);
			result = dataType->init(NULL);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to initialize the data type node.\n"));
			}
		}

		// Note: this has to be called after the parent-child relationship is established (HasSubtype reference)
		result = dataType->setDataTypeDescription(dataTypeDescription, dataTypeDescriptionId, encodingId, m_dictionaryId);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the data type description.\n"));
		}


		DataValue dval;
		//////////////////////
		//Variables
		//////////////////////
		// scalar
		{
			Value structVal;
			VariablePtr structExtendedVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructExtended");

			structExtendedVar->setNodeId(NodeId(namespaceIndex, varName));
			structExtendedVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structExtendedVar->setDisplayName(LocalizedText(varName, _T("en")));
			structExtendedVar->setAccessLevel(structExtendedVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			structExtendedVar->setValueRank(ValueRank_Scalar);
			structExtendedVar->setDataType(m_childStructureDataTypeId);

			// provide field values
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value(true));
			fieldValues.push_back(Value((double)110.0));
			fieldValues.push_back(Value((double)70.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			fieldValues.push_back(Value((OTInt32)2));
#endif
			fieldValues.push_back(Value(_T("Extended information")));
			fieldValues.push_back(Value((OTInt16)20));
			{
				EnumStatusCode setResult = structVal.setStructuredValue(dataTypeDescription, fieldValues);
				if(StatusCode::isBad(setResult))
				{
					_tprintf(_T("Failed to set the structured value.\n"));
				}
			}

			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			structExtendedVar->setDataValue(dval);

			structExtendedVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structExtendedVar);
		}

		// array
		{
			Value structVal;
			VariablePtr structExtendedVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructExtendedArray");

			structExtendedVar->setNodeId(NodeId(namespaceIndex, varName));
			structExtendedVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structExtendedVar->setDisplayName(LocalizedText(varName, _T("en")));
			structExtendedVar->setAccessLevel(structExtendedVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			structExtendedVar->setValueRank(ValueRank_Scalar);
			structExtendedVar->setValueRank(1); // one-dimensional array
			std::vector<OTUInt32> arrayDimensions;
			arrayDimensions.push_back(2); // exact 2 elements
			structExtendedVar->setArrayDimensions(arrayDimensions);
			structExtendedVar->setDataType(m_childStructureDataTypeId);

			// provide field values
			std::vector<std::vector<Value> > arrayValues;
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value(false));
			fieldValues.push_back(Value((double)20.0));
			fieldValues.push_back(Value((double)15.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			fieldValues.push_back(Value((OTInt32)1));
#endif
			fieldValues.push_back(Value(_T("Index 0 extended information")));
			fieldValues.push_back(Value((OTInt16)10));
			arrayValues.push_back(fieldValues);

			fieldValues.clear();
			fieldValues.push_back(Value(true));
			fieldValues.push_back(Value((double)30.0));
			fieldValues.push_back(Value((double)25.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			fieldValues.push_back(Value((OTInt32)2));
#endif
			fieldValues.push_back(Value(_T("Index 1 extended information")));
			fieldValues.push_back(Value((OTInt16)20));
			arrayValues.push_back(fieldValues);

			{
				EnumStatusCode setResult = structVal.setStructuredValueArray(dataTypeDescription, arrayValues);
				if(StatusCode::isBad(setResult))
				{
					_tprintf(_T("Failed to set the structured value array.\n"));
				}
			}

			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			structExtendedVar->setDataValue(dval);

			structExtendedVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structExtendedVar);
		}

		// matrix
		{
			Value structVal;
			VariablePtr structExtendedVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructExtendedMatrix");

			structExtendedVar->setNodeId(NodeId(namespaceIndex, varName));
			structExtendedVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structExtendedVar->setDisplayName(LocalizedText(varName, _T("en")));
			structExtendedVar->setAccessLevel(structExtendedVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			structExtendedVar->setValueRank(ValueRank_Scalar);
			structExtendedVar->setValueRank(1); // one-dimensional array
			std::vector<OTUInt32> arrayDimensions;
			arrayDimensions.push_back(2); // exact 2 elements
			arrayDimensions.push_back(2); // exact 2 elements
			structExtendedVar->setArrayDimensions(arrayDimensions);
			structExtendedVar->setDataType(m_childStructureDataTypeId);

			// provide field values
			std::vector<std::vector<Value> > arrayValues;
			std::vector<Value> fieldValues;

			// field 1
			fieldValues.push_back(Value(false));
			fieldValues.push_back(Value((double)20.0));
			fieldValues.push_back(Value((double)15.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			fieldValues.push_back(Value((OTInt32)1));
#endif
			fieldValues.push_back(Value(_T("StringVal[0][0]")));
			fieldValues.push_back(Value((OTInt16)10));
			arrayValues.push_back(fieldValues);

			// field 2
			fieldValues.clear();
			fieldValues.push_back(Value(true));
			fieldValues.push_back(Value((double)30.0));
			fieldValues.push_back(Value((double)25.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			fieldValues.push_back(Value((OTInt32)2));
#endif
			fieldValues.push_back(Value(_T("StringVal[0][1]")));
			fieldValues.push_back(Value((OTInt16)20));
			arrayValues.push_back(fieldValues);

			// field 3
			fieldValues.clear();
			fieldValues.push_back(Value(false));
			fieldValues.push_back(Value((double)20.0));
			fieldValues.push_back(Value((double)15.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			fieldValues.push_back(Value((OTInt32)1));
#endif
			fieldValues.push_back(Value(_T("StringVal[1][0]")));
			fieldValues.push_back(Value((OTInt16)10));
			arrayValues.push_back(fieldValues);

			// field 4
			fieldValues.clear();
			fieldValues.push_back(Value(true));
			fieldValues.push_back(Value((double)30.0));
			fieldValues.push_back(Value((double)25.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			fieldValues.push_back(Value((OTInt32)2));
#endif
			fieldValues.push_back(Value(_T("StringVal[1][1]")));
			fieldValues.push_back(Value((OTInt16)20));
			arrayValues.push_back(fieldValues);

			{
				EnumStatusCode setResult = structVal.setStructuredValueMatrix(dataTypeDescription, arrayDimensions, arrayValues);
				if(StatusCode::isBad(setResult))
				{
					_tprintf(_T("Failed to set the structured value matrix.\n"));
				}
			}

			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			structExtendedVar->setDataValue(dval);

			structExtendedVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structExtendedVar);
		}
	}

	// defined nested extended structure (incl. variable)
	{
		m_childStructureNestedDataTypeId = NodeId(namespaceIndex, 630);
		NodeId parentDataTypeId(EnumNumericNodeId_Structure);
		NodeId dataTypeDescriptionId = NodeId(namespaceIndex, 631);
		NodeId encodingId = NodeId(namespaceIndex, 632);

		DataTypePtr dataType;

		tstring typeName = _T("NestedExtendedStatusType");

		std::vector<StructureField> fields;
		fields.push_back(StructureField());
		fields.back().setName(_T("ParentString"));
		fields.back().setDataType(*Statics::DataTypeId_String);

		fields.push_back(StructureField());
		fields.back().setName(_T("ExtendedNested"));
		fields.back().setDataType(m_childStructureDataTypeId);

		StructuredDataTypeDescriptionPtr dataTypeDescription = StructuredDataTypeDescription::create();
		result = dataTypeDescription->setFields(fields);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the field descriptions\n"));
		}

		dataType = DataType::create();
		(void)dataType->setNodeId(m_childStructureNestedDataTypeId);
		(void)dataType->setBrowseName(QualifiedName(typeName, namespaceIndex));
		(void)dataType->setDisplayName(LocalizedText(typeName, _T("")));
		// empty description
		// default write mask

		result = dataType->setDataTypeDescription(dataTypeDescription, dataTypeDescriptionId, encodingId, m_dictionaryId);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the data type description.\n"));
		}

		if(StatusCode::isGood(result))
		{
			DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
			(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);
			result = dataType->init(NULL);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to initialize the data type node.\n"));
			}
		}


		DataValue dval;
		//////////////////////
		//Variables
		//////////////////////
		// scalar
		{
			Value structVal;
			VariablePtr structExtendedNestedVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructExtendedNested");

			structExtendedNestedVar->setNodeId(NodeId(namespaceIndex, varName));
			structExtendedNestedVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structExtendedNestedVar->setDisplayName(LocalizedText(varName, _T("en")));
			structExtendedNestedVar->setAccessLevel(structExtendedNestedVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			structExtendedNestedVar->setValueRank(ValueRank_Scalar);
			structExtendedNestedVar->setDataType(m_childStructureNestedDataTypeId);

			// inner field values
			std::vector<Value> innerFieldValues;
			innerFieldValues.push_back(Value(true));
			innerFieldValues.push_back(Value((double)110.0));
			innerFieldValues.push_back(Value((double)70.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			innerFieldValues.push_back(Value((OTInt32)2));
#endif
			innerFieldValues.push_back(Value(_T("Extended information")));
			innerFieldValues.push_back(Value((OTInt16)20));

			// provide field values
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value(_T("Parent string value")));
			fieldValues.push_back(Value());
			fieldValues.back().setValueArray(innerFieldValues);

			{
				EnumStatusCode setResult = structVal.setStructuredValue(dataTypeDescription, fieldValues);
				if(StatusCode::isBad(setResult))
				{
					_tprintf(_T("Failed to set the structured value.\n"));
				}
			}
			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			structExtendedNestedVar->setDataValue(dval);

			structExtendedNestedVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structExtendedNestedVar);
		}

		// array
		{
			Value structVal;
			VariablePtr structExtendedNestedVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructExtendedNestedArray");

			structExtendedNestedVar->setNodeId(NodeId(namespaceIndex, varName));
			structExtendedNestedVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structExtendedNestedVar->setDisplayName(LocalizedText(varName, _T("en")));
			structExtendedNestedVar->setAccessLevel(structExtendedNestedVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			std::vector<OTUInt32> arrayDimensions;
			arrayDimensions.push_back(2); // exact 2 elements
			structExtendedNestedVar->setValueRank(1);
			structExtendedNestedVar->setDataType(m_childStructureNestedDataTypeId);

			std::vector<std::vector<Value> > arrayValues;

			// inner field values (1)
			std::vector<Value> innerFieldValues;
			innerFieldValues.push_back(Value(true));
			innerFieldValues.push_back(Value((double)110.0));
			innerFieldValues.push_back(Value((double)70.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			innerFieldValues.push_back(Value((OTInt32)2));
#endif
			innerFieldValues.push_back(Value(_T("Extended information 0")));
			innerFieldValues.push_back(Value((OTInt16)10));

			// provide field values (1)
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value(_T("parent[0]")));
			fieldValues.push_back(Value());
			fieldValues.back().setValueArray(innerFieldValues);
			arrayValues.push_back(fieldValues);

			// inner field values (2)
			innerFieldValues.clear();
			innerFieldValues.push_back(Value(false));
			innerFieldValues.push_back(Value((double)50.0));
			innerFieldValues.push_back(Value((double)25.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			innerFieldValues.push_back(Value((OTInt32)2));
#endif
			innerFieldValues.push_back(Value(_T("Extended information 1")));
			innerFieldValues.push_back(Value((OTInt16)20));

			// provide field values (2)
			fieldValues.clear();
			fieldValues.push_back(Value(_T("parent[1]")));
			fieldValues.push_back(Value());
			fieldValues.back().setValueArray(innerFieldValues);
			arrayValues.push_back(fieldValues);

			{
				EnumStatusCode setResult = structVal.setStructuredValueArray(dataTypeDescription, arrayValues);
				if(StatusCode::isBad(setResult))
				{
					_tprintf(_T("Failed to set the structured value array.\n"));
				}
			}
			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			structExtendedNestedVar->setDataValue(dval);

			structExtendedNestedVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structExtendedNestedVar);
		}

		// matrix
		{
			Value structVal;
			VariablePtr structExtendedNestedVar = TestVariableForAuditEvents::create();
			tstring varName = _T("StructExtendedNestedMatrix");

			structExtendedNestedVar->setNodeId(NodeId(namespaceIndex, varName));
			structExtendedNestedVar->setBrowseName(QualifiedName(varName, namespaceIndex));
			structExtendedNestedVar->setDisplayName(LocalizedText(varName, _T("en")));
			structExtendedNestedVar->setAccessLevel(structExtendedNestedVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			std::vector<OTUInt32> arrayDimensions;
			arrayDimensions.push_back(2); // exact 2 elements
			arrayDimensions.push_back(2); // exact 2 elements
			structExtendedNestedVar->setValueRank(2);
			structExtendedNestedVar->setDataType(m_childStructureNestedDataTypeId);

			std::vector<std::vector<Value> > arrayValues;

			// inner field values [0][0]
			std::vector<Value> innerFieldValues;
			innerFieldValues.push_back(Value(true));
			innerFieldValues.push_back(Value((double)100.0));
			innerFieldValues.push_back(Value((double)10.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			innerFieldValues.push_back(Value((OTInt32)1));
#endif
			innerFieldValues.push_back(Value(_T("nested[0][0]")));
			innerFieldValues.push_back(Value((OTInt16)10));

			// provide field values [0][0]
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value(_T("parent[0][0]")));
			fieldValues.push_back(Value());
			fieldValues.back().setValueArray(innerFieldValues);
			arrayValues.push_back(fieldValues);

			// inner field values [0][1]
			innerFieldValues.clear();
			innerFieldValues.push_back(Value(false));
			innerFieldValues.push_back(Value((double)200.0));
			innerFieldValues.push_back(Value((double)20.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			innerFieldValues.push_back(Value((OTInt32)1));
#endif
			innerFieldValues.push_back(Value(_T("nested[0][1]")));
			innerFieldValues.push_back(Value((OTInt16)20));

			// provide field values [0][1]
			fieldValues.clear();
			fieldValues.push_back(Value(_T("parent[0][1]")));
			fieldValues.push_back(Value());
			fieldValues.back().setValueArray(innerFieldValues);
			arrayValues.push_back(fieldValues);

			// inner field values [1][0]
			innerFieldValues.clear();
			innerFieldValues.push_back(Value(false));
			innerFieldValues.push_back(Value((double)300.0));
			innerFieldValues.push_back(Value((double)30.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			innerFieldValues.push_back(Value((OTInt32)1));
#endif
			innerFieldValues.push_back(Value(_T("nested[1][0]")));
			innerFieldValues.push_back(Value((OTInt16)30));

			// provide field values [0][1]
			fieldValues.clear();
			fieldValues.push_back(Value(_T("parent[1][0]")));
			fieldValues.push_back(Value());
			fieldValues.back().setValueArray(innerFieldValues);
			arrayValues.push_back(fieldValues);

			// inner field values [1][1]
			innerFieldValues.clear();
			innerFieldValues.push_back(Value(false));
			innerFieldValues.push_back(Value((double)400.0));
			innerFieldValues.push_back(Value((double)40.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
			innerFieldValues.push_back(Value((OTInt32)2));
#endif
			innerFieldValues.push_back(Value(_T("nested[1][1]")));
			innerFieldValues.push_back(Value((OTInt16)40));

			// provide field values [1][1]
			fieldValues.clear();
			fieldValues.push_back(Value(_T("parent[1][1]")));
			fieldValues.push_back(Value());
			fieldValues.back().setValueArray(innerFieldValues);
			arrayValues.push_back(fieldValues);

			{
				EnumStatusCode setResult = structVal.setStructuredValueMatrix(dataTypeDescription, arrayDimensions, arrayValues);
				if(StatusCode::isBad(setResult))
				{
					_tprintf(_T("Failed to set the structured value matrix.\n"));
				}
			}
			dval.setValue(structVal);
			dval.setSourceTimestamp(DateTime::getUtcNow());
			dval.setServerTimestamp(DateTime::getUtcNow());
			structExtendedNestedVar->setDataValue(dval);

			structExtendedNestedVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, structExtendedNestedVar);
		}
	}
	return result;
}

void StructuredDataTypeModule::initBigBarrel(OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = EnumStatusCode_Good;
	m_bigBarrelTypeId = NodeId(namespaceIndex, 363);	// same identifier as on .Net StructoAutomoldServer
	NodeId bigBarrelEncodingId(namespaceIndex, 368);	// same identifier as on .Net StructoAutomoldServer
	NodeId bigBarrelDescriptionId(namespaceIndex, 369);	// same identifier as on .Net StructoAutomoldServer
	NodeId parentDataTypeId(EnumNumericNodeId_Structure);

	// create the field descriptions
	std::vector<StructureField> fields;
	std::vector<EnumNumericNodeId> typeIds;
	std::vector<Value> scalarValues, arrayValues;	// two vectors to separate the oder of scalars and arrays

	// all supported data types
	// use the same values as .Net StructoAutomoldServer
	typeIds.push_back(EnumNumericNodeId_Boolean);	// 0
	scalarValues.push_back(Value(true));
	insertArrayValues<bool>(arrayValues, true, false, false);
	typeIds.push_back(EnumNumericNodeId_SByte);	// 1
	scalarValues.push_back(Value((OTInt8)-127));
	insertArrayValues<OTInt8>(arrayValues, -1, -2, -3);
	typeIds.push_back(EnumNumericNodeId_Byte);	// 2
	scalarValues.push_back(Value((OTUInt8)127));
	insertArrayValues<OTUInt8>(arrayValues, 127, 101, 1);
	typeIds.push_back(EnumNumericNodeId_Int16);	// 3
	scalarValues.push_back(Value((OTInt16)1));
	insertArrayValues<OTInt16>(arrayValues, -1, -2, -3);
	typeIds.push_back(EnumNumericNodeId_UInt16);	// 4
	scalarValues.push_back(Value((OTUInt16)1));
	insertArrayValues<OTUInt16>(arrayValues, 4, 5, 6);
	typeIds.push_back(EnumNumericNodeId_Int32);	// 5
	scalarValues.push_back(Value((OTInt32)1));
	insertArrayValues<OTInt32>(arrayValues, -4, -5, -6);
	typeIds.push_back(EnumNumericNodeId_UInt32);	// 6
	scalarValues.push_back(Value((OTUInt32)1));
	insertArrayValues<OTUInt32>(arrayValues, 1, 2, 3);
	typeIds.push_back(EnumNumericNodeId_Int64);	// 7
	scalarValues.push_back(Value((OTInt64)1));
	insertArrayValues<OTInt64>(arrayValues, -7, -8, -9);
	typeIds.push_back(EnumNumericNodeId_UInt64);	// 8
	scalarValues.push_back(Value((OTUInt64)13));
	insertArrayValues<OTUInt64>(arrayValues, 7, 8, 9);
	typeIds.push_back(EnumNumericNodeId_Float);	// 9
	scalarValues.push_back(Value((OTFloat)-4));
	insertArrayValues<OTFloat>(arrayValues, -4, -3, -2);
	typeIds.push_back(EnumNumericNodeId_Double);	// 10
	scalarValues.push_back(Value((OTDouble)2.5));
	insertArrayValues<OTDouble>(arrayValues, -1, -1, 5, 0, 0.5);
	typeIds.push_back(EnumNumericNodeId_String);	// 11
	scalarValues.push_back(Value(_T("some string")));
	insertArrayValues<tstring>(arrayValues, _T("1"), _T("2"), _T("3"));
	typeIds.push_back(EnumNumericNodeId_DateTime);	// 12
	{
		DateTime dt;
		dt.parse(_T("2014-03-25T00:00:00Z"));
		scalarValues.push_back(Value(&dt));
		insertArrayValues<DateTime>(arrayValues, dt, dt, dt);
	}
	typeIds.push_back(EnumNumericNodeId_Guid);	// 13
	{
		OTUInt8 data4[8] = { 0xa0, 0x65, 0xc1, 0xef, 0x7c, 0xd3, 0x53, 0x4d };
		Guid scalar(0xd90dbbb6, 0xabe0, 0x475d, data4);
		scalarValues.push_back(Value(&scalar));
		OTUInt8 data41[8] = { 0x99, 0xd4, 0xe5, 0x58, 0x7f, 0x82, 0x6b, 0x67 };
		Guid g1(0x8b711692, 0xe703, 0x4ccc, data41);
		OTUInt8 data42[8] = { 0x87, 0xa6, 0xd6, 0x78, 0x92, 0x63, 0x15, 0xe1 };
		Guid g2(0xf5612dba, 0xcdcf, 0x41ed, data42);
		insertArrayValues<>(arrayValues, g1, g2);
	}
	typeIds.push_back(EnumNumericNodeId_ByteString);	// 14
	{
		unsigned char scalar[3] = { 10, 30, 40 };
		ByteString bs(3, scalar);
		scalarValues.push_back(Value(&bs));
		unsigned char bytes1[3] = { 12, 34, 48 };
		unsigned char bytes2[3] = { 21, 37, 28 };
		insertArrayValues<ByteString>(arrayValues, ByteString(3, bytes1), ByteString(3, bytes2));
	}
	typeIds.push_back(EnumNumericNodeId_XmlElement);	// 15
	{
		tstring scalarContent = _T("<xml value=\"test1\" />");
		XmlElement scalar(scalarContent);
		Value val, arrayVal;
		val.setXmlElement(scalar);	// XmlElements can not be set via the constructor
		scalarValues.push_back(val);
		// TODO change to empty array again, when we support empty ExtensionObjectArrays
		//arrayValues.push_back(Value(std::vector<XmlElement>()));	// empty array
		std::vector<XmlElement> elements;
		elements.push_back(scalar);
		arrayVal.setXmlElementArray(elements);
		arrayValues.push_back(arrayVal);
	}
	typeIds.push_back(EnumNumericNodeId_NodeId);	// 16
	{
		NodeId scalar(1, 1);
		scalarValues.push_back(Value(&scalar));
		insertArrayValues<NodeId>(arrayValues, NodeId(1, 1), NodeId(2, 2));
	}
	typeIds.push_back(EnumNumericNodeId_ExpandedNodeId);	// 17
	{
		ExpandedNodeId scalar(1, 1);
		scalarValues.push_back(Value(&scalar));
		insertArrayValues<ExpandedNodeId>(arrayValues, ExpandedNodeId(1, 1), ExpandedNodeId(2, 2));
	}
	typeIds.push_back(EnumNumericNodeId_StatusCode);	// 18
	scalarValues.push_back(Value(EnumStatusCode_BadBoundNotFound));
	insertArrayValues<EnumStatusCode>(arrayValues, EnumStatusCode_Good, EnumStatusCode_Bad, EnumStatusCode_Uncertain);
	//typeIds.push_back(EnumNumericNodeId_DiagnosticInfo);	// not yet supported
	//{
	//	DiagnosticInfo scalar;
	//	scalar.setNamespaceUri(1);
	//	scalar.setSymbolicId(1);
	//	scalar.setLocale(1);
	//	scalar.setLocalizedText(1);
	//	scalar.setInnerStatusCode(EnumStatusCode_Good);
	//	scalarValues.push_back(Value(&scalar));
	//	arrayValues.push_back(Value(std::vector<DiagnosticInfo>()));	// empty array
	//}
	typeIds.push_back(EnumNumericNodeId_QualifiedName);	// 19
	{
		QualifiedName scalar(_T("QualifiedName"), 1);
		scalarValues.push_back(Value(&scalar));
		insertArrayValues<QualifiedName>(arrayValues, QualifiedName(_T("QualifiedName"), 1), QualifiedName(_T("QualifiedName"), 2));
	}
	typeIds.push_back(EnumNumericNodeId_LocalizedText);	// 20
	{
		LocalizedText scalar(_T("locale"), _T("en-US"));
		scalarValues.push_back(Value(&scalar));
		insertArrayValues<>(arrayValues, scalar, LocalizedText(_T("locale"), _T("de-De")));
	}
	// end of data types, similar to .Net StructoAutomoldServer

	// start of additional types
#ifdef ADD_NESTED_ENCODEABLE_SRTUCTURES
	typeIds.push_back(EnumNumericNodeId_Argument);	// 22
	{
		Argument scalar;
		scalar.setDataType(Statics::DataTypeId_Float);
		scalar.setName(_T("TestFloatArgument"));
		scalar.setDescription(LocalizedText(_T("Describes a float argument."), _T("")));
		scalarValues.push_back(Value(&scalar));
		Argument arr1;
		arr1.setDataType(Statics::DataTypeId_UInt16);
		arr1.setName(_T("TestUInt16Argument"));
		arr1.setDescription(LocalizedText(_T("Describes a UInt16 argument."), _T("")));
		Argument arr2;
		arr2.setDataType(Statics::DataTypeId_Double);
		arr2.setName(_T("TestDoubleArgument"));
		arr2.setDescription(LocalizedText(_T("Describes a double argument."), _T("")));
		Argument arr3;
		arr3.setDataType(Statics::DataTypeId_String);
		arr3.setName(_T("TestStringArgument"));
		arr3.setDescription(LocalizedText(_T("Describes a string argument."), _T("")));
		insertArrayValues<Argument>(arrayValues, arr1, arr2, arr3);
	}
	typeIds.push_back(EnumNumericNodeId_ApplicationDescription);	// 23
	{
		ApplicationDescription scalar;
		scalar.setApplicationUri(_T("ScalarApplicationUri"));
		scalar.setProductUri(_T("ProductUri"));
		scalar.setApplicationName(LocalizedText(_T("StructoAutomoldApplicationName"), _T("")));
		scalar.setApplicationType(EnumApplicationType_Server);
		scalar.setGatewayServerUri(_T("GatewayServerUri"));
		scalar.setDiscoveryProfileUri(_T("DiscoveryProfileUri"));
		std::vector<tstring> discoveryUrls;
		discoveryUrls.push_back(_T("Url1"));
		discoveryUrls.push_back(_T("Url2"));
		scalar.setDiscoveryUrls(discoveryUrls);

		scalarValues.push_back(Value(&scalar));
		ApplicationDescription arr1;
		arr1.setApplicationUri(_T("ArrayApplicationUri1"));
		arr1.setProductUri(_T("ProductUri1"));
		arr1.setApplicationName(LocalizedText(_T("StructoAutomoldApplicationName1"), _T("")));
		arr1.setApplicationType(EnumApplicationType_Client);
		ApplicationDescription arr2;
		arr2.setApplicationUri(_T("ArrayApplicationUri2"));
		arr2.setProductUri(_T("ProductUri2"));
		arr2.setApplicationName(LocalizedText(_T("StructoAutomoldApplicationName2"), _T("")));
		arr2.setApplicationType(EnumApplicationType_Both);
		ApplicationDescription arr3;
		arr3.setApplicationUri(_T("ArrayApplicationUri3"));
		arr3.setProductUri(_T("ProductUri3"));
		arr3.setApplicationName(LocalizedText(_T("StructoAutomoldApplicationName3"), _T("")));
		arr3.setApplicationType(EnumApplicationType_DiscoveryServer);
		insertArrayValues<ApplicationDescription>(arrayValues, arr1, arr2, arr3);
	}
	typeIds.push_back(EnumNumericNodeId_AnonymousIdentityToken);	// 24
	{
		UserIdentityToken scalar;
		scalar.setAnonymousIdentityToken(_T("AnonymousPolicy1"));
		scalarValues.push_back(Value(&scalar));
		UserIdentityToken arr2;
		arr2.setAnonymousIdentityToken(_T("AnonymousPolicy2"));
		UserIdentityToken arr3;
		arr3.setAnonymousIdentityToken(_T("AnonymousPolicy3"));
		insertArrayValues<UserIdentityToken>(arrayValues, scalar, arr2, arr3);
	}
	typeIds.push_back(EnumNumericNodeId_UserNameIdentityToken);	// 25
	{
		UserIdentityToken scalar;
		ByteString pwd;
		pwd.setUtf8String(_T("pwd"));
		scalar.setUserNameIdentityToken(_T("UserNamePolicy1"), _T("User1"), &pwd);
		scalarValues.push_back(Value(&scalar));
		UserIdentityToken arr2;
		pwd.setUtf8String(_T("Password"));
		arr2.setUserNameIdentityToken(_T("UserNamePolicy2"), _T("User2"), &pwd);
		insertArrayValues<UserIdentityToken>(arrayValues, scalar, arr2);
	}
	typeIds.push_back(EnumNumericNodeId_X509IdentityToken);	// 26
	{
		X509IdentityToken scalar;
		scalar.setPolicyId(_T("X509Policy"));
		ByteString bs;
		bs.setUtf8String(_T("Certificate1"));
		scalar.setCertificate(bs);
		// we don't have the demo store available to load from files so we let the private key empty
		UserIdentityToken userToken1, userToken2;
		userToken1.setX509IdentityToken(&scalar);
		scalarValues.push_back(Value(&userToken1));
		X509IdentityToken arr2;
		bs.setUtf8String(_T("Certificate2"));
		arr2.setCertificate(bs);
		userToken2.setX509IdentityToken(&arr2);
		insertArrayValues<UserIdentityToken>(arrayValues, userToken1, userToken2);
	}
	typeIds.push_back(EnumNumericNodeId_IssuedIdentityToken);	// 27
	{
		UserIdentityToken scalar;
		ByteString bs;
		bs.setUtf8String(_T("StrangeToken1"));
		scalar.setIssuedIdentityToken(_T("IssuedPolicy1"), &bs);
		scalarValues.push_back(Value(&scalar));
		UserIdentityToken arr2;
		bs.setUtf8String(_T("StrangeToken2"));
		arr2.setIssuedIdentityToken(_T("IssuedPolicy2"), &bs);
		insertArrayValues<UserIdentityToken>(arrayValues, scalar, arr2);
	}
	typeIds.push_back(EnumNumericNodeId_BuildInfo);	// 28
	{
		BuildInfo scalar;
		scalar.setProductUri(_T("ProductUri1"));
		scalar.setManufacturerName(_T("Manufacturer1"));
		scalar.setProductName(_T("Product1"));
		scalar.setSoftwareVersion(_T("1.0"));
		scalar.setBuildNumber(_T("1234"));
		DateTime dt;
		dt.parse(_T("22.09.2014"));
		scalar.setBuildDate(dt);
		scalarValues.push_back(Value(&scalar));
		BuildInfo arr2;
		arr2.setProductUri(_T("ProductUri2"));
		arr2.setManufacturerName(_T("Manufacturer2"));
		arr2.setProductName(_T("Product2"));
		arr2.setSoftwareVersion(_T("2.0"));
		arr2.setBuildNumber(_T("2345"));
		dt.parse(_T("22.09.2014"));
		arr2.setBuildDate(dt);
		BuildInfo arr3;
		arr3.setProductUri(_T("ProductUri3"));
		arr3.setManufacturerName(_T("Manufacturer3"));
		arr3.setProductName(_T("Product3"));
		arr3.setSoftwareVersion(_T("3.0"));
		arr3.setBuildNumber(_T("3456"));
		dt.parse(_T("22.09.2014"));
		arr3.setBuildDate(dt);
		insertArrayValues<BuildInfo>(arrayValues, scalar, arr2, arr3);
	}
	typeIds.push_back(EnumNumericNodeId_SignedSoftwareCertificate);	// 29
	{
		SignedSoftwareCertificate scalar;
		ByteString bs;
		bs.setUtf8String(_T("InvalidDummyData1"));
		scalar.setCertificateData(bs);
		bs.setUtf8String(_T("InvalidSignature1"));
		scalar.setSignature(bs);
		scalarValues.push_back(Value(&scalar));
		SignedSoftwareCertificate arr2;
		bs.setUtf8String(_T("InvalidDummyData1"));
		arr2.setCertificateData(bs);
		bs.setUtf8String(_T("InvalidSignature1"));
		arr2.setSignature(bs);
		insertArrayValues<SignedSoftwareCertificate>(arrayValues, scalar, arr2);
	}
	typeIds.push_back(EnumNumericNodeId_ReferenceDescription);	// 30
	{
		ReferenceDescription scalar;
		scalar.setBrowseName(QualifiedName(_T("BrowseName1"), 1));
		scalar.setDisplayName(LocalizedText(_T("DisplayName1"), _T("")));
		scalar.setIsForward(true);
		scalar.setNodeClass(EnumNodeClass_Object);
		scalar.setNodeId(NodeId(1, _T("NodeId1")));
		scalar.setReferenceTypeId(Statics::ReferenceTypeId_Organizes);
		scalar.setTypeDefinition(ExpandedNodeId(1, _T("TypeId1")));
		scalarValues.push_back(Value(&scalar));
		ReferenceDescription arr2;
		arr2.setBrowseName(QualifiedName(_T("BrowseName2"), 1));
		scalar.setDisplayName(LocalizedText(_T("DisplayName2"), _T("")));
		arr2.setIsForward(false);
		arr2.setNodeClass(EnumNodeClass_Object);
		arr2.setNodeId(NodeId(1, _T("NodeId2")));
		arr2.setReferenceTypeId(Statics::ReferenceTypeId_Organizes);
		arr2.setTypeDefinition(ExpandedNodeId(1, _T("TypeId2")));
		ReferenceDescription arr3;
		arr3.setBrowseName(QualifiedName(_T("BrowseName3"), 1));
		scalar.setDisplayName(LocalizedText(_T("DisplayName3"), _T("")));
		arr3.setIsForward(true);
		arr3.setNodeClass(EnumNodeClass_Object);
		arr3.setNodeId(NodeId(1, _T("NodeId3")));
		arr3.setReferenceTypeId(Statics::ReferenceTypeId_HasComponent);
		arr3.setTypeDefinition(ExpandedNodeId(1, _T("TypeId3")));
		insertArrayValues<ReferenceDescription>(arrayValues, scalar, arr2, arr3);
	}
	typeIds.push_back(EnumNumericNodeId_EventFilter);	// 31
	{
		EventFilter scalar;
		SimpleAttributeOperand selectClause;
		selectClause.setAttributeId(EnumAttributeId_Value);
		selectClause.setTypeDefinitionId(Statics::VariableTypeId_BaseDataVariableType);
		QualifiedName qname(_T("MyProperty"), 1);
		selectClause.addBrowsePath(&qname);
		scalar.addSelectClause(&selectClause);
		ContentFilter whereClause;
		ContentFilterElement cfElem;
		cfElem.setFilterOperator(EnumFilterOperator_OfType);
		FilterOperand filter;
		LiteralOperand litOp;
		NodeId nodeId(1, _T("MyType"));
		litOp.setValue(Value(&nodeId));
		filter.setLiteralOperand(&litOp);
		cfElem.addFilterOperand(&filter);
		whereClause.addElement(&cfElem);
		scalar.setWhereClause(whereClause);
		scalarValues.push_back(Value(&scalar));
		insertArrayValues<EventFilter>(arrayValues, scalar);
	}
	typeIds.push_back(EnumNumericNodeId_ServerDiagnosticsSummaryDataType);	// 32
	{
		ServerDiagnosticsSummaryDataType scalar;
		scalar.setCumulatedSessionCount(2);
		scalar.setCurrentSessionCount(1);
		scalar.setCumulatedSubscriptionCount(4);
		scalar.setCurrentSubscriptionCount(2);
		scalar.setPublishingIntervalCount(0);
		scalar.setRejectedRequestsCount(100);
		scalar.setRejectedSessionCount(5);
		scalar.setSecurityRejectedRequestsCount(1);
		scalar.setSecurityRejectedSessionCount(2);
		scalar.setServerViewCount(1);
		scalar.setSessionAbortCount(100);
		scalar.setSessionTimeoutCount(2);
		scalarValues.push_back(Value(&scalar));
		insertArrayValues<ServerDiagnosticsSummaryDataType>(arrayValues, scalar, scalar);
	}
	typeIds.push_back(EnumNumericNodeId_ServerStatusDataType);	// 33
	{
		ServerStatusDataType scalar;
		BuildInfo buildInfo;
		buildInfo.setProductUri(_T("ProductUri1"));
		buildInfo.setManufacturerName(_T("Manufacturer1"));
		buildInfo.setProductName(_T("Product1"));
		buildInfo.setSoftwareVersion(_T("1.0"));
		buildInfo.setBuildNumber(_T("1234"));
		DateTime dt;
		dt.parse(_T("22.09.2014"));
		buildInfo.setBuildDate(&dt);
		scalar.setBuildInfo(buildInfo);
		dt.parse(_T("2014-09-22T14:00:00Z"));
		scalar.setStartTime(dt);
		dt.parse(_T("2014-09-22T15:00:00Z"));
		scalar.setCurrentTime(dt);
		scalar.setState(EnumServerState_Running);
		scalarValues.push_back(Value(&scalar));
		ServerStatusDataType arr2;
		buildInfo.clear();
		buildInfo.setProductUri(_T("ProductUri2"));
		buildInfo.setManufacturerName(_T("Manufacturer2"));
		buildInfo.setProductName(_T("Product2"));
		buildInfo.setSoftwareVersion(_T("2.0"));
		buildInfo.setBuildNumber(_T("2345"));
		dt.parse(_T("22.09.2014"));
		buildInfo.setBuildDate(dt);
		arr2.setBuildInfo(buildInfo);
		dt.parse(_T("2014-09-21T14:00:00Z"));
		arr2.setStartTime(dt);
		dt.parse(_T("2014-09-21T15:00:00Z"));
		arr2.setCurrentTime(dt);
		arr2.setState(EnumServerState_Running);
		insertArrayValues<ServerStatusDataType>(arrayValues, scalar, arr2, scalar);
	}
	typeIds.push_back(EnumNumericNodeId_SessionDiagnosticsDataType);	// 34
	{
		SessionDiagnosticsDataType scalar;
		scalar.setSessionId(NodeId(1, 1234));
		scalar.setSessionName(_T("SessionName"));
		//scalar.setClientDescription()	// left out
		scalar.setServerUri(_T("ServerUri"));
		scalar.setEndpointUrl(_T("opc.tcp://localhost:4880"));
		std::vector<tstring> localeIds;
		localeIds.push_back(_T("en"));
		localeIds.push_back(_T("de"));
		scalar.setLocaleIds(localeIds);
		scalar.setActualSessionTimeout(60.0);
		scalar.setMaxResponseMessageSize(10000);
		DateTime dt;
		dt.parse(_T("2014-09-22T14:30:00Z"));
		scalar.setClientConnectionTime(dt);
		dt.parse(_T("2014-09-22T15:00:00Z"));
		scalar.setClientLastContactTime(dt);
		scalar.setCurrentSubscriptionsCount(1);
		scalar.setCurrentMonitoredItemsCount(5);
		scalar.setCurrentPublishRequestsInQueue(3);
		scalar.setTotalRequestCount(ServiceCounterDataType(500, 5));
		scalar.setUnauthorizedRequestCount(30);
		scalar.setReadCount(ServiceCounterDataType(100, 2));
		scalar.setHistoryReadCount(ServiceCounterDataType(0, 0));
		scalar.setWriteCount(ServiceCounterDataType(50, 2));
		scalar.setHistoryUpdateCount(ServiceCounterDataType(0, 0));
		scalar.setCallCount(ServiceCounterDataType(20, 1));
		scalar.setCreateMonitoredItemsCount(ServiceCounterDataType(5, 1));
		scalar.setModifyMonitoredItemsCount(ServiceCounterDataType(5, 0));
		scalar.setSetMonitoringModeCount(ServiceCounterDataType(2, 0));
		scalar.setSetTriggeringCount(ServiceCounterDataType(1, 0));
		scalar.setDeleteMonitoredItemsCount(0);
		scalar.setCreateSubscriptionCount(ServiceCounterDataType(1, 0));
		scalar.setModifySubscriptionCount(ServiceCounterDataType(0, 0));
		scalar.setSetPublishingModeCount(ServiceCounterDataType(1, 0));
		scalar.setPublishCount(ServiceCounterDataType(2, 1));
		scalar.setRepublishCount(ServiceCounterDataType(1, 0));
		scalar.setTransferSubscriptionsCount(ServiceCounterDataType(1, 1));
		scalar.setDeleteSubscriptionsCount(0);
		scalar.setAddNodesCount(ServiceCounterDataType(5, 1));
		scalar.setAddReferencesCount(ServiceCounterDataType(10, 1));
		scalar.setDeleteNodesCount(ServiceCounterDataType(4, 0));
		scalar.setDeleteNodesCount(ServiceCounterDataType(1, 0));
		scalar.setDeleteReferencesCount(ServiceCounterDataType(2, 0));
		scalar.setBrowseCount(ServiceCounterDataType(50, 1));
		scalar.setBrowseNextCount(ServiceCounterDataType(100, 1));
		scalar.setTranslateBrowsePathsToNodeIdsCount(ServiceCounterDataType(6, 1));
		scalar.setQueryFirstCount(ServiceCounterDataType(3, 1));
		scalar.setQueryNextCount(ServiceCounterDataType(2, 0));
		scalar.setRegisterNodesCount(ServiceCounterDataType(5, 3));
		scalar.setUnregisterNodesCount(ServiceCounterDataType(5, 3));
		scalarValues.push_back(Value(&scalar));
		insertArrayValues<SessionDiagnosticsDataType>(arrayValues, scalar, scalar);
	}
	typeIds.push_back(EnumNumericNodeId_SessionSecurityDiagnosticsDataType);	// 35
	{
		SessionSecurityDiagnosticsDataType scalar;
		scalar.setSessionId(NodeId(1, 1234));
		scalar.setClientUserIdOfSession(_T("User1"));
		std::vector<tstring> hUsers;
		hUsers.push_back(_T("LastUser"));
		hUsers.push_back(_T("User1"));
		scalar.setClientUserIdHistory(hUsers);
		scalar.setAuthenticationMechanism(_T("AuthenticationMechanism"));
		scalar.setEncoding(_T("UA Binary"));
		scalar.setTransportProtocol(_T("TCP"));
		scalar.setSecurityMode(EnumMessageSecurityMode_None);
		scalar.setSecurityPolicyUri(SecurityPolicyUri_None);
		ByteString bs;
		bs.setUtf8String(_T("InvalidDummyCertificate"));
		scalar.setClientCertificate(bs);

		scalarValues.push_back(Value(&scalar));
		insertArrayValues<SessionSecurityDiagnosticsDataType>(arrayValues, scalar);
	}
	typeIds.push_back(EnumNumericNodeId_ServiceCounterDataType);	// 36
	{
		ServiceCounterDataType scalar(10, 5);
		scalarValues.push_back(Value(&scalar));
		insertArrayValues<ServiceCounterDataType>(arrayValues, ServiceCounterDataType(9, 4), ServiceCounterDataType(8, 3), ServiceCounterDataType(7, 2));
	}
	typeIds.push_back(EnumNumericNodeId_SubscriptionDiagnosticsDataType);	// 37
	{
		SubscriptionDiagnosticsDataType scalar;
		scalar.setSessionId(NodeId(1, 1234));
		scalar.setSubscriptionId(5);
		scalar.setPriority(128);
		scalar.setPublishingInterval(1000.0);
		scalar.setMaxKeepAliveCount(3);
		scalar.setMaxLifetimeCount(10);
		scalar.setMaxNotificationsPerPublish(10000);
		scalar.setPublishingEnabled(true);
		scalar.setModifyCount(5);
		scalar.setEnableCount(1);
		scalar.setDisableCount(1);
		scalar.setRepublishRequestCount(2);
		scalar.setRepublishMessageRequestCount(2);
		scalar.setRepublishMessageCount(10);
		scalar.setTransferRequestCount(3);
		scalar.setTransferredToAltClientCount(1);
		scalar.setTransferredToSameClientCount(0);
		scalar.setPublishRequestCount(5);
		scalar.setDataChangeNotificationsCount(30);
		scalar.setEventNotificationsCount(20);
		scalar.setNotificationsCount(50);
		scalar.setLatePublishRequestCount(0);
		scalar.setCurrentKeepAliveCount(1);
		scalar.setCurrentLifetimeCount(3);
		scalar.setUnacknowledgedMessageCount(10);
		scalar.setDiscardedMessageCount(3);
		scalar.setMonitoredItemCount(10);
		scalar.setDisabledMonitoredItemCount(2);
		scalar.setMonitoringQueueOverflowCount(1);
		scalar.setNextSequenceNumber(48);
		scalar.setEventQueueOverFlowCount(1);

		scalarValues.push_back(Value(&scalar));
		insertArrayValues<SubscriptionDiagnosticsDataType>(arrayValues, scalar, scalar, scalar);
	}
	typeIds.push_back(EnumNumericNodeId_ModelChangeStructureDataType);	// 38
	{
		ModelChangeStructureDataType scalar;
		scalar.setAffected(NodeId(1, _T("MyNode")));
		scalar.setAffectedType(NodeId(EnumNumericNodeId_BaseDataVariableType));
		scalar.setVerb(EnumModelChangeStructureVerbMask_DataTypeChanged);
		Value val1, val2, arrayVal;
		val1.setModelChangeStructureDataType(&scalar);
		scalarValues.push_back(val1);
		ModelChangeStructureDataType arr2;
		arr2.setAffected(NodeId(1, _T("MyNode2")));
		arr2.setAffectedType(NodeId(EnumNumericNodeId_BaseObjectType));
		arr2.setVerb(EnumModelChangeStructureVerbMask_ReferenceDeleted | EnumModelChangeStructureVerbMask_NodeDeleted);
		std::vector<ModelChangeStructureDataType> modelChanges;
		modelChanges.push_back(scalar);
		modelChanges.push_back(arr2);
		arrayVal.setModelChangeStructureDataTypeArray(modelChanges);
		arrayValues.push_back(arrayVal);
	}
	typeIds.push_back(EnumNumericNodeId_Range);	// 39
	{
		Range scalar(3, 5);
		scalarValues.push_back(Value(&scalar));
		insertArrayValues<Range>(arrayValues, Range(2, 6), Range(1, 7), Range(0, 8));
	}
	typeIds.push_back(EnumNumericNodeId_EUInformation);	// 40
	{
		EUInformation scalar;
		scalar.setDisplayName(LocalizedText(_T("m/s"), _T("")));
		scalar.setDescription(LocalizedText(_T("Meter per second"), _T("")));
		scalar.setUnitId(-1);
		scalarValues.push_back(Value(&scalar));
		EUInformation arr2;
		arr2.setDisplayName(LocalizedText(_T("V"), _T("")));
		arr2.setDescription(LocalizedText(_T("Volt"), _T("")));
		arr2.setUnitId(-1);
		insertArrayValues<EUInformation>(arrayValues, scalar, arr2);
	}
	typeIds.push_back(EnumNumericNodeId_SemanticChangeStructureDataType);	// 41
	{
		SemanticChangeStructureDataType scalar;
		scalar.setAffected(NodeId(1, _T("MyNode")));
		scalar.setAffectedType(NodeId(EnumNumericNodeId_BaseDataVariableType));
		Value val1, arrayVal;
		val1.setSemanticChangeStructureDataType(&scalar);
		scalarValues.push_back(Value(&val1));
		SemanticChangeStructureDataType arr2;
		arr2.setAffected(NodeId(1, _T("MyNode2")));
		arr2.setAffectedType(NodeId(EnumNumericNodeId_BaseObjectType));
		std::vector<SemanticChangeStructureDataType> semanticArray;
		semanticArray.push_back(scalar);
		semanticArray.push_back(arr2);
		arrayVal.setSemanticChangeStructureDataTypeArray(semanticArray);
		arrayValues.push_back(arrayVal);
	}
	typeIds.push_back(EnumNumericNodeId_EnumValueType);	// 42
	{
		EnumValueType scalar;
		scalar.setDisplayName(LocalizedText(_T("Zero"), _T("")));
		scalar.setDescription(LocalizedText(_T("Represents the value 0."), _T("")));
		scalar.setValue(0);
		scalarValues.push_back(Value(&scalar));
		EnumValueType arr2;
		arr2.setDisplayName(LocalizedText(_T("One"), _T("")));
		arr2.setDescription(LocalizedText(_T("Represents the value 1."), _T("")));
		arr2.setValue(0);
		EnumValueType arr3;
		arr3.setDisplayName(LocalizedText(_T("Two"), _T("")));
		arr3.setDescription(LocalizedText(_T("Represents the value 2."), _T("")));
		arr3.setValue(0);
		insertArrayValues<EnumValueType>(arrayValues, scalar, arr2, arr3);
	}
	typeIds.push_back(EnumNumericNodeId_TimeZoneDataType);	// 43
	{
		TimeZone scalar(60, true);
		scalarValues.push_back(Value(&scalar));
		insertArrayValues<TimeZone>(arrayValues, TimeZone(120, true), TimeZone(60, false), TimeZone(0, false));
	}

#endif

	// create the field descriptions
	// first all scalar, then all array fields
	for(size_t i = 0; i < typeIds.size(); i++)
	{
		createScalarFieldDescription(fields, typeIds[i]);
	}

#ifdef ADD_NESTED_CUSTOM_STRUCTURE
	// nested scalar custom structured type
	{
		FieldDescriptionPtr field = FieldDescription::create();	// 44
		field->setName(_T("RefrigeratorStatus"));
		field->setType(NodeId(m_refrigeratorStatusDataTypeId));
		fields.push_back(field);
	}
#endif
	for(size_t i = 0; i < typeIds.size(); i++)
	{
		createArrayFieldDescription(fields, typeIds[i]);
	}

#ifdef ADD_NESTED_CUSTOM_STRUCTURE
	// nested array custom structured type
	{
		FieldDescriptionPtr arrayField = FieldDescription::create();
		arrayField->setName(_T("RefrigeratorStatusArray"));
		arrayField->setType(NodeId(m_refrigeratorStatusDataTypeId));
		arrayField->setLength(std::numeric_limits<OTUInt32>::max());
		fields.push_back(arrayField);
	}

	// create the values for the nested custom structured type
	{
		DataTypeDescriptionConstPtr refrigeratorStatusType = AddressSpaceRoot::instance()->getDataTypeDescription(m_refrigeratorStatusDataTypeId);
		if(refrigeratorStatusType.isNull())
		{
			_tprintf(_T("Failed to get the refrigerator status type.\n"));
		}
		else
		{
			StructuredDataTypeDescriptionPtr structDescription = (StructuredDataTypeDescription*)refrigeratorStatusType.p();
			std::vector<Value> fieldValues;
			fieldValues.push_back(Value(true));
			fieldValues.push_back(Value((double)110.0));
			fieldValues.push_back(Value((double)70.0));
			Value structVal, arrayVal;
			EnumStatusCode setResult = structVal.setValueArray(fieldValues);
			if(StatusCode::isBad(setResult))
			{
				_tprintf(_T("Failed to set the structured value.\n"));
			}
			scalarValues.push_back(structVal);

			std::vector<Value> allFieldValues;
			allFieldValues.push_back(structVal);
			allFieldValues.push_back(structVal);
			arrayValues.push_back(allFieldValues);
		}
	}
#endif

	// append all scalar and array values
	_ASSERTION(scalarValues.size() == arrayValues.size(), _T("For each data type, there should be one scalar and one array field."));
	scalarValues.insert(scalarValues.end(), arrayValues.begin(), arrayValues.end());

	// create the data type description
	StructuredDataTypeDescriptionPtr bigBarrelTypeDescription = StructuredDataTypeDescription::create();
	// don't set a DataTypeDescription name to use the same as the data type
	{
		EnumStatusCode setResult = bigBarrelTypeDescription->setFields(fields);
		if(StatusCode::isBad(setResult))
		{
			_tprintf(_T("Failed to set the field descriptions.\n"));
		}
	}

	// create data type
	DataTypePtr bigBarrelType = DataType::create();
	(void)bigBarrelType->setNodeId(m_bigBarrelTypeId);
	(void)bigBarrelType->setBrowseName(QualifiedName(_T("BigBarrelType"), namespaceIndex));
	(void)bigBarrelType->setDisplayName(LocalizedText(_T("BigBarrelType"), _T("")));
	// empty description
	// default write mask
	result = bigBarrelType->setDataTypeDescription(bigBarrelTypeDescription, bigBarrelDescriptionId, bigBarrelEncodingId, m_dictionaryId);
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to set the datatestServer type description.\n"));
	}
	else
	{
		DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
		(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, bigBarrelType);
		result = bigBarrelType->init(NULL);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to initialize the data type node.\n"));
		}
	}

	///////////////
	// Variables //
	///////////////

	if(StatusCode::isGood(result))
	{
		// bigBarrel variable
		VariablePtr bigBarrelVar = TestVariableForAuditEvents::create();
		bigBarrelVar->setNodeId(NodeId(namespaceIndex, 2));	// same identifier as .Net StructoAutomoldServer
		bigBarrelVar->setBrowseName(QualifiedName(_T("BigBarrelFull"), namespaceIndex));
		bigBarrelVar->setDisplayName(LocalizedText(_T("BigBarrelFull"), _T("")));
		bigBarrelVar->setAccessLevel(bigBarrelVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		bigBarrelVar->setValueRank(ValueRank_Scalar);
		bigBarrelVar->setDataType(m_bigBarrelTypeId);
		bigBarrelVar->init(NULL);
		parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, bigBarrelVar);

		Value structVal;
		{
			EnumStatusCode setResult = structVal.setStructuredValue(bigBarrelTypeDescription, scalarValues);	// scalarValues also contains the array values
			if(StatusCode::isBad(setResult))
			{
				_tprintf(_T("Failed to set the structured value.\n"));
			}
		}
		DataValue dval;
		dval.setValue(structVal);
		dval.setSourceTimestamp(DateTime::getUtcNow());
		dval.setServerTimestamp(DateTime::getUtcNow());
		bigBarrelVar->setDataValue(dval);
	}
#define USE_IMPORTED_TYPE_DESC 1
#if USE_IMPORTED_TYPE_DESC
	{
		DataTypeDescriptionConstPtr typeDesc = AddressSpaceRoot::instance()->getDataTypeDescription(NodeId(EnumNumericNodeId_XVType));
		if(typeDesc && typeDesc->getDescriptionType() == EnumDataTypeDescriptionType_Structure)
		{
			StructuredDataTypeDescriptionConstPtr xvTypeDesc = (const StructuredDataTypeDescription*)typeDesc.p();
			StructVarPtr xvTypeVar = StructVar::create();
			xvTypeVar->setNodeId(NodeId(namespaceIndex, _T("XVType"))); // only available if imported
			xvTypeVar->setBrowseName(QualifiedName(_T("XVType"), namespaceIndex));
			xvTypeVar->setDisplayName(LocalizedText(_T("XVType"), _T("")));
			xvTypeVar->setAccessLevel(xvTypeVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			xvTypeVar->setValueRank(ValueRank_Scalar);
			xvTypeVar->setDataType(NodeId(EnumNumericNodeId_XVType));
			xvTypeVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, xvTypeVar);

			std::vector<Value> fieldValues;
			Value structVal;
			DataValue dv;

			fieldValues.push_back(Value((OTDouble)12.25));
			fieldValues.push_back(Value((OTFloat)1.5));
			structVal.setStructuredValue(xvTypeDesc, fieldValues);
			dv.setValue(structVal);
			xvTypeVar->setDataValue(&dv);

			StructVarPtr xvTypeArrayVar = StructVar::create();
			xvTypeArrayVar->setNodeId(NodeId(namespaceIndex, _T("XVTypeArray"))); // only available if imported
			xvTypeArrayVar->setBrowseName(QualifiedName(_T("XVTypeArray"), namespaceIndex));
			xvTypeArrayVar->setDisplayName(LocalizedText(_T("XVTypeArray"), _T("")));
			xvTypeArrayVar->setAccessLevel(xvTypeArrayVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
			xvTypeArrayVar->setValueRank(1);
			xvTypeArrayVar->setDataType(NodeId(EnumNumericNodeId_XVType));
			xvTypeArrayVar->init(NULL);
			parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, xvTypeArrayVar);

			std::vector<std::vector<Value> > arrayXVValues;
			fieldValues.clear();
			fieldValues.push_back(Value((OTDouble)17.25));
			fieldValues.push_back(Value((OTFloat)3.75));
			arrayXVValues.push_back(fieldValues);
			fieldValues.clear();
			fieldValues.push_back(Value((OTDouble)23.75));
			fieldValues.push_back(Value((OTFloat)19.5));
			arrayXVValues.push_back(fieldValues);
			structVal.setStructuredValueArray(xvTypeDesc, arrayXVValues);
			dv.setValue(structVal);
			xvTypeArrayVar->setDataValue(&dv);
		}
	}
#endif
}

void StructuredDataTypeModule::createScalarFieldDescription(std::vector<SoftingOPCToolbox5::StructureField>& fields, EnumNumericNodeId type)
{
	fields.push_back(StructureField());
	fields.back().setName(getEnumNumericNodeIdString(type));
	fields.back().setDataType(NodeId(type));
}

void StructuredDataTypeModule::createArrayFieldDescription(std::vector<SoftingOPCToolbox5::StructureField>& fields, EnumNumericNodeId type)
{
	fields.push_back(StructureField());
	fields.back().setName(getEnumNumericNodeIdString(type) + tstring(_T("Array")));
	fields.back().setDataType(NodeId(type));
	std::vector<OTUInt32> arrayDimensions;
	arrayDimensions.push_back(0);
	fields.back().setArrayDimensions(arrayDimensions);
}

EnumStatusCode StructuredDataTypeModule::initUnion(OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = EnumStatusCode_Good;

	// 1. Define data types
	StructuredDataTypeDescriptionPtr dataTypeDescription, dataTypeDescriptionNested;
	{
		// A union
		m_unionTypeId = NodeId(namespaceIndex, 640);
		NodeId parentDataTypeId(EnumNumericNodeId_Union);
		NodeId dataTypeDescriptionId = NodeId(namespaceIndex, 641);
		NodeId encodingId = NodeId(namespaceIndex, 642);

		DataTypePtr dataType;

		tstring typeName = _T("TestUnionType");

		std::vector<StructureField> fields;
		fields.push_back(StructureField());
		fields.back().setName(_T("Double"));
		fields.back().setDataType(*Statics::DataTypeId_Double);

		fields.push_back(StructureField());
		fields.back().setName(_T("String"));
		fields.back().setDataType(*Statics::DataTypeId_String);

		fields.push_back(StructureField());
		fields.back().setName(_T("Int16"));
		fields.back().setDataType(*Statics::DataTypeId_Int16);

		fields.push_back(StructureField());
		fields.back().setName(_T("UInt32"));
		fields.back().setDataType(*Statics::DataTypeId_UInt32);

		dataTypeDescription = StructuredDataTypeDescription::create();
		result = dataTypeDescription->setFields(fields);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the field descriptions\n"));
			return result;
		}
		// Specify this is a union's description
		result = dataTypeDescription->setUnion(true);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set union flag on structured data type description\n"));
			return result;
		}

		dataType = DataType::create();
		(void)dataType->setNodeId(m_unionTypeId);
		(void)dataType->setBrowseName(QualifiedName(typeName, namespaceIndex));
		(void)dataType->setDisplayName(LocalizedText(typeName, _T("")));
		// empty description
		// default write mask
		result = dataType->setDataTypeDescription(dataTypeDescription, dataTypeDescriptionId, encodingId, m_dictionaryId);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the data type description.\n"));
			return result;
		}

		if(StatusCode::isGood(result))
		{
			DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
			(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);
			result = dataType->init(NULL);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to initialize the data type node.\n"));
				return result;
			}
		}
	}

	{
		// A union with nested union and structure
		m_unionNestedTypeId = NodeId(namespaceIndex, 643);
		NodeId parentDataTypeId(EnumNumericNodeId_Union);
		NodeId dataTypeDescriptionId = NodeId(namespaceIndex, 644);
		NodeId encodingId = NodeId(namespaceIndex, 645);

		DataTypePtr dataType;

		tstring typeName = _T("TestUnionNestedType");

		std::vector<StructureField> fields;
		fields.push_back(StructureField());
		fields.back().setName(_T("String"));
		fields.back().setDataType(*Statics::DataTypeId_String);

		fields.push_back(StructureField());
		fields.back().setName(_T("NestedUnion"));
		fields.back().setDataType(m_unionTypeId);

		fields.push_back(StructureField());
		fields.back().setName(_T("NestedStructure"));
		fields.back().setDataType(m_childStructureDataTypeId);

		dataTypeDescriptionNested = StructuredDataTypeDescription::create();
		result = dataTypeDescriptionNested->setFields(fields);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the field descriptions\n"));
			return result;
		}
		// Specify this is a union's description
		result = dataTypeDescriptionNested->setUnion(true);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set union flag on structured data type description\n"));
			return result;
		}

		dataType = DataType::create();
		(void)dataType->setNodeId(m_unionNestedTypeId);
		(void)dataType->setBrowseName(QualifiedName(typeName, namespaceIndex));
		(void)dataType->setDisplayName(LocalizedText(typeName, _T("")));
		// empty description
		// default write mask
		result = dataType->setDataTypeDescription(dataTypeDescriptionNested, dataTypeDescriptionId, encodingId, m_dictionaryId);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set the data type description.\n"));
			return result;
		}

		if(StatusCode::isGood(result))
		{
			DataTypePtr parentType = AddressSpaceRoot::instance()->getDataType(&parentDataTypeId);
			(void)parentType->addReference(Statics::ReferenceTypeId_HasSubtype, dataType);
			result = dataType->init(NULL);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Failed to initialize the data type node.\n"));
				return result;
			}
		}
	}

	// 2. Define variables
	DataValue dval;
	// scalar
	{
		Value unionVal;
		VariablePtr unionVar = TestVariableForAuditEvents::create();
		tstring varName = _T("Union");

		unionVar->setNodeId(NodeId(namespaceIndex, varName));
		unionVar->setBrowseName(QualifiedName(varName, namespaceIndex));
		unionVar->setDisplayName(LocalizedText(varName, _T("en")));
		unionVar->setAccessLevel(unionVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		unionVar->setValueRank(ValueRank_Scalar);
		unionVar->setDataType(m_unionTypeId);

		// field 4, UInt32
		Value fieldValue;
		fieldValue.setUInt32(123);
		result = unionVal.setUnionValue(dataTypeDescription, 4, fieldValue);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set structured value.\n"));
			return result;
		}

		dval.setValue(unionVal);
		unionVar->setDataValue(dval);

		unionVar->init(NULL);
		parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, unionVar);
	}
	// array
	{
		Value unionVal;
		VariablePtr unionVar = TestVariableForAuditEvents::create();
		tstring varName = _T("UnionArray");

		unionVar->setNodeId(NodeId(namespaceIndex, varName));
		unionVar->setBrowseName(QualifiedName(varName, namespaceIndex));
		unionVar->setDisplayName(LocalizedText(varName, _T("en")));
		unionVar->setAccessLevel(unionVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		unionVar->setValueRank(1); // one-dimensional array
		std::vector<OTUInt32> arrayDimansions;
		arrayDimansions.push_back(4); // exact 4 elements
		unionVar->setArrayDimensions(arrayDimansions);
		unionVar->setDataType(m_unionTypeId);

		std::vector<Value> arrayFieldValues;
		std::vector<OTUInt32> selectors;
		arrayFieldValues.push_back(Value());
		arrayFieldValues.back().setDouble(2.31);
		selectors.push_back(1);
		arrayFieldValues.push_back(Value());
		arrayFieldValues.back().setString(_T("231"));
		selectors.push_back(2);
		arrayFieldValues.push_back(Value());
		arrayFieldValues.back().setInt16(231);
		selectors.push_back(3);
		arrayFieldValues.push_back(Value());
		arrayFieldValues.back().setUInt32(2310);
		selectors.push_back(4);

		result = unionVal.setUnionValueArray(dataTypeDescription, selectors, arrayFieldValues);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set union array values.\n"));
			return result;
		}

		dval.setValue(unionVal);
		unionVar->setDataValue(dval);

		unionVar->init(NULL);
		parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, unionVar);
	}
	// matrix
	{
		Value unionVal;
		VariablePtr unionVar = TestVariableForAuditEvents::create();
		tstring varName = _T("UnionMatrix");

		unionVar->setNodeId(NodeId(namespaceIndex, varName));
		unionVar->setBrowseName(QualifiedName(varName, namespaceIndex));
		unionVar->setDisplayName(LocalizedText(varName, _T("en")));
		unionVar->setAccessLevel(unionVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		unionVar->setValueRank(2); // two-dimensional matrix
		std::vector<OTUInt32> arrayDimansions;
		arrayDimansions.push_back(2); // exact 2 elements
		arrayDimansions.push_back(2); // exact 2 elements
		unionVar->setArrayDimensions(arrayDimansions);
		unionVar->setDataType(m_unionTypeId);

		std::vector<Value> arrayFieldValues;
		std::vector<OTUInt32> selectors;
		arrayFieldValues.push_back(Value());
		arrayFieldValues.back().setString(_T("StringVal[0][0]"));
		selectors.push_back(2);
		arrayFieldValues.push_back(Value());
		arrayFieldValues.back().setString(_T("StringVal[0][1]"));
		selectors.push_back(2);
		arrayFieldValues.push_back(Value());
		arrayFieldValues.back().setString(_T("StringVal[1][0]"));
		selectors.push_back(2);
		arrayFieldValues.push_back(Value());
		arrayFieldValues.back().setString(_T("StringVal[1][1]"));
		selectors.push_back(2);

		result = unionVal.setUnionValueMatrix(dataTypeDescription, arrayDimansions, selectors, arrayFieldValues);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set union matrix values.\n"));
			return result;
		}

		dval.setValue(unionVal);
		unionVar->setDataValue(dval);

		unionVar->init(NULL);
		parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, unionVar);
	}
	// scalar with nested
	{
		Value unionVal1, unionVal2;
		VariablePtr unionVar = TestVariableForAuditEvents::create();
		tstring varName = _T("UnionWithNestedUnion");

		unionVar->setNodeId(NodeId(namespaceIndex, varName));
		unionVar->setBrowseName(QualifiedName(varName, namespaceIndex));
		unionVar->setDisplayName(LocalizedText(varName, _T("en")));
		unionVar->setAccessLevel(unionVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		unionVar->setValueRank(ValueRank_Scalar);
		unionVar->setDataType(m_unionNestedTypeId);

		// child union - field 4, UInt32
		Value fieldValue;
		fieldValue.setUInt32(123);
		result = unionVal1.setUnionValue(dataTypeDescription, 4, fieldValue);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set structured value.\n"));
			return result;
		}
		// parent union - field 2, Union
		result = unionVal2.setUnionValue(dataTypeDescriptionNested, 2, unionVal1);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set structured value.\n"));
			return result;
		}

		dval.setValue(unionVal2);
		unionVar->setDataValue(dval);

		unionVar->init(NULL);
		parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, unionVar);
	}
	// array with nested
	{
		Value unionVal;
		VariablePtr unionVar = TestVariableForAuditEvents::create();
		tstring varName = _T("UnionWithNestedUnionArray");

		unionVar->setNodeId(NodeId(namespaceIndex, varName));
		unionVar->setBrowseName(QualifiedName(varName, namespaceIndex));
		unionVar->setDisplayName(LocalizedText(varName, _T("en")));
		unionVar->setAccessLevel(unionVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		unionVar->setValueRank(1); // one-dimensional array
		std::vector<OTUInt32> arrayDimansions;
		arrayDimansions.push_back(4); // exact 4 elements
		unionVar->setArrayDimensions(arrayDimansions);
		unionVar->setDataType(m_unionNestedTypeId);

		std::vector<Value> arrayFieldValues;
		std::vector<OTUInt32> selectors;

		// child union - not used
		arrayFieldValues.push_back(Value());
		result = arrayFieldValues.back().setString(_T("string value"));
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set structured value.\n"));
			return result;
		}
		selectors.push_back(1);		// parent: field 1, String

		// child union - field 1, Double
		Value childFieldValue;
		childFieldValue.setDouble(12.3);
		arrayFieldValues.push_back(Value());
		result = arrayFieldValues.back().setUnionValue(dataTypeDescription, 1, childFieldValue);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set structured value.\n"));
			return result;
		}
		selectors.push_back(2);		// parent: field 2, Union

		// child union - field 2, String
		childFieldValue.setString(_T("nested"));
		arrayFieldValues.push_back(Value());
		result = arrayFieldValues.back().setUnionValue(dataTypeDescription, 2, childFieldValue);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set structured value.\n"));
			return result;
		}
		selectors.push_back(2);		// parent: field 2, Union

		// child structure
		std::vector<Value> innerFieldValues;
		innerFieldValues.push_back(Value(true));
		innerFieldValues.push_back(Value((double)110.0));
		innerFieldValues.push_back(Value((double)70.0));
#if TEST_SERVER_ADD_ENUM_TO_STRUCT
		innerFieldValues.push_back(Value((OTInt32)2));
#endif
		innerFieldValues.push_back(Value(_T("Extended information")));
		innerFieldValues.push_back(Value((OTInt16)20));

		arrayFieldValues.push_back(Value());
		result = arrayFieldValues.back().setValueArray(innerFieldValues);
		selectors.push_back(3);		// parent: field 3, Structure

		result = unionVal.setUnionValueArray(dataTypeDescriptionNested, selectors, arrayFieldValues);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set union array values.\n"));
		}

		dval.setValue(unionVal);
		dval.setSourceTimestamp(DateTime::getUtcNow());
		dval.setServerTimestamp(DateTime::getUtcNow());
		unionVar->setDataValue(dval);

		unionVar->init(NULL);
		parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, unionVar);
	}
	// matrix with nested
	{
		Value unionVal;
		VariablePtr unionVar = TestVariableForAuditEvents::create();
		tstring varName = _T("UnionWithNestedUnionMatrix");

		unionVar->setNodeId(NodeId(namespaceIndex, varName));
		unionVar->setBrowseName(QualifiedName(varName, namespaceIndex));
		unionVar->setDisplayName(LocalizedText(varName, _T("en")));
		unionVar->setAccessLevel(unionVar->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		unionVar->setValueRank(2); // two-dimensional matrix
		std::vector<OTUInt32> arrayDimansions;
		arrayDimansions.push_back(2); // exact 2 elements
		arrayDimansions.push_back(2); // exact 2 elements
		unionVar->setArrayDimensions(arrayDimansions);
		unionVar->setDataType(m_unionNestedTypeId);

		std::vector<Value> arrayFieldValues;
		std::vector<OTUInt32> selectors;

		// child union - field 2, String
		Value childFieldValue;
		childFieldValue.setString(_T("StringVal[0][0]"));
		arrayFieldValues.push_back(Value());
		result = arrayFieldValues.back().setUnionValue(dataTypeDescription, 2, childFieldValue);
		selectors.push_back(2);		// parent: field 2, Union

		// child union - field 2, String
		childFieldValue.setString(_T("StringVal[0][1]"));
		arrayFieldValues.push_back(Value());
		result = arrayFieldValues.back().setUnionValue(dataTypeDescription, 2, childFieldValue);
		selectors.push_back(2);		// parent: field 2, Union

		// child union - field 2, String
		childFieldValue.setString(_T("StringVal[1][0]"));
		arrayFieldValues.push_back(Value());
		result = arrayFieldValues.back().setUnionValue(dataTypeDescription, 2, childFieldValue);
		selectors.push_back(2);		// parent: field 2, Union

		// child union - field 2, String
		childFieldValue.setString(_T("StringVal[1][1]"));
		arrayFieldValues.push_back(Value());
		result = arrayFieldValues.back().setUnionValue(dataTypeDescription, 2, childFieldValue);
		selectors.push_back(2);		// parent: field 2, Union

		result = unionVal.setUnionValueMatrix(dataTypeDescriptionNested, arrayDimansions, selectors, arrayFieldValues);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Failed to set union matrix values.\n"));
		}

		dval.setValue(unionVal);
		unionVar->setDataValue(dval);

		unionVar->init(NULL);
		parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, unionVar);
	}
	return result;
}
