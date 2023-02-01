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
#ifndef _OFF_THE_SHELF_MODULE_H_
#define _OFF_THE_SHELF_MODULE_H_
#if TB5_ALARMS

#include "ServerObject.h"

#include "ServerTripAlarm.h"
#include "ServerOffNormalAlarm.h"
#include "ServerExclusiveLimitAlarm.h"
#include "ServerNonExclusiveLimitAlarm.h"
#include "ServerExclusiveLevelAlarm.h"
#include "ServerNonExclusiveLevelAlarm.h"
#include "ServerExclusiveRateOfChangeAlarm.h"
#include "ServerNonExclusiveRateOfChangeAlarm.h"
#include "ServerExclusiveDeviationAlarm.h"
#include "ServerNonExclusiveDeviationAlarm.h"

#include "Util_Mutex.h"
#include "Util_TimerThread.h"
#include "ModuleFolder.h"

using namespace SoftingOPCToolbox5::Server;

/**
 * @brief Manager for the OffTheShelf alarms
 */
class OffTheShelfAlarmsModule
	: public ModuleFolder
{
public:
	OffTheShelfAlarmsModule();

	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);

	virtual void endModule();

private:
	TripAlarmPtr                     m_tripAlarm;
	OffNormalAlarmPtr                m_offNormalAlarm;
	ExclusiveLimitAlarmPtr           m_exclusiveLimitAlarm;
	NonExclusiveLimitAlarmPtr        m_nonExclusiveLimitAlarm;
	ExclusiveLevelAlarmPtr           m_exclusiveLevelAlarm;
	NonExclusiveLevelAlarmPtr        m_nonExclusiveLevelAlarm;
	ExclusiveRateOfChangeAlarmPtr    m_exclusiveRateOfChangeAlarm;
	NonExclusiveRateOfChangeAlarmPtr m_nonExclusiveRateOfChangeAlarm;
	ExclusiveDeviationAlarmPtr       m_exclusiveDeviationAlarm;
	NonExclusiveDeviationAlarmPtr    m_nonExclusiveDeviationAlarm;
};

OffTheShelfAlarmsModule* getOffTheShelfAlarmModule();

#endif
#endif //_OFF_THE_SHELF_MODULE_H_
