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
#ifndef SUBSCRIPTION_MANIPULATION_METHODS_H
#define SUBSCRIPTION_MANIPULATION_METHODS_H

#include "ServerMethod.h"
#include "ServerMethodCallRequest.h"
#include "Util_TimerThread.h"

#include "ServerSubscription.h"
#include "ServerDeleteSubscriptionRequest.h"
#include "Util_WorkItem.h"

class DeleteSubscriptionWorkItem : public Util_WorkItem
{
public:
	DeleteSubscriptionWorkItem(const SoftingOPCToolbox5::Server::DeleteSubscriptionRequestPtr request, SoftingOPCToolbox5::Server::SubscriptionPtr subscription);
	~DeleteSubscriptionWorkItem();
	virtual void execute();
private:
	DeleteSubscriptionWorkItem();
	SoftingOPCToolbox5::Server::DeleteSubscriptionRequestPtr m_request;
	SoftingOPCToolbox5::Server::SubscriptionPtr m_subscription;
};
/*! Action to delete a Subscription */
class DeleteSubscriptionAction
	: public Util_ScheduledAction
{
public:
	DeleteSubscriptionAction(const SoftingOPCToolbox5::Server::DeleteSubscriptionRequestPtr request, SoftingOPCToolbox5::Server::SubscriptionPtr subscription);
	virtual ~DeleteSubscriptionAction();
	virtual void execute();

	virtual void cleanUp();
private:
	DeleteSubscriptionAction();
	SoftingOPCToolbox5::Server::DeleteSubscriptionRequestPtr m_request;
	SoftingOPCToolbox5::Server::SubscriptionPtr m_subscription;
};

#endif	// SUBSCRIPTION_MANIPULATION_METHODS_H
