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

#include "stdafx.h"

#include "Util_Event.h"

#include <stdlib.h>

#ifdef __VXWORKS__
#include <version.h>
#endif

#if (defined(SODEVENV_LINUX) && defined (CLOCK_MONOTONIC) && defined(__USE_XOPEN2K))
#define SUPPORTS_MONOTONIC_CLOCK_IN_CONDITION 1
#endif

#if defined(SODEVENV_VXWORKS)
#if defined(__RTP__)
#define SUPPORTS_MONOTONIC_CLOCK_IN_CONDITION 1
#else
/* Since SR0510 this is supported */
#if _WRS_VXWORKS_MAJOR >= 7
#define SUPPORTS_MONOTONIC_CLOCK_IN_CONDITION 1
#else
#define SUPPORTS_MONOTONIC_CLOCK_IN_CONDITION 0
#endif
#endif
#endif

/*!
  * @brief Constructor. Creates an uninitialized event object. Initilization should be done using create() method.
  */
Util_Event::Util_Event(void)
{
#ifdef SOOS_POSIX
#ifdef SODEVENV_CYGWIN
	memset(&m_mutex, 0, sizeof(m_mutex));
	memset(&m_cond, 0, sizeof(m_cond));
#endif
	pthread_mutex_init(&m_mutex, NULL);
	pthread_condattr_t *pAttr = NULL;
#if SUPPORTS_MONOTONIC_CLOCK_IN_CONDITION
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
	pAttr = &attr;
#endif
	pthread_cond_init(&m_cond, pAttr);
#if SUPPORTS_MONOTONIC_CLOCK_IN_CONDITION
	pthread_condattr_destroy(&attr);
#endif
	m_manualReset = false;
	m_signal = false;
	m_isInit = false;
#endif // SOOS_POSIX
#ifdef SOOS_WINDOWS
	m_handle = 0;
	m_flags = 0;
#endif
}

/*!
* @brief Destructor
*/
Util_Event::~Util_Event(void)
{
#ifdef SOOS_POSIX
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
#endif // SOOS_POSIX
#ifdef SOOS_WINDOWS
	if (m_handle != 0)
	{
		::CloseHandle(m_handle);
		m_handle = 0;
	}
#endif
}

/*!
* @brief Initializes this event object. This method creates the actual event.
* @param manualReset If true, after beeing set to the signaled state, the event must be manually reset, by calling the reset() method. Default: false;
* @param initialState If true, the event is created the signaled state. Default: false;
* @param name An name for this event. Default: null;
* @return true If the creation of this event was successfull.
*/
bool Util_Event::create(
	bool manualReset,
	bool initialState,
	const OTChar* name)
{
	#ifdef SOOS_POSIX
	_UNUSED(name);
	//_ASSERTION(name == NULL, "linux: named events not implemented");
	//_ASSERTION(!m_isInit, "event::create was already called");
	m_manualReset = manualReset;
	m_signal = initialState;
	m_isInit = true;
	return (m_isInit);
	#endif // SOOS_POSIX
	#ifdef SOOS_WINDOWS
	if ((!manualReset) && (!name))
	{
		m_handle = CreateEvent(NULL,false,false,NULL);
		if (m_handle != 0)
		{
			if (initialState)
			{
				signal();
			}
		}
	}
	else
	{
		m_handle = ::CreateEvent(
			NULL,
			manualReset,
			initialState,
			name);
	}

	return (m_handle != 0);
	#endif
}

#ifdef SOOS_WINDOWS
bool Util_Event::create(
	SECURITY_ATTRIBUTES *pSD,
	bool manualReset,
	bool initialState,
	const OTChar* name)
{
	m_handle = ::CreateEvent(
		pSD,
		manualReset,
		initialState,
		name);
	return (m_handle != 0);
}
#endif

#ifdef SOOS_POSIX
bool Util_Event::signal(void)
{
	pthread_mutex_lock(&m_mutex);
	m_signal = true;
	pthread_mutex_unlock(&m_mutex);

	if (m_manualReset)
	{
		pthread_cond_broadcast(&m_cond);
	}
	else
	{
		pthread_cond_signal(&m_cond);
	}
	return (true);
}

bool Util_Event::reset(void)
{
	pthread_mutex_lock(&m_mutex);
	m_signal = false;
	pthread_mutex_unlock(&m_mutex);
	return (true);
}

bool Util_Event::waitFor(unsigned long timeout)
{
	bool retval = false;
	pthread_mutex_lock(&m_mutex);
	if (!m_signal && timeout != 0)
	{
		if (timeout == INFINITE)
		{
			while(!m_signal)
			{
				pthread_cond_wait(&m_cond, &m_mutex);
			}
		}
		else // (timeout<INFINITE)
		{
			struct timespec later;
#ifdef SODEVENV_CYGWIN
			if (timeout>0x7FFFFFFF)
			{
				timeout=0x7FFFFFFF;
			}
#endif

			struct timespec now;
			int getClockRet = 0;
#if SUPPORTS_MONOTONIC_CLOCK_IN_CONDITION
			getClockRet = clock_gettime(CLOCK_MONOTONIC, &now);
#else
			getClockRet = clock_gettime(CLOCK_REALTIME, &now);
#endif
			if(getClockRet != 0)
			{
#if defined(_WRS_VXWORKS_MAJOR) && ((_WRS_VXWORKS_MAJOR < 6) || ((_WRS_VXWORKS_MAJOR == 6) && (_WRS_VXWORKS_MINOR <= 8)))
				// VxWorks 6.8 does not support gettimeofday
				_REPORT_ASSERTION(_T("clock_gettime failed"));
#else
				struct timeval timevalnow;
				gettimeofday(&timevalnow, NULL);
				now.tv_sec = timevalnow.tv_sec;
				now.tv_nsec = 1000 * timevalnow.tv_usec;
#endif
			}
			later.tv_sec = now.tv_sec + timeout / 1000;
			later.tv_nsec = now.tv_nsec;

			later.tv_nsec += 1000000 * (timeout % 1000);
			if (later.tv_nsec  >  999999999)
			{
				later.tv_nsec -= 1000000000;
				later.tv_sec++;
			}
			while (!m_signal)
			{
				int res = pthread_cond_timedwait(&m_cond, &m_mutex, &later);
				if (res == ETIMEDOUT || time(NULL) < now.tv_sec)
				{
					break;
				}
				/* other values: res == EINTR or res == 0 */
			}
		}
	}

	if (m_signal)
	{
		retval = true;
		if (!m_manualReset)
		{
			m_signal = false;
		}
	}
	pthread_mutex_unlock(&m_mutex);
	return retval;
}

bool Util_Event::isInit(void)
{
	return (m_isInit);
}


#endif // SOOS_POSIX

