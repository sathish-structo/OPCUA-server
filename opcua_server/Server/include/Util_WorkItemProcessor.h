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
#ifndef _UTIL_WORKITEMPROCESSOR_H_
#define _UTIL_WORKITEMPROCESSOR_H_

#include "Util_Processor.h"
#include "Util_WorkItem.h"

//lint -sem(WorkItemProcessor::addWorkItem, custodial(1))
class Util_WorkItemProcessor
{
public:
	Util_WorkItemProcessor();
	virtual ~Util_WorkItemProcessor();

	EnumStatusCode start();
	EnumStatusCode stop();
	bool isRunning();
	EnumStatusCode wait(OTUInt32 timeout);
	EnumStatusCode addWorkItem(Util_WorkItem *pWorkItem);

	size_t getNumWorkItems();
	void setMaxQueuedWorkItems(OTUInt32 value);
	OTUInt32 getMaxQueuedWorkItems();

protected:
	Util_Processor m_processor;
};

#endif // _WORKITEMPROCESSOR_H_
