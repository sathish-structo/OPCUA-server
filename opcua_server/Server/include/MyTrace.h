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
#ifndef _MY_TRACE_
#define _MY_TRACE_

class MyTrace
{
public:
	static void OTAPI_CALL onLogMessage(const tstring& timeStamp,
			const tstring& traceLevel,
			OTUInt32 threadId,
			const tstring& traceGroup,
			const tstring& location,
			const tstring& message);

	static void setTraceFile(const tstring& callbackTracefileName);
private:
	static tstring m_callbackTracefileName;
};

#endif
