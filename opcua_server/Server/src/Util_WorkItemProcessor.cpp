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
#include "Util_WorkItemProcessor.h"

Util_WorkItemProcessor::Util_WorkItemProcessor()
{
}

Util_WorkItemProcessor::~Util_WorkItemProcessor()
{
}

EnumStatusCode Util_WorkItemProcessor::start()
{
	if(m_processor.start())
	{
		return EnumStatusCode_Good;
	}
	return EnumStatusCode_BadInternalError;
}

EnumStatusCode Util_WorkItemProcessor::stop()
{
	if(m_processor.stop())
	{
		return EnumStatusCode_Good;
	}
	return EnumStatusCode_BadInternalError;
}

bool Util_WorkItemProcessor::isRunning()
{
	return m_processor.isRunning();
}

EnumStatusCode Util_WorkItemProcessor::wait(OTUInt32 timeout)
{
	if(m_processor.wait(timeout))
	{
		return EnumStatusCode_Good;
	}
	return EnumStatusCode_BadTimeout;
}

EnumStatusCode Util_WorkItemProcessor::addWorkItem(Util_WorkItem *pWorkItem)
{
	if(m_processor.isRunning())
	{
		if(m_processor.addItem(pWorkItem))
		{
			return EnumStatusCode_Good;
		}
		else
		{
			return EnumStatusCode_BadInternalError;
		}
	}
	else
	{
		return EnumStatusCode_BadInvalidState;
	}
}

void Util_WorkItemProcessor::setMaxQueuedWorkItems(OTUInt32 value)
{
	m_processor.setMaxQueuedWorkItems(value);
}

OTUInt32 Util_WorkItemProcessor::getMaxQueuedWorkItems()
{
	return m_processor.getMaxQueuedWorkItems();
}

size_t Util_WorkItemProcessor::getNumWorkItems()
{
	return m_processor.getStore().size();
}

