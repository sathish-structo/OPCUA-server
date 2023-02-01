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
#ifndef _EVENTINGMODULE_H_
#define _EVENTINGMODULE_H_
#if TB5_ALARMS

#include "ServerObject.h"
#include "EventSourceObject.h"
#include "Util_Mutex.h"
#include "Util_TimerThread.h"
#include "ModuleFolder.h"
#include "ServerObjectType.h"
#include "ServerMethod.h"
#include "ServerCondition.h"

using namespace SoftingOPCToolbox5;
using namespace Server;

// forward references
class ApplicationModule;

class EventingModuleEventType;
typedef ObjectPointer<EventingModuleEventType> TestEventTypePtr;
typedef ObjectPointer<const EventingModuleEventType> TestEventTypeConstPtr;

class EventingModuleEventType : public ObjectType
{
public:
	static NodeId* s_testStringProperty;
	static NodeId* s_moodProperty;
	static NodeId* s_nodeId;

public:
	EventingModuleEventType();
	~EventingModuleEventType();

	virtual EnumStatusCode init(const INodeId* typeId);

protected:
	VariablePtr m_testString;
	VariablePtr m_mood;
};

/*!
 @brief Triggers a requested condition.
 */
class TriggerEventMethod
: public Method
{
public:
	TriggerEventMethod(const NodeId& eventSourceId);

	virtual EnumStatusCode handleExecuteRequest(MethodCallTransaction* transaction, MethodCallRequest* request);
private:
	NodeId m_eventSourceId;
};

typedef ObjectPointer < TriggerEventMethod > TriggerEventMethodPtr;

class ScheduledEventAction: public Util_ScheduledAction
{
public:
	ScheduledEventAction();
	~ScheduledEventAction();

	void init(ObjectPtr object);
	void execute();

protected:
	ObjectPtr m_object;
};

/**
 * @brief EventingModule Sets up a set of objects which can generate events. Event generation
 * is triggered by methods which shall be called by the client.
 */
class EventingModule
	: public ModuleFolder
{
public:
	EventingModule();

	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, ObjectPtr parentFolder);

	virtual void endModule();

	/*! @brief Create the custom event type definitions */
	void createCustomEventTypes();

	/*! Creates an instance of a specific Condition or Alarm type */
	static EnumStatusCode createCondition(const INodeId& conditionTypeId, ConditionPtr& condition);
private:

	ScheduledEventAction m_boilerEventGenerator;
	EventSourceObjectPtr m_notificationArea;
	EventSourceObjectPtr m_boiler;
	EventSourceObjectPtr m_triggerEventSink;
	TestEventTypePtr m_testEventType;
	TriggerEventMethodPtr m_triggerEventMethod;
};

#endif
#endif
