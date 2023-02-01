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
#if TB5_ALARMS
#include "MeasureConditionBranchPerformanceMethod.h"

#include "ApplicationModule.h"
#include "ServerMethodCallTransaction.h"
#include "Util_Timer.h"
#include "Argument.h"
#include "Constants.h"
#include "ServerMethodCallRequest.h"

using namespace SoftingOPCToolbox5::Server;

class MeasureConditionBranchPerformanceThread : public Util_Thread
{
public:
	MethodCallRequestPtr request;
	ConditionPtr condition;
	unsigned int branchesToCreate;

	virtual long run()
	{
		EnumStatusCode result = EnumStatusCode_Good;

		//
		// Measure time to create branches
		//

		Util_Timer timer;
		float timePerBranch = 0;
		timer.start();

		for(unsigned int i = 0; i < branchesToCreate; i++)
		{
			ConditionPtr branch = Condition::create();

			AddressSpaceRoot::instance()->lock();
			result = condition->createBranch(branch);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Create branch returned bad status code: %s\n"), getEnumStatusCodeString(result));
			}
			AddressSpaceRoot::instance()->unlock();

			NodeId branchId;
			result = condition->addBranch(branch, branchId);
			if(StatusCode::isBad(result))
			{
				_tprintf(_T("Add branch returned bad status code: %s\n"), getEnumStatusCodeString(result));
			}
			if(timer.elapsed() > 600000)
			{
				_tprintf(_T("creating %u branches took more than 10 minutes, aborting now!\n"), branchesToCreate);
				branchesToCreate = i + 1;
				break;
			}
		}

		timer.stop();
		long totalTime = timer.elapsed();
		timePerBranch = (float)totalTime / branchesToCreate;

		_tprintf(_T("Average time to create a condition branch: %.2f ms\nTotal time to create %u branches: %ld ms\n"),
			timePerBranch, branchesToCreate, totalTime);

		//
		// Measure time to delete branches
		//

		timer.start();

		std::vector<NodeId> branchIds;
		condition->getBranches(branchIds);
		for(std::vector<NodeId>::iterator it = branchIds.begin();
			it != branchIds.end(); it++)
		{
			condition->deleteBranch(&(*it));
		}

		timer.stop();
		totalTime = timer.elapsed();
		timePerBranch = (float)totalTime / branchesToCreate;

		_tprintf(_T("Average time to delete a condition branch: %.2f ms\nTotal time to delete %u branches: %ld ms\n\n"),
			timePerBranch, branchesToCreate, totalTime);

		request->setStatusCode(EnumStatusCode_Good);
		request->complete();

		request.reset();
		condition.reset();

		return 0;
	}
};

class MeasureConditionBranchPerformanceMethodPrivate
: public MeasureConditionBranchPerformanceThread
{
public:
	ConditionPtr d_condition;
	bool isJoinable;
};

MeasureConditionBranchPerformanceMethod::MeasureConditionBranchPerformanceMethod( ObjectPtr parent, ConditionPtr condition )
: d(new MeasureConditionBranchPerformanceMethodPrivate())
{
	d->d_condition = condition;
	d->isJoinable  = false;

	// add this node to the address space....
	tstring parentNodeId = parent->getNodeId()->getIdentifier().getString();
	tstring methodNodeId = parentNodeId + _T(".MeasureConditionBranchPerformance");
	unsigned short parentNamespaceIndex = parent->getNodeId()->getNamespaceIndex();
	setNodeId(NodeId(parentNamespaceIndex, methodNodeId));
	setDisplayName(LocalizedText(_T("Measure ConditionBranch performance"), _T("en")));
	setBrowseName(QualifiedName(_T("MeasureConditionBranchPerformance"), getApplicationModule()->getNameSpaceIndex()));
	insert();
	setExecutable(true);
	parent->addReference(Statics::ReferenceTypeId_HasComponent, this);

	// add the input argument
	vector<Argument> inputArguments;

	Argument param;
	param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
	param.setName(_T("BranchesToCreate"));
	param.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(param);

	Value containedValue;
	containedValue.setArgumentArray(inputArguments);
	DataValue value;
	value.setValue(containedValue);

	tstring inputArgumentsNodeId = methodNodeId + _T(".InputArguments");
	VariablePtr inputArgumentsVar = Variable::create();
	inputArgumentsVar->setDataValue(value);
	inputArgumentsVar->setNodeId(NodeId(parentNamespaceIndex, inputArgumentsNodeId));
	inputArgumentsVar->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
	inputArgumentsVar->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
	inputArgumentsVar->setVariableType(Statics::VariableTypeId_PropertyType);
	inputArgumentsVar->insert();
	inputArgumentsVar->setAccessLevel(EnumAccessLevel_CurrentRead);
	addReference(Statics::ReferenceTypeId_HasProperty, inputArgumentsVar);
}

MeasureConditionBranchPerformanceMethod::~MeasureConditionBranchPerformanceMethod()
{
	delete d;
}

void MeasureConditionBranchPerformanceMethod::cleanup()
{
	if (d->isJoinable)
	{
		d->wait(INFINITE);
		d->isJoinable = false;
	}
}

EnumStatusCode MeasureConditionBranchPerformanceMethod::handleExecuteRequest( Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request )
{
	_UNUSED(transaction);
	EnumStatusCode result = EnumStatusCode_Good;

	// Give the running thread some time to finish its work.
	// If we don't wait here, a client using this method for stress testing
	// may call the method too fast in a row and may run into BadInvalidState
	// errors.
	if (d->isJoinable)
	{
		if (d->isRunning())
		{
			request->setStatusCode(EnumStatusCode_BadInvalidState);
			request->complete();
			return result;
		}
		d->wait(INFINITE);
		d->isJoinable = false;
	}

	std::vector<Value> inputArguments = request->getInputArguments();
	if(inputArguments.size() == 1)
	{
		d->branchesToCreate = inputArguments[0].getUInt32();
		d->request = request;
		d->condition = d->d_condition;
		d->isJoinable = d->start();
	}
	else
	{
		request->setStatusCode(EnumStatusCode_BadArgumentsMissing);
		request->complete();
	}

	return result;
}

#endif
