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
//                - SOCmnMutex: Mutex synchronisation class
//                - SOCmnSingleLock: Template for locking of syncs

#ifndef _UTIL_SYNC_H_
#define _UTIL_SYNC_H_


#ifdef SOOS_POSIX
#include <pthread.h>
#endif

/*!
 * @brief Implements a synchronisation class, similar to mutex (but much faster - at least on Windows).
 *
 * This is a synchronisation class for threads. Its implementation relies on:
 *		- CRITICAL_SECTION on Windows
 *		- pthread_mutex_t on POSIX
 *
 * The implementation of the class is verry fast (much faster than Mutex or Event on Windows). But it also
 * has a drawback, compared to Mutex: there is no lock method taking a timeout.
 * @see Mutex, Event, Events
 */
class Util_Sync
{
public:
	Util_Sync();
	virtual ~Util_Sync();

	// gain access to the mutex object
	virtual bool lock();

	// release access to the mutex object
	virtual bool unlock();

protected:
#ifdef SOOS_POSIX
	pthread_mutex_t m_mutex;				/*!< Linux: The underlying mutex structure, used on POSIX systems */
	OTAtomic m_owner;
	int m_recur;
#endif
#ifdef SOOS_WINDOWS
	CRITICAL_SECTION m_criticalSection;		/*!< Windows: The underlying CRITICAL_SECTION structure */
#endif
#ifdef _DEBUG
protected:
	std::string m_name;			/**< In debug mode, the mutex will have a name. This is used for debugging purpose. */
public:
	inline std::string getName() const { return m_name; }
	inline void setName(const std::string& name) { m_name = name; }
#endif
};

#endif
