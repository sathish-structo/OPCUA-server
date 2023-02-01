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

#include "stdafx.h"
#include "Util_Thread.h"

#ifdef SOOS_WINDOWS
/*! @brief WINDOWS: Windows 'thread function'. This method does nothing but calling the run() method of the
 * Thread object provided as parameter.
 */
DWORD WINAPI Util_WinThreadProc( LPVOID lpParam )
{
	DWORD ret;
	Util_Thread *thread = (Util_Thread *)lpParam;
	ret = (DWORD)thread->run();
	// We clean up the threads state.
	thread->cleanup();
	return ret;
}
#endif

#ifdef SOOS_POSIX
/*! @brief LINUX: POSIX 'thread function'. This method does nothing but calling the run() method of the
 * Thread object provided as parameter.
 */
void * Util_PosixThreadProc(void *pParam)
{
	Util_Thread *thread = (Util_Thread *)pParam;
	long ret = thread->run();
	// We clean up the threads state.
	thread->cleanup();
	return (void*)ret;
}
#endif

/*! @brief Constructor. Creates a thread in inactive state.
 */
#ifdef SOOS_WINDOWS
Util_Thread::Util_Thread(void)
: m_id(0)
, m_handle(NULL)
, m_isRunning(false)
{
}
#endif
#ifdef SOOS_POSIX
Util_Thread::Util_Thread(void)
 : m_isStarted(false)
 , m_isRunning(false)
{
}
#endif

/*! @brief Destructor
 */
Util_Thread::~Util_Thread(void)
{
	// Verify that thread is running, and terminate it, if it's the case
	if(isRunning())
	{
		terminate();
	}
}

/*! @brief Reset all the internal members. This method is intended to be used internally, for cleaning up the
 * threads state, for example, after the run() method finishes.
 */
void Util_Thread::cleanup()
{
#ifdef SOOS_WINDOWS
	m_id = 0;
#endif
	interlockedSet(&m_isRunning, false);
	SoftingOPCToolbox5::cleanupUserThreadState();
}

/*! @brief Starts the thread.
 * @return - true If thread has successfully been started
 *         - false otherwise
 */
bool Util_Thread::start()
{
	if(isRunning())
	{
		// Thread seems to be running already
		return false;
	}

	interlockedSet(&m_isRunning, true);

#ifdef SOOS_WINDOWS
	if ((m_handle = CreateThread(NULL,0,Util_WinThreadProc,this,0,&m_id)) != NULL)
	{
		return true;
	}
#endif
#ifdef SOOS_POSIX
	if (0 == pthread_create(&m_thread, opcua_p_thread_attr, Util_PosixThreadProc, this))
	{
		m_isStarted = true;
		return true;
	}
#endif

	interlockedSet(&m_isRunning, false);
	return false;
}

/*! @brief Forces termination of the thread.
 * This method forces the termination of the thread, and should be used with care. It should only
 * be used if the 'normal' termination of the thread fails. A normal termination of the thread
 * would be for the thread to exit the \e run() method.
 * This class does not provide any mechanism for exiting the run() method. Such a mechanism should be
 * implemented in the class where run() is implemented. In the sample code below, the call of the stop()
 * method cause the run() method to exit immediatly:
 \code
 class MyThread: public Util_Thread
 {
 public:
 MyThread(): m_keepOnRunning(true)
 {
 m_event.create();
 }

 virtual ~MyThread()
 {
 }

 bool stop()
 {
 m_keepOnRunning = false;
 return m_event.signal();
 }

 // We overwrite the start method, so that we get a chance to set
 // the m_keepOnRunning flag to true.
 bool start()
 {
 m_keepOnRunning = true;
 return Thread::start();
 }

 long run()
 {
 while(m_keepOnRunning)
 {
 //
 // implement your code here
 //

 // Sleep for 1000 millis while waiting for an event
 m_event.waitFor(1000);
 }
 return 0;
 }

 protected:
 Event m_event;
 bool m_keepOnRunning;
 };
 \endcode
 *
 * And this is how the stop() method would be used:
 \code
 void run_and_stop_thread{)
 {
 MyThread myThread;
 myThread.start();
 //
 // do something in this time
 //
 bool threadStopSucceeded;
 if(threadStopSucceeded = myThread.stop())
 {
 // Wait for the normal termination of the thread
 threadStopSucceeded = myThread.wait(1000);
 }
 // If normal termination failed, force the thread to terminate
 if(!threadStopSucceeded)
 {
 myThread.terminate();
 }
 }
 \endcode
 */
