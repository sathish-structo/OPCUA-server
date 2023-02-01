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
#ifndef _SIMVAR_METHOD_H_
#define _SIMVAR_METHOD_H_

#include "ServerMethod.h"
#include "ServerMethodCallRequest.h"
#include "SimVarManager.h"
#include "ToolkitTrace.h"
#include "ServerMethodCallTransaction.h"
#include "ServerSession.h"

/*! @brief A method to start input simulation for simulation variables. */
class StartSimulationSimVarsMethod
	: public Server::Method
{
	virtual EnumStatusCode handleExecuteRequest
		(
		Server::MethodCallTransaction* /* transaction */,
		Server::MethodCallRequest* request
		/*Server::BaseNode* node*/
		)
	{
		std::vector<Value> inputArgs = request->getInputArguments();
		vector<EnumStatusCode> inputStatusCodes;
		EnumStatusCode result = EnumStatusCode_Good;
		std::vector<Value> outputArgs;
		Value outputValue;

		bool argsCorrect = true;
		InputSimulationRecord* pRec = new InputSimulationRecord();

		// Validate the input params
		argsCorrect = (inputArgs.size() == 7);
		for (size_t i=0; i < inputArgs.size(); i++)
		{
			EnumStatusCode argStatus = EnumStatusCode_Good;
			switch (i)
			{
			case 0:
				if (StatusCode::isFAILED(inputArgs[i].getUInt32(&pRec->simVarSetId)))
				{
					argStatus = EnumStatusCode_BadInvalidArgument;
					argsCorrect = false;
				}
				break;
			case 1:
				if (StatusCode::isFAILED(inputArgs[i].getUInt32(&pRec->changeInterval)))
				{
					argStatus = EnumStatusCode_BadInvalidArgument;
					argsCorrect = false;
				}
				break;
			case 2:
				if (StatusCode::isFAILED(inputArgs[i].getUInt32(&pRec->repeatCount)))
				{
					argStatus = EnumStatusCode_BadInvalidArgument;
					argsCorrect = false;
				}
				break;
			case 3:
				if (StatusCode::isFAILED(inputArgs[i].getDouble(&pRec->increment)))
				{
					argStatus = EnumStatusCode_BadInvalidArgument;
					argsCorrect = false;
				}
				break;
			case 4:
				if (StatusCode::isFAILED(inputArgs[i].getUInt32(&pRec->changeCount)))
				{
					argStatus = EnumStatusCode_BadInvalidArgument;
					argsCorrect = false;
				}
				break;
			case 5:
				if (StatusCode::isFAILED(inputArgs[i].getUInt32(&pRec->bulkHandling)))
				{
					argStatus = EnumStatusCode_BadInvalidArgument;
					argsCorrect = false;
				}
				break;
			case 6:
				if (StatusCode::isFAILED(inputArgs[i].getUInt32(&pRec->bulkSize)))
				{
					argStatus = EnumStatusCode_BadInvalidArgument;
					argsCorrect = false;
				}
				break;
			}
			inputStatusCodes.push_back(argStatus);
		}

		if(argsCorrect)
		{
			// If all arguments were read successfull call the creation of the variables method
			SimVarManager& simVarManager = SimVarManager::getInstance();
			result = simVarManager.startSimulation(pRec);
		}
		else
		{
			result = EnumStatusCode_BadInvalidArgument;
		}

		delete pRec;
		request->setStatusCode(result);
		request->setInputArgumentResults(inputStatusCodes);
		request->setOutputArguments(outputArgs);
		request->complete();

		return EnumStatusCode_Good;
	}
};


/*! @brief A method to start input simulation for simulation variables. */
class ResetSimulationsMethod
	: public Server::Method
{
	virtual EnumStatusCode handleExecuteRequest
		(
		Server::MethodCallTransaction* /* transaction */,
		Server::MethodCallRequest* request
		)
	{
		request->setStatusCode(SimVarManager::getInstance().stopAllSimulations());
		request->complete();

		return EnumStatusCode_Good;
	}
};


