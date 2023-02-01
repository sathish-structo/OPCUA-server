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
#ifndef _UTIL_WORKITEM_STORE_H_
#define _UTIL_WORKITEM_STORE_H_

#include "Util_NativeLinkedQueue.h"
#include "Util_WorkItem.h"
#include "Util_WorkerThread.h"
#include "Util_Sync.h"
#include "Util_SingleLock.h"

	class Util_WorkItemStore
	{
	public:
		typedef Util_Sync Util_SyncClass;
		typedef Util_SingleLock<Util_SyncClass> LockClass;

	public:
		Util_WorkItemStore();
		virtual ~Util_WorkItemStore();

		void clear();

		bool push(Util_WorkItem *workItem);
		Util_WorkItem *pop();
		size_t size();
		void stop();

		inline Util_Sync* getSyncObject()
		{
			return &m_sync;
		}

	protected:
		Util_NativeLinkedQueue<Util_WorkItem> m_queue;
		Util_SyncClass m_sync;
		bool m_stopped;
	};

#endif

