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
#ifndef _UTIL_WORKERTHRED_H_
#define _UTIL_WORKERTHRED_H_

#include "Util_Thread.h"
#include "Util_Event.h"

	class Util_WorkItemStore;
	class Util_WorkItem;

	class Util_WorkerThread: public Util_Thread
	{
	public:
		static const OTUInt32 m_timeToWait = 3000;

	public:
		Util_WorkerThread( Util_Event *pEvent, Util_WorkItemStore *pStore );
		virtual ~Util_WorkerThread();

		virtual bool start();
		virtual long run();
		virtual bool stop();

	protected:
		Util_WorkItemStore *m_pStore;
		Util_Event *m_pEvent;
		OTAtomic m_bKeepOnRunning;
	};


#endif