/*! @brief a method to stop the input simulation for a set of simulation variables.*/
class StopSimVarsMethod
	: public Server::Method
{
	virtual EnumStatusCode handleExecuteRequest
		(
		Server::MethodCallTransaction*	/* transaction */,
		Server::MethodCallRequest* request
		)
	{
		std::vector<Value> inputArgs = request->getInputArguments();
		vector<EnumStatusCode> inputStatusCodes;
		EnumStatusCode result;
		std::vector<Value> outputArgs;
		Value outputValue;

		bool argsRead = false;
		// The ID associated with the current set of sim vars created.
		OTUInt32 simVarSetID = 0;

		// Validate the input params
		if(inputArgs.size() == 1)
		{
			// First argument should be the nodeId array
			Value arg0 = inputArgs[0];

			if(arg0.getUInt32(&simVarSetID) == EnumStatusCode_Good)
			{
				inputStatusCodes.push_back(EnumStatusCode_Good);
				argsRead = true;
			}
			else
			{
				inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
			}
		}

		if(argsRead)
		{
			// If all arguments were read successfull call the creation of the variables method
			SimVarManager& simVarManager = SimVarManager::getInstance();
			result = simVarManager.stopSimulation(simVarSetID);
		}
		else
		{
			result = EnumStatusCode_BadInvalidArgument;
		}

		request->setStatusCode(result);
		request->setInputArgumentResults(inputStatusCodes);
		request->setOutputArguments(outputArgs);
		request->complete();

		return EnumStatusCode_Good;
	}
};

/*! @brief a method to stop the input simulation for a set of simulation variables.*/
class DeleteVarsMethod
	: public Server::Method
{
	virtual EnumStatusCode handleExecuteRequest
		(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request
		)
	{
		_UNUSED(transaction);
		std::vector<Value> inputArgs = request->getInputArguments();
		vector<EnumStatusCode> inputStatusCodes;
		EnumStatusCode result;
		std::vector<Value> outputArgs;
		Value outputValue;

		bool argsRead = false;
		// The ID associated with the current set of sim vars created.
		OTUInt32 simVarSetID = 0;

		// Validate the input params
		if(inputArgs.size() == 1)
		{
			// First argument should be the nodeId array
			Value arg0 = inputArgs[0];

			if(arg0.getUInt32(&simVarSetID) == EnumStatusCode_Good)
			{
				inputStatusCodes.push_back(EnumStatusCode_Good);
				argsRead = true;
			}
			else
			{
				inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
			}
		}

		if(argsRead)
		{
			// If all arguments were read successfull call the creation of the variables method
			SimVarManager& simVarManager = SimVarManager::getInstance();
			simVarManager.stopSimulation(simVarSetID);
			result = simVarManager.deleteVariables(simVarSetID);
		}
		else
		{
			result = EnumStatusCode_BadInvalidArgument;
		}

		request->setStatusCode(result);
		request->setInputArgumentResults(inputStatusCodes);
		request->setOutputArguments(outputArgs);
		request->complete();

		return EnumStatusCode_Good;
	}
};

/*! A method just for debug, can be deleted after tests.
*   Can be called asynch, from SubscriptionUtils::dump...
*/
class DumpSimVars
	: public Server::Method
{
	virtual EnumStatusCode handleExecuteRequest
		(
		Server::MethodCallTransaction* /* transaction */,
		Server::MethodCallRequest* request
		)
	{
		vector<EnumStatusCode> inputStatusCodes;
		std::vector<Value> outputArgs;

		SimVarManager& simVarManager = SimVarManager::getInstance();
		simVarManager.dump();

		request->setInputArgumentResults(inputStatusCodes);
		request->setOutputArguments(outputArgs);
		request->complete();

		return EnumStatusCode_Good;
	}
};

class CreateSimVarsByRangeMethod
	: public Server::Method
{
	virtual EnumStatusCode handleExecuteRequest
		(
		Server::MethodCallTransaction* transaction,
		Server::MethodCallRequest* request
		)
	{
		_UNUSED(transaction);
		CreateSimVarWorkItem* workItem = new CreateSimVarWorkItem(request);
		if(!getApplicationModule()->addWorkItem(workItem))
		{
			workItem->execute();
			delete workItem;
		}
		return EnumStatusCode_Good;
	}
};


#endif // _TESTMETHOD_H_
