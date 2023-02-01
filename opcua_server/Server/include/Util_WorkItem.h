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
#ifndef _UTIL_WORKITEM_H_
#define _UTIL_WORKITEM_H_

	class Util_WorkItemStore;

	class Util_WorkItem
	{
	public:
		Util_WorkItem(): pNext(NULL), pPrev(NULL) {}
		virtual ~Util_WorkItem(){};
		virtual void execute() = 0;

		// used by NativeLinkedQueue:
		Util_WorkItem   *pNext, *pPrev;
	};


#endif // _WORKITEM_H_
