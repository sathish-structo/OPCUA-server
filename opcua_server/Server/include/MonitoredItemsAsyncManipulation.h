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
#ifndef MONITOREDITEMS_MANIPULATION_METHODS_H
#define MONITOREDITEMS_MANIPULATION_METHODS_H

#include "ServerMethod.h"
#include "ServerMethodCallRequest.h"
#include "Util_TimerThread.h"
//#include "TestServerSubscription.h"
#include "ServerCreateMonitoredItemRequest.h"
#include "ServerDeleteMonitoredItemRequest.h"
#include "ServerModifyMonitoredItemRequest.h"
#include "ServerSubscription.h"
#include "Util_WorkItem.h"

/*! Action to create a new MonitoredItems */
class CreateMonitoredItemsWorkItem : public Util_WorkItem
{
public:
	CreateMonitoredItemsWorkItem(const std::vector<SoftingOPCToolbox5::Server::CreateMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription);
	~CreateMonitoredItemsWorkItem();
	virtual void execute();
private:
	CreateMonitoredItemsWorkItem();
	std::vector<SoftingOPCToolbox5::Server::CreateMonitoredItemRequestPtr> m_requests;
	SoftingOPCToolbox5::Server::SubscriptionPtr m_subscription;
};
class CreateMonitoredItemsAction
	: public Util_ScheduledAction
{
public:
	CreateMonitoredItemsAction(const std::vector<SoftingOPCToolbox5::Server::CreateMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription);
	virtual ~CreateMonitoredItemsAction();
	virtual void execute();

	virtual void cleanUp();
private:
	CreateMonitoredItemsAction();
	std::vector<SoftingOPCToolbox5::Server::CreateMonitoredItemRequestPtr> m_requests;
	SoftingOPCToolbox5::Server::SubscriptionPtr m_subscription;
};

/*! Action to perform a delayed MonitoredItems removal */
class RemoveMonitoredItemsWorkItem : public Util_WorkItem
{
public:
	RemoveMonitoredItemsWorkItem(const std::vector<SoftingOPCToolbox5::Server::DeleteMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription);
	~RemoveMonitoredItemsWorkItem();
	virtual void execute();
private:
	RemoveMonitoredItemsWorkItem();
	std::vector<SoftingOPCToolbox5::Server::DeleteMonitoredItemRequestPtr> m_requests;
	SoftingOPCToolbox5::Server::SubscriptionPtr m_subscription;
};
class RemoveMonitoredItemsAction
	: public Util_ScheduledAction
{
public:
	RemoveMonitoredItemsAction(const std::vector<SoftingOPCToolbox5::Server::DeleteMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription);
	virtual ~RemoveMonitoredItemsAction();
	virtual void execute();

	virtual void cleanUp();

private:
	std::vector<SoftingOPCToolbox5::Server::DeleteMonitoredItemRequestPtr> m_requests;
	SoftingOPCToolbox5::Server::SubscriptionPtr m_subscription;
};

/*! Action to temporarily close and reopen an MonitoredItems
 * This action enqueues the OpenMonitoredItemsAction */
class ModifyMonitoredItemsWorkItem : public Util_WorkItem
{
public:
	ModifyMonitoredItemsWorkItem(const std::vector<SoftingOPCToolbox5::Server::ModifyMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription);
	~ModifyMonitoredItemsWorkItem();
	virtual void execute();
private:
	ModifyMonitoredItemsWorkItem();
	std::vector<SoftingOPCToolbox5::Server::ModifyMonitoredItemRequestPtr> m_requests;
	SoftingOPCToolbox5::Server::SubscriptionPtr m_subscription;
};
class ModifyMonitoredItemsAction
	: public Util_ScheduledAction
{
public:
	ModifyMonitoredItemsAction(const std::vector<SoftingOPCToolbox5::Server::ModifyMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription);
	virtual ~ModifyMonitoredItemsAction();
	virtual void execute();

		virtual void cleanUp();
private:
	std::vector<SoftingOPCToolbox5::Server::ModifyMonitoredItemRequestPtr> m_requests;
	SoftingOPCToolbox5::Server::SubscriptionPtr m_subscription;
};

#endif	// MONITOREDITEMS_MANIPULATION_METHODS_H
