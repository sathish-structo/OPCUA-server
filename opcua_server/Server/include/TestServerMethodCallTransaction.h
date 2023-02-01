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
#ifndef _TestServerMethodCallTransaction_H_
#define _TestServerMethodCallTransaction_H_

#include "ServerMethodCallTransaction.h"

using namespace SoftingOPCToolbox5;

class TestServerMethodCallTransaction
	: public Server::MethodCallTransaction
{
public:
	/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
	TestServerMethodCallTransaction(TBHandle otbHandle);

public:
	/*! Destructor */
	virtual ~TestServerMethodCallTransaction();

	/*! Callback function to handle all read requests of the data transaction in one call.
	* That allows to sort the different requests e.g. depending on the target which contains the requested value.
	*/
	virtual EnumStatusCode handleRequests();
};

#endif // _TestServerMethodCallTransaction_H_