void Util_Thread::terminate()
{
#ifdef SOOS_WINDOWS
	if(m_handle)
	{
		if(!TerminateThread(m_handle,0))
		{
			_REPORT_ASSERTION(_T("TerminateThread failed"));
		}
		if(!CloseHandle(m_handle))
		{
			_REPORT_ASSERTION(_T("CloseHandle failed"));
		}
	}
	m_handle = NULL;
	m_id = 0;
#endif
#ifdef SOOS_POSIX
	pthread_kill(m_thread,SIGKILL);
#endif
	interlockedSet(&m_isRunning, false);
}

/*! @brief Set the threads priority
 * @param priority The new priority of the thread
 */
bool Util_Thread::changePriority(bool increment)
{
	bool result = true;
	if (!isRunning())
	{
		return false;
	}
#ifdef SOOS_WINDOWS
	int newPrio = GetThreadPriority(m_handle);
	switch (newPrio)
	{
	case THREAD_PRIORITY_IDLE:
		if (increment)
		{ 
			newPrio = THREAD_PRIORITY_LOWEST;
		}
		else
		{
			result = false;
		}
		break;
	case THREAD_PRIORITY_LOWEST:
		if (increment)
		{ 
			newPrio = THREAD_PRIORITY_BELOW_NORMAL;
		}
		else
		{
			newPrio = THREAD_PRIORITY_IDLE;
		}
		break;
	case THREAD_PRIORITY_BELOW_NORMAL:
		if (increment)
		{ 
			newPrio = THREAD_PRIORITY_NORMAL;
		}
		else
		{
			newPrio = THREAD_PRIORITY_LOWEST;
		}
		break;
	case THREAD_PRIORITY_NORMAL:
		if (increment)
		{ 
			newPrio = THREAD_PRIORITY_ABOVE_NORMAL;
		}
		else
		{
			newPrio = THREAD_PRIORITY_BELOW_NORMAL;
		}
		break;
	case  THREAD_PRIORITY_ABOVE_NORMAL:
		if (increment)
		{ 
			newPrio = THREAD_PRIORITY_HIGHEST;
		}
		else
		{
			newPrio = THREAD_PRIORITY_NORMAL;
		}
		break;
	case  THREAD_PRIORITY_HIGHEST:
		if (increment)
		{ 
			newPrio = THREAD_PRIORITY_TIME_CRITICAL;
		}
		else
		{
			newPrio = THREAD_PRIORITY_ABOVE_NORMAL;
		}
		break;
	case  THREAD_PRIORITY_TIME_CRITICAL:
		if (increment)
		{ 
			result = false;
		}
		else
		{
			newPrio = THREAD_PRIORITY_HIGHEST;
		}
		break;
	default:
		_REPORT_ASSERTION(_T("unknown thread priority"));
		result = false;
		break;
	}
	if (result)
	{
		result = (TRUE == SetThreadPriority(m_handle, newPrio));
	}
	return result;
#endif

#ifdef SOOS_POSIX
	int policy;
	struct sched_param schedParam;

	result = (0 == pthread_getschedparam(pthread_self(), &policy, &schedParam));
	if (result)
	{
		if (increment)
		{
			schedParam.sched_priority++;
		}
		else
		{
			schedParam.sched_priority++;
		}
	}

	if (result)
	{
		result = (0 == pthread_setschedparam(m_thread,policy,&schedParam));
	}
	return result;
#endif
}

/*! @brief Wait for the thread to end (join the thread).
 * @param timeout The number of milliseconds to wait
 * @return - true - if the thread has stopped in the expected amount of time
 *         - false - if the timeout occured
 */
bool Util_Thread::wait(long timeout)
{
#ifdef SOOS_WINDOWS
	if (m_handle == NULL)
	{
		return false;
	}

	if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, timeout))
	{
		return false;
	}

	CloseHandle(m_handle);
	m_handle = NULL;
	return true;
#endif
#ifdef SOOS_POSIX
	_UNUSED(timeout);

	if (!m_isStarted)
	{
		return false;
	}

	m_isStarted = false;

	// pthread_join - must be called so the thread is cleaned up properly
	return 0 == pthread_join(m_thread,NULL);
#endif
}

/*!
 * @brief get the current thread id;
 */
Util_Thread::ThreadIdType Util_Thread::getCurrentThreadId()
{
#ifdef SOOS_WINDOWS
	return GetCurrentThreadId();
#endif

#ifdef SOOS_POSIX
	return pthread_self();
#endif
}

