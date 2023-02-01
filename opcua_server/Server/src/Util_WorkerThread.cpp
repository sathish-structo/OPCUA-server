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
#include "stdafx.h"

#include "Util_WorkerThread.h"
#include "Util_WorkItemStore.h"

#include "Util_WorkItem.h"

	Util_WorkerThread::Util_WorkerThread(Util_Event *pEvent,Util_WorkItemStore *pStore)
	: m_pStore(pStore)
	, m_pEvent(pEvent)
	, m_bKeepOnRunning(false)
	{
	}

	Util_WorkerThread::~Util_WorkerThread()
	{
	}


	bool Util_WorkerThread::start()
	{
		interlockedSet(&m_bKeepOnRunning, true);
		return Util_Thread::start();
	}

	long Util_WorkerThread::run()
	{
		Util_WorkItem *pWorkItem;
		if(m_pStore && m_pEvent)
		{
			while(interlockedGet(&m_bKeepOnRunning) || m_pStore->size() > 0)
			{
				if(m_pEvent->waitFor(m_timeToWait))
				{
					while(m_pStore->size() > 0)
					{
						pWorkItem = m_pStore->pop();
						if(pWorkItem)
						{
							pWorkItem->execute();
							delete pWorkItem;
						}
					}
				}
			}
		}
		return 0;
	}

	bool Util_WorkerThread::stop()
	{
		m_pStore->stop();
		interlockedSet(&m_bKeepOnRunning, false);
		return true;
	}

