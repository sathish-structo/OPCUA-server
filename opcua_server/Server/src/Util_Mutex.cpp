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
//  Description : Synchronisation classes
//                - Mutex: mutex implementation

#include "stdafx.h"
#include "Util_Mutex.h"

/*!
 * @brief Default constructor, creates an unlocked mutex.
 */
Util_Mutex::Util_Mutex()
{
#ifdef SOOS_WINDOWS

	m_mutex = CreateMutex(
				NULL,
				FALSE,
				NULL);

#endif // SOOS_WINDOWS

#ifdef SOOS_POSIX

	pthread_mutexattr_t attr;
#ifdef SODEVENV_CYGWIN
	memset(
		&attr,
		0,
		sizeof(attr));
	memset(
		&m_mutex,
		0,
		sizeof(m_mutex));
#endif
	pthread_mutexattr_init(&attr);
	/*
	pthread_mutexattr_settype(
	&attr,
	PTHREAD_MUTEX_RECURSIVE);
	*/
	pthread_mutex_init(
		&m_mutex,
		&attr);
	pthread_mutexattr_destroy(&attr);

#endif
}

/*!
* @brief Destructor.
*/
Util_Mutex::~Util_Mutex()
{
#ifdef SOOS_WINDOWS
	if (m_mutex != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_mutex);
	}
#endif // SOOS_WINDOWS
#ifdef SOOS_POSIX
	pthread_mutex_destroy(&m_mutex);
#endif
}


/*!
* @brief Tries to lock the mutex.
* @param timeout The number of milliseconds to wait on the mutex to lock, before returning.
* @return True if the mutex has been locked, false otherwise.
*/
bool Util_Mutex::lock(unsigned long timeout)	// maximum time to wait for mutex
{
#ifdef SOOS_WINDOWS
	DWORD res;
	res = ::WaitForSingleObject(
		m_mutex,
		timeout);
	return (res == WAIT_OBJECT_0);
#endif // SOOS_WINDOWS
#ifdef SOOS_POSIX
	bool res;
	if (timeout == 0)
	{
		res = !pthread_mutex_trylock(&m_mutex);
	}
	else
	{
		res = !pthread_mutex_lock(&m_mutex);
	}
	return res;
#endif
} // lock

/*!
* @brief Unlocks the mutex.
* @return True if the unlock was successfull, false otherwise.
*/
bool Util_Mutex::unlock(void)
{
#ifdef SOOS_WINDOWS
	return (TRUE == ReleaseMutex(m_mutex));
#endif
#ifdef SOOS_POSIX
	return (0 == pthread_mutex_unlock(&m_mutex));
#endif
} // unlock

