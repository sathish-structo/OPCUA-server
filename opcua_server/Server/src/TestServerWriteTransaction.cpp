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
#include "TestServerWriteTransaction.h"
#include "ServerWriteRequest.h"
#include "DiagnosticMessages.h"
#include "DynamicNodeCache.h"
#include "ServerAddressSpaceRoot.h"

TestServerWriteTransaction::TestServerWriteTransaction(TBHandle tbHandle)
	: WriteTransaction(tbHandle)
{
}

TestServerWriteTransaction::~TestServerWriteTransaction()
{
}

EnumStatusCode TestServerWriteTransaction::handleRequests()
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

	// Process the requests
	std::vector< ObjectPointer<Server::WriteRequest> > requests = getRequests();
	for(unsigned i = 0; i < requests.size(); i++)
	{
		if (requests[i]->getStatusCode() != EnumStatusCode_Good)
		{
			// is this for the dynamic node?
			if(!DynamicNodeCache::instance()->handleWriteRequest(requests[i]))
			{
				requests[i]->complete();
				continue;
			}
		}

		ObjectPointer<Server::BaseNode> pNode = requests[i]->getNode();

		if(pNode.isNotNull())
		{
			pNode->handleWriteRequest(this, requests[i]);
		}
		else // node not found!
		{
			requests[i]->setStatusCode(EnumStatusCode_BadNodeIdUnknown);
			requests[i]->complete();
		}
	}

	return EnumStatusCode_Good;
}

