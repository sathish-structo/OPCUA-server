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
#ifndef _DYNAMICTESTVARIABLE_H_
#define _DYNAMICTESTVARIABLE_H_

#include "TestVariable.h"
#include "ServerDataTransaction.h"
#include "Util_TimerThread.h"

using namespace SoftingOPCToolbox5;

/**
 * @brief A test variable that has the ability to change its value. The set ov valid values must be specified in the consturctor,
 *        but the change must be triggered from the outside.
 * (Test purpose)
 */
class DynamicTestVariable
	: public TestVariableForAuditEvents
{
public:
	DynamicTestVariable(const std::vector<DataValue>& dataValues);
	DynamicTestVariable(void* otbHandle);
	virtual ~DynamicTestVariable();

	void changeValue(DateTime &now);

protected:
	std::vector<DataValue> m_dataValues;
	unsigned m_idx;
};

typedef ObjectPointer<DynamicTestVariable> DynamicTestVariablePtr;
typedef ObjectPointer<const DynamicTestVariable> DynamicTestVariableConstPtr;


class DynamicVariableChange: public Util_ScheduledAction
{
public:
	DynamicVariableChange(unsigned long timeout = 1000, bool isCyclic = true): Util_ScheduledAction(timeout, isCyclic)
	{
	}

	virtual ~DynamicVariableChange()
	{
	}

	void add(DynamicTestVariablePtr variable)
	{
		UTIL_SINGLE_LOCK(Util_Sync, arrSync, &m_mutex);
		m_dynamicVariables.push_back(variable);
	}

	void execute()
	{
		UTIL_SINGLE_LOCK(Util_Sync, arrSync, &m_mutex);
		DateTime now;
		now.utcNow();
		for(unsigned int i = 0; i < m_dynamicVariables.size(); i++)
		{
			m_dynamicVariables[i]->changeValue(now);
		}
	}

protected:
	Util_Sync m_mutex;
	std::vector<DynamicTestVariablePtr> m_dynamicVariables;
};

#endif
