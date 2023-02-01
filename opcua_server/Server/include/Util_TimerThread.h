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

#ifndef _UTIL_TIMERTHREAD_H_
#define _UTIL_TIMERTHREAD_H_

#include "Util_Thread.h"
#include "Util_Sync.h"
#include "Util_Mutex.h"
#include "Util_SingleLock.h"
#include "Util_Timer.h"
#include <queue>
#include <list>
#include <set>

using namespace std;

#define UTIL_MAX_TIMERTHREAD_TIMEOUT		0xFFFFFFFF

/*! @brief
 * Class representing an action that might be scheduled by the TimerThread.
 * The implementing class must overload the execute() method with an appropriate implementation.
 * @note It is also possible to add a priority parameter to the action. In order to achieve this
 *       this class should be extended with a priority parameter, and the comparison operators
 *       must be adjusted.
 * @note The execute() method should be a 'short and fast' implementation. Delays that occur in the
 *       execute method can cause the delay of execution of following actions.
 */
class Util_ScheduledAction
{
public:
	/*! @brief
	 * Default constructor initializes start and end time to the current time..
	 */
	Util_ScheduledAction(bool isCyclic = false): m_isCyclic(isCyclic), m_autoDeleteAction(false)
	{
		m_startTime = m_endTime = Util_Timer::currentTickCount();
	}

	/*! @brief
	 * Construction of an ScheduledAction object, given a specified timeout. The starting point
	 * for the countdown will be considered to be the current tick count.
	 */
	Util_ScheduledAction(unsigned long timeout, bool isCyclic = false): m_isCyclic(isCyclic), m_autoDeleteAction(false)
	{
		m_startTime = Util_Timer::currentTickCount();
		m_endTime = m_startTime + timeout;
	}

	/*! @brief
	 * Construction of an ScheduledAction object, given a start and end time.
	 *
	 * @note startTime must not be in the future, it must either equal the current 'tick count' or lie in the past.
	 */
	Util_ScheduledAction(unsigned long startTime, unsigned long endTime, bool isCyclic = false):
		m_startTime(startTime),
		m_endTime(endTime),
		m_isCyclic(isCyclic),
		m_autoDeleteAction(false)
	{
	}

	/*! @brief
	 * Destructor
	 */
	virtual ~Util_ScheduledAction()
	{
	}

	/*! @brief
	* This method must be reimplemented in derived classes. This method will be called
	* when the action is triggered.
	*/
	virtual void execute() = 0;

	/* !brief
	 * virtual method to stop running actions
	 */
	virtual void cleanUp()
	{

	}

	/*! @brief
	* Returns the scheduled end time of this action
	*/
	inline unsigned long getEndTime() const
	{
		return m_endTime;
	}

	/*! @brief
	* Returns the start time of this action
	*/
	inline unsigned long getStartTime() const
	{
		return m_startTime;
	}

	/*! @brief Comparison operator with another action object. Elements are compared by the scheduled
	* time. Overflow of 'tick count' is payed attention to.
	*/
	bool operator<(const Util_ScheduledAction& right) const
	{
		return (long)(m_endTime - right.m_endTime) < 0;
	}

	/*! @brief Comparison operator with another action object. Elements are compared by the scheduled
	* time. Overflow of 'tick count' is payed attention to.
	*/
	bool operator<=(const Util_ScheduledAction& right) const
	{
		return (long)(m_endTime - right.m_endTime) <= 0;
	}


	/*! @brief Comparison operator with another action object. Elements are compared by the scheduled
	* time. Overflow of 'tick count' is payed attention to.
	*/
	inline bool operator>(const Util_ScheduledAction& right) const
	{
		return (long)(m_endTime - right.m_endTime) > 0;
	}

	/*! @brief Comparison operator with another action object. Elements are compared by the scheduled
	* time. Overflow of 'tick count' is payed attention to.
	*/
	inline bool operator>=(const Util_ScheduledAction& right) const
	{
		return (long)(m_endTime - right.m_endTime) >= 0;
	}


	/*! @brief Verifies if the timeout for the action has already expired
	*
	* @return   - true if the timeout expired
	*			- false otherwise
	*/
	inline bool expired(unsigned long currentTick)
	{
		return (long)(m_endTime - currentTick) <= 0;
	}

	/*! @brief Calculates the number of millisecods until the timeout expires, starting from the current moment.
	*
	* @return   - if the timeout has not yet expired: the number of milliseconds until the action must be triggered
	*			- 0 otherwise (the timeout has already expired)
	*/
	inline unsigned long getSleepingTimeToTrigger(unsigned long currentTick)
	{

		// First check the case when no overflow occurs for this action
		if(!expired(currentTick))
		{
			return m_endTime - currentTick;
		}
		return 0;
	}

