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
#include "MyNonExclusiveLimitAlarm.h"

#include "Application.h"
#include "Creator.h"
#include "ServerExclusiveLimitStateMachine.h"

#include "ServerTwoStateVariable.h"
#include "ServerAddressSpaceRoot.h"
#include "ServerState.h"
#include "ServerShelvedStateMachine.h"
#include "ServerFiniteStateVariable.h"
#include "Constants.h"

MyNonExclusiveLimitAlarm::MyNonExclusiveLimitAlarm()
{
}

MyNonExclusiveLimitAlarm::~MyNonExclusiveLimitAlarm()
{
}

void MyNonExclusiveLimitAlarm::onVariableChanged(Server::Variable* var, const IDataValue* value)
{
	// parameter check
	if (NULL == value)
		return;

	// Set the quality of the alarm to the quality of the input value
	setQuality(value->getStatusCode());

	// check mandatory components
	ObjectPointer<Server::TwoStateVariable> activeState = getActiveStateVar();
	if (activeState.isNull())
		return;

	// remember the old active state value to decide whether a branch has to be created
	bool oldActiveState = activeState->getId();

	// check if any of the limit values are violated
	double newValue = 0.0;
	if ((value->getValue()->canCastTo(EnumDataTypeId_Double) == EnumValueCastType_Identity) ||
		(value->getValue()->canCastTo(EnumDataTypeId_Double) == EnumValueCastType_Implicit))
	{
		Value tmpValue;
		if (StatusCode::isFAILED(value->getValue()->castTo(EnumDataTypeId_Double, tmpValue)))
		{
			// can not get value
			return;
		}
		if (StatusCode::isFAILED(tmpValue.getDouble(&newValue)))
		{
			// can not get value
			return;
		}
	}


	const unsigned int statesCount = 4;
	bool reportAlarm = false;

	// declare an array of boolean states
	bool oldState[statesCount];
	bool limitExceeded[statesCount];

	// initialize the arrays
	memset(oldState, false, statesCount*sizeof(bool));
	memset(limitExceeded, false, statesCount*sizeof(bool));

	// get optional components
	ObjectPointer<Server::TwoStateVariable> lowLowState   = getLowLowStateVar();
	ObjectPointer<Server::TwoStateVariable> lowState      = getLowStateVar();
	ObjectPointer<Server::TwoStateVariable> highState     = getHighStateVar();
	ObjectPointer<Server::TwoStateVariable> highHighState = getHighHighStateVar();

	double lowLowLimit = 0.0;
	double lowLimit = 0.0;
	double highLimit = 0.0;
	double highHighLimit = 0.0;

	// check Low limit
	if(lowState.isNotNull() && StatusCode::isGood(getLowLimit(lowLimit)))
	{
		oldState[0] = lowState->getId();
		limitExceeded[0] = newValue <= lowLimit;
		if (oldState[0] != limitExceeded[0])
		{
			reportAlarm = true;
		}
	}

	// check High limit
	if(highState.isNotNull() && StatusCode::isGood(getHighLimit(highLimit)))
	{
		oldState[1] = highState->getId();
		limitExceeded[1] = newValue >= highLimit;
		if (oldState[1] != limitExceeded[1])
		{
			reportAlarm = true;
		}
	}

	// check LowLow limit
	if(lowLowState.isNotNull() && StatusCode::isGood(getLowLowLimit(lowLowLimit)))
	{
		oldState[2] = lowLowState->getId();
		limitExceeded[2] = newValue <= lowLowLimit;
		if (oldState[2] != limitExceeded[2])
		{
			reportAlarm = true;
		}
	}

	// check HighHigh limit
	if(highHighState.isNotNull() && StatusCode::isGood(getHighHighLimit(highHighLimit)))
	{
		oldState[3] = highHighState->getId();
		limitExceeded[3] = newValue >= highHighLimit;
		if (oldState[3] != limitExceeded[3])
		{
			reportAlarm = true;
		}
	}

	// if we have a change in any of the states
	if (reportAlarm)
	{
		// determine if the alarm is active or not
		bool dummyArray[statesCount];
		memset(dummyArray, false, statesCount*sizeof(bool));
		bool newActiveState = (memcmp(limitExceeded, dummyArray, statesCount*sizeof(bool)) != 0);

		bool acknowledged = getAckedStateVar()->getId();
		bool confirmed = getConfirmedStateVar()->getId();

		if(oldActiveState && !newActiveState && !(acknowledged && confirmed))
		{
			// transition from active to inactive and condition has not been acknowledged
			// and confirmed before: create a branch of the old active state

			// in this example, only one branch for every limit state set shall exist
			// (e.g. only one branch for a condition with active "High" and "HighHigh" state)

			EnumStatusCode result = EnumStatusCode_Bad;
			std::vector<NodeId> branchList;
			unsigned int ui;
			NodeId branchToDelete;

			// get a list of already existing branches
			if(StatusCode::isSUCCEEDED(result = getBranches(branchList)))
			{
				for (ui=0; ui < branchList.size(); ui++)
				{
					NodeId branchId = branchList[ui];

					ConditionPtr pBranch;
					if(StatusCode::isSUCCEEDED(getBranch(&branchId, pBranch)))
					{
						MyNonExclusiveLimitAlarmPtr pMyBranch(static_pointer_cast<MyNonExclusiveLimitAlarm>(pBranch));

						ObjectPointer<Server::TwoStateVariable> branchLowLowState   = pMyBranch->getLowLowStateVar();
						ObjectPointer<Server::TwoStateVariable> branchLowState      = pMyBranch->getLowStateVar();
						ObjectPointer<Server::TwoStateVariable> branchHighState     = pMyBranch->getHighStateVar();
						ObjectPointer<Server::TwoStateVariable> branchHighHighState = pMyBranch->getHighHighStateVar();

						bool sameLowState = branchLowState.isNotNull() && branchLowState->getId() == oldState[0];
						bool sameHighState = branchHighState.isNotNull() && branchHighState->getId() == oldState[1];
						bool sameLowLowState = branchLowLowState.isNotNull() && branchLowLowState->getId() == oldState[2];
						bool sameHighHighState = branchHighHighState.isNotNull() && branchHighHighState->getId() == oldState[3];

						if ( sameLowState && sameHighState && sameLowLowState && sameHighHighState)
						{
							// there is already a branch with the same limit state set
							branchToDelete = branchId;
							break;
						}
					}
				}

				// remove the old branch
				if(!branchToDelete.isNull())
				{
					result = deleteBranch(&branchToDelete);
				}
			}

			// now create a new branch of the current state
			// the necessary information is copied from the condition instance
			ObjectPointer<Condition> newBranch = new MyNonExclusiveLimitAlarm();
			if(StatusCode::isSUCCEEDED(result = createBranch(newBranch)))
			{
				// add branch to branch list
				NodeId branchId;
				result = addBranch(newBranch, branchId);
			}
		}

		// set the new limit states
		lowState->setCurrentState(limitExceeded[0]);
		highState->setCurrentState(limitExceeded[1]);
		lowLowState->setCurrentState(limitExceeded[2]);
		highHighState->setCurrentState(limitExceeded[3]);

		// set the current active state
		activeState->setCurrentState(newActiveState);

		// set the InputNode to the variables' NodeId
		setInputNode(var->getNodeId());
		setTime(DateTime::getUtcNow());
		setReceiveTime(DateTime::getUtcNow());
		// set local time to MESZ (GMT+1 +1DST)
		TimeZone localTime(120, true);
		setLocalTime(localTime);

		if (newActiveState)
		{
			setMessage(LocalizedText(_T("Limit exceeded"), _T("")));
			onActivate();
		}
		else
		{
			setMessage(LocalizedText(BrowseName_NormalState, _T("")));
			onDeactivate();
		}
	}
}

void MyNonExclusiveLimitAlarm::onAcknowledge()
{
	// Acknowledge the alarm and report an event for the condition
	NonExclusiveLimitAlarm::onAcknowledge();
}

void MyNonExclusiveLimitAlarm::onConfirm()
{
	// confirm the alarm and report an event
	NonExclusiveLimitAlarm::onConfirm();

	// delete the branch and report another event with retain set to false
	NodeId branchId = getBranchId();
	if (!branchId.isNull())
	{
		ObjectPointer<Server::Condition> conditionTrunk;
		if(StatusCode::isSUCCEEDED(getTrunk(conditionTrunk)) && conditionTrunk.isNotNull())
		{
			conditionTrunk->deleteBranch(&branchId);
		}
	}
}
#endif
