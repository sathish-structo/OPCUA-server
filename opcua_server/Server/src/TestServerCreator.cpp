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
#include "TestServerCreator.h"
#include "StructoAutomoldApplication.h"
#include "TestServerReadTransaction.h"
#include "TestServerWriteTransaction.h"
#include "TestServerMethodCallTransaction.h"
#include "TestServerHistoryReadRawTransaction.h"
#include "TestServerHistoryReadProcessedTransaction.h"
#include "TestServerBrowseTransaction.h"
#include "TestServerHistoryReadEventTransaction.h"
#include "TestServerSession.h"
#include "TestServerSubscription.h"
#include "TestServerAddressSpaceRoot.h"
#include "TestServerMonitoredItem.h"
#include "TestServerBrowseNextTransaction.h"
#include "TestServerTranslateBrowsePathsToNodeIdsTransaction.h"
#include "TestEndpoint.h"

SoftingOPCToolbox5::Application* TestServerCreator::createApplication(TBHandle otbHandle)
{
	return new StructoAutomoldApplication(otbHandle);
}

Server::ReadTransaction* TestServerCreator::createServerReadTransaction(TBHandle otbHandle)
{
	return new TestServerReadTransaction(otbHandle);
}

Server::WriteTransaction* TestServerCreator::createServerWriteTransaction(TBHandle otbHandle)
{
	return new TestServerWriteTransaction(otbHandle);
}

Server::MethodCallTransaction* TestServerCreator::createServerMethodCallTransaction(TBHandle otbHandle)
{
	return new TestServerMethodCallTransaction(otbHandle);
}

#if TB5_HISTORY
Server::HistoryReadRawTransaction* TestServerCreator::createServerHistoryReadRawTransaction(TBHandle otbHandle)
{
	return new TestServerHistoryReadRawTransaction(otbHandle);
}

Server::HistoryReadProcessedTransaction* TestServerCreator::createServerHistoryReadProcessedTransaction(TBHandle otbHandle)
{
	return new TestServerHistoryReadProcessedTransaction(otbHandle);
}

Server::HistoryReadEventTransaction* TestServerCreator::createServerHistoryReadEventTransaction(TBHandle otbHandle)
{
	return new TestServerHistoryReadEventTransaction(otbHandle);
}
#endif

Server::BrowseTransaction* TestServerCreator::createServerBrowseTransaction(TBHandle otbHandle)
{
	return new TestServerBrowseTransaction(otbHandle);
}

Server::AddressSpaceRoot* TestServerCreator::createServerAddressSpaceRoot(TBHandle otbHandle)
{
	return new TestServerAddressSpaceRoot(otbHandle);
}

Server::Session* TestServerCreator::createServerSession(TBHandle otbHandle)
{
	return new TestServerSession(otbHandle);
}

Server::Subscription* TestServerCreator::createServerSubscription( TBHandle otbHandle )
{
	return new TestServerSubscription(otbHandle);
}

Server::Endpoint* TestServerCreator::createServerEndpoint(TBHandle otbHandle)
{
	return new TestEndpoint(otbHandle);
}

Server::MonitoredItem* TestServerCreator::createServerMonitoredItem(TBHandle otbHandle)
{
	return new TestServerMonitoredItem(otbHandle);
}

SoftingOPCToolbox5::Server::BrowseNextTransaction* TestServerCreator::createServerBrowseNextTransaction( TBHandle otbHandle )
{
	return new TestServerBrowseNextTransaction(otbHandle);
}

SoftingOPCToolbox5::Server::TranslateBrowsePathsToNodeIdsTransaction* TestServerCreator::createServerTranslateBrowsePathsToNodeIdsTransaction(TBHandle otbHandle)
{
	return new TestServerTranslateBrowsePathsToNodeIdsTransaction(otbHandle);
}