	/**
	* @brief Recalculate the start and end time. The start will be considered the current time.
	* This method should be called after triggering an action and before rescheduling the action.
	*/
	virtual inline void reschedule(unsigned long currentTick)
	{
		// reschedule only if timer is expired
		if (expired(currentTick))
		{
			unsigned long timeout = m_endTime - m_startTime;

			// timer expired, reschedule
			if (timeout != 0)
			{
				unsigned long factor = (currentTick - m_startTime)/timeout;
				m_startTime += factor * timeout;
				m_endTime = m_startTime + timeout;
			}
			else
			{
				// timeout == 0
				m_endTime = m_startTime = currentTick;
			}
		}
	}

	/**
	 * @brief Getter for the cyclic property.
	 */
	inline bool isCyclic()
	{
		return m_isCyclic;
	}

	/**
	 * @brief Sets the cyclic property.
	 */
	inline void setCyclic(bool isCyclic)
	{
		m_isCyclic = isCyclic;
	}

	/*! @brief
	 * Sets the timeout for this action. The start time will be set to the current start time,
	 * and the end time will be calculated by appending the timeout.
	 */
	inline void setTimeout(unsigned long timeout)
	{
		m_startTime = Util_Timer::currentTickCount();
		m_endTime = m_startTime + timeout;
	}

	inline bool isAutoDelete()
	{
		return m_autoDeleteAction;
	}

	/*! Marks this action to be automatically deleted when it has ended.
	* This is used to give the TimerThread the ownership of allocated Actions. */
	inline void setAutoDelete(bool autoDelete)
	{
		m_autoDeleteAction = autoDelete;
	}

private:
	unsigned long m_startTime;		/*!< Time when the action has been created. */
	unsigned long m_endTime;		/*!< The time when this action needs to be triggered. */
	bool m_isCyclic;				/*!< True if this action must be scheduled over and over again. */
	bool m_autoDeleteAction;		/*!< Indicates whether to delete the action after it is automatically removed from the queue */
};

/*! @brief
 * Helper struct only for using the STL priority_queue class which needs a comparison operator for
 * scheduled actions.
 */
struct  Util_ScheduledAction_less
{
	/*! @brief
	 * Compare the actions and return true if left is <= right
	 */
	bool operator()(const Util_ScheduledAction* Left, const Util_ScheduledAction* Right) const
	{
		// That is the right order (not clear by thinking but tested in the implementation!)
		//std::cout << Left->getEndTime() << " > " << Right->getEndTime() << "  ==> " << (((*Left) > (*Right))?"TRUE":"FALSE") << std::endl;
		return ((*Left) < (*Right));
	}
};

/*! @brief
 * Definition of priority class, used in timer thread.
 */
typedef multiset < Util_ScheduledAction*, Util_ScheduledAction_less > Util_SortedScheduledActionQueue;



/*! @brief TimerThread object.
 * This class implements a action scheduler. Actions are scheduled by a the time when they
 * need to be triggered/executed.
 * @note It is also possible to add a priority parameter to the action. See the ScheduledAction class for details.
 */
class  Util_TimerThread
	: public Util_Thread
{
public:
	Util_TimerThread();
	virtual ~Util_TimerThread();

	inline size_t numActions();
	virtual bool start();
	virtual bool stop();
	virtual bool addAction(Util_ScheduledAction* pAction);
	virtual bool removeAction(Util_ScheduledAction* pAction);
	virtual long run();


	Util_Sync& getSync()
	{
		return m_queueSync;
	}
#if 0
	virtual void dumpActionList(std::ostream &out);
#endif

protected:
	virtual bool scheduleAction(Util_ScheduledAction* pAction);

private:
	Util_SortedScheduledActionQueue m_queue;					/*!< Sorted queue of actions that must be executed. First action is the first one to be executed. */
	std::list<Util_ScheduledAction*> m_triggeredActionsList;	/*!< Temporary list for storing the actions triggered in the current run cycle. */
	Util_Sync m_queueSync;										/*!< Synchronisation object (mutex) for the queue */
	Util_Event m_event;											/*!< Event, triggering a new processing cycle in the run method */
	OTAtomic m_bKeepOnRunning;										/*!< This variable is actually verified at avery cycle in the run() method. If false, the run() method will return. */
};

/*! @brief Return the number of currently registered actions in the TimerThread.
 */
inline size_t Util_TimerThread::numActions()
{
	Util_SingleLock<Util_Sync> m_lock(&m_queueSync);
	return m_queue.size();
}

#endif

