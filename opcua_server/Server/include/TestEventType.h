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
#ifndef _TEST_EVENTTYPE_H_
#define _TEST_EVENTTYPE_H_

#include "CustomVariable.h"
#include "ServerObjectType.h"
#include "ServerLimitAlarm.h"
#include "EventSourceObject.h"
#include "ServerMethod.h"
#include "ServerMethodCallRequest.h"
#include "ServerVariable.h"

#include "Util_Mutex.h"
#include "Util_TimerThread.h"

// forward references
class ApplicationModule;

class TestEventType : public Server::ObjectType
{
public:
	static NodeId* s_typeId;
	static QualifiedName* s_propertyBrowseName;
	static NodeId* s_propertyId;
	static QualifiedName* s_userSpecificPropertyBrowseName;
	static NodeId* s_userSpecificPropertyId;

public:
	TestEventType();
	virtual ~TestEventType();

	static EnumStatusCode initialize(OTUInt16 namespaceIndex);
	static void uninitialize();

};

class TestUserSpecificEventType : public TestEventType
{
public:
	static NodeId* s_typeId;

public:
	TestUserSpecificEventType();
	virtual ~TestUserSpecificEventType();

	static EnumStatusCode initialize(OTUInt16 namespaceIndex);
	static void uninitialize();

};

#endif
