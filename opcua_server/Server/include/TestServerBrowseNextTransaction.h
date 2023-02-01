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
#ifndef _TESTSERVERBROWSENEXTTRANSACTION_H_
#define _TESTSERVERBROWSENEXTTRANSACTION_H_

#include "ServerBrowseNextTransaction.h"

class TestServerBrowseNextTransaction
	: public SoftingOPCToolbox5::Server::BrowseNextTransaction
{
public:
	/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
	TestServerBrowseNextTransaction(TBHandle otbHandle);

public:
	/*! Destructor */
	virtual ~TestServerBrowseNextTransaction();

	/*! Callback function that is called to handle all BrowseNext requests of the data transaction in one call.
	* That allows to sort the different requests e.g. depending on the target which contains the requested value.
	*
	* The default implementation handles all not completed requests.
	*
	* @note	When overriding this method you mustn't use blocking calls within this method since
	* this blocks the UA communication. */
	virtual EnumStatusCode handleRequests();
};

#endif // _TESTSERVERBROWSENEXTTRANSACTION_H_
