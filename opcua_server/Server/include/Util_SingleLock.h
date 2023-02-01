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

#ifndef _UTIL_SINGLELOCK_H_
#define _UTIL_SINGLELOCK_H_

#include "Util_Thread.h"


// In Debug mode we will set the name of the mutex. This will be used for debugging and tracing
// purposes (and dead-lock checking).
#ifdef _DEBUG
	#define UTIL_INIT_SYNC(__mutex,__name)  (__mutex)->setName(__name)
#else
	#define UTIL_INIT_SYNC(__mutex,__name)
#endif

#ifdef _DEBUG
	#define UTIL_SINGLE_LOCK(__class,__varName,__syncObject)  Util_SingleLock<__class> __varName((__syncObject));
#else
	#define UTIL_SINGLE_LOCK(__class,__varName,__syncObject) Util_SingleLock<__class> __varName((__syncObject));
#endif



/*!
 * @brief Implements class, which is safely handling the lock/unlock of synchronisation objects (like Mutex, Sync).
 *
 * The use of this class is recomended over directly using the lock/unlock methods
 * of the sychronisation classes, due to the fact that this class guarantees the unlock of the
 * synchronisation on destruction. Hence a synchronisation object may not remain in the locked state
 * because of an unlock that has been forgotten on one of the return paths of a function for example.
 */
template <class T> class Util_SingleLock
{
public:
	/*!
	 * @brief Creates a SingleLock object, and also lock sthe synchronisation object \e sync
	 * @param sync The synchronisation object, which should be locked/unlocked. MUST NOT be locked.
     * @note Parameter sync <b>MUST NEVER EVER</b> be locked before passing it to a SingleLock, nor
     *      should it be locked from outside while the SingleLock is still alive. This may
     *      lead to undefined behaviour.
	 */
	Util_SingleLock(T *sync, bool doLock = true): m_isLocked(false)
	{
		m_sync = sync;
		if(doLock)
			m_isLocked = lock();
	}

	/*!
	 * @brief Creates a SingleLock object, and also lock sthe synchronisation object \e sync
	 * @param sync The synchronisation object, which should be locked/unlocked. MUST NOT be locked.
     * @note Parameter sync <b>MUST NEVER EVER</b> be locked before passing it to a SingleLock, nor
     *      should it be locked from outside while the SingleLock is still alive. This may
     *      lead to undefined behaviour.
	 */
	Util_SingleLock(T& sync, bool doLock = true): m_isLocked(false)
	{
		m_sync = &sync;
		if(doLock)
			m_isLocked = lock();
	}

	/*!
	 * @brief Destructor. The destructor also unlocks the sychronisation object.
	 */
	~Util_SingleLock()
	{
		if(m_isLocked)
			unlock();
	}

	/*!
	* @brief Gain access to the synchronisation object.
	* @return True if the operation succeeded, false otherwise.
	*/
	bool lock(void)
	{
		if (m_sync)
		{
			return (m_isLocked = m_sync->lock());
		}
		else
		{
			return false;
		}
	}

	/*!
	* @brief Gain access to the synchronisation object.
	* @return True if the operation succeeded, false otherwise.
	* @param p A paramater that is passed on to the underlying synchronisation object. In case that this
	* class is used on an mutex, the parameter might be an <i> unsigned int </i>, defining the timeout.
	*/
	template <class Util_ParamType>
	bool lock(Util_ParamType p)
	{
		if (m_sync && !m_isLocked)
		{
			return (m_isLocked = m_sync->lock(p));
		}
		else
		{
			return false;
		}
	}
	/*!
	* @brief Release access to the synchronisation object.
	* @return True if the operation succeeded, false otherwise.
	*/
	bool unlock(void)
	{
		if (m_sync && m_isLocked)
		{
			m_isLocked = !m_sync->unlock();
			return !m_isLocked;
		}
		else
		{
			return false;
		}
	}

protected:
	T *m_sync;			/*!< The synchronisation object that is beeing handled by the SingleLock instance */
	bool m_isLocked;	/*!< Remember if the sychronisation object is locked or not. We want to avoid
						* unlocking it twice. (unlocking an object twice causes problems with Windows critical
						* memory sections, used in \e Sync class)*/
}; // Util_SingleLock

#endif
