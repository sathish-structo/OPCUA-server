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
#ifndef _TESTSERVER_SUBSCRIPTION_H_
#define _TESTSERVER_SUBSCRIPTION_H_

#include "ServerSubscription.h"

/* @brief Overloaded subscription */

// due to onCreateMonitoredItems
class TestServerSubscription
	: public Server::Subscription
{
public:

	/*! @brief Constructor */
	TestServerSubscription(void* otbHandle);

	virtual ~TestServerSubscription();

	virtual EnumStatusCode onCreate();
	virtual EnumStatusCode handleDelete(Server::DeleteSubscriptionRequestPtr deleteSubscriptionRequest);

	virtual EnumStatusCode handleCreateMonitoredItems(const std::vector<Server::CreateMonitoredItemRequestPtr>& requests);
	virtual void handleModifyMonitoredItems(const std::vector<Server::ModifyMonitoredItemRequestPtr>& requests);
	virtual void handleDeleteMonitoredItems(const std::vector<Server::DeleteMonitoredItemRequestPtr>& requests);
	virtual std::vector<EnumInitialValueProvider> onProvideInitialValues(const std::vector<Server::MonitoredItem*>& monitoredItems);

	std::set<OTUInt32> m_monitoredItemTestIds;
};

typedef ObjectPointer<TestServerSubscription> TestServerSubscriptionPtr;
typedef ObjectPointer<const TestServerSubscription> TestServerSubscriptionConstPtr;

#endif
