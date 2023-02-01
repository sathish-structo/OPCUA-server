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
#include "MonitoredItemsAsyncManipulation.h"

//#include "TestServerSubscription.h"
#include "ApplicationModule.h"
#include "TestServerSession.h"
#include "IDataChangeFilter.h"
#include "ServerMonitoredItem.h"
#include "DynamicNodeCache.h"
#include "ToolkitTrace.h"
#include "TestServerSubscription.h"
#include "TestServerMonitoredItem.h"

using namespace SoftingOPCToolbox5;

CreateMonitoredItemsWorkItem::CreateMonitoredItemsWorkItem(const std::vector<SoftingOPCToolbox5::Server::CreateMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription)
{
	m_requests = requests;
	m_subscription = subscription;
}

CreateMonitoredItemsWorkItem::CreateMonitoredItemsWorkItem()
{
}

CreateMonitoredItemsWorkItem::~CreateMonitoredItemsWorkItem()
{
}

void CreateMonitoredItemsWorkItem::execute()
{
	// check whether the session has any restrictions regarding event monitored item queue size
	TestServerSessionPtr testSession = static_cast<TestServerSession*>((Server::Session *)(m_subscription->getSession()));

	// if the subscription can be already timed out an disconnected from session
	if(testSession.isNotNull())
	{
		OTUInt32 maxEventQueueSize = testSession->getMaxEventQueueSize();

		if(maxEventQueueSize != 0)
		{
			for(size_t i = 0; i < m_requests.size(); i++)
			{
				if(m_requests[i].isNotNull())
				{
					// set queue size only for event items
					if(m_requests[i]->getMonitoredItem()->isEventItem())
					{
						m_requests[i]->getMonitoredItem()->setQueueSize(maxEventQueueSize);
					}
				}
			}
		}
	}

	for(OTUInt32 i = 0; i < m_requests.size(); i++)
	{
		// a MI can be NULL e.g. an invalid node was specified by client
		Server::MonitoredItemPtr mi = m_requests[i]->getMonitoredItem();
		if(mi)
		{
			TestServerMonitoredItem* testMI = (TestServerMonitoredItem*)mi.p();
			OTUInt32 testId = testMI->m_testId;
			// MI should be linked to a subscription
			Server::SubscriptionPtr subscription = m_requests[i]->getMonitoredItem()->getSubscription();
			if(subscription)
			{
				TestServerSubscriptionPtr testSubscription = (TestServerSubscription*)subscription.p();
				std::pair<std::set<OTUInt32>::iterator, bool> insertResult = testSubscription->m_monitoredItemTestIds.insert(testId);
				if(!insertResult.second)
				{
					_tprintf(_T("Monitored Item test ID %u is already in subscription during creation\n"), testId);
				}
			}

#if TB5_ALARMS
			const IEventFilter* pEventFilter = m_requests[i]->getMonitoredItem()->getEventFilter();
			_UNUSED(pEventFilter);
#endif
			const IDataChangeFilter* pDataChangeFilter = m_requests[i]->getMonitoredItem()->getFilter();
			_UNUSED(pDataChangeFilter);
			if(m_requests[i]->getMonitoredItem()->getAttributeId() == EnumAttributeId_EventNotifier)
			{
#if TB5_ALARMS
				_ASSERTION(pEventFilter != NULL, _T("Event filter is NULL"));
#endif
				_ASSERTION(pDataChangeFilter == NULL, _T("Data change filter is not NULL"));
			}
			else
			{
#if TB5_ALARMS
				_ASSERTION(pEventFilter == NULL, _T("Event filter is not NULL"));
#endif
				_ASSERTION(pDataChangeFilter != NULL, _T("Data change filter is NULL"));
			}
			const INodeId* pMonitoredNodeId = m_requests[i]->getMonitoredItem()->getNodeId();
			_UNUSED(pMonitoredNodeId);
			_ASSERTION(pMonitoredNodeId != NULL, _T("NodeId is NULL"));
			const INumericRange* pIndexRange = m_requests[i]->getMonitoredItem()->getIndexRange();
			_UNUSED(pIndexRange);
			_ASSERTION(pIndexRange == NULL ? true : pIndexRange->getInternHandle() != NULL, _T("Index range is invalid"));

#ifdef TRACE_MONITORED_ITEM_SETTINGS
			_tprintf(_T("Created MI:\n"));
			if(pMonitoredNodeId != NULL)
			{
				_tprintf(_T("\tMonitored node: %s\n"), pMonitoredNodeId->toString().c_str());
			}
			if(pIndexRange != NULL)
			{
				_tprintf(_T("\tIndex range: %s\n"), pIndexRange->toString().c_str());
			}
			if(pDataChangeFilter != NULL)
			{
				_tprintf(_T("\tData change filter provided:\n"));
				_tprintf(_T("\t\tTrigger: %s\n"), getEnumDataChangeTriggerString(pDataChangeFilter->getTrigger()));
				_tprintf(_T("\t\tDeadband type: %s\n"), getEnumDeadbandTypeString(pDataChangeFilter->getDeadbandType()));
				_tprintf(_T("\t\tDeadband value: %f\n"), pDataChangeFilter->getDeadbandValue());
			}
			if(pEventFilter != NULL)
			{
				_tprintf(_T("\tEvent filter provided:\n"));
				_tprintf(_T("\t\tSelect clauses: %u\n"), pEventFilter->getSelectClauseCount());
				_tprintf(_T("\t\tWhere clause elements: %u\n"), pEventFilter->getWhereClause()->getElementCount());
			}
#endif

			// may be it is a dynamic node
			EnumStatusCode handleResult = DynamicNodeCache::instance()->handleMonitoredItemCreation(m_requests[i]->getMonitoredItem(), m_requests[i]->getStatusCode());
			m_requests[i]->setStatusCode(handleResult);
		}
	}

	// complete all requests
	for(size_t i = 0; i < m_requests.size(); i++)
	{
		if(m_requests[i]->getRequestState() != EnumRequestState_Completed)
		{
			m_requests[i]->complete();
		}
	}
}

