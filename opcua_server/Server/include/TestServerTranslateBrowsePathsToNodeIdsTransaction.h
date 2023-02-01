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
#ifndef TESTSERVERTRANSLATEBROWSEPATHSTONODEIDSTRANSACTION_H
#define TESTSERVERTRANSLATEBROWSEPATHSTONODEIDSTRANSACTION_H

#include "ServerTranslateBrowsePathsToNodeIdsTransaction.h"
#include "ServerBaseNode.h"

class TestServerTranslateBrowsePathsToNodeIdsTransaction
	: public SoftingOPCToolbox5::Server::TranslateBrowsePathsToNodeIdsTransaction
{
public:
	/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
	TestServerTranslateBrowsePathsToNodeIdsTransaction(TBHandle otbHandle);

public:
	/*! Destructor */
	virtual ~TestServerTranslateBrowsePathsToNodeIdsTransaction();

	/*! Callback function to handle all read requests of the data transaction in one call.
	* That allows to sort the different requests e.g. depending on the target which contains the requested value. */
	virtual EnumStatusCode handleRequests();

};

#endif	// TESTSERVERTRANSLATEBROWSEPATHSTONODEIDSTRANSACTION_H
