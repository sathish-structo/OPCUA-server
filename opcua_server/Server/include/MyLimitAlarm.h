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
#ifndef _MY_LIMIT_ALARM_
#define _MY_LIMIT_ALARM_
#if TB5_ALARMS

#include <list>
#include "Util_Sync.h"
#include "LocalizedText.h"
#include "QualifiedName.h"
#include "ServerLimitAlarm.h"
#include "ServerVariable.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

class MyLimitAlarm: public LimitAlarm
{
public:
	MyLimitAlarm(VariablePtr isEnabled);
	~MyLimitAlarm();

	virtual EnumStatusCode init(const INodeId* typeId);
	virtual void onEnable();
	virtual void onDisable();

protected:
	VariablePtr m_isEnabled;
};

typedef ObjectPointer<MyLimitAlarm> MyLimitAlarmPtr;
typedef ObjectPointer<const MyLimitAlarm> MyLimitAlarmConstPtr;

#endif
#endif
