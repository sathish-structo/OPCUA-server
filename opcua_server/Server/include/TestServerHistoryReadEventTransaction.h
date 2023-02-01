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
#ifndef _TestServerHistoryReadEventTransaction_H_
#define _TestServerHistoryReadEventTransaction_H_
#if TB5_HISTORY

#include "ServerHistoryReadEventTransaction.h"

using namespace SoftingOPCToolbox5;

class TestServerHistoryReadEventTransaction
	: public Server::HistoryReadEventTransaction
{
public:
	/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
	TestServerHistoryReadEventTransaction(TBHandle otbHandle);

public:
	/*! Destructor */
	virtual ~TestServerHistoryReadEventTransaction();

	/*! Callback function to handle all read requests of the data transaction in one call.
	* That allows to sort the different requests e.g. depending on the target which contains the requested value.
	*/
	virtual EnumStatusCode handleRequests();

};

#endif
#endif // _TestServerHistoryReadEventTransaction_H_
