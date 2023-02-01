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
#include "SubscriptionAsyncManipulation.h"

//#include "TestServerSubscription.h"
#include "ApplicationModule.h"
#include "TestServerSession.h"
#include "IDataChangeFilter.h"
#include "ServerMonitoredItem.h"
#include "DynamicNodeCache.h"
#include "ToolkitTrace.h"

using namespace SoftingOPCToolbox5;

DeleteSubscriptionWorkItem::DeleteSubscriptionWorkItem(const SoftingOPCToolbox5::Server::DeleteSubscriptionRequestPtr request, SoftingOPCToolbox5::Server::SubscriptionPtr subscription)
{
	m_request = request;
	m_subscription = subscription;
}

DeleteSubscriptionWorkItem::DeleteSubscriptionWorkItem()
{
}

DeleteSubscriptionWorkItem::~DeleteSubscriptionWorkItem()
{
}

void DeleteSubscriptionWorkItem::execute()
{
	std::vector<Server::MonitoredItemPtr> monitoredItems = m_subscription->getMonitoredItems();
	_ASSERTION(monitoredItems.size() == 0, _T("MonitoredItems should be removed before onDelete is invoked"));
	for(size_t i = 0; i<monitoredItems.size(); i++)
	{
		// in case of a dynamic node some additional actions are necessary
		(void)DynamicNodeCache::instance()->handleMonitoredItemDeletion(monitoredItems[i], EnumStatusCode_Good);
	}
	m_request->complete();
}

DeleteSubscriptionAction::DeleteSubscriptionAction(const Server::DeleteSubscriptionRequestPtr request, Server::SubscriptionPtr subscription)
{
	m_request = request;
	m_subscription = subscription;
}

DeleteSubscriptionAction::~DeleteSubscriptionAction()
{
}

void DeleteSubscriptionAction::execute()
{
	DeleteSubscriptionWorkItem* workItem = new DeleteSubscriptionWorkItem(m_request, m_subscription);
	if(!getApplicationModule()->addWorkItem(workItem))
	{
		workItem->execute();
		delete workItem;
	}
}

void DeleteSubscriptionAction::cleanUp()
{
	execute();
}