CreateMonitoredItemsAction::CreateMonitoredItemsAction(const std::vector<Server::CreateMonitoredItemRequestPtr> requests, Server::SubscriptionPtr subscription)
{
	m_requests = requests;
	m_subscription = subscription;
}

CreateMonitoredItemsAction::~CreateMonitoredItemsAction()
{
}

void CreateMonitoredItemsAction::execute()
{
	CreateMonitoredItemsWorkItem* workItem = new CreateMonitoredItemsWorkItem(m_requests, m_subscription);
	if(!getApplicationModule()->addWorkItem(workItem))
	{
		workItem->execute();
		delete workItem;
	}
}

void CreateMonitoredItemsAction::cleanUp()
{
	execute();
}


RemoveMonitoredItemsWorkItem::RemoveMonitoredItemsWorkItem(const std::vector<SoftingOPCToolbox5::Server::DeleteMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription)
{
	m_requests = requests;
	m_subscription = subscription;
}

RemoveMonitoredItemsWorkItem::RemoveMonitoredItemsWorkItem()
{
}

RemoveMonitoredItemsWorkItem::~RemoveMonitoredItemsWorkItem()
{
}

void RemoveMonitoredItemsWorkItem::execute()
{
	for(OTUInt32 i=0; i<m_requests.size(); i++)
	{
		Server::MonitoredItemPtr mi = m_requests[i]->getMonitoredItem();
		if(mi)
		{
			TestServerMonitoredItem* testMI = (TestServerMonitoredItem*)mi.p();
			OTUInt32 testId = testMI->m_testId;
			// MI should be linked to a subscription
			Server::SubscriptionPtr subscription = m_requests[i]->getMonitoredItem()->getSubscription();
			if(subscription)
			{
				TestServerSubscriptionPtr testSubscription = (TestServerSubscription*)subscription.p();
				if(testSubscription->m_monitoredItemTestIds.find(testId) == testSubscription->m_monitoredItemTestIds.end())
				{
					_tprintf(_T("Monitored Item test ID %u is not in subscription during deletion\n"), testId);
				}
			}
		}
		else
		{
			continue;
		}
#if TB5_ALARMS
		const IEventFilter* pEventFilter = m_requests[i]->getMonitoredItem()->getEventFilter();
		_UNUSED(pEventFilter);
#endif
		const IDataChangeFilter* pDataChangeFilter = m_requests[i]->getMonitoredItem()->getFilter();
		_UNUSED(pDataChangeFilter);
		if(m_requests[i]->getMonitoredItem()->getAttributeId() == EnumAttributeId_EventNotifier)
		{
#if TB5_ALARMS
			_ASSERTION(pEventFilter != NULL, _T("Event filter is NULL"));
#endif
			_ASSERTION(pDataChangeFilter == NULL, _T("Data change filter is not NULL"));
		}
		else
		{
#if TB5_ALARMS
			_ASSERTION(pEventFilter == NULL, _T("Event filter is not NULL"));
#endif
			_ASSERTION(pDataChangeFilter != NULL, _T("Data change filter is NULL"));
		}
		const INodeId* pMonitoredNodeId = m_requests[i]->getMonitoredItem()->getNodeId();
		_UNUSED(pMonitoredNodeId);
		_ASSERTION(pMonitoredNodeId != NULL, _T("NodeId is NULL"));

		const INumericRange* pIndexRange = m_requests[i]->getMonitoredItem()->getIndexRange();
		_UNUSED(pIndexRange);
		_ASSERTION(pIndexRange == NULL ? true : pIndexRange->getInternHandle() != NULL, _T("Index range is invalid"));

#ifdef TRACE_MONITORED_ITEM_SETTINGS
		_tprintf(_T("Deleted MI:\n"));
		if(pMonitoredNodeId != NULL)
		{
			_tprintf(_T("\tMonitored node: %s\n"), pMonitoredNodeId->toString().c_str());
		}
		if(pIndexRange != NULL)
		{
			_tprintf(_T("\tIndex range: %s\n"), pIndexRange->toString().c_str());
		}
		if(pDataChangeFilter != NULL)
		{
			_tprintf(_T("\tData change filter provided:\n"));
			_tprintf(_T("\t\tTrigger: %s\n"), getEnumDataChangeTriggerString(pDataChangeFilter->getTrigger()));
			_tprintf(_T("\t\tDeadband type: %s\n"), getEnumDeadbandTypeString(pDataChangeFilter->getDeadbandType()));
			_tprintf(_T("\t\tDeadband value: %f\n"), pDataChangeFilter->getDeadbandValue());
		}
		if(pEventFilter != NULL)
		{
			_tprintf(_T("\tEvent filter provided:\n"));
			_tprintf(_T("\t\tSelect clauses: %u\n"), pEventFilter->getSelectClauseCount());
			_tprintf(_T("\t\tWhere clause elements: %u\n"), pEventFilter->getWhereClause()->getElementCount());
		}
#endif

		// MI needs not be linked to a node e.g. the node was removed while MI exists
		if(m_requests[i]->getMonitoredItem()->getNode().isNull())
		{
			// can happen - e.g. if subscription is already timed out or node was deleted
			Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User1, _T("No node attached to MI during deletion"), _T(""));
		}
		// MI should be linked to a subscription
		if(m_requests[i]->getMonitoredItem()->getSubscription().isNull())
		{
			// can happen - e.g. if subscription is already timed out or node was deleted
			Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User1, _T("No subscription attached to MI during deletion"), _T(""));
		}

		// in case of a dynamic node some additional actions are necessary
		(void)DynamicNodeCache::instance()->handleMonitoredItemDeletion(m_requests[i]->getMonitoredItem(), m_requests[i]->getStatusCode());
	}

	// complete all requests
	for(size_t i = 0; i < m_requests.size(); i++)
	{
		m_requests[i]->complete();
	}
}

