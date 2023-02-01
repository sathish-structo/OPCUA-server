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
#include "TestServerMethodCallTransaction.h"
#include "ServerMethodCallRequest.h"
#include "ServerBaseNode.h"
#include "ServerAddressSpaceRoot.h"
#include "ServerMethod.h"
#include "DiagnosticMessages.h"

TestServerMethodCallTransaction::TestServerMethodCallTransaction(TBHandle otbHandle)
	: Server::MethodCallTransaction(otbHandle)
{
}

TestServerMethodCallTransaction::~TestServerMethodCallTransaction()
{
}

EnumStatusCode TestServerMethodCallTransaction::handleRequests()
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

	return Server::MethodCallTransaction::handleRequests();
}
