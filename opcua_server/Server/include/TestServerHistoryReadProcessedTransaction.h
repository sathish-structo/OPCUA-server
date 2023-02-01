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
#ifndef _TestServerHistoryReadProcessedTransaction_H_
#define _TestServerHistoryReadProcessedTransaction_H_
#if TB5_HISTORY

#include "ServerHistoryReadProcessedTransaction.h"

using namespace SoftingOPCToolbox5;

class TestServerHistoryReadProcessedTransaction
	: public Server::HistoryReadProcessedTransaction
{
public:
	/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
	TestServerHistoryReadProcessedTransaction(TBHandle otbHandle);

public:
	/*! Destructor */
	virtual ~TestServerHistoryReadProcessedTransaction();

	/*! Callback function to handle all read requests of the data transaction in one call.
	* That allows to sort the different requests e.g. depending on the target which contains the requested value.
	*/
	virtual EnumStatusCode handleRequests();

};

#endif
#endif // _TestServerHistoryReadProcessedTransaction_H_
