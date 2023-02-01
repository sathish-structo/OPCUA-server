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
#ifndef _TestServerReadTransaction_H_
#define _TestServerReadTransaction_H_

#include "ServerReadTransaction.h"

using namespace SoftingOPCToolbox5;

class TestServerReadTransaction
	: public Server::ReadTransaction
{
public:
	/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
	TestServerReadTransaction(TBHandle otbHandle);

public:
	/*! Destructor */
	virtual ~TestServerReadTransaction();

	/*! Callback function to handle all read requests of the data transaction in one call.
	* That allows to sort the different requests e.g. depending on the target which contains the requested value.
	*/
	virtual EnumStatusCode handleRequests();
};

#endif // _TestServerReadTransaction_H_
