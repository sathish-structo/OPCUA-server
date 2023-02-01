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
#ifndef _MY_EXCLUSIVELIMIT_ALARM_
#define _MY_EXCLUSIVELIMIT_ALARM_
#if TB5_ALARMS

#include <list>
#include "Util_Sync.h"
#include "LocalizedText.h"
#include "QualifiedName.h"
#include "ServerExclusiveLimitAlarm.h"
#include "ServerVariable.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

class MyExclusiveLimitAlarm: public ExclusiveLimitAlarm
{
public:
	MyExclusiveLimitAlarm();
	virtual ~MyExclusiveLimitAlarm();

	virtual void onVariableChanged(Server::Variable* var, const IDataValue* value);
	virtual void onAcknowledge();
	virtual void onConfirm();

private:
};

typedef ObjectPointer<MyExclusiveLimitAlarm> MyExclusiveLimitAlarmPtr;
typedef ObjectPointer<const MyExclusiveLimitAlarm> MyExclusiveLimitAlarmConstPtr;

#endif
#endif
