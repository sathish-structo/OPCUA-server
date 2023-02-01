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
#include "ChangeTwoStateItemSemanticsMethod.h"
#include "ServerHistoricalDataConfiguration.h"
#include "ServerAggregateConfiguration.h"
#include "Constants.h"
#include "HistorizingExclusiveLimitAlarm.h"
#include "EnumValueType.h"
#include "ServerMultiStateValueItem.h"
#include "ServerObjectType.h"

using namespace SoftingOPCToolbox5;

/*! Create the methods for simulation variables. */

EnumStatusCode AnalogItemsModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	tstring strName;

	// Analogue Value with valid EURange
	{
		Range range;
		// EURange
		range.setLow(0.0);
		range.setHigh(1000.0);
		strName = _T("TCAnalogItem_Item1");
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, strName),
			Statics::DataTypeId_Double,
			LocalizedText(strName, _T("en")),
			QualifiedName(strName,2),
			range);

		// InstrumentRange
		range.setLow(-9999.99);
		range.setHigh(9999.99);
		analogItem->setInstrumentRange(range);

		// Property EngineeringUnits
		EUInformation euInformation;
		euInformation.setDescription(LocalizedText(_T("Kilometers per Hour"), _T("en")));
		euInformation.setDisplayName(LocalizedText(_T("km/h"), _T("en")));
		analogItem->setEngineeringUnits(euInformation);

		// Property Definition
		analogItem->setDefinition(_T("Track Speed"));

		// Property ValuePrecision
		analogItem->setPrecision(1.0);

		Value value;
		value.setDouble(10.0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// Analogue Value with invalid EURange
	{
		Range range;
		// EURange
		range.setLow(1000.0);
		range.setHigh(0.0);
		strName = _T("TCAnalogItem_Item2");
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, strName),
			Statics::DataTypeId_Double,
			LocalizedText(strName, _T("en")),
			QualifiedName(strName,2),
			range);

		// InstrumentRange
		range.setLow(-2000.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		Value value;
		value.setDouble(10.0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}

	// create analog items of different data types
	// float
	{
		Range range;
		// EURange
		range.setLow(-1000.0);
		range.setHigh(1000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_Float")),
			Statics::DataTypeId_Float,
			LocalizedText(_T("TCAnalogItem_Float"), _T("en")),
			QualifiedName(_T("TCAnalogItem_Float"),0),
			range);

		// InstrumentRange
		range.setLow(-2000.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		// Property ValuePrecision
		analogItem->setPrecision(1.0);

		Value value;
		value.setFloat(20.0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// Int16
	{
		Range range;
		// EURange
		range.setLow(-1000.0);
		range.setHigh(1000.0);
		// different initialization calls with init(NULL)
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create();
		analogItem->setNodeId(NodeId(1, _T("TCAnalogItem_Int16")));
		analogItem->init(NULL);
		analogItem->setDataType(Statics::DataTypeId_Int16);
		analogItem->setDisplayName(LocalizedText(_T("TCAnalogItem_Int16"), _T("en")));
		analogItem->setBrowseName(QualifiedName(_T("TCAnalogItem_Int16"), 1));
		analogItem->setEURange(range);

		// InstrumentRange
		range.setLow(-2000.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		Value value;
		value.setInt16(-20);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// UInt16
	{
		Range range;
		// EURange
		range.setLow(0.0);
		range.setHigh(1000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_UInt16")),
			Statics::DataTypeId_UInt16,
			LocalizedText(_T("TCAnalogItem_UInt16"), _T("en")),
			QualifiedName(_T("TCAnalogItem_UInt16"),0),
			range);

		// InstrumentRange
		range.setLow(0.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		Value value;
		value.setUInt16(20);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// Int32
	{
		Range range;
		// EURange
		range.setLow(-1000.0);
		range.setHigh(1000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_Int32")),
			Statics::DataTypeId_Int32,
			LocalizedText(_T("TCAnalogItem_Int32"), _T("en")),
			QualifiedName(_T("TCAnalogItem_Int32"),0),
			range);

		// InstrumentRange
		range.setLow(-2000.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		analogItem->setMinimumSamplingInterval(100);
		_ASSERTION(analogItem->getMinimumSamplingInterval() == 100, _T("Minimum sampling interval is not 100ms"));

		Value value;
		value.setInt32(-20);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// UInt32
	{
		Range range;
		// EURange
		range.setLow(0.0);
		range.setHigh(1000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_UInt32")),
			Statics::DataTypeId_UInt32,
			LocalizedText(_T("TCAnalogItem_UInt32"), _T("en")),
			QualifiedName(_T("TCAnalogItem_UInt32"),0),
			range);

		// InstrumentRange
		range.setLow(0.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		Value value;
		value.setUInt32(20);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// Int64
	{
		Range range;
		// EURange
		range.setLow(-1000.0);
		range.setHigh(1000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_Int64")),
			Statics::DataTypeId_Int64,
			LocalizedText(_T("TCAnalogItem_Int64"), _T("en")),
			QualifiedName(_T("TCAnalogItem_Int64"),0),
			range);

		// InstrumentRange
		range.setLow(-2000.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		Value value;
		value.setInt64(20);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// UInt64
	{
		Range range;
		// EURange
		range.setLow(0.0);
		range.setHigh(1000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_UInt64")),
			Statics::DataTypeId_UInt64,
			LocalizedText(_T("TCAnalogItem_UInt64"), _T("en")),
			QualifiedName(_T("TCAnalogItem_UInt64"),0),
			range);

		// InstrumentRange
		range.setLow(0.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		Value value;
		value.setUInt64(20);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// Byte
	{
		Range range;
		// EURange
		range.setLow(0.0);
		range.setHigh(100.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_Byte")),
			Statics::DataTypeId_Byte,
			LocalizedText(_T("TCAnalogItem_Byte"), _T("en")),
			QualifiedName(_T("TCAnalogItem_Byte"),0),
			range);

		// InstrumentRange
		range.setLow(0.0);
		range.setHigh(200.0);
		analogItem->setInstrumentRange(range);

		Value value;
		value.setUInt8(20);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// SByte
	{
		Range range;
		// EURange
		range.setLow(-50);
		range.setHigh(50.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_SByte")),
			Statics::DataTypeId_SByte,
			LocalizedText(_T("TCAnalogItem_SByte"), _T("en")),
			QualifiedName(_T("TCAnalogItem_SByte"),0),
			range);

		// InstrumentRange
		range.setLow(-100.0);
		range.setHigh(100.0);
		analogItem->setInstrumentRange(range);

		Value value;
		value.setInt8((signed char)-20);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// Int16 Array
	{
		Range range;
		// EURange
		range.setLow(-1000.0);
		range.setHigh(1000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_Int16Array")),
			Statics::DataTypeId_Int16,
			LocalizedText(_T("TCAnalogItem_Int16Array"), _T("en")),
			QualifiedName(_T("TCAnalogItem_Int16Array"),0),
			range);

		// InstrumentRange
		range.setLow(-2000.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		analogItem->setValueRank(1);

		std::vector<short> int16Array;
		int16Array.push_back(-20);
		int16Array.push_back(-10);
		int16Array.push_back(10);
		int16Array.push_back(20);

		Value value;
		value.setInt16Array(int16Array);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// Int32 Array
	{
		Range range;
		// EURange
		range.setLow(-1000.0);
		range.setHigh(1000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_Int32Array")),
			Statics::DataTypeId_Int32,
			LocalizedText(_T("TCAnalogItem_Int32Array"), _T("en")),
			QualifiedName(_T("TCAnalogItem_Int32Array"),0),
			range);

		// InstrumentRange
		range.setLow(-2000.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		analogItem->setValueRank(1);

		std::vector<OTInt32> int32Array;
		int32Array.push_back(-20);
		int32Array.push_back(-10);
		int32Array.push_back(10);
		int32Array.push_back(20);

		Value value;
		value.setInt32Array(int32Array);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// UInt16 Array
	{
		Range range;
		// EURange
		range.setLow(0.0);
		range.setHigh(2000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_UInt16Array")),
			Statics::DataTypeId_UInt16,
			LocalizedText(_T("TCAnalogItem_UInt16Array"), _T("en")),
			QualifiedName(_T("TCAnalogItem_UInt16Array"),0),
			range);

		// InstrumentRange
		range.setLow(0.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		analogItem->setValueRank(1);

		std::vector<unsigned short> uint16Array;
		uint16Array.push_back(0);
		uint16Array.push_back(10);
		uint16Array.push_back(20);
		uint16Array.push_back(30);

		Value value;
		value.setUInt16Array(uint16Array);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// UInt32 Array
	{
		Range range;
		// EURange
		range.setLow(0.0);
		range.setHigh(2000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_UInt32Array")),
			Statics::DataTypeId_UInt32,
			LocalizedText(_T("TCAnalogItem_UInt32Array"), _T("en")),
			QualifiedName(_T("TCAnalogItem_UInt32Array"),0),
			range);

		// InstrumentRange
		range.setLow(0.0);
		range.setHigh(2500.0);
		analogItem->setInstrumentRange(range);

		analogItem->setValueRank(1);

		std::vector<OTUInt32> uint32Array;
		uint32Array.push_back(0);
		uint32Array.push_back(10);
		uint32Array.push_back(20);
		uint32Array.push_back(30);

		Value value;
		value.setUInt32Array(uint32Array);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// Float Array
	{
		Range range;
		// EURange
		range.setLow(-1000.0);
		range.setHigh(1000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_FloatArray")),
			Statics::DataTypeId_Float,
			LocalizedText(_T("TCAnalogItem_FloatArray"), _T("en")),
			QualifiedName(_T("TCAnalogItem_FloatArray"),0),
			range);

		// InstrumentRange
		range.setLow(-2000.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		analogItem->setValueRank(1);

		std::vector<float> floatArray;
		floatArray.push_back(-20.0f);
		floatArray.push_back(-10.0f);
		floatArray.push_back(10.0f);
		floatArray.push_back(20.0f);

		Value value;
		value.setFloatArray(floatArray);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}
	// Double Array
	{
		Range range;
		// EURange
		range.setLow(-1000.0);
		range.setHigh(1000.0);
		AnalogItemPtr analogItem= AnalogItemForAuditEvents::create(NodeId(1, _T("TCAnalogItem_DoubleArray")),
			Statics::DataTypeId_Double,
			LocalizedText(_T("TCAnalogItem_DoubleArray"), _T("en")),
			QualifiedName(_T("TCAnalogItem_DoubleArray"),0),
			range);

		// InstrumentRange
		range.setLow(-2000.0);
		range.setHigh(2000.0);
		analogItem->setInstrumentRange(range);

		analogItem->setValueRank(1);

		std::vector<double> doubleArray;
		doubleArray.push_back(-20.0f);
		doubleArray.push_back(-10.0f);
		doubleArray.push_back(10.0f);
		doubleArray.push_back(20.0f);

		Value value;
		value.setDoubleArray(doubleArray);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		analogItem->setDataValue(&dataValue);

		analogItem->setAccessLevel(analogItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
	}

	return result;
}

EnumStatusCode DiscreteItemsModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}
	tstring strName;

	// add method to allow clients to change the semantic of TwoStateItems
	ChangeTwoStateItemSemanticsMethodPtr ChangeTwoStateItemsSemanticsMethod = new ChangeTwoStateItemSemanticsMethod(this);

	// add 5 two-state items (1+4)
	{
		// call with init(NULL)
		OTChar buff[20];
		_stprintf(buff, _T("TwoStateItem_%d"), 1);
		TwoStateItemPtr twoStateItem = TwoStateItemForAuditEvents::create();
		twoStateItem->setNodeId(NodeId(1, buff));
		twoStateItem->setDataType(Statics::DataTypeId_Boolean);
		twoStateItem->init(NULL);
		twoStateItem->setDisplayName(LocalizedText(buff, _T("en")));
		twoStateItem->setBrowseName(QualifiedName(buff, 1));
		twoStateItem->setTrueText(LocalizedText(_T("on"), _T("en")));
		twoStateItem->setFalseText(LocalizedText(_T("off"), _T("en")));
		_stprintf(buff, _T("Power switch %d"), 1);
		twoStateItem->setDefinition(buff);
		twoStateItem->setAccessLevel(twoStateItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, twoStateItem);
	}
	for (int i=1; i<5; i++)
	{
		OTChar buff[20];
		_stprintf(buff, _T("TwoStateItem_%d"), i+1);
		TwoStateItemPtr twoStateItem = TwoStateItemForAuditEvents::create(NodeId(1, buff),
			LocalizedText(buff, _T("en")),
			QualifiedName(buff, 1),
			LocalizedText(_T("on"), _T("en")),
			LocalizedText(_T("off"), _T("en")));
		_stprintf(buff, _T("Power switch %d"), i);
		twoStateItem->setDefinition(buff);
		twoStateItem->setAccessLevel(twoStateItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		this->addReference(Statics::ReferenceTypeId_HasComponent, twoStateItem);
	}

	// add 5 multi-state items (1+4)
	{
		// call with init(NULL)
		OTChar buff[20];
		_stprintf(buff, _T("MultiStateItem_%d"), 1);
		MultiStateItemPtr multiStateItem = MultiStateItemForAuditEvents::create();
		multiStateItem->setNodeId(NodeId(1, buff));
		multiStateItem->init(NULL);
		multiStateItem->setDisplayName(LocalizedText(buff, _T("en")));
		multiStateItem->setBrowseName(QualifiedName(buff, 1));
		multiStateItem->setDataType(Statics::DataTypeId_UInt16); // first shall have UInt16 as data type
		OTUInt32 state;
		state = multiStateItem->addState(LocalizedText(_T("empty"), _T("en")));
		_ASSERTION(state != MultiStateItem::INVALID_STATE, _T("addState failed"));
		_UNUSED(state);
		multiStateItem->addState(LocalizedText(_T("filling"), _T("en")));
		multiStateItem->addState(LocalizedText(_T("full"), _T("en")));
		multiStateItem->setDefinition(_T("0 = empty, 1 = filling, 2 = full"));
		multiStateItem->setAccessLevel(multiStateItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		multiStateItem->setState(0);
		this->addReference(Statics::ReferenceTypeId_HasComponent, multiStateItem);
	}
	for (int i=1; i<5; i++)
	{
		OTChar buff[20];
		_stprintf(buff, _T("MultiStateItem_%d"), i+1);
		MultiStateItemPtr multiStateItem = MultiStateItemForAuditEvents::create(NodeId(1, buff),
			LocalizedText(buff, _T("en")),
			QualifiedName(buff, 1));
		multiStateItem->setDataType(Statics::DataTypeId_UInt32); // the others shall have UInt32 as data type
		OTUInt32 state;
		state = multiStateItem->addState(LocalizedText(_T("empty"), _T("en")));
		_ASSERTION(state != MultiStateItem::INVALID_STATE, _T("addState failed"));
		_UNUSED(state);
		multiStateItem->addState(LocalizedText(_T("filling"), _T("en")));
		multiStateItem->addState(LocalizedText(_T("full"), _T("en")));
		multiStateItem->setDefinition(_T("0 = empty, 1 = filling, 2 = full"));
		multiStateItem->setAccessLevel(multiStateItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		multiStateItem->setState(0);
		this->addReference(Statics::ReferenceTypeId_HasComponent, multiStateItem);
	}

	// Add one multi-state value item with manual configuration (with EnumValues)
	// This variable shall NOT reject write requests with "BadOutOfRange" since it is a "normal" variable!
	{
		OTChar buff[50];
		DataValue dataValue;
		Value varValue;
		_stprintf(buff, _T("MultiStateItemValue_1"));
		VariablePtr multiStateItem = TestVariableForAuditEvents::create();
		multiStateItem->setNodeId(NodeId(1, buff));
		multiStateItem->setBrowseName(QualifiedName(buff, 1));
		multiStateItem->setDisplayName(LocalizedText(buff, _T("en")));
		multiStateItem->setDataType(Statics::DataTypeId_Int64);
		multiStateItem->setAccessLevel(multiStateItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

		// add the EnumValues property
		_stprintf(buff, _T("MultiStateItemValue_EnumValues_1"));
		VariablePtr enumValuesVar = TestVariableForAuditEvents::create();
		enumValuesVar->setNodeId(NodeId(1, buff));
		enumValuesVar->setBrowseName(QualifiedName(BrowseName_EnumValues, 0));
		enumValuesVar->setDisplayName(LocalizedText(BrowseName_EnumValues, _T("")));
		enumValuesVar->setDataType(NodeId(EnumNumericNodeId_EnumValueType));
		enumValuesVar->setValueRank(1);
		enumValuesVar->setAccessLevel(EnumAccessLevel_CurrentRead);
		enumValuesVar->init(Statics::VariableTypeId_PropertyType);

		// fill the data
		std::vector<EnumValueType> enumArray;
		EnumValueType enum1;

		enum1.setDisplayName(LocalizedText(_T("empty"), _T("en")));
		enum1.setValue(1);
		enumArray.push_back(enum1);
		enum1.setDisplayName(LocalizedText(_T("filling"), _T("en")));
		enum1.setValue(2);
		enumArray.push_back(enum1);
		enum1.setDisplayName(LocalizedText(_T("full"), _T("en")));
		enum1.setValue(4);
		enumArray.push_back(enum1);

		varValue.setEnumValueTypeArray(enumArray);
		dataValue.setValue(varValue);
		enumValuesVar->setDataValue(dataValue);

		multiStateItem->addReference(Statics::ReferenceTypeId_HasProperty, enumValuesVar);

		// add the ValeuAsText property
		_stprintf(buff, _T("MultiStateItemValue_ValueAsText_1"));
		VariablePtr valuesAsTextVar = TestVariableForAuditEvents::create();
		valuesAsTextVar->setNodeId(NodeId(1, buff));
		valuesAsTextVar->setBrowseName(QualifiedName(BrowseName_ValueAsText, 0));
		valuesAsTextVar->setDisplayName(LocalizedText(BrowseName_ValueAsText, _T("")));
		valuesAsTextVar->setDataType(NodeId(EnumNumericNodeId_LocalizedText));
		valuesAsTextVar->setAccessLevel(EnumAccessLevel_CurrentRead);
		valuesAsTextVar->init(Statics::VariableTypeId_PropertyType);
		multiStateItem->addReference(Statics::ReferenceTypeId_HasProperty, valuesAsTextVar);

		multiStateItem->init(Statics::VariableTypeId_MultiStateValueDiscreteType);
		multiStateItem->insertTree();

		varValue.setInt64(1);
		dataValue.setValue(varValue);
		multiStateItem->setDataValue(dataValue);

		this->addReference(Statics::ReferenceTypeId_HasComponent, multiStateItem);
	}

	// Add one multi-state value item with SDK support (with EnumValues)
	// This variable SHALL reject write requests with "BadOutOfRange" since check is performed in MultiStateValueItem
	{
		OTChar buff[50];
		_stprintf(buff, _T("MultiStateItemValue_ValueAsText_2"));
		NodeId  valueAsTextNodeId(1, buff);
		_stprintf(buff, _T("MultiStateItemValue_EnumValues_2"));
		NodeId  enumValuesNodeId(1, buff);
		_stprintf(buff, _T("MultiStateItemValue_2"));
		NodeId  itemNodeId(1, buff);
		MultiStateValueItemPtr multiStateItem = MultiStateValueItem::create(itemNodeId, LocalizedText(buff, _T("en")), QualifiedName(buff, 1), Statics::DataTypeId_Int64,
			&enumValuesNodeId, &valueAsTextNodeId);
		multiStateItem->setAccessLevel(multiStateItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

		// fill the data
		EnumValueType enumVal;

		enumVal.setDisplayName(LocalizedText(_T("empty"), _T("en")));
		enumVal.setValue(1);
		multiStateItem->addState(enumVal);

		enumVal.setDisplayName(LocalizedText(_T("filling"), _T("en")));
		enumVal.setValue(2);
		multiStateItem->addState(enumVal);

		enumVal.setDisplayName(LocalizedText(_T("full"), _T("en")));
		enumVal.setValue(4);
		multiStateItem->addState(enumVal);

		multiStateItem->setState(Value((OTInt64)2));

		this->addReference(Statics::ReferenceTypeId_HasComponent, multiStateItem);
	}

	{
		OTChar buff[50];
		// add an object type with a analog item property, instantiate this and validate that the correct C++ objecet is constructed
		ObjectTypePtr anaObjectType = ObjectType::create();
		ObjectTypePtr baseObjectType = AddressSpaceRoot::instance()->getObjectType(Statics::ObjectTypeId_BaseObjectType);
		_stprintf(buff, _T("AnaObjectType"));
		NodeId anaObjectTypeNodeId(1, buff);
		anaObjectType->setNodeId(anaObjectTypeNodeId);
		anaObjectType->setBrowseName(QualifiedName(buff, 1));
		anaObjectType->setDisplayName(LocalizedText(buff, _T("")));
		baseObjectType->addReference(Statics::ReferenceTypeId_HasSubtype, anaObjectType);

		// add the mandatory component analog item
		_stprintf(buff, _T("MyAnalogItem"));
		QualifiedName anaItemBrowseName(buff, 1);
		Range anaItemRange(20.0, 80.0);
		AnalogItemPtr analogItem = AnalogItemForAuditEvents::create(NodeId(1, buff), Statics::DataTypeId_Double, LocalizedText(buff, _T("")), anaItemBrowseName, anaItemRange);
		NodeId mandatoryModellingRuleId(EnumNumericNodeId_ModellingRule_Mandatory);
		analogItem->addReference(Statics::ReferenceTypeId_HasModellingRule, AddressSpaceRoot::instance()->getNode(&mandatoryModellingRuleId));
		anaObjectType->addReference(Statics::ReferenceTypeId_HasComponent, analogItem);
		anaObjectType->insertTree();

		// instantiate the object type
		_stprintf(buff, _T("AnaObject"));
		ObjectPtr anaObject = Object::create();
		anaObject->setNodeId(NodeId(1, buff));
		anaObject->setBrowseName(QualifiedName(buff, 1));
		anaObject->setDisplayName(LocalizedText(buff, _T("")));
		anaObject->init(&anaObjectTypeNodeId);
		this->addReference(Statics::ReferenceTypeId_HasComponent, anaObject);

		// test correct C++ object by invoking a method
		VariablePtr varAnaItem = anaObject->getVariable(anaItemBrowseName);
		analogItem = (AnalogItem*)varAnaItem.p();
		analogItem->setEURange(Range(0.0, 1.0));

		// remove these objects / types again
		anaObject->removeTree();
		anaObjectType->removeTree();
	}

	return result;
}

EnumStatusCode AttributeItemsModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	tstring strName;
	this->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName | EnumWriteMask_WriteMask);

	// add a variable
	VariablePtr varItem = TestVariableForAuditEvents::create();
	strName = _T("VarItem_Attr");
	varItem->setNodeId(NodeId(namespaceIndex, strName));
	varItem->setDisplayName(LocalizedText(strName, _T("en")));
	varItem->setBrowseName(QualifiedName(strName, namespaceIndex));
	varItem->setAccessLevel(varItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	varItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName | EnumWriteMask_WriteMask);
	varItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	varItem->insert();
	this->addReference(Statics::ReferenceTypeId_HasComponent, varItem);

	// add a method
	MethodPtr methodItem = new TestMethod(0,0);
	strName = _T("MethodItem_Attr");
	methodItem->setNodeId(NodeId(namespaceIndex, strName));
	methodItem->setBrowseName(QualifiedName(strName, namespaceIndex));
	methodItem->setDisplayName(LocalizedText(strName, _T("en")));
	methodItem->setExecutable(true);
	methodItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName | EnumWriteMask_WriteMask | EnumWriteMask_Executable);
	methodItem->insert();
	this->addReference(Statics::ReferenceTypeId_HasComponent, methodItem);

	// add a object
	ObjectPtr objectItem = Object::create();
	strName = _T("Object_Attr");
	objectItem->setNodeId(NodeId(namespaceIndex, strName));
	objectItem->setBrowseName(QualifiedName(strName, namespaceIndex));
	objectItem->setDisplayName(LocalizedText(strName, _T("en")));
	objectItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName | EnumWriteMask_WriteMask);
	objectItem->init(Statics::ObjectTypeId_FolderType);
	this->addReference(Statics::ReferenceTypeId_HasComponent, objectItem);

	return result;
}

EnumStatusCode DynamicHistoricalItemsModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	this->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName | EnumWriteMask_WriteMask);

#if TB5_HISTORY
	tstring strName;
	HistoricalDataConfigurationPtr historicalDataConfiguration;
	{
		// the HA configuration might has already been created by the StaticHistoricalItemsModule
		tstring dataConfigName = BrowseName_HAConfiguration;
		NodeId haConfigurationId(namespaceIndex, dataConfigName);

		ObjectPtr haConfigObject = AddressSpaceRoot::instance()->getObject(&haConfigurationId);
		if(haConfigObject.isNotNull() &&
			haConfigObject->getApiClass() == EnumApiClassType_ServerHistoricalDataConfiguration)
		{
			historicalDataConfiguration = static_pointer_cast<HistoricalDataConfiguration>(haConfigObject);
		}

		// create if if it does not yet exist
		if(historicalDataConfiguration.isNull())
		{
			//
			// Add HistoricalDataConfiguration object that defines support for the "Average" aggregate
			//
			Server::AggregateConfigurationPtr aggregateConfiguration = Server::AggregateConfiguration::create();
			strName = BrowseName_AggregateConfiguration;
			aggregateConfiguration->setNodeId(NodeId(namespaceIndex, strName));
			aggregateConfiguration->setDisplayName(LocalizedText(strName, _T("en")));
			aggregateConfiguration->setBrowseName(QualifiedName(strName, namespaceIndex));
			aggregateConfiguration->init(NULL);

			historicalDataConfiguration = HistoricalDataConfiguration::create(
				haConfigurationId,
				LocalizedText(dataConfigName, _T("en")),
				QualifiedName(dataConfigName, 0),
				aggregateConfiguration);

			ObjectPtr aggregateFunctions = historicalDataConfiguration->getObject(BrowseName_AggregateFunctions);
			NodeId averageFunctionNodeId(EnumNumericNodeId_AggregateFunction_Average);
			ObjectPtr averageFunction = AddressSpaceRoot::instance()->getObject(&averageFunctionNodeId);
			if(aggregateFunctions && averageFunction)
			{
				aggregateFunctions->addReference(Statics::ReferenceTypeId_Organizes, averageFunction);
			}
		}
	}

	// add a variable allowing historical read access (UInt32)
	{
		HistorizingVariablePtr historicalDataItem = new HistorizingVariable(5);
		strName = _T("DynamicHistoricalDataItem_UInt32");
		historicalDataItem->setNodeId(NodeId(namespaceIndex, strName));
		historicalDataItem->setDisplayName(LocalizedText(strName, _T("en")));
		historicalDataItem->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalDataItem->setAccessLevel(historicalDataItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_HistoryRead);
		historicalDataItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName | EnumWriteMask_WriteMask);
		historicalDataItem->setDataType(NodeId(0, EnumDataTypeId_UInt32));
		historicalDataItem->setDescription(LocalizedText(_T("A variable which historizes the five last written values"), _T("en")));
		historicalDataItem->setIsHistorizing(true);
		historicalDataItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		historicalDataItem->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, historicalDataItem);
		historicalDataItem->addReference(Statics::ReferenceTypeId_HasHistoricalConfiguration, historicalDataConfiguration);

		Value value;
		value.setUInt32(0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		historicalDataItem->setDataValue(&dataValue);
	}

	// add a variable allowing historical read access (Byte)
	{
		HistorizingVariablePtr historicalDataItem = new HistorizingVariable(5);
		strName = _T("DynamicHistoricalDataItem_Byte");
		historicalDataItem->setNodeId(NodeId(namespaceIndex, strName));
		historicalDataItem->setDisplayName(LocalizedText(strName, _T("en")));
		historicalDataItem->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalDataItem->setAccessLevel(historicalDataItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_HistoryRead);
		historicalDataItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName | EnumWriteMask_WriteMask);
		historicalDataItem->setDataType(NodeId(0, EnumDataTypeId_Byte));
		historicalDataItem->setDescription(LocalizedText(_T("A variable which historizes the five last written values"), _T("en")));
		historicalDataItem->setIsHistorizing(true);
		historicalDataItem->insert();
		historicalDataItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		this->addReference(Statics::ReferenceTypeId_HasComponent, historicalDataItem);
		historicalDataItem->addReference(Statics::ReferenceTypeId_HasHistoricalConfiguration, historicalDataConfiguration);

		Value value;
		value.setUInt8(0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		historicalDataItem->setDataValue(&dataValue);
	}

	// add a variable allowing historical read access (Int32)
	{
		HistorizingVariablePtr historicalDataItem = new HistorizingVariable(5);
		strName = _T("DynamicHistoricalDataItem_Int32");
		historicalDataItem->setNodeId(NodeId(namespaceIndex, strName));
		historicalDataItem->setDisplayName(LocalizedText(strName, _T("en")));
		historicalDataItem->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalDataItem->setAccessLevel(historicalDataItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_HistoryRead);
		historicalDataItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName | EnumWriteMask_WriteMask);
		historicalDataItem->setDataType(NodeId(0, EnumDataTypeId_Int32));
		historicalDataItem->setDescription(LocalizedText(_T("A variable which historizes the five last written values"), _T("en")));
		historicalDataItem->setIsHistorizing(true);
		historicalDataItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		historicalDataItem->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, historicalDataItem);
		historicalDataItem->addReference(Statics::ReferenceTypeId_HasHistoricalConfiguration, historicalDataConfiguration);

		Value value;
		value.setInt32(0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		historicalDataItem->setDataValue(&dataValue);
	}

	if(!ApplicationModule::s_accessPatternEnabled)
	{
		// add a variable allowing writing of historizing bit
		EnumPermissionType commonRights = EnumPermissionType_Browse | EnumPermissionType_Read | EnumPermissionType_Write | EnumPermissionType_ReadHistory;
		std::vector<RolePermissionType> rolePermissions;
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdDefault);
		rolePermissions.back().setPermissions(commonRights);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup1);
		rolePermissions.back().setPermissions(commonRights | EnumPermissionType_WriteHistorizing);
		rolePermissions.push_back(RolePermissionType());
		rolePermissions.back().setRoleId(ApplicationModule::s_RoleIdGroup2);
		rolePermissions.back().setPermissions(commonRights);

		HistorizingVariablePtr historicalDataItem = new HistorizingVariable(5);
		strName = _T("HistorizingVarWriteMask");
		historicalDataItem->setNodeId(NodeId(namespaceIndex, strName));
		historicalDataItem->setDisplayName(LocalizedText(strName, _T("en")));
		historicalDataItem->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalDataItem->setAccessLevel(historicalDataItem->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_HistoryRead);
		// allow writing of historizing bit in general
		historicalDataItem->setWriteMask(EnumWriteMask_Historizing);
		historicalDataItem->setDataType(NodeId(0, EnumDataTypeId_UInt32));
		historicalDataItem->setDescription(LocalizedText(_T("A variable which can be change historizing (group 1)"), _T("en")));
		historicalDataItem->setIsHistorizing(true);
		historicalDataItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);

		historicalDataItem->setRolePermissions(rolePermissions);
		historicalDataItem->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, historicalDataItem);
		historicalDataItem->addReference(Statics::ReferenceTypeId_HasHistoricalConfiguration, historicalDataConfiguration);

		Value value;
		value.setUInt32(0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		historicalDataItem->setDataValue(&dataValue);
	}


#endif

	return result;
}

EnumStatusCode StaticHistoricalItemsModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	this->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName | EnumWriteMask_WriteMask);

#if TB5_HISTORY
	tstring strName;
	HistoricalDataConfigurationPtr historicalDataConfiguration;
	{
		// the HA configuration might has already been created by the DynamicHistoricalItemsModule
		tstring dataConfigName = BrowseName_HAConfiguration;
		NodeId haConfigurationId(namespaceIndex, dataConfigName);
		ObjectPtr haConfigObject = AddressSpaceRoot::instance()->getObject(&haConfigurationId);
		if(haConfigObject.isNotNull() &&
			haConfigObject->getApiClass() == EnumApiClassType_ServerHistoricalDataConfiguration)
		{
			historicalDataConfiguration = static_pointer_cast<HistoricalDataConfiguration>(haConfigObject);
		}

		// create if if it does not yet exist
		if(historicalDataConfiguration.isNull())
		{
			//
			// Add HistoricalDataConfiguration object that defines support for the "Average" aggregate
			//
			Server::AggregateConfigurationPtr aggregateConfiguration = Server::AggregateConfiguration::create();
			strName = BrowseName_AggregateConfiguration;
			aggregateConfiguration->setNodeId(NodeId(namespaceIndex, strName));
			aggregateConfiguration->setDisplayName(LocalizedText(strName, _T("en")));
			aggregateConfiguration->setBrowseName(QualifiedName(strName, namespaceIndex));
			aggregateConfiguration->init(NULL);

			historicalDataConfiguration = HistoricalDataConfiguration::create(
				haConfigurationId,
				LocalizedText(dataConfigName, _T("en")),
				QualifiedName(dataConfigName, 0),
				aggregateConfiguration);
		}
	}

	// add a variable allowing historical read access (UInt32)
	{
		HistorizingVariablePtr historicalDataItem = new HistorizingVariable();
		strName = _T("StaticHistoricalDataItem_UInt32");
		historicalDataItem->setNodeId(NodeId(namespaceIndex, strName));
		historicalDataItem->setDisplayName(LocalizedText(strName, _T("en")));
		historicalDataItem->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalDataItem->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead);
		historicalDataItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName);
		historicalDataItem->setDataType(NodeId(0, EnumDataTypeId_UInt32));
		historicalDataItem->setIsHistorizing(true);
		historicalDataItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		historicalDataItem->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, historicalDataItem);
		historicalDataItem->addReference(Statics::ReferenceTypeId_HasHistoricalConfiguration, historicalDataConfiguration);

		Value value;
		value.setUInt32(0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		historicalDataItem->setDataValue(&dataValue);

		// adding a history
		DateTime timestamp;

		value.setUInt32(10);
		timestamp.parse(_T("2011-01-01T12:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt32(20);
		timestamp.parse(_T("2011-01-01T13:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt32(30);
		timestamp.parse(_T("2011-01-01T14:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt32(20);
		timestamp.parse(_T("2011-01-01T15:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt32(10);
		timestamp.parse(_T("2011-01-01T16:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);
	}

	// add a variable allowing historical read access (Byte)
	{
		HistorizingVariablePtr historicalDataItem = new HistorizingVariable();
		strName = _T("StaticHistoricalDataItem_Byte");
		historicalDataItem->setNodeId(NodeId(namespaceIndex, strName));
		historicalDataItem->setDisplayName(LocalizedText(strName, _T("en")));
		historicalDataItem->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalDataItem->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead);
		historicalDataItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName);
		historicalDataItem->setDataType(NodeId(0, EnumDataTypeId_Byte));
		historicalDataItem->setIsHistorizing(true);
		historicalDataItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		historicalDataItem->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, historicalDataItem);
		historicalDataItem->addReference(Statics::ReferenceTypeId_HasHistoricalConfiguration, historicalDataConfiguration);

		Value value;
		value.setUInt8(0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		historicalDataItem->setDataValue(&dataValue);


		// adding a history
		DateTime timestamp;

		value.setUInt8(30);
		timestamp.parse(_T("2011-01-01T12:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt8(20);
		timestamp.parse(_T("2011-01-01T13:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt8(10);
		timestamp.parse(_T("2011-01-01T14:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt8(20);
		timestamp.parse(_T("2011-01-01T15:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt8(30);
		timestamp.parse(_T("2011-01-01T16:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);
	}

	// add a variable allowing historical read access (Int32)
	{
		HistorizingVariablePtr historicalDataItem = new HistorizingVariable();
		strName = _T("StaticHistoricalDataItem_Int32");
		historicalDataItem->setNodeId(NodeId(namespaceIndex, strName));
		historicalDataItem->setDisplayName(LocalizedText(strName, _T("en")));
		historicalDataItem->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalDataItem->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead);
		historicalDataItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName);
		historicalDataItem->setDataType(NodeId(0, EnumDataTypeId_Int32));
		// grant HistoryRead access to anonymous but not to authenticated users
		ApplicationModule::assignAccessPatternAndRolePermissions(historicalDataItem, ApplicationModule::s_AccessKey_ReadHistory_Read_Read_None_None);

		historicalDataItem->setIsHistorizing(true);
		historicalDataItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		historicalDataItem->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, historicalDataItem);
		historicalDataItem->addReference(Statics::ReferenceTypeId_HasHistoricalConfiguration, historicalDataConfiguration);

		Value value;
		value.setInt32(0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		historicalDataItem->setDataValue(&dataValue);


		// adding a history
		DateTime timestamp;

		value.setInt32(-20);
		timestamp.parse(_T("2011-01-01T12:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setInt32(-10);
		timestamp.parse(_T("2011-01-01T13:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setInt32(0);
		timestamp.parse(_T("2011-01-01T14:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_UncertainSensorNotAccurate);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setInt32(-10);
		timestamp.parse(_T("2011-01-01T15:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_BadNoCommunication);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setInt32(-20);
		timestamp.parse(_T("2011-01-01T16:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);
	}

	// add a variable allowing historical read access (Int16)
	{
		HistorizingVariablePtr historicalDataItem = new HistorizingVariable();
		strName = _T("StaticHistoricalDataItem_Int16");
		historicalDataItem->setNodeId(NodeId(namespaceIndex, strName));
		historicalDataItem->setDisplayName(LocalizedText(strName, _T("en")));
		historicalDataItem->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalDataItem->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead);
		historicalDataItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName);
		historicalDataItem->setDataType(NodeId(0, EnumDataTypeId_Int16));
		// grant HistoryRead access to anonymous but not to authenticated users
		ApplicationModule::assignAccessPatternAndRolePermissions(historicalDataItem, ApplicationModule::s_AccessKey_ReadHistory_Read_Read_None_None);
		historicalDataItem->setIsHistorizing(true);
		historicalDataItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		historicalDataItem->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, historicalDataItem);
		historicalDataItem->addReference(Statics::ReferenceTypeId_HasHistoricalConfiguration, historicalDataConfiguration);

		Value value;
		value.setInt16(0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		historicalDataItem->setDataValue(&dataValue);


		// adding a history
		DateTime timestamp;

		value.setInt16(-20);
		timestamp.parse(_T("2011-01-01T12:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setInt16(-10);
		timestamp.parse(_T("2011-01-01T13:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setInt16(0);
		timestamp.parse(_T("2011-01-01T14:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_UncertainSensorNotAccurate);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setInt16(-10);
		timestamp.parse(_T("2011-01-01T15:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_BadNoCommunication);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setInt16(-20);
		timestamp.parse(_T("2011-01-01T16:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);
	}

	// add a variable allowing historical read access (UInt16)
	{
		HistorizingVariablePtr historicalDataItem = new HistorizingVariable();
		strName = _T("StaticHistoricalDataItem_UInt16");
		historicalDataItem->setNodeId(NodeId(namespaceIndex, strName));
		historicalDataItem->setDisplayName(LocalizedText(strName, _T("en")));
		historicalDataItem->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalDataItem->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead);
		historicalDataItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName);
		historicalDataItem->setDataType(NodeId(0, EnumDataTypeId_UInt16));
		historicalDataItem->setIsHistorizing(true);
		// grant HistoryRead access to anonymous but not to authenticated users
		ApplicationModule::assignAccessPatternAndRolePermissions(historicalDataItem, ApplicationModule::s_AccessKey_ReadHistory_Read_Read_None_None);
		historicalDataItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		historicalDataItem->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, historicalDataItem);
		historicalDataItem->addReference(Statics::ReferenceTypeId_HasHistoricalConfiguration, historicalDataConfiguration);

		Value value;
		value.setUInt16(0);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		historicalDataItem->setDataValue(&dataValue);


		// adding a history
		DateTime timestamp;

		value.setUInt16(30);
		timestamp.parse(_T("2011-01-01T12:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt16(15);
		timestamp.parse(_T("2011-01-01T13:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt16(18);
		timestamp.parse(_T("2011-01-01T14:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_UncertainSensorNotAccurate);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt16(25);
		timestamp.parse(_T("2011-01-01T15:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_BadNoCommunication);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		value.setUInt16(23);
		timestamp.parse(_T("2011-01-01T16:00:00Z"));
		dataValue.setValue(value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);
	}

	// add a variable allowing historical read access (Int32Array)
	{
		HistorizingVariablePtr historicalDataItem = new HistorizingVariable();
		strName = _T("StaticHistoricalDataItem_Int32Array");
		historicalDataItem->setNodeId(NodeId(namespaceIndex, strName));
		historicalDataItem->setDisplayName(LocalizedText(strName, _T("en")));
		historicalDataItem->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalDataItem->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_HistoryRead);
		historicalDataItem->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName);
		historicalDataItem->setDataType(NodeId(0, EnumDataTypeId_Int32));
		historicalDataItem->setValueRank(1);
		historicalDataItem->setIsHistorizing(true);
		historicalDataItem->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		historicalDataItem->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, historicalDataItem);
		historicalDataItem->addReference(Statics::ReferenceTypeId_HasHistoricalConfiguration, historicalDataConfiguration);

		std::vector<OTInt32> int32Array;
		int32Array.push_back(0);
		int32Array.push_back(0);
		int32Array.push_back(0);

		Value value;
		value.setInt32Array(int32Array);
		DataValue dataValue;
		dataValue.setValue(&value);
		dataValue.setStatusCode(EnumStatusCode_Good);
		historicalDataItem->setDataValue(&dataValue);


		// adding a history
		DateTime timestamp;

		int32Array.clear();
		int32Array.push_back(-20);
		int32Array.push_back(0);
		int32Array.push_back(20);
		value.setInt32Array(int32Array);
		timestamp.parse(_T("2011-01-01T12:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		int32Array.clear();
		int32Array.push_back(-10);
		int32Array.push_back(0);
		int32Array.push_back(10);
		value.setInt32Array(int32Array);
		timestamp.parse(_T("2011-01-01T13:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		int32Array.clear();
		int32Array.push_back(0);
		int32Array.push_back(0);
		int32Array.push_back(0);
		value.setInt32Array(int32Array);
		timestamp.parse(_T("2011-01-01T14:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		int32Array.clear();
		int32Array.push_back(-10);
		int32Array.push_back(0);
		int32Array.push_back(10);
		value.setInt32Array(int32Array);
		timestamp.parse(_T("2011-01-01T15:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);

		int32Array.clear();
		int32Array.push_back(-20);
		int32Array.push_back(0);
		int32Array.push_back(20);
		value.setInt32Array(int32Array);
		timestamp.parse(_T("2011-01-01T16:00:00Z"));
		dataValue.setValue(value);
		dataValue.setServerTimestamp(timestamp);
		dataValue.setSourceTimestamp(timestamp);
		historicalDataItem->addHistoricalValue(dataValue);
	}
#endif
	return result;
}

EnumStatusCode HistoricalEventsModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	this->setWriteMask(EnumWriteMask_BrowseName | EnumWriteMask_Description | EnumWriteMask_DisplayName | EnumWriteMask_WriteMask);

#if TB5_HISTORY
	tstring strName;
	// add a historizing event
	{
		ObjectPointer<HistorizingExclusiveLimitAlarm> historicalAlarm = new HistorizingExclusiveLimitAlarm();
		strName = _T("HistoricalExclusiveLimitAlarm");
		historicalAlarm->setNodeId(NodeId(namespaceIndex, strName));
		historicalAlarm->setDisplayName(LocalizedText(strName, _T("en")));
		historicalAlarm->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalAlarm->setConditionName(strName);
		historicalAlarm->setEventNotifier(EnumEventNotifier_SubscribeToEvents | EnumEventNotifier_HistoryRead);
		historicalAlarm->init(NULL);
		this->addReference(Statics::ReferenceTypeId_Organizes, historicalAlarm);
		this->addReference(Statics::ReferenceTypeId_HasNotifier, historicalAlarm);

		NodeId eventTypeId;
		historicalAlarm->getTypeNodeId(&eventTypeId);

		// Add the HistoricalEventFilter property. This property serves as a hint
		// for the server (and client) to determine which event fields are historized.
		VariablePtr historicalEventFilterVariable = TestVariableForAuditEvents::create();
		strName = _T("HistoricalExclusiveLimitAlarm.HistoricalEventFilter");
		historicalEventFilterVariable->setNodeId(NodeId(namespaceIndex, strName));
		historicalEventFilterVariable->setDisplayName(LocalizedText(BrowseName_HistoricalEventFilter, _T("en")));
		historicalEventFilterVariable->setBrowseName(QualifiedName(BrowseName_HistoricalEventFilter, 0));
		historicalEventFilterVariable->setAccessLevel(EnumAccessLevel_CurrentRead);
		historicalEventFilterVariable->setDataType(Statics::DataTypeId_EventFilter);
		historicalEventFilterVariable->init(Statics::VariableTypeId_PropertyType);
		historicalAlarm->addReference(Statics::ReferenceTypeId_HasProperty, historicalEventFilterVariable);

		// LimitState / CurrentState / Name
		EventFilter historicalEventFilter;

		std::vector<QualifiedName> limitStateCurrentStateName;
		limitStateCurrentStateName.push_back(QualifiedName(BrowseName_LimitState, 0));
		limitStateCurrentStateName.push_back(QualifiedName(BrowseName_CurrentState, 0));
		limitStateCurrentStateName.push_back(QualifiedName(BrowseName_Name, 0));

		SimpleAttributeOperand historicalEventFilterSelectClause;
		historicalEventFilterSelectClause.setAttributeId(EnumAttributeId_Value);
		historicalEventFilterSelectClause.setTypeDefinitionId(&eventTypeId);
		for(size_t i = 0; i < limitStateCurrentStateName.size(); i++)
		{
			const QualifiedName& browsePathElement = limitStateCurrentStateName.at(i);
			historicalEventFilterSelectClause.addBrowsePath(&browsePathElement);
		}
		historicalEventFilter.addSelectClause(&historicalEventFilterSelectClause);

		// ActiveState / Id
		std::vector<QualifiedName> activeStateId;
		activeStateId.push_back(QualifiedName(BrowseName_ActiveState, 0));
		activeStateId.push_back(QualifiedName(BrowseName_Id, 0));

		historicalEventFilterSelectClause.clear();
		historicalEventFilterSelectClause.setAttributeId(EnumAttributeId_Value);
		historicalEventFilterSelectClause.setTypeDefinitionId(&eventTypeId);
		for(size_t i = 0; i < activeStateId.size(); i++)
		{
			const QualifiedName& browsePathElement = activeStateId.at(i);
			historicalEventFilterSelectClause.addBrowsePath(&browsePathElement);
		}
		historicalEventFilter.addSelectClause(&historicalEventFilterSelectClause);

		// UnderlyingAlarmVariable
		QualifiedName underlyingAlarmVariable(_T("UnderlyingAlarmVariable"), 0);

		historicalEventFilterSelectClause.clear();
		historicalEventFilterSelectClause.setAttributeId(EnumAttributeId_Value);
		historicalEventFilterSelectClause.setTypeDefinitionId(&eventTypeId);
		historicalEventFilterSelectClause.addBrowsePath(&underlyingAlarmVariable);

		historicalEventFilter.addSelectClause(&historicalEventFilterSelectClause);

		DataValue historicalEventFilterDataValue;
		historicalEventFilterDataValue.setSourceTimestamp(DateTime::getUtcNow());
		historicalEventFilterDataValue.setServerTimestamp(DateTime::getUtcNow());
		Value historicalEventFilterValue(&historicalEventFilter);
		historicalEventFilterDataValue.setValue(&historicalEventFilterValue);
		historicalEventFilterVariable->setDataValue(&historicalEventFilterDataValue);

		// add an event history
		DateTime time;
		Value value;
		{
			time.parse(_T("2011-01-01T12:00:00Z"));
			HistoricalEvent historicalEvent(eventTypeId, time);

			// LimitState/CurrentState/Name
			QualifiedName limitStateName(BrowseName_High, 0);
			value.setQualifiedName(&limitStateName);
			historicalEvent.addEventField(limitStateCurrentStateName, EnumAttributeId_Value, value);

			// ActiveState/Id
			value.setBoolean(true);
			historicalEvent.addEventField(activeStateId, EnumAttributeId_Value, value);

			// UnderlyingAlarmVariable
			const unsigned char underlyingAlarmVariableValue[] = {0xDE, 0xAD, 0xBE, 0xEF};
			ByteString byteString(sizeof(underlyingAlarmVariableValue), underlyingAlarmVariableValue);
			value.setByteString(byteString);
			historicalEvent.addEventField(underlyingAlarmVariable, EnumAttributeId_Value, value);

			historicalAlarm->addHistoricalEvent(historicalEvent);
		}
		{
			time.parse(_T("2011-01-01T13:00:00Z"));
			HistoricalEvent historicalEvent(eventTypeId, time);

			// LimitState/CurrentState/Name
			QualifiedName limitStateName(BrowseName_HighHigh, 0);
			value.setQualifiedName(&limitStateName);
			historicalEvent.addEventField(limitStateCurrentStateName, EnumAttributeId_Value, value);

			// ActiveState/Id
			value.setBoolean(true);
			historicalEvent.addEventField(activeStateId, EnumAttributeId_Value, value);

			// UnderlyingAlarmVariable
			const unsigned char underlyingAlarmVariableValue[] = {0xFE, 0xED, 0xBE, 0xEF};
			ByteString byteString(sizeof(underlyingAlarmVariableValue), underlyingAlarmVariableValue);
			value.setByteString(byteString);
			historicalEvent.addEventField(underlyingAlarmVariable, EnumAttributeId_Value, value);

			historicalAlarm->addHistoricalEvent(historicalEvent);
		}
		{
			time.parse(_T("2011-01-01T14:00:00Z"));
			HistoricalEvent historicalEvent(eventTypeId, time);

			// LimitState/CurrentState/Name
			QualifiedName limitStateName(BrowseName_High, 0);
			value.setQualifiedName(&limitStateName);
			historicalEvent.addEventField(limitStateCurrentStateName, EnumAttributeId_Value, value);

			// ActiveState/Id
			value.setBoolean(true);
			historicalEvent.addEventField(activeStateId, EnumAttributeId_Value, value);

			// UnderlyingAlarmVariable
			const unsigned char underlyingAlarmVariableValue[] = {0xFE, 0xED};
			ByteString byteString(sizeof(underlyingAlarmVariableValue), underlyingAlarmVariableValue);
			value.setByteString(byteString);
			historicalEvent.addEventField(underlyingAlarmVariable, EnumAttributeId_Value, value);

			historicalAlarm->addHistoricalEvent(historicalEvent);
		}
		{
			time.parse(_T("2011-01-01T15:00:00Z"));
			HistoricalEvent historicalEvent(eventTypeId, time);

			// LimitState/CurrentState/Name
			QualifiedName limitStateName;
			value.setQualifiedName(&limitStateName);
			historicalEvent.addEventField(limitStateCurrentStateName, EnumAttributeId_Value, value);

			// ActiveState/Id
			value.setBoolean(false);
			historicalEvent.addEventField(activeStateId, EnumAttributeId_Value, value);

			// UnderlyingAlarmVariable
			const unsigned char underlyingAlarmVariableValue[] = {0xBE, 0xEF};
			ByteString byteString(sizeof(underlyingAlarmVariableValue), underlyingAlarmVariableValue);
			value.setByteString(byteString);
			historicalEvent.addEventField(underlyingAlarmVariable, EnumAttributeId_Value, value);

			historicalAlarm->addHistoricalEvent(historicalEvent);
		}
		{
			time.parse(_T("2011-01-01T16:00:00Z"));
			HistoricalEvent historicalEvent(eventTypeId, time);

			// LimitState/CurrentState/Name
			QualifiedName limitStateName(BrowseName_Low, 0);
			value.setQualifiedName(&limitStateName);
			historicalEvent.addEventField(limitStateCurrentStateName, EnumAttributeId_Value, value);

			// ActiveState/Id
			value.setBoolean(true);
			historicalEvent.addEventField(activeStateId, EnumAttributeId_Value, value);

			// UnderlyingAlarmVariable
			const unsigned char underlyingAlarmVariableValue[] = {0xDE, 0xAD, 0xBE, 0xEF};
			ByteString byteString(sizeof(underlyingAlarmVariableValue), underlyingAlarmVariableValue);
			value.setByteString(byteString);
			historicalEvent.addEventField(underlyingAlarmVariable, EnumAttributeId_Value, value);

			historicalAlarm->addHistoricalEvent(historicalEvent);
		}
	}

	// add another historizing event
	{
		ObjectPointer<HistorizingExclusiveLimitAlarm> historicalAlarm = new HistorizingExclusiveLimitAlarm();
		strName = _T("AnotherHistoricalExclusiveLimitAlarm");
		historicalAlarm->setNodeId(NodeId(namespaceIndex, strName));
		historicalAlarm->setDisplayName(LocalizedText(strName, _T("en")));
		historicalAlarm->setBrowseName(QualifiedName(strName, namespaceIndex));
		historicalAlarm->setConditionName(strName);
		historicalAlarm->setEventNotifier(EnumEventNotifier_SubscribeToEvents | EnumEventNotifier_HistoryRead);
		historicalAlarm->init(NULL);
		this->addReference(Statics::ReferenceTypeId_Organizes, historicalAlarm);
		this->addReference(Statics::ReferenceTypeId_HasNotifier, historicalAlarm);

		NodeId eventTypeId;
		historicalAlarm->getTypeNodeId(&eventTypeId);

		// Add the HistoricalEventFilter property. This property serves as a hint
		// for the server (and client) to determine which event fields are historized.
		VariablePtr historicalEventFilterVariable = TestVariableForAuditEvents::create();
		strName = _T("AnotherHistoricalExclusiveLimitAlarm.HistoricalEventFilter");
		historicalEventFilterVariable->setNodeId(NodeId(namespaceIndex, strName));
		historicalEventFilterVariable->setDisplayName(LocalizedText(BrowseName_HistoricalEventFilter, _T("en")));
		historicalEventFilterVariable->setBrowseName(QualifiedName(BrowseName_HistoricalEventFilter, 0));
		historicalEventFilterVariable->setAccessLevel(EnumAccessLevel_CurrentRead);
		historicalEventFilterVariable->setDataType(Statics::DataTypeId_EventFilter);
		historicalEventFilterVariable->init(Statics::VariableTypeId_PropertyType);
		historicalAlarm->addReference(Statics::ReferenceTypeId_HasProperty, historicalEventFilterVariable);

		// LimitState / CurrentState / Name
		EventFilter historicalEventFilter;

		std::vector<QualifiedName> limitStateCurrentStateName;
		limitStateCurrentStateName.push_back(QualifiedName(BrowseName_LimitState, 0));
		limitStateCurrentStateName.push_back(QualifiedName(BrowseName_CurrentState, 0));
		limitStateCurrentStateName.push_back(QualifiedName(BrowseName_Name, 0));

		SimpleAttributeOperand historicalEventFilterSelectClause;
		historicalEventFilterSelectClause.setAttributeId(EnumAttributeId_Value);
		historicalEventFilterSelectClause.setTypeDefinitionId(&eventTypeId);
		for(size_t i = 0; i < limitStateCurrentStateName.size(); i++)
		{
			const QualifiedName& browsePathElement = limitStateCurrentStateName.at(i);
			historicalEventFilterSelectClause.addBrowsePath(&browsePathElement);
		}
		historicalEventFilter.addSelectClause(&historicalEventFilterSelectClause);

		// ActiveState / Id
		std::vector<QualifiedName> activeStateId;
		activeStateId.push_back(QualifiedName(BrowseName_ActiveState, 0));
		activeStateId.push_back(QualifiedName(BrowseName_Id, 0));

		historicalEventFilterSelectClause.clear();
		historicalEventFilterSelectClause.setAttributeId(EnumAttributeId_Value);
		historicalEventFilterSelectClause.setTypeDefinitionId(&eventTypeId);
		for(size_t i = 0; i < limitStateCurrentStateName.size(); i++)
		{
			const QualifiedName& browsePathElement = limitStateCurrentStateName.at(i);
			historicalEventFilterSelectClause.addBrowsePath(&browsePathElement);
		}
		historicalEventFilter.addSelectClause(&historicalEventFilterSelectClause);

		DataValue historicalEventFilterDataValue;
		historicalEventFilterDataValue.setSourceTimestamp(DateTime::getUtcNow());
		historicalEventFilterDataValue.setServerTimestamp(DateTime::getUtcNow());
		Value historicalEventFilterValue(&historicalEventFilter);
		historicalEventFilterDataValue.setValue(&historicalEventFilterValue);
		historicalEventFilterVariable->setDataValue(&historicalEventFilterDataValue);

		// add an event history
		DateTime time;
		Value value;
		{
			time.parse(_T("2011-01-01T12:00:00Z"));
			HistoricalEvent historicalEvent(eventTypeId, time);

			// LimitState/CurrentState/Name
			QualifiedName limitStateName(BrowseName_Low, 0);
			value.setQualifiedName(&limitStateName);
			historicalEvent.addEventField(limitStateCurrentStateName, EnumAttributeId_Value, value);

			// ActiveState/Id
			value.setBoolean(true);
			historicalEvent.addEventField(activeStateId, EnumAttributeId_Value, value);

			historicalAlarm->addHistoricalEvent(historicalEvent);
		}
		{
			time.parse(_T("2011-01-01T13:00:00Z"));
			HistoricalEvent historicalEvent(eventTypeId, time);

			// LimitState/CurrentState/Name
			QualifiedName limitStateName(BrowseName_LowLow, 0);
			value.setQualifiedName(&limitStateName);
			historicalEvent.addEventField(limitStateCurrentStateName, EnumAttributeId_Value, value);

			// ActiveState/Id
			value.setBoolean(true);
			historicalEvent.addEventField(activeStateId, EnumAttributeId_Value, value);

			historicalAlarm->addHistoricalEvent(historicalEvent);
		}
		{
			time.parse(_T("2011-01-01T14:00:00Z"));
			HistoricalEvent historicalEvent(eventTypeId, time);

			// LimitState/CurrentState/Name
			QualifiedName limitStateName(BrowseName_Low, 0);
			value.setQualifiedName(&limitStateName);
			historicalEvent.addEventField(limitStateCurrentStateName, EnumAttributeId_Value, value);

			// ActiveState/Id
			value.setBoolean(true);
			historicalEvent.addEventField(activeStateId, EnumAttributeId_Value, value);

			historicalAlarm->addHistoricalEvent(historicalEvent);
		}
		{
			time.parse(_T("2011-01-01T15:00:00Z"));
			HistoricalEvent historicalEvent(eventTypeId, time);

			// LimitState/CurrentState/Name
			QualifiedName limitStateName;
			value.setQualifiedName(&limitStateName);
			historicalEvent.addEventField(limitStateCurrentStateName, EnumAttributeId_Value, value);

			// ActiveState/Id
			value.setBoolean(false);
			historicalEvent.addEventField(activeStateId, EnumAttributeId_Value, value);

			historicalAlarm->addHistoricalEvent(historicalEvent);
		}
		{
			time.parse(_T("2011-01-01T16:00:00Z"));
			HistoricalEvent historicalEvent(eventTypeId, time);

			// LimitState/CurrentState/Name
			QualifiedName limitStateName(BrowseName_High, 0);
			value.setQualifiedName(&limitStateName);
			historicalEvent.addEventField(limitStateCurrentStateName, EnumAttributeId_Value, value);

			// ActiveState/Id
			value.setBoolean(true);
			historicalEvent.addEventField(activeStateId, EnumAttributeId_Value, value);

			historicalAlarm->addHistoricalEvent(historicalEvent);
		}
	}
#endif

	return result;
}
