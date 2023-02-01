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

#ifndef _UTIL_TIMER_H_
#define _UTIL_TIMER_H_


/*! @brief Timer class.
 * Provides means to start/stop the timer, verify the elapsed time and helper
 * methods for handling the machine tick count.
 *
 */
class Util_Timer
{
public:
	Util_Timer();
	virtual ~Util_Timer();

	virtual unsigned long start();
	virtual unsigned long stop();
	virtual unsigned long elapsed();
	virtual bool isRunning();

	static unsigned long currentTickCount();
	static OTUInt64 currentTickCount64();
	inline static unsigned long tickCountDiff(unsigned long first,unsigned long second);
	inline static unsigned long ticksUntil(unsigned long t);

protected:
	unsigned long m_startTickCount;
	unsigned long m_stopTickCount;
	bool m_isRunning;
};

/*! @brief Starts the timer
 * @return The current tick count (tick count when timer has been started)
 */
inline unsigned long Util_Timer::start()
{
	m_isRunning = true;
	return (m_startTickCount = currentTickCount());
}

/*! @brief Stops the timer
 * @return The tick count when timer has been stopped.
 */
inline unsigned long Util_Timer::stop()
{
	m_isRunning = false;
	return (m_stopTickCount = currentTickCount());
}

/*! @brief Verify that timer is running
 * @return True if timer is runing, false otherwise.
 */
inline bool Util_Timer::isRunning()
{
	return m_isRunning;
}

/*! @brief Get the elapse time
 * @return	- the number of milliseconds elapsed since the timer has been started, if the timer is still running
			- the number of milliseconds between the time when the timer has been started and stoped.
 */
inline unsigned long Util_Timer::elapsed()
{
	if(m_isRunning)
	{
		return tickCountDiff(m_startTickCount, currentTickCount());
	}
	return tickCountDiff(m_startTickCount,m_stopTickCount);
}

/*! @brief Get the current system tick count
 */

/*! @brief Calculates the difference between 2 tick count values, paying attention to overflow.
* @param first The first tick count
* @param second The second tick count
* @return unsigned long - The difference in milliseconds.
*/
inline unsigned long Util_Timer::tickCountDiff(unsigned long first,unsigned long second)
{
	return second - first;
}

/*! @brief Calculates the number of milliseconds until \e t
* @return The number of millis until \e t
*/
inline unsigned long Util_Timer::ticksUntil(unsigned long t)
{
	return tickCountDiff(currentTickCount(),t);
}

#endif

