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
#include "Util_WorkItemStore.h"
#include "Util_WorkItem.h"

	Util_WorkItemStore::Util_WorkItemStore()
	{
		m_stopped = false;
	}

	Util_WorkItemStore::~Util_WorkItemStore()
	{
		clear();
	}

	void Util_WorkItemStore::clear()
	{
		m_sync.lock();
		Util_NativeLinkedQueue<Util_WorkItem>::iterator it = m_queue.begin();
		while(it != m_queue.end())
		{
			Util_NativeLinkedQueue<Util_WorkItem>::iterator it1 = it++;
			delete (*it1);
		}
		m_queue.clear();
		m_sync.unlock();
	}

	bool Util_WorkItemStore::push(Util_WorkItem *workItem)
	{
		if(workItem != NULL)
		{
			m_sync.lock();
			if (m_stopped)
			{
				m_sync.unlock();
				return false;
			}
			m_queue.push_back(workItem);
			m_sync.unlock();
			return true;
		}
		return false;
	}

	Util_WorkItem *Util_WorkItemStore::pop()
	{
		m_sync.lock();
		Util_WorkItem *ret = NULL;
		if(!m_queue.empty())
		{
			ret = m_queue.front();
			m_queue.pop_front();
		}
		m_sync.unlock();
		return ret;
	}

	size_t Util_WorkItemStore::size()
	{
		size_t queueSize;
		m_sync.lock();
		queueSize = m_queue.size();
		m_sync.unlock();
		return queueSize;
	}

	void Util_WorkItemStore::stop()
	{
		m_sync.lock();
		m_stopped = true;
		m_sync.unlock();
	}