RemoveMonitoredItemsAction::RemoveMonitoredItemsAction(const std::vector<SoftingOPCToolbox5::Server::DeleteMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription)
{
	m_requests = requests;
	m_subscription = subscription;
}

RemoveMonitoredItemsAction::~RemoveMonitoredItemsAction()
{
}

void RemoveMonitoredItemsAction::execute()
{
	RemoveMonitoredItemsWorkItem* workItem = new RemoveMonitoredItemsWorkItem(m_requests, m_subscription);
	if(!getApplicationModule()->addWorkItem(workItem))
	{
		workItem->execute();
		delete workItem;
	}
}

void RemoveMonitoredItemsAction::cleanUp()
{
	execute();
}

ModifyMonitoredItemsWorkItem::ModifyMonitoredItemsWorkItem(const std::vector<SoftingOPCToolbox5::Server::ModifyMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription)
{
	m_requests = requests;
	m_subscription = subscription;
}

ModifyMonitoredItemsWorkItem::ModifyMonitoredItemsWorkItem()
{
}

ModifyMonitoredItemsWorkItem::~ModifyMonitoredItemsWorkItem()
{
}

void ModifyMonitoredItemsWorkItem::execute()
{
	for(OTUInt32 i=0; i<m_requests.size(); i++)
	{
		// a MI can be NULL e.g. an invalid node was specified by client
		if(m_requests[i]->getMonitoredItem() != NULL)
		{
			// MI should be linked to a node
			if(m_requests[i]->getMonitoredItem()->getNode().isNull())
			{
				// can happen - e.g. if subscription is already timed out or node was deleted
				Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User1, _T("No node attached to MI during modification"), _T(""));
			}
			// MI should be linked to a subscription
			if(m_requests[i]->getMonitoredItem()->getSubscription().isNull())
			{
				// can happen - e.g. if subscription is already timed out or node was deleted
				Trace::instance()->writeLine(EnumTraceLevel_Warning, EnumTraceGroup_User1, _T("No subscription attached to MI during modification"), _T(""));
			}

#if TB5_ALARMS
			const IEventFilter* pEventFilter = m_requests[i]->getMonitoredItem()->getEventFilter();
			_UNUSED(pEventFilter);
#endif
			const IDataChangeFilter* pDataChangeFilter = m_requests[i]->getMonitoredItem()->getFilter();
			_UNUSED(pDataChangeFilter);
			if(m_requests[i]->getMonitoredItem()->getAttributeId() == EnumAttributeId_EventNotifier)
			{
#if TB5_ALARMS
				_ASSERTION(pEventFilter != NULL, _T("Event filter is NULL"));
#endif
				_ASSERTION(pDataChangeFilter == NULL, _T("Data change filter is not NULL"));
			}
			else
			{
#if TB5_ALARMS
				_ASSERTION(pEventFilter == NULL, _T("Event filter is not NULL"));
#endif
				_ASSERTION(pDataChangeFilter != NULL, _T("Data change filter is NULL"));
			}
			const INodeId* pMonitoredNodeId = m_requests[i]->getMonitoredItem()->getNodeId();
			_UNUSED(pMonitoredNodeId);
			_ASSERTION(pMonitoredNodeId != NULL, _T("NodeId is NULL"));
			const INumericRange* pIndexRange = m_requests[i]->getMonitoredItem()->getIndexRange();
			_UNUSED(pIndexRange);
			_ASSERTION(pIndexRange == NULL ? true : pIndexRange->getInternHandle() != NULL, _T("Index range is invalid"));
#ifdef TRACE_MONITORED_ITEM_SETTINGS
			_tprintf(_T("Created MI:\n"));
			if(pMonitoredNodeId != NULL)
			{
				_tprintf(_T("\tMonitored node: %s\n"), pMonitoredNodeId->toString().c_str());
			}
			if(pIndexRange != NULL)
			{
				_tprintf(_T("\tIndex range: %s\n"), pIndexRange->toString().c_str());
			}
			if(pDataChangeFilter != NULL)
			{
				_tprintf(_T("\tData change filter provided:\n"));
				_tprintf(_T("\t\tTrigger: %s\n"), getEnumDataChangeTriggerString(pDataChangeFilter->getTrigger()));
				_tprintf(_T("\t\tDeadband type: %s\n"), getEnumDeadbandTypeString(pDataChangeFilter->getDeadbandType()));
				_tprintf(_T("\t\tDeadband value: %f\n"), pDataChangeFilter->getDeadbandValue());
			}
			if(pEventFilter != NULL)
			{
				_tprintf(_T("\tEvent filter provided:\n"));
				_tprintf(_T("\t\tSelect clauses: %u\n"), pEventFilter->getSelectClauseCount());
				_tprintf(_T("\t\tWhere clause elements: %u\n"), pEventFilter->getWhereClause()->getElementCount());
			}
#endif
		}
	}

	// complete all requests
	for(size_t i = 0; i < m_requests.size(); i++)
	{
		m_requests[i]->complete();
	}
}

ModifyMonitoredItemsAction::ModifyMonitoredItemsAction(const std::vector<SoftingOPCToolbox5::Server::ModifyMonitoredItemRequestPtr> requests, SoftingOPCToolbox5::Server::SubscriptionPtr subscription)
{
	m_requests = requests;
	m_subscription = subscription;
}

ModifyMonitoredItemsAction::~ModifyMonitoredItemsAction()
{
}

void ModifyMonitoredItemsAction::execute()
{
	ModifyMonitoredItemsWorkItem* workItem = new ModifyMonitoredItemsWorkItem(m_requests, m_subscription);
	if(!getApplicationModule()->addWorkItem(workItem))
	{
		workItem->execute();
		delete workItem;
	}
}

void ModifyMonitoredItemsAction::cleanUp()
{
	execute();
}

