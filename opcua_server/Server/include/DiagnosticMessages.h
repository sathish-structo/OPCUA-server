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
#ifndef _DIAGNOSTIC_MESSAGES_H_
#define _DIAGNOSTIC_MESSAGES_H_

#include "DiagnosticInfo.h"
#include "StringTable.h"
#include "ServerDataTransaction.h"
#include "LocalizedText.h"

using namespace SoftingOPCToolbox5;

class DiagnosticMessages
{
public:
	// Diagnostic messages
	typedef enum
	{
		EnumDiagnosticMessage_NoError		= 0,
		EnumDiagnosticMessage_GeneralError,
		EnumDiagnosticMessage_NUMBER_OF_ENTRIES
	} EnumDiagnosticMessage;

	typedef enum
	{
		EnumDiagnosticMessageLevel_Service,
		EnumDiagnosticMessageLevel_Operation
	} EnumDiagnosticMessageLevel;

	static DiagnosticInfo createDiagnosticMessage(EnumDiagnosticMessageLevel level, EnumDiagnosticMessage messageType, Server::DataTransaction* pTransaction);

private:
	struct DiagnosticMessage
	{
	public:
		const OTChar* namespaceUri;
		const OTChar* symbolicId;
		const OTChar* locale;
		const OTChar* localizedText;
	};

	static DiagnosticMessage s_diagnosticMessages[EnumDiagnosticMessage_NUMBER_OF_ENTRIES];
};

#endif // _DIAGNOSTIC_MESSAGES_H_
