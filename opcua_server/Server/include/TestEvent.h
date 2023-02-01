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
#ifndef _TEST_EVENT_H_
#define _TEST_EVENT_H_
#if TB5_ALARMS

#include "CustomVariable.h"
#include "ServerObject.h"
#include "ServerLimitAlarm.h"
#include "EventSourceObject.h"
#include "ServerMethod.h"
#include "ServerMethodCallRequest.h"
#include "TestEventType.h"

#include "Util_Mutex.h"
#include "Util_TimerThread.h"

using namespace SoftingOPCToolbox5;

// forward references
class ApplicationModule;

class TestEvent : public Server::BaseEvent
{
public:
	static ObjectPointer<TestEvent> create();
	virtual ~TestEvent();
	EnumStatusCode init(const INodeId* typeId);

protected:
	TestEvent();
};

typedef ObjectPointer<TestEvent> TestUserSpecificEventPtr;
typedef ObjectPointer<const TestEvent> TestUserSpecificEventConstPtr;

#endif
#endif
