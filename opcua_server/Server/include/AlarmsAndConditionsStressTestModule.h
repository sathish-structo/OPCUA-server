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
#ifndef _ALARMS_AND_CONDITIONS_STRESS_TEST_MODULE_H_
#define _ALARMS_AND_CONDITIONS_STRESS_TEST_MODULE_H_
#if TB5_ALARMS

#include "ServerMethod.h"
#include "EventSourceObject.h"
#include "Util_Sync.h"
#include "Util_TimerThread.h"
#include "ServerLimitAlarm.h"
#include "ServerVariable.h"
#include "ModuleFolder.h"

using namespace SoftingOPCToolbox5;

class AlarmsAndConditionsStressTestContext;

class CreateNodesForAlarmsAndConditionsStressTest
: public Server::Method
{
public:
	CreateNodesForAlarmsAndConditionsStressTest(OTUInt16 objectsNamespaceIndex);
	void setContext(AlarmsAndConditionsStressTestContext * pContext);
	virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);
private:
	OTUInt16 m_objectsNamespaceIndex;
	AlarmsAndConditionsStressTestContext * m_pContext;

};
typedef ObjectPointer<CreateNodesForAlarmsAndConditionsStressTest> CreateNodesForAlarmsAndConditionsStressTestPtr;
typedef ObjectPointer<const CreateNodesForAlarmsAndConditionsStressTest> CreateNodesForAlarmsAndConditionsStressTestConstPtr;

class DestroyNodesForAlarmsAndConditionsStressTest
: public Server::Method
{
public:
	DestroyNodesForAlarmsAndConditionsStressTest();
	void setContext(AlarmsAndConditionsStressTestContext * pContext);
	virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);
private:
	AlarmsAndConditionsStressTestContext * m_pContext;
};
typedef ObjectPointer<DestroyNodesForAlarmsAndConditionsStressTest> DestroyNodesForAlarmsAndConditionsStressTestPtr;
typedef ObjectPointer<const DestroyNodesForAlarmsAndConditionsStressTest> DestroyNodesForAlarmsAndConditionsStressTestConstPtr;

class StartEventFlowOnTheNodesForAlarmsAndConditionsStressTest
: public Server::Method
{
public:
	StartEventFlowOnTheNodesForAlarmsAndConditionsStressTest();
	void setContext(AlarmsAndConditionsStressTestContext * pContext);
	virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);
private:
	AlarmsAndConditionsStressTestContext * m_pContext;
};

typedef ObjectPointer<StartEventFlowOnTheNodesForAlarmsAndConditionsStressTest> StartEventFlowOnTheNodesForlarmsAndConditionsStressTestPtr;
typedef ObjectPointer<const StartEventFlowOnTheNodesForAlarmsAndConditionsStressTest> StartEventFlowOnTheNodesForlarmsAndConditionsStressTestConstPtr;

class StopEventFlowOnTheNodesForlarmsAndConditionsStressTest
: public Server::Method
{
public:
	StopEventFlowOnTheNodesForlarmsAndConditionsStressTest();
	void setContext(AlarmsAndConditionsStressTestContext * pContext);
	virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);
private:
	AlarmsAndConditionsStressTestContext * m_pContext;
};

typedef ObjectPointer<StopEventFlowOnTheNodesForlarmsAndConditionsStressTest> StopEventFlowOnTheNodesForlarmsAndConditionsStressTestPtr;
typedef ObjectPointer<const StopEventFlowOnTheNodesForlarmsAndConditionsStressTest> StopEventFlowOnTheNodesForlarmsAndConditionsStressTestConstPtr;

class EventFlowAction
: public Util_ScheduledAction
{
public:
	EventFlowAction();
	void setContext(AlarmsAndConditionsStressTestContext * pContent);
	virtual void execute();
private:
	AlarmsAndConditionsStressTestContext * m_pContext;

	void triggerEvents(const std::vector<EventSourceObjectPtr> & eventSources);
};

class AlarmsAndConditionsStressTestContext
{
public:
	AlarmsAndConditionsStressTestContext();
	~ AlarmsAndConditionsStressTestContext();

	Util_Sync & getSync() { return m_eventFlowThread.getSync(); }

	bool createObjects(OTUInt32 count, OTUInt16 namespaceIndex);
	bool startEventFlow(OTUInt32 timeout);
	void stopEventFlow();
	void destroyObjects();

	bool hasObjects() const { return m_hasObjects; }
	bool hasEventFlow() const { return m_hasEventFlow; }

	Server::LimitAlarmPtr & getAlarm()
	{
		return m_limitAlarm;
	}

	const std::vector<EventSourceObjectPtr> & getEventSources()
	{
		return m_eventSources;
	}

	void waitAfterEventFlowThread();

	Server::VariablePtr getEventCountVar()
	{
		return m_eventCountVar;
	}

	OTUInt32 getEventCount()
	{
		return m_eventCount;
	}

	void setEventCount(OTUInt32 value)
	{
		m_eventCount = value;
	}

	unsigned long getTimeout() { return m_timeout; }

	void setShuttingDown() { interlockedSet(&m_shuttingDown, true); }
	bool getShuttingDown() { return interlockedGet(&m_shuttingDown) != 0; }

private:
	OTUInt16 m_namespaceIndex;

	EventSourceObjectPtr m_eventSink;
	std::vector<EventSourceObjectPtr> m_eventSources;
	EventFlowAction  m_eventFlowAction;
	Util_TimerThread m_eventFlowThread;
	Server::LimitAlarmPtr m_limitAlarm;
	bool m_hasObjects;
	bool m_hasEventFlow;
	Server::VariablePtr m_eventCountVar;
	OTUInt32 m_eventCount;
	unsigned long m_timeout;
	OTAtomic m_shuttingDown;
};

class AlarmsAndConditionsStressTestModule
	: public ModuleFolder
{
public:
	AlarmsAndConditionsStressTestModule();

	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);

	virtual void endModule();

private:
	EventSourceObjectPtr m_theFolder;
	AlarmsAndConditionsStressTestContext m_context;
	CreateNodesForAlarmsAndConditionsStressTestPtr m_createNodesMethod;
	StartEventFlowOnTheNodesForlarmsAndConditionsStressTestPtr m_startEventFlowOnNodesMethod;
	StopEventFlowOnTheNodesForlarmsAndConditionsStressTestPtr m_stopEventFlowOnMethodsFlowMethod;
	DestroyNodesForAlarmsAndConditionsStressTestPtr m_destroyNodesMethod;
};

#endif
#endif
