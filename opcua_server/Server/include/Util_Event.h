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
//  Description : System event classes
//                - SOCmnEvent: Event objects

#ifndef _UTIL_EVENT_H_
#define _UTIL_EVENT_H_


/*!
 * @brief Implements a signaling and waiting for signal mechanism, between threads.
 */
class Util_Event
{
public:
	Util_Event(void);
	~Util_Event(void);

	bool create(bool manualReset = false, bool initialState = false, const OTChar* name = NULL);
	bool signal(void);
	bool reset(void);
	bool waitFor(unsigned long timeout = INFINITE);

	bool isInit(void);

#ifdef SOOS_WINDOWS
	HANDLE getHandle(void) { return m_handle; }
	bool create(
		SECURITY_ATTRIBUTES *pSD,
		bool manualReset,
		bool initialState,
		const OTChar* name);
#endif

protected:
#ifdef SOOS_POSIX

	pthread_mutex_t m_mutex;
	pthread_cond_t  m_cond;

	bool m_manualReset;
	bool m_signal;
	bool m_isInit;

#endif // SOOS_POSIX
#ifdef SOOS_WINDOWS

	HANDLE m_handle;		/*!< WINDOWS: Handle to the windows event. */
	BYTE m_flags;			/*!< WINDOWS: Flags the event has been created with. */

#endif
}; // Util_Event

#define UTIL_EVENT_INVALID_ID	0xFFFFFFFF

#ifdef SOOS_WINDOWS
/*!
  * @brief Sets the state of the event to signaled. One of the waiting threads for this event
  * will be woken. ( i. e. one of the concurrent calls to \e waitFor() method will return \e true).
  * @return true If setting the state to signaled succeeded.
  */
inline bool Util_Event::signal(void)
{
	return (TRUE == ::SetEvent(m_handle));
}

/*!
  * @brief Resets the signaled state of the event.
  * @note This method only needs to be called for \e manual events. That are events created with the \e
  * manualReset flag.
  * @see create().
  */
inline bool Util_Event::reset(void)
{
	return (TRUE == ::ResetEvent(m_handle));
}

/*!
  * @brief Waits on an event to be set to the signaled state, for a specified amount of time.
  * @param timeout The number of milliseconds to wait.
  * @return true If the event has been set to the signaled state, false if a timeout occured
  */
inline bool Util_Event::waitFor(unsigned long timeout)
{
	return (WAIT_OBJECT_0 == ::WaitForSingleObject(
		m_handle,
		timeout));
}

/*!
  * @brief Verifies that th event has been initialiesed/created.
  * @see create()
  */
inline bool Util_Event::isInit(void)
{
	return (m_handle != 0);
}


#endif // SOOS_WINDOWS

#endif
