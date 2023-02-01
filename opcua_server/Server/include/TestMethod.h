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
#ifndef _TESTMETHOD_H_
#define _TESTMETHOD_H_

#include "OTMemory.h"
#include "ServerMethod.h"
#include "ServerMethodCallTransaction.h"
#include "ServerMethodCallRequest.h"
#include "Util_WorkItem.h"

using namespace SoftingOPCToolbox5;

class TestMethod;

typedef struct {
	TestMethod *pTestMethod;
	Server::MethodCallTransactionPtr transaction;
	Server::MethodCallRequestPtr request;
}
AsyncExecData;


class TestMethod
	: public Server::Method
{
public:
	TestMethod(unsigned int inputCount, unsigned int outputCount);

	virtual EnumStatusCode handleExecuteRequest(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request);
	virtual EnumStatusCode doRequest(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request);


	bool isAsync() { return m_executeAssync;};
	void setAsync(bool async) { m_executeAssync = async;};

private:
	bool m_executeAssync;
	unsigned int m_inputCount;
	unsigned int m_outputCount;

};
typedef ObjectPointer<TestMethod> TestMethodPtr;
typedef ObjectPointer<const TestMethod> TestMethodConstPtr;

class TestPerfomanceMethod
	: public Server::Method
{
public:
	TestPerfomanceMethod();

	virtual EnumStatusCode handleExecuteRequest(
		Server::MethodCallTransaction* /* transaction */ ,
		Server::MethodCallRequest* request);
	virtual EnumStatusCode doRequest(Server::MethodCallRequest* request);


private:
};
typedef ObjectPointer<TestPerfomanceMethod> TestPerfomanceMethodPtr;
typedef ObjectPointer<const TestPerfomanceMethod> TestPerfomanceMethodConstPtr;


class TestParallelAccessMethod
	: public Server::Method
{
public:
	TestParallelAccessMethod();
	static ObjectPointer<TestParallelAccessMethod> initialize(OTUInt16 namespaceIndex, const tstring& name);

	virtual EnumStatusCode handleExecuteRequest(
		Server::MethodCallTransaction* /* transaction */ ,
		Server::MethodCallRequest* request);
private:
};
typedef ObjectPointer<TestParallelAccessMethod> TestParallelAccessMethodPtr;
typedef ObjectPointer<const TestParallelAccessMethod> TestParallelAccessMethodConstPtr;

class ParallelAccessWorkItem : public Util_WorkItem
{
public:
	ParallelAccessWorkItem(SoftingOPCToolbox5::Server::MethodCallRequestPtr request);
	~ParallelAccessWorkItem();
	virtual void execute();
private:
	SoftingOPCToolbox5::Server::MethodCallRequestPtr m_request;
};

// used to test event queue overflow
class SetQueueSizeMethod
	: public Server::Method
{
public:
	SetQueueSizeMethod();

	virtual EnumStatusCode handleExecuteRequest(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request);

private:
};
typedef ObjectPointer<SetQueueSizeMethod> SetQueueSizeMethodPtr;
typedef ObjectPointer<const SetQueueSizeMethod> SetQueueSizeMethodConstPtr;

// used to test injection of data changes directly into server MI
class InsertDataNotificationMethod
	: public Server::Method
{
public:
	InsertDataNotificationMethod();

	virtual EnumStatusCode handleExecuteRequest(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request);

private:
};
typedef ObjectPointer<InsertDataNotificationMethod> InsertDataNotificationMethodPtr;
typedef ObjectPointer<const InsertDataNotificationMethod> InsertDataNotificationMethodConstPtr;

#if TB5_ALARMS
// used to test injection of event notifications directly into server MI
class InsertEventNotificationMethod
	: public Server::Method
{
public:
	InsertEventNotificationMethod();

	virtual EnumStatusCode handleExecuteRequest(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request);

private:
};
typedef ObjectPointer<InsertEventNotificationMethod> InsertEventNotificationMethodPtr;
typedef ObjectPointer<const InsertEventNotificationMethod> InsertEventNotificationMethodConstPtr;
#endif

// used to enable / disable tracing
class EnableTracingMethod
	: public Server::Method
{
public:
	EnableTracingMethod();

	virtual EnumStatusCode handleExecuteRequest(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request);

private:
	tstring m_oldFileName;
	OTUInt32 m_maxFileSize;
	OTUInt32 m_maxBackups;
	EnumTraceLevel m_oldTraceLevel;
	EnumTraceGroup m_oldTraceGroup;
};
typedef ObjectPointer<EnableTracingMethod> EnableTracingMethodPtr;
typedef ObjectPointer<const EnableTracingMethod> EnableTracingMethodConstPtr;

// used to test setting of attributes directly by method (not setAttribute)
class SetAttributeByTypeMethod
	: public Server::Method
{
public:
	SetAttributeByTypeMethod();

	virtual EnumStatusCode handleExecuteRequest(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request);

private:
};
typedef ObjectPointer<SetAttributeByTypeMethod> SetAttributeByTypeMethodPtr;
typedef ObjectPointer<const SetAttributeByTypeMethod> SetAttributeByTypeMethodConstPtr;

// used to test value validation / default value with "HasArgumentDescription" and "HasOptionalInputArgumentDescription"
class ArgumentDescriptionMethod
	: public Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);

	virtual EnumStatusCode handleExecuteRequest(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request);

private:
};
typedef ObjectPointer<ArgumentDescriptionMethod> ArgumentDescriptionMethodPtr;
typedef ObjectPointer<const ArgumentDescriptionMethod> ArgumentDescriptionMethodConstPtr;

#endif // _TESTMETHOD_H_
