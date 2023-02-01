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
#include "DiagnosticMessages.h"

// This array contains the error definitions
DiagnosticMessages::DiagnosticMessage DiagnosticMessages::s_diagnosticMessages[DiagnosticMessages::EnumDiagnosticMessage_NUMBER_OF_ENTRIES] = {
	{ _T("urn:Softing:DiagnosticMessages"), _T("NoError"), _T("en"), _T("No error") },
	{ _T("urn:Softing:DiagnosticMessages"), _T("GeneralError"), _T("en"), _T("General error") }
};

DiagnosticInfo DiagnosticMessages::createDiagnosticMessage(EnumDiagnosticMessageLevel level, EnumDiagnosticMessage messageType, Server::DataTransaction* pTransaction)
{
	EnumStatusCode result = EnumStatusCode_Good;
	_UNUSED(result); // used for assertions only

	StringTablePtr stringTable = pTransaction->getStringTable();
	OTUInt32 diagnosticsToReturn = pTransaction->getDiagnosticsToReturn();

	DiagnosticInfo diagnosticInfo;
	if(level == EnumDiagnosticMessageLevel_Service)
	{
		if(diagnosticsToReturn & EnumDiagnosticsToReturn_ServiceLevel_SymbolicId)
		{
			result = diagnosticInfo.setNamespaceUri(s_diagnosticMessages[messageType].namespaceUri, stringTable);
			_ASSERTION(StatusCode::isGood(result), _T("setNamespaceUri() failed"));

			result = diagnosticInfo.setSymbolicId(s_diagnosticMessages[messageType].symbolicId, stringTable);
			_ASSERTION(StatusCode::isGood(result), _T("setSymbolicId() failed"));
		}

		if(diagnosticsToReturn & EnumDiagnosticsToReturn_ServiceLevel_LocalizedText)
		{
			result = diagnosticInfo.setLocale(s_diagnosticMessages[messageType].locale, stringTable);
			_ASSERTION(StatusCode::isGood(result), _T("setLocale() failed"));

			result = diagnosticInfo.setLocalizedText(s_diagnosticMessages[messageType].localizedText, stringTable);
			_ASSERTION(StatusCode::isGood(result), _T("setLocalizedText() failed"));
		}
	}
	else if(level == EnumDiagnosticMessageLevel_Operation)
	{
		if(diagnosticsToReturn & EnumDiagnosticsToReturn_OperationLevel_SymbolicId)
		{
			result = diagnosticInfo.setNamespaceUri(s_diagnosticMessages[messageType].namespaceUri, stringTable);
			_ASSERTION(StatusCode::isGood(result), _T("setNamespaceUri() failed"));

			result = diagnosticInfo.setSymbolicId(s_diagnosticMessages[messageType].symbolicId, stringTable);
			_ASSERTION(StatusCode::isGood(result), _T("setSymbolicId() failed"));
		}

		if(diagnosticsToReturn & EnumDiagnosticsToReturn_OperationLevel_LocalizedText)
		{
			result = diagnosticInfo.setLocale(s_diagnosticMessages[messageType].locale, stringTable);
			_ASSERTION(StatusCode::isGood(result), _T("setLocale() failed"));

			result = diagnosticInfo.setLocalizedText(s_diagnosticMessages[messageType].localizedText, stringTable);
			_ASSERTION(StatusCode::isGood(result), _T("setLocalizedText() failed"));
		}
	}

	return diagnosticInfo;
}
