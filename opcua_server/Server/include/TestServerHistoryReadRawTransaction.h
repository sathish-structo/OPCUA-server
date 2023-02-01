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
#ifndef _TestServerHistoryReadRawTransaction_H_
#define _TestServerHistoryReadRawTransaction_H_
#if TB5_HISTORY

#include "ServerHistoryReadRawTransaction.h"
#include "Util_TimerThread.h"
#include "Util_WorkItem.h"

using namespace SoftingOPCToolbox5;

class TestServerHistoryReadRawTransaction
	: public Server::HistoryReadRawTransaction
{
public:
	TestServerHistoryReadRawTransaction(TBHandle otbHandle);

public:
	/*! Destructor */
	virtual ~TestServerHistoryReadRawTransaction();

	/*! Callback function to handle all read requests of the data transaction in one call.
	* That allows to sort the different requests e.g. depending on the target which contains the requested value. */
	virtual EnumStatusCode handleRequests();
	EnumStatusCode executeRequests();

};
typedef ObjectPointer<TestServerHistoryReadRawTransaction> TestServerHistoryReadRawTransactionPtr;

// for delayed results
class HistoryReadRawWorkItem : public Util_WorkItem
{
public:
	HistoryReadRawWorkItem(TestServerHistoryReadRawTransaction* transaction);
	~HistoryReadRawWorkItem();
	virtual void execute();
private:
	HistoryReadRawWorkItem();
	TestServerHistoryReadRawTransactionPtr m_transaction;
};

class HistoryReadRawAction
	: public Util_ScheduledAction
{
public:
	HistoryReadRawAction(TestServerHistoryReadRawTransaction* transaction);
	virtual ~HistoryReadRawAction();
	virtual void cleanUp() { execute(); };
	virtual void execute();
private:
	HistoryReadRawAction();
	TestServerHistoryReadRawTransactionPtr m_transaction;
};


#endif
#endif // _TestServerHistoryReadRawTransaction_H_
