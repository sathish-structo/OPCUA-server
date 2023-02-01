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
#include "ServerMonitoredItem.h"
#include "ServerBaseNode.h"
#include "TestServerSession.h"
#include "TestServerSubscription.h"
#include "DynamicNodeCache.h"
#include "ServerModifyMonitoredItemRequest.h"
#include "ServerDeleteMonitoredItemRequest.h"
#include "ServerDeleteSubscriptionRequest.h"
#include "ToolkitTrace.h"
#include "MonitoredItemsAsyncManipulation.h"
#include "ApplicationModule.h"
#include "SubscriptionAsyncManipulation.h"
#include "ServerAddressSpaceRoot.h"
#include "CApi.h"

extern bool g_cttCompatible;

// #define TRACE_SUBSCRIPTIONS
#define TRACE_REGISTERNODES
// #define TRACE_MONITORED_ITEM_SETTINGS

/* @brief Constructor
* @param otbHandle OTB handle */
TestServerSubscription::TestServerSubscription(void* otbHandle)
: Server::Subscription(otbHandle)
{
	// in the TestServerSubscription::onCreate() an addref on the subscription object keeps tis instance alive
#ifdef TRACE_SUBSCRIPTIONS
	_tprintf(_T("TestServerSubscription::TestServerSubscription %p\n"), this);
#endif //TRACE_SUBSCRIPTIONS
}

/* @brief Destructor */
TestServerSubscription::~TestServerSubscription()
{
	// in the TestServerSubscription::onDelete() an release on the subscription object allows desctuction
#ifdef TRACE_SUBSCRIPTIONS
	_tprintf(_T("TestServerSubscription::~TestServerSubscription %p\n"), this);
#endif //TRACE_SUBSCRIPTIONS
}

EnumStatusCode TestServerSubscription::onCreate()
{
	if(g_cttCompatible)
	{
		// CTT performs strange tests regarding client handles
		enableClientHandleValidation(false);
	}
	return EnumStatusCode_Good;
}

EnumStatusCode TestServerSubscription::handleDelete(Server::DeleteSubscriptionRequestPtr deleteSubscriptionRequest)
{
	DeleteSubscriptionAction* pAction = new DeleteSubscriptionAction(deleteSubscriptionRequest, this);
	OTUInt32 delay = getApplicationModule()->getServiceDelay(EnumNumericNodeId_DeleteSubscriptionsRequest);

	if(delay != 0)  
	{
		// service shall be executed asynchronously with a delay
		pAction->setAutoDelete(true);
		pAction->setTimeout(delay);
		if(getApplicationModule()->addAction(pAction))
		{
			// ownership successfully transferred to the TimerThread
			pAction = NULL;
		}
	}

	if(pAction)
	{
		// either no delay is set or add action failed
		// ==> execute synchronous
		pAction->execute();
		delete pAction;
	}

	return EnumStatusCode_Good;
}

EnumStatusCode TestServerSubscription::handleCreateMonitoredItems(const std::vector<Server::CreateMonitoredItemRequestPtr>& requests)
{
	CreateMonitoredItemsAction* pAction = new CreateMonitoredItemsAction(requests, this);
	OTUInt32 delay = getApplicationModule()->getServiceDelay(EnumNumericNodeId_CreateMonitoredItemsRequest);

	for(size_t i=0; i<requests.size(); i++)
	{
		Server::MonitoredItemPtr mi = requests[i]->getMonitoredItem();
		if((mi != NULL) && mi->isDataItem() && (mi->getAttributeId() == EnumAttributeId_Value))
		{
			if(mi->getNodeId()->compare(TestVariable::getDirectHandledVarNodeId()) == 0)
			{
				TestVariablePtr directHandledVar = (TestVariable*)(AddressSpaceRoot::instance()->getNode(TestVariable::getDirectHandledVarNodeId()).p());
				if(directHandledVar)
				{
					directHandledVar->addMonitoredIItem(mi);
				}
			}
		}
	}

	if(delay != 0)  
	{
		// service shall be executed asynchronously with a delay
		pAction->setAutoDelete(true);
		pAction->setTimeout(delay);
		if(getApplicationModule()->addAction(pAction))
		{
			// ownership successfully transferred to the TimerThread
			pAction = NULL;
		}
	}

	if(pAction)
	{
		// either no delay is set or add action failed
		// ==> execute synchronous
		pAction->execute();
		delete pAction;
	}
	return EnumStatusCode_Good;
}

void TestServerSubscription::handleModifyMonitoredItems(const std::vector<Server::ModifyMonitoredItemRequestPtr>& requests)
{
	ModifyMonitoredItemsAction* pAction = new ModifyMonitoredItemsAction(requests, this);
	OTUInt32 delay = getApplicationModule()->getServiceDelay(EnumNumericNodeId_ModifyMonitoredItemsRequest);

	if(delay != 0)  
	{
		// service shall be executed asynchronously with a delay
		pAction->setAutoDelete(true);
		pAction->setTimeout(delay);
		if(getApplicationModule()->addAction(pAction))
		{
			// ownership successfully transferred to the TimerThread
			pAction = NULL;
		}
	}

	if(pAction)
	{
		// either no delay is set or add action failed
		// ==> execute synchronous
		pAction->execute();
		delete pAction;
	}
}

void TestServerSubscription::handleDeleteMonitoredItems(const std::vector<Server::DeleteMonitoredItemRequestPtr>& requests)
{

	RemoveMonitoredItemsAction* pAction = new RemoveMonitoredItemsAction(requests, this);
	OTUInt32 delay = getApplicationModule()->getServiceDelay(EnumNumericNodeId_DeleteMonitoredItemsRequest);

	for(size_t i=0; i<requests.size(); i++)
	{
		Server::MonitoredItemPtr mi = requests[i]->getMonitoredItem();
		if((mi != NULL) && mi->isDataItem() && (mi->getAttributeId() == EnumAttributeId_Value))
		{
			if(mi->getNodeId()->compare(TestVariable::getDirectHandledVarNodeId()) == 0)
			{
				TestVariablePtr directHandledVar = (TestVariable*)(AddressSpaceRoot::instance()->getNode(TestVariable::getDirectHandledVarNodeId()).p());
				if(directHandledVar)
				{
					directHandledVar->removeMonitoredItem(mi);
				}
			}
		}
	}

	if(delay != 0)  
	{
		// service shall be executed asynchronously with a delay
		pAction->setAutoDelete(true);
		pAction->setTimeout(delay);
		if(getApplicationModule()->addAction(pAction))
		{
			// ownership successfully transferred to the TimerThread
			pAction = NULL;
		}
	}

	if(pAction)
	{
		// either no delay is set or add action failed
		// ==> execute synchronous
		pAction->execute();
		delete pAction;
	}
}

std::vector<EnumInitialValueProvider> TestServerSubscription::onProvideInitialValues(const std::vector<Server::MonitoredItem*>& monitoredItems)
{
	std::vector<EnumInitialValueProvider> result;

	for(size_t i=0; i<monitoredItems.size(); i++)
	{
		EnumInitialValueProvider provideResult = EnumInitialValueProvider_CurrentAttributeValue;
		if(monitoredItems[i]->getNodeId()->compare(TestVariable::getDirectHandledVarNodeId()) == 0)
		{
			TestVariablePtr directHandledVar = (TestVariable*)(AddressSpaceRoot::instance()->getNode(TestVariable::getDirectHandledVarNodeId()).p());
			if(directHandledVar)
			{
				provideResult = directHandledVar->onProvideInitialValues(monitoredItems[i]);
			}
		}
		result.push_back(provideResult);
	}

	return result;
}
