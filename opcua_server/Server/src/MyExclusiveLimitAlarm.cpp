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
#include "MyExclusiveLimitAlarm.h"

#include "Application.h"
#include "Creator.h"
#include "ServerExclusiveLimitStateMachine.h"

#include "ServerTwoStateVariable.h"
#include "ServerAddressSpaceRoot.h"
#include "ServerState.h"
#include "ServerShelvedStateMachine.h"
#include "ServerFiniteStateVariable.h"
#include "Constants.h"

MyExclusiveLimitAlarm::MyExclusiveLimitAlarm()
{
}

MyExclusiveLimitAlarm::~MyExclusiveLimitAlarm()
{
}

void MyExclusiveLimitAlarm::onVariableChanged(Server::Variable* var, const IDataValue* value)
{
	tstring message;
	// parameter check
	if (NULL == value)
		return;

	// Set the quality of the alarm to the quality of the input value
	setQuality(value->getStatusCode());

	// check mandatory components
	ObjectPointer<Server::ExclusiveLimitStateMachine> limitState = getLimitState();
	ObjectPointer<Server::TwoStateVariable> activeState = getActiveStateVar();
	if (limitState.isNull() || activeState.isNull())
		return;

	// get the new value in a local double variable
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


	// examine actual status and state node
	bool oldActiveState = activeState->getId();
	NodeId oldStateNode;
	ObjectPointer<Server::FiniteStateVariable> currStateVar = limitState->getCurrentStateVar();
	if (currStateVar.isNotNull())
	{
		oldStateNode = currStateVar->getId();
	}

	// calculate the new state
	bool newActiveState = false;
	EnumStatusCode result = EnumStatusCode_Good;
	ObjectPointer<Server::State> newState;

	// get optional components
	ObjectPointer<Server::State> lowLow   = limitState->getLowLow();
	ObjectPointer<Server::State> low      = limitState->getLow();
	ObjectPointer<Server::State> high     = limitState->getHigh();
	ObjectPointer<Server::State> highHigh = limitState->getHighHigh();

	double lowLowLimit = 0.0;
	double lowLimit = 0.0;
	double highLimit = 0.0;
	double highHighLimit = 0.0;

	// check if any of the limit values are violated
	if(lowLow.isNotNull() &&
		StatusCode::isGood(getLowLowLimit(lowLowLimit)) &&
		newValue <= lowLowLimit)
	{
		// we have a LowLow breach
		newState = lowLow;
		newActiveState = true;
		message = BrowseName_LowLow;
	}
	else if(low.isNotNull() &&
		StatusCode::isGood(getLowLimit(lowLimit)) &&
		newValue <= lowLimit)
	{
		// we have a Low breach
		newState = low;
		newActiveState = true;
		message = BrowseName_Low;
	}
	else if(highHigh.isNotNull() &&
		StatusCode::isGood(getHighHighLimit(highHighLimit)) &&
		newValue >= highHighLimit)
	{
		// we have a HighHigh breach
		newState = highHigh;
		newActiveState = true;
		message = BrowseName_HighHigh;
	}
	else if(high.isNotNull() &&
		StatusCode::isGood(getHighLimit(highLimit)) &&
		newValue >= highLimit)
	{
		// we have a High breach
		newState = high;
		newActiveState = true;
		message = BrowseName_High;
	}
	else
	{
		// the new value is in the normal limits
		newState = NULL;
		message = BrowseName_NormalState;
	}

	// get node ID of new state
	NodeId newStateNode;
	if (newState.isNotNull())
	{
		newStateNode = newState->getNodeId();
	}
	// if we switch state, report the alarm
	if (oldStateNode.compare(newStateNode) != 0)
	{
		bool acknowledged = getAckedStateVar()->getId();
		bool confirmed = getConfirmedStateVar()->getId();

		if (oldActiveState && !newActiveState && !(acknowledged && confirmed))
		{
			// transition from active to inactive and condition has not been acknowledged
			// and confirmed before:
			// create a branch of the old active state

			// in this example only one branch shall exist - so search the old branch
			// and remove it

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
						MyExclusiveLimitAlarmPtr pMyBranch(static_pointer_cast<MyExclusiveLimitAlarm>(pBranch));

						bool bBranchActive = pMyBranch->getActiveStateVar()->getId();
						if (bBranchActive)
						{
							// there is already a branch which represents an active state
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
			ObjectPointer<Condition> newBranch = new MyExclusiveLimitAlarm();
			if(StatusCode::isSUCCEEDED(result = createBranch(newBranch)))
			{
				// add branch to branch list
				NodeId branchId;
				result = addBranch(newBranch, branchId);
			}
		}

		// now change the state
		result = limitState->changeState(newState);

		// set the current active state
		activeState->setCurrentState(newActiveState);

		// set the InputNode to the variables' NodeId
		setInputNode(var->getNodeId());
		setTime(DateTime::getUtcNow());
		setReceiveTime(DateTime::getUtcNow());
		// set local time to MESZ (GMT+1 +1DST)
		TimeZone localTime(120, true);
		setLocalTime(localTime);
		setMessage(LocalizedText(message, _T("")));

		// ..  and activate / deactivate the condition (with sending notifications)
		if (newActiveState)
		{
			onActivate();
		}
		else
		{
			onDeactivate();
		}
	}
}

void MyExclusiveLimitAlarm::onAcknowledge()
{
	// Acknowledge the alarm and report an event for the condition
	ExclusiveLimitAlarm::onAcknowledge();
}

void MyExclusiveLimitAlarm::onConfirm()
{
	// confirm the alarm and report an event
	ExclusiveLimitAlarm::onConfirm();

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
