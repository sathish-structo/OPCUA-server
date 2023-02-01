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
#ifndef _TBUTIL_PROCESSOR_H_
#define _TBUTIL_PROCESSOR_H_

#include "Util_WorkerThread.h"
#include "Util_WorkItemStore.h"
#include "OTTypes.h"

	class Util_Processor
	{
	public:
		Util_Processor();
		virtual ~Util_Processor();

		bool start();
		bool isRunning();
		bool stop();
		bool addItem(Util_WorkItem *item);
		bool wait(OTUInt32 timeout);
		Util_WorkItemStore& getStore();
		inline void setMaxQueuedWorkItems(OTUInt32 value);
		inline OTUInt32 getMaxQueuedWorkItems();

		bool changePriority(bool increment);

	private:
		Util_WorkerThread *m_pThread;
		Util_Event m_event;
		Util_WorkItemStore m_store;
		OTUInt32 m_maxQueuedWorkItems;
	};

	inline void Util_Processor::setMaxQueuedWorkItems(OTUInt32 value)
	{
		m_maxQueuedWorkItems = value;
	}

	inline OTUInt32 Util_Processor::getMaxQueuedWorkItems()
	{
		return m_maxQueuedWorkItems;
	}

#endif
