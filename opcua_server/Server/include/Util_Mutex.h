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

#ifndef _UTIL_MUTEX_H_
#define _UTIL_MUTEX_H_

#include <string>


#ifdef SOOS_POSIX
	#include <pthread.h>
#endif


/*!
 * @brief Implements a Mutex synchronisation class.
 * @see Sync
 */
class Util_Mutex
{
public:
	Util_Mutex();
	virtual ~Util_Mutex();

	// gain access to the mutex object
	virtual	bool lock(unsigned long timeout = INFINITE);

	// release access to the mutex object
	virtual bool unlock(void);

protected:
#ifdef SOOS_POSIX
	pthread_mutex_t m_mutex;	/*!< Linux: The mutex object */
#endif
#ifdef SOOS_WINDOWS
	HANDLE m_mutex;			/*!< Windows: The handle to the mutex */
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
