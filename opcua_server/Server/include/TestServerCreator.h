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
#ifndef _TestServerCreator_H_
#define _TestServerCreator_H_

#include "Creator.h"
#include "ClientSession.h"
#include "ServerSubscription.h"
#include "ServerDataTransaction.h"


/* @brief Overloaded Creator */
class TestServerCreator : public SoftingOPCToolbox5::Creator
{
public:
	virtual SoftingOPCToolbox5::Application* createApplication(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::AddressSpaceRoot* createServerAddressSpaceRoot(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::Session* createServerSession(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::Subscription* createServerSubscription(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::Endpoint* createServerEndpoint(TBHandle otbHandle);

	//////////////////////////////////////////////////////////////////////////
	// Transactions
	//////////////////////////////////////////////////////////////////////////

	virtual SoftingOPCToolbox5::Server::ReadTransaction* createServerReadTransaction(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::WriteTransaction* createServerWriteTransaction(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::MethodCallTransaction* createServerMethodCallTransaction(TBHandle otbHandle);

#if TB5_HISTORY
	virtual SoftingOPCToolbox5::Server::HistoryReadRawTransaction* createServerHistoryReadRawTransaction(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::HistoryReadProcessedTransaction* createServerHistoryReadProcessedTransaction(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::HistoryReadEventTransaction* createServerHistoryReadEventTransaction(TBHandle otbHandle);
#endif

	virtual SoftingOPCToolbox5::Server::BrowseTransaction* createServerBrowseTransaction(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::MonitoredItem* createServerMonitoredItem(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::BrowseNextTransaction* createServerBrowseNextTransaction(TBHandle otbHandle);

	virtual SoftingOPCToolbox5::Server::TranslateBrowsePathsToNodeIdsTransaction* createServerTranslateBrowsePathsToNodeIdsTransaction(TBHandle otbHandle);

};

#endif
