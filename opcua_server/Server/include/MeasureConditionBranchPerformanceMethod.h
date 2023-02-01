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
#ifndef _MEASURE_CONDITION_BRANCH_PERFORMANCE_METHOD_H_
#define _MEASURE_CONDITION_BRANCH_PERFORMANCE_METHOD_H_
#if TB5_ALARMS

#include "ServerMethod.h"
#include "ServerCondition.h"
#include "Util_Thread.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

class MeasureConditionBranchPerformanceMethodPrivate;

/**
* @brief Tests the performance of creation and deletion of condition branches
*/
class MeasureConditionBranchPerformanceMethod : public Server::Method
{
public:
	/**
	 * Constructs a Method node which allows testing the performance
	 * of the creation and deletion of condition branches and
	 * inserts the node below the @p parentNode
	 *
	 * @param parent the parent node
	 * @param condition the condition to use for testing
	 */
	MeasureConditionBranchPerformanceMethod(ObjectPtr parent, ConditionPtr condition);
	virtual ~MeasureConditionBranchPerformanceMethod();

	virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);

	void cleanup();

private:
	MeasureConditionBranchPerformanceMethodPrivate* d;
};

typedef ObjectPointer<MeasureConditionBranchPerformanceMethod> MeasureConditionBranchPerformanceMethodPtr;
typedef ObjectPointer<const MeasureConditionBranchPerformanceMethod> MeasureConditionBranchPerformanceMethodConstPtr;

#endif
#endif //_ADDRESSSPACETEST_H_
