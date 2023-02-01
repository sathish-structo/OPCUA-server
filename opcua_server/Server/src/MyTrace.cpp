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
#include "MyTrace.h"

tstring MyTrace::m_callbackTracefileName;

void MyTrace::onLogMessage(const tstring& timeStamp,
			const tstring& traceLevel,
			OTUInt32 threadId,
			const tstring& traceGroup,
			const tstring& location,
			const tstring& message)
{
	_UNUSED(traceGroup);

	FILE* f;
	OTChar locationTruncated[50];
	if(location.length()>29)
	{
		_tcscpy(locationTruncated, location.substr(location.length()-29, 29).c_str());
	}
	else
	{
		_tcscpy(locationTruncated, location.c_str());
	}

	f=_tfopen(MyTrace::m_callbackTracefileName.c_str(),_T("a"));
	if(f)
	{
		_ftprintf (f, _T("%s %7u (0x%08X) %-8s%29s %s\n"), timeStamp.c_str(),threadId, threadId, traceLevel.c_str(), locationTruncated,  message.c_str());
		fclose(f);
	}

	return;
}

void MyTrace::setTraceFile(const tstring& callbackTracefileName)
{
	m_callbackTracefileName = callbackTracefileName;
}
