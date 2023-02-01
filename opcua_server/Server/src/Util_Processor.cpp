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
#include "Util_Processor.h"


	Util_Processor::Util_Processor()
		: m_pThread(NULL),
		m_maxQueuedWorkItems(0)
	{
	}

	Util_Processor::~Util_Processor()
	{
		if(m_pThread)
		{
			delete m_pThread;
		}
	}

	bool Util_Processor::start()
	{
		if(!m_event.isInit())
		{
			if(!m_event.create(false,true))
			{
				return false;
			}
		}

		if(!m_pThread)
		{
			m_pThread = new Util_WorkerThread(&m_event,&m_store);
		}

		if(m_pThread)
		{
			return m_pThread->start();
		}
		return false;
	}

	bool Util_Processor::isRunning()
	{
		if(m_pThread)
		{
			return m_pThread->isRunning();
		}
		return false;
	}

	bool Util_Processor::stop()
	{
		bool ret = false;
		if(m_pThread)
		{
			ret = m_pThread->stop();
			if(ret)
			{
				// We signal the thread, so that it wakes wrom waiting on the event
				m_event.signal();
			}
		}
		return ret;
	}

	bool Util_Processor::wait(OTUInt32 timeout)
	{
		if(m_pThread)
		{
			return m_pThread->wait(timeout);
		}
		return false;
	}

	bool Util_Processor::addItem(Util_WorkItem *item)
	{
		bool ret = false;
		if(m_maxQueuedWorkItems == 0 || m_store.size() < m_maxQueuedWorkItems)
		{
			ret = m_store.push(item);
			if(ret)
			{
				m_event.signal();
			}
		}
		return ret;
	}

	Util_WorkItemStore& Util_Processor::getStore()
	{
		return m_store;
	}

	bool Util_Processor::changePriority(bool increment)
	{
		return m_pThread->changePriority(increment);
	}

