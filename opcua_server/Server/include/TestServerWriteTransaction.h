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
#ifndef _TestServerWriteTransaction_H_
#define _TestServerWriteTransaction_H_

#include "ServerVariable.h"
#include "ClientSession.h"
#include "ServerReadTransaction.h"
#include "ServerWriteTransaction.h"
#include "ServerReadRequest.h"
#include "ServerWriteRequest.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

/**
 * @brief Overloaded Write transaction with support for index ranges
 */
class TestServerWriteTransaction
	: public Server::WriteTransaction
{
public:
	/*! @brief Constructor */
	TestServerWriteTransaction(TBHandle tbHandle);

	/*! @brief Destructor */
	virtual ~TestServerWriteTransaction();

	/*! @brief Overloaded handle requests */
	virtual EnumStatusCode handleRequests();

private:
};

#endif // _TestServerWriteTransaction_H_
