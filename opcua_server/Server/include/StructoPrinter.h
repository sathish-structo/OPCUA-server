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
#ifndef _PRINTER_MODULE_H_
#define _PRINTER_MODULE_H_
#if TB5_ALARMS

#include "CustomVariable.h"
#include "ServerObject.h"
//#include "ServerExclusiveLimitAlarm.h"
#include "EventSourceObject.h"
#include "ServerMethod.h"
#include "ServerMethodCallRequest.h"
#include "ServerObject.h"
//#include "MeasureConditionBranchPerformanceMethod.h"
#include "ModuleFolder.h"
#include "pthread.h"

#include "Util_Mutex.h"
#include "Util_TimerThread.h"

// forward references
class ApplicationModule;

using namespace SoftingOPCToolbox5;

/**
 * @brief Simulates a boiler monitor, that is triggering alarms in when the boilers  monitored
 * values are out of range.
 */
class PrinterModule
	: public ModuleFolder
{
public:
	PrinterModule();

	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);

	virtual void endModule();
	virtual void Update();	
	pthread_t serverAutoUpdate;

private:
	EventSourceObjectPtr p_monitoredNotificationArea;	
	CustomVariablePtr p_status;
	CustomVariablePtr p_tankStatus;
	CustomVariablePtr p_pallatStatus;
	EventSourceObjectPtr p_motorArea;
	CustomVariablePtr p_distance;
	CustomVariablePtr p_accleration;
	CustomVariablePtr p_velocity;
	void* UpdateData();
	static void *serverUpdateVariables(void *pCtx){
		return reinterpret_cast<PrinterModule*>(pCtx)->UpdateData();
	}
};

#endif
#endif
