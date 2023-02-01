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
#include "DynamicTestVariable.h"
#include "ServerDataTransaction.h"
#include "ServerReadRequest.h"
#include "ServerWriteRequest.h"

#include <iostream>

/**
* @brief Constructor
*/
DynamicTestVariable::DynamicTestVariable (const std::vector<DataValue>& dataValues)
: TestVariableForAuditEvents(),
m_dataValues(dataValues),
m_idx(0)
{
}

/**
* @brief Constructor
* @param otbHandle Void pointer
*/
DynamicTestVariable::DynamicTestVariable(void* otbHandle)
: TestVariableForAuditEvents(otbHandle),
m_idx(0)
{
}

/**
* @brief Destructor
*/
DynamicTestVariable::~DynamicTestVariable()
{
}

/**
* @brief Change the value of the test variable
*/
void DynamicTestVariable::changeValue(DateTime &now)
{
	if(m_dataValues.size() > 0)
	{
		m_idx++;
		if(m_idx >= m_dataValues.size())
			m_idx = 0;
		//int idx = rand() % m_dataValues.size();


		m_dataValues[m_idx].setServerTimestamp(now);
		m_dataValues[m_idx].setSourceTimestamp(now);
		setDataValue(m_dataValues[m_idx]);
	}
}
