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
#ifndef _SIMULATIONVARIABLE_H_
#define _SIMULATIONVARIABLE_H_

#include "TestVariable.h"
#include "ServerDataTransaction.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

/**
 * @brief Simulation variable
 */
class SimulationVariable
	: public TestVariableForAuditEvents
{
public:

	/*! @brief Constructor */
	SimulationVariable ();

	/*! @brief Constructor */
	SimulationVariable (void* otbHandle);

	/*! @brief Destructor */
	virtual ~SimulationVariable ();

	/*! @brief Overloaded handleRequest */
	virtual EnumStatusCode handleWriteRequest(Server::WriteTransaction* pTransaction, Server::WriteRequest* pRequest);

	/*! @brief Overloaded handleRequest */
	virtual EnumStatusCode handleReadRequest(Server::ReadTransaction* pTransaction, Server::ReadRequest* pRequest);

	/*! @brief Increment the stored value */
	void incrementValue(double newValue = 1.0, DateTime *pTimestamp = NULL, DataValue* pValue = NULL);

private:
	/*! Used to generate dummy values */
	double m_baseValue;
};


#endif
