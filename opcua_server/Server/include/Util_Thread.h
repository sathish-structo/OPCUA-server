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
//  Description : Basic thread object class
//                - Thread: thread object

#ifndef _UTIL_THREAD_H_
#define _UTIL_THREAD_H_

#include "Util_Event.h"
#include "Util_Mutex.h"

#ifdef SOOS_POSIX

#include <pthread.h>
#include <signal.h>

#endif // SOOS_POSIX


/*! @brief Threading class. Users of this class must reimplement the run() method. That is where
 * all the processing should go.
 * @note The threads terminates its execution after the run() method exits. A mechanism for stoping/exiting
 * the run method is not provided. See the documentation of the terminate() method for an example of
 * how to implement a stop() method.
 * @note Make sure that the thread has terminated before destroying the thread object. The destructor
 * will call the terminate() method.
 */
class Util_Thread
{
#ifdef SOOS_WINDOWS
	typedef DWORD ThreadIdType;	/**< Define the thread id type on windows */
#endif

#ifdef SOOS_POSIX
	typedef pthread_t ThreadIdType;	/**< Define the thread id type on windows */
#endif
public:
	Util_Thread(void);
	virtual ~Util_Thread(void);

	virtual bool start();
	virtual void terminate();

	virtual bool isRunning();
	virtual bool changePriority(bool increment);
	virtual bool wait(long timeout);

	virtual long run() = 0;

	static ThreadIdType getCurrentThreadId();
protected:
	virtual void cleanup();

#ifdef SOOS_WINDOWS
	// We declare the Windows Thread function as friend, so we can set the m_isRunning flag and cleanup
	// the threads state, after the run() method exists.
	friend DWORD WINAPI Util_WinThreadProc( LPVOID lpParam );

	DWORD m_id;			/*!< Windows: The thread id */
	HANDLE m_handle;	/*!< Windows: The thread handle */
#endif

#ifdef SOOS_POSIX
	// We declare the Posix Thread function as friend, so we can set the m_isRunning flag and cleanup
	// the threads state, after the run() method exists.
	friend void * Util_PosixThreadProc(void *pParam);

	bool m_isStarted;
	pthread_t m_thread;		/*!< Linux: The thread object */
#endif

	OTAtomic m_isRunning;	/*!< True if the thread is running */
};

/*! @brief Returns true if the thread is running.
 * @note The thread is considered to be running as long as the run() method is running. The \e m_isRunning
 *       flag is set to false as soon as the run() method finishes.
 */
inline bool Util_Thread::isRunning()
{
	return interlockedGet(&m_isRunning) != false;
}

#endif
