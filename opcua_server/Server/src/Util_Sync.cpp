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
#include "Util_Sync.h"


/*!
 * @brief Default constructor, creates an unlocked mutex.
 */
Util_Sync::Util_Sync()
{
#ifdef SOOS_WINDOWS
	InitializeCriticalSection( &m_criticalSection );
#endif // SOOS_WINDOWS
#ifdef SOOS_POSIX
#ifdef SODEVENV_CYGWIN
	memset(
		&m_mutex,
		0,
		sizeof(m_mutex));
#endif
	pthread_mutex_init(
		&m_mutex,
		NULL);
	m_owner = 0;
	m_recur = 0;
#endif
}

/*!
 * @brief Destructor.
 */
Util_Sync::~Util_Sync()
{
#ifdef SOOS_WINDOWS
	DeleteCriticalSection( &m_criticalSection );
#endif // SOOS_WINDOWS
#ifdef SOOS_POSIX
	pthread_mutex_destroy(&m_mutex);
#endif
}


/*!
 * @brief Tries to lock the object.
 * @return True if the mutex has been locked, false otherwise.
 */
bool Util_Sync::lock()
{
#ifdef SOOS_WINDOWS
	EnterCriticalSection( &m_criticalSection );
#endif // SOOS_WINDOWS
#ifdef SOOS_POSIX
	pthread_t owner = interlockedGet(&m_owner);
	if (owner != pthread_self())
	{
		pthread_mutex_lock(&m_mutex);
		interlockedSet(&m_owner, pthread_self());
	}
	m_recur++;
#endif

	return true;
} // lock

/*!
 * @brief Unlocks the mutex.
 * @return True if the unlock was successfull, false otherwise.
 */
bool Util_Sync::unlock(void)
{
#ifdef SOOS_WINDOWS
	LeaveCriticalSection( &m_criticalSection );
#endif // SOOS_WINDOWS
#ifdef SOOS_POSIX
	pthread_t owner = interlockedGet(&m_owner);
	if (owner != pthread_self())
	{
		return false;
	}

	if (!--m_recur)
	{
		interlockedSet(&m_owner, 0);
		pthread_mutex_unlock(&m_mutex);
	}
#endif

	return true;
} // unlock
