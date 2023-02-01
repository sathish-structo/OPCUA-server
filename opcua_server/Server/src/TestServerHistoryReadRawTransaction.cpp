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
#if TB5_HISTORY
#include "TestServerHistoryReadRawTransaction.h"
#include "DiagnosticMessages.h"
#include "Util_TimerThread.h"
#include "ApplicationModule.h"

TestServerHistoryReadRawTransaction::TestServerHistoryReadRawTransaction(TBHandle otbHandle)
	: Server::HistoryReadRawTransaction(otbHandle)
{
}

TestServerHistoryReadRawTransaction::~TestServerHistoryReadRawTransaction()
{
}

EnumStatusCode TestServerHistoryReadRawTransaction::handleRequests()
{
	HistoryReadRawAction* pAction = new HistoryReadRawAction(this);
	OTUInt32 delay = getApplicationModule()->getServiceDelay(EnumNumericNodeId_HistoryReadRequest);

	if(delay != 0)  
	{
		// service shall be executed asynchronously with a delay
		pAction->setAutoDelete(true);
		pAction->setTimeout(delay);
		if(getApplicationModule()->addAction(pAction))
		{
			// ownership successfully transferred to the TimerThread
			pAction = NULL;
		}
	}

	if(pAction)
	{
		// either no delay is set or add action failed
		// ==> execute synchronous
		pAction->execute();
		delete pAction;
	}

	return EnumStatusCode_Good;
}

EnumStatusCode TestServerHistoryReadRawTransaction::executeRequests()
{
	// Supply service diagnostics if requested
	if(DiagnosticInfo::returnServiceLevelDiagnostics(getDiagnosticsToReturn()))
	{
		DiagnosticInfo serviceDiagnostics = DiagnosticMessages::createDiagnosticMessage(
			DiagnosticMessages::EnumDiagnosticMessageLevel_Service, DiagnosticMessages::EnumDiagnosticMessage_NoError, this);

		if(getDiagnosticsToReturn() & EnumDiagnosticsToReturn_ServiceLevel_AdditionalInfo)
		{
			serviceDiagnostics.setAdditionalInfo(_T("No additional information available"));
		}
		if(getDiagnosticsToReturn() & EnumDiagnosticsToReturn_OperationLevel_InnerDiagnostics)
		{
			DiagnosticInfo innerDiagnosticInfo = DiagnosticMessages::createDiagnosticMessage(
				DiagnosticMessages::EnumDiagnosticMessageLevel_Operation, DiagnosticMessages::EnumDiagnosticMessage_NoError, this);

			serviceDiagnostics.setInnerDiagnosticInfo(&innerDiagnosticInfo);
		}

		EnumStatusCode setServiceDiagnosticsResult = setServiceDiagnostics(serviceDiagnostics);
		_UNUSED(setServiceDiagnosticsResult);
		_ASSERTION(StatusCode::isGood(setServiceDiagnosticsResult), _T("setServiceDiagnostics() failed"));
	}

	return HistoryReadRawTransaction::handleRequests();
}

HistoryReadRawAction::HistoryReadRawAction(TestServerHistoryReadRawTransaction* transaction)
{
	m_transaction = transaction;
}

HistoryReadRawAction::HistoryReadRawAction()
{
}

HistoryReadRawAction::~HistoryReadRawAction()
{
}

void HistoryReadRawAction::execute()
{
	HistoryReadRawWorkItem* workItem = new HistoryReadRawWorkItem(m_transaction);
	if(!getApplicationModule()->addWorkItem(workItem))
	{
		workItem->execute();
		delete workItem;
	}
}

HistoryReadRawWorkItem::HistoryReadRawWorkItem(TestServerHistoryReadRawTransaction* transaction)
{
	m_transaction = transaction;
}

HistoryReadRawWorkItem::HistoryReadRawWorkItem()
{
}

HistoryReadRawWorkItem::~HistoryReadRawWorkItem()
{
}

void HistoryReadRawWorkItem::execute()
{
	m_transaction->executeRequests();
}
#endif
