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
// TimerThread.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Util_TimerThread.h"

/*! @brief
* Constructs the thread in an incative state
*/
Util_TimerThread::Util_TimerThread()
{
	interlockedSet(&m_bKeepOnRunning, false);
	m_event.create();
	UTIL_INIT_SYNC(&m_queueSync,"Util_TimerThread");
}

/*! @brief
* Destructor
*/
Util_TimerThread::~Util_TimerThread()
{
}

/*! @brief Starts the thread
*/
bool Util_TimerThread::start()
{
	interlockedSet(&m_bKeepOnRunning, true);
	return Util_Thread::start();
}

/*! @brief Stops the thread by setting the m_bKeepOnRunning flag to \e false, and triggering an event to wake from sleep.
*/
bool Util_TimerThread::stop()
{
	interlockedSet(&m_bKeepOnRunning, false);

	{
		Util_SingleLock<Util_Sync> m_lock(&m_queueSync);
		while(!m_queue.empty())
		{
			Util_ScheduledAction* action = *m_queue.begin();
			removeAction(action);
			action->cleanUp();
			if(action->isAutoDelete())
			{
				delete action;
			}
			action = NULL;
		}
	}
	return m_event.signal();
}

/*! @brief Schedules a new action.
 *
 * @param pAction Action to be scheduled. If null nothing will be scheduled.
 */
bool Util_TimerThread::addAction(Util_ScheduledAction* pAction)
{
	UTIL_SINGLE_LOCK(Util_Sync,lock,&m_queueSync);
	if(!m_bKeepOnRunning)
	{
		return false;
	}
	return scheduleAction(pAction);
}

bool Util_TimerThread::scheduleAction(Util_ScheduledAction* pAction)
{
	if(pAction)
	{
		Util_ScheduledAction* pTopAction;
		bool bRescheduleSleep = true;

		// If the action beeing added must be triggered first (before any other action in our queue)
		// the we must reschedule the sleep of the trhead.
		if(!m_queue.empty())
		{
			pTopAction = *(m_queue.begin());
			if(pTopAction)
			{
				bRescheduleSleep = (*pAction < *pTopAction);
			}
			else
			{
				// error, null pointer, should not occur
			}
		}

		// Following insert does not return error
		m_queue.insert(pAction);
		if(bRescheduleSleep)
		{
			m_event.signal();
		}
	}
	return true;
}

/*! @brief Remove the action from the scheduler / timer thread
 *
 * @param pAction The action to be removed. If null nothing happens, and the function returns.
 * @return True if the action has been found and succefully removed, false otherwise.
 */
bool Util_TimerThread::removeAction(Util_ScheduledAction* pAction)
{
	if(pAction)
	{
		UTIL_SINGLE_LOCK(Util_Sync,lock,&m_queueSync);
		Util_SortedScheduledActionQueue::iterator queueIt;
		std::list<Util_ScheduledAction*>::iterator tilIt;
		for(queueIt = m_queue.begin();
			queueIt != m_queue.end();
			queueIt++)
		{
			if((*queueIt) == pAction)
			{
				m_queue.erase(queueIt);
				return true;
			}
		}

		for(tilIt = m_triggeredActionsList.begin();
			tilIt != m_triggeredActionsList.end();
			tilIt++)
		{
			if((*tilIt) == pAction)
			{
				m_triggeredActionsList.erase(tilIt);
				return true;
			}
		}
	}
	return false;
}

/*! @brief The thread's "main loop".
 *
 * This method will wait either for the \e m_event to be triggered,
 * or for the timeout to expire. In each cycle:
 *    a. the m_bKeepOnRunning is verified first
 *    b. it is verified that actions have expired or not. Expired actions are executed and removed from queue.
 *    c. a new timeout is computed (that would be the timeout used when waiting for \e m_event).
 */
