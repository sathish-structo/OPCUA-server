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
#include "TestServerReadTransaction.h"
#include "ServerReadRequest.h"
#include "DiagnosticMessages.h"
#include "ServerBaseNode.h"
#include "DynamicNodeCache.h"

TestServerReadTransaction::TestServerReadTransaction(TBHandle otbHandle)
	: Server::ReadTransaction(otbHandle)
{
}

TestServerReadTransaction::~TestServerReadTransaction()
{
}

EnumStatusCode TestServerReadTransaction::handleRequests()
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

	// handle the read of the dynamic node
	std::vector<Server::ReadRequestPtr> requests = getRequests();

	for(size_t i = 0; i < requests.size(); i++)
	{
		// check the existence of the node first, the node cache lock is not necessary for that
		if(requests[i]->getNode().isNull())
		{
			DynamicNodeCache::instance()->handleReadRequest(this, requests[i]);
		}
		// else existing nodes are handled by the default implementation including the existing dynamic node
	}

	return Server::ReadTransaction::handleRequests();
}
