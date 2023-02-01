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
#include "SimulationVariable.h"
#include "ServerReadTransaction.h"
#include "ServerReadRequest.h"
#include "ServerWriteRequest.h"
#include <limits>
#include "ServerDummyValueGenerator.h"
#include "ServerWriteTransaction.h"
#include "ServerSession.h"

/**
 * @brief Constructor
 */
SimulationVariable::SimulationVariable()
 : TestVariableForAuditEvents()
{
	m_baseValue = 0;
}

/**
 * @brief Constructor
 * @param otbHandle OTB handle
 */
SimulationVariable::SimulationVariable(void* otbHandle)
 : TestVariableForAuditEvents(otbHandle)
{
	m_baseValue = 0;
}

/**
 * @brief Destructor
 */
SimulationVariable::~SimulationVariable()
{
}

/**
 * @brief Request handling
 * @param pTransaction Pointer to transaction
 * @param pRequest Pointer to data request
 * @return void
 */
EnumStatusCode SimulationVariable::handleReadRequest(Server::ReadTransaction* pTransaction, ReadRequest* pRequest)
{
	EnumAttributeId attributeId = pRequest->getValueId()->getAttributeId();

	if (attributeId == EnumAttributeId_Value)
	{ // data value
		DataValue value;
		if (pRequest->getValueId()->getIndexRange()->getDimensions() > 0)
		{
			// simulation variables never are arrays
			value.setStatusCode(EnumStatusCode_BadIndexRangeNoData);
		}
		else
		{
			value = getDataValue();
		}
		pTransaction->updateTimeStamps(&value, attributeId);
		pRequest->setDataValue(&value);
		pRequest->complete();
	}
	else
	{
		return Variable::handleReadRequest(pTransaction, pRequest);
	}
	return EnumStatusCode_Good;
}

void SimulationVariable::incrementValue(double inc/* = 1.0 */, DateTime *pTimestamp /* = NULL */, DataValue* pDataValue/* = NULL */)
{
	DataValue currentValue;
	DataValue* pLocalDataValue = pDataValue;
	if (!pLocalDataValue)
	{
		// no value passed - use local variable
		pLocalDataValue = &currentValue;
	}
	*pLocalDataValue = getDataValue();
	IValue *value = pLocalDataValue->getValue();
	// use the existing value as base value for the simple types (for scalar only)
	if(value->getArrayType() == EnumValueArrayType_Scalar)
	{
		switch(value->getDataType())
		{
		case EnumDataTypeId_Byte:
			m_baseValue = value->getUInt8();
			break;
		case EnumDataTypeId_SByte:
			m_baseValue = value->getInt8();
			break;
		case EnumDataTypeId_UInt16:
			m_baseValue = value->getUInt16();
			break;
		case EnumDataTypeId_Int16:
			m_baseValue = value->getInt16();
			break;
		case EnumDataTypeId_UInt32:
			m_baseValue = value->getUInt32();
			break;
		case EnumDataTypeId_Int32:
			m_baseValue = value->getInt32();
			break;
		case EnumDataTypeId_UInt64:
			m_baseValue = (double)value->getUInt64();
			break;
		case EnumDataTypeId_Int64:
			m_baseValue = (double)value->getInt64();
			break;
		case EnumDataTypeId_Float:
			m_baseValue = value->getFloat();
			break;
		case EnumDataTypeId_Double:
			m_baseValue = value->getDouble();
			break;
		default:
			// keep m_baseValue as it is
			break;
		}
	}
	std::vector<OTUInt32> arrayDimensions;
	switch(value->getArrayType())
	{
	case EnumValueArrayType_Scalar:
		// empty means scalar
		break;
	case EnumValueArrayType_Array:
		{
			OTInt32 length = (OTUInt32)((ValueStruct*)value->getInternHandle())->value.Array.Length;
			if(length >= 0)
			{
				arrayDimensions.push_back((OTUInt32)length);
			}
			else
			{
				arrayDimensions.push_back(0);
			}
		}
		break;
	case EnumValueArrayType_Matrix:
		{
			ValueStructMatrixValue* pMatrix = ((ValueStruct*)value->getInternHandle())->value.Matrix;
			for(OTUInt32 i = 0; i < pMatrix->NoOfDimensions; i++)
			{
				arrayDimensions.push_back(pMatrix->Dimensions[i]);
			}
		}
		break;
	}
	// increment
	m_baseValue += inc;

	value->set(DummyValueGenerator::generateDummyValue(value->getDataType(), (size_t)m_baseValue, &arrayDimensions));

	DateTime timestamp;
	if (pTimestamp == NULL)
	{
		timestamp = DateTime::getUtcNow();
		pTimestamp = &timestamp;
	}
	pLocalDataValue->setServerTimestamp(pTimestamp);
	pLocalDataValue->setSourceTimestamp(pTimestamp);

	// if caller is interested in incremented value, do not perform setting to core here
	// caller might collect values and write them as a bulk
	if(pDataValue == NULL)
	{
		setDataValue(pLocalDataValue);
	}
}

/**
 * @brief Request handling
 * @param pTransaction Pointer to transaction
 * @param pRequest Pointer to data request
 * @return void
 */
EnumStatusCode SimulationVariable::handleWriteRequest(Server::WriteTransaction* pTransaction, WriteRequest* pRequest)
{
	EnumStatusCode result = EnumStatusCode_Good;

	// Allow data type changes for simulation variables in order
	// to test the generation of ModelChangeEvents for changed data types
	if(pRequest->getWriteValue()->getAttributeId() == EnumAttributeId_DataType)
	{
		result = setAttribute(EnumAttributeId_DataType, pRequest->getWriteValue()->getDataValue(), false, pTransaction->getSession());

		pRequest->setStatusCode(result);
		pRequest->complete();
	}
	else
	{
		result = Variable::handleWriteRequest(pTransaction, pRequest);
	}

	return result;
}