long Util_TimerThread::run()
{
	static const long maxWaitingTime = 60 * 1000;
	Util_ScheduledAction* pAction;
	long sleepingTime = 0;
	bool isSleepingTimeSet;
	std::list<Util_ScheduledAction*>::iterator it;
	Util_SortedScheduledActionQueue::iterator top;

	while(interlockedGet(&m_bKeepOnRunning))
	{
		{
			// Lock the queue access
			m_queueSync.lock();
			sleepingTime = 0;
			isSleepingTimeSet = false;

			#if _HAS_EXCEPTIONS
			try
			#endif // _HAS_EXCEPTIONS
			{
				unsigned long currentTick = Util_Timer::currentTickCount();
				while(sleepingTime == 0 && interlockedGet(&m_bKeepOnRunning) && !m_queue.empty())
				{
					top = m_queue.begin();
					pAction = (*top);
					if(pAction)
					{
						// If this action has expired then we will also check the next one. Maybe it has
						// been scheduled for the same time. Otherwise we will continue with another
						// we do not continue, there are no more actions to be triggered now, because
						// the queue is sorted, in the order the actions need to be triggered.
						sleepingTime = pAction->getSleepingTimeToTrigger(currentTick);
						isSleepingTimeSet = true;
						if(sleepingTime == 0)
						{
							// m_queue.erase and triggeredActionsList.push_back must
							// be performed before pAction->execute, because te user might
							// try to remove the action from within the execute. And for that
							// the lists must be up to date.
							m_queue.erase(top);
							m_triggeredActionsList.push_back(pAction);

							// unlock the queue while executing the action 
							// some actions call to UA stack while other actions are added in context of stack threads -> possible deadlock 
							pAction->execute();
						}
					}
					else
					{
						// error, null pointer, maybe do some logging
					}
				}

				// Reinsert all the triggered actions into the sorted queue
				if(!m_triggeredActionsList.empty())
				{
					currentTick = Util_Timer::currentTickCount();
					// Insert all the triggered actions into the queue
					for(it = m_triggeredActionsList.begin();
						it != m_triggeredActionsList.end();
						it++)
					{
						pAction = (*it);
						pAction->reschedule(currentTick);
						// Only cyclic actions are rescheduled
						if(pAction->isCyclic())
						{
							m_queue.insert(pAction);
						}
						else
						{
							if(pAction->isAutoDelete())
							{
								delete pAction;
								pAction = NULL;
							}
						}
					}
					m_triggeredActionsList.clear();

					// Get the sleeping time to the next action to be triggered
					if(!m_queue.empty())
					{
						pAction = *m_queue.begin();
						if(pAction)
						{
							sleepingTime = pAction->getSleepingTimeToTrigger(currentTick);
							isSleepingTimeSet = true;
						}
					}
				}
			}
			#if _HAS_EXCEPTIONS
			catch(std::exception &e)
			{
				printf("FATAL: Error occurred in timer thread when trying to trigger and reschedule actions: '%s'", e.what());
			}
			#endif // _HAS_EXCEPTIONS

			m_queueSync.unlock();
		}

		if(!isSleepingTimeSet)
		{
			sleepingTime = maxWaitingTime;
		}

		if(sleepingTime == 0)
		{
			// This can happen if the server is under heavy load, and the timer thread gets delayed ...
			//_ASSERTION(sleepingTime != 0, _T("That should never happen!"));
			sleepingTime = 10;
		}
		m_event.waitFor(sleepingTime);
	}
	return 0;
}

#if 0
/*! @brief Write the informations about the scheduled actions to an stream
 */
void Util_TimerThread::dumpActionList(std::ostream &out)
{
	UTIL_SINGLE_LOCK(Util_Sync,lock,&m_queueSync);
	Util_SortedScheduledActionQueue::iterator it;
	Util_ScheduledAction* pAction;
	unsigned int idx;
	for(it = m_queue.begin(), idx = 0;
		it != m_queue.end();
		it++, idx++)
	{
		pAction = *it;
		out << idx << ". Action[" << pAction->getStartTime() << "," << pAction->getEndTime() << "]" << std::endl;
	}
}
#endif
