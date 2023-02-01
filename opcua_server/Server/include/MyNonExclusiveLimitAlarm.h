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
#ifndef _MY_NONEXCLUSIVELIMIT_ALARM_
#define _MY_NONEXCLUSIVELIMIT_ALARM_
#if TB5_ALARMS

#include <list>
#include "Util_Sync.h"
#include "LocalizedText.h"
#include "QualifiedName.h"
#include "ServerNonExclusiveLimitAlarm.h"
#include "ServerVariable.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

class MyNonExclusiveLimitAlarm: public NonExclusiveLimitAlarm
{
public:
	MyNonExclusiveLimitAlarm();
	virtual ~MyNonExclusiveLimitAlarm();

	virtual void onVariableChanged(Server::Variable* var, const IDataValue* value);
	virtual void onAcknowledge();
	virtual void onConfirm();

private:
};

typedef ObjectPointer<MyNonExclusiveLimitAlarm> MyNonExclusiveLimitAlarmPtr;
typedef ObjectPointer<const MyNonExclusiveLimitAlarm> MyNonExclusiveLimitAlarmConstPtr;

#endif
#endif
