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
#include "ChangeTwoStateItemSemanticsMethod.h"
#include "LocalizedText.h"
#include "Argument.h"
#include "ServerTwoStateItem.h"
#include "ApplicationModule.h"
#include "ServerMethodCallRequest.h"
#include "Constants.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

ChangeTwoStateItemSemanticsMethod::ChangeTwoStateItemSemanticsMethod(ObjectPtr parent)
{
	setNodeId(NodeId(getApplicationModule()->getNameSpaceIndex(), _T("ChangeTwoStateItemSemanticsMethod")));
	setDisplayName(LocalizedText(_T("Change TwoStateItem Semantics"), _T("en")));
	setBrowseName(QualifiedName(_T("ChangeTwoStateItemSemantics"), getApplicationModule()->getNameSpaceIndex()));
	setDescription(LocalizedText(_T("This method enables clients to change the semantics (i.e. texts for 'true' and 'false' values) of TwoStateItems and toggles their current state."), _T("en")));
	setExecutable(true);

	// Define method arguments
	vector<Argument> inputArguments;
	{
		Argument argument;
		argument.setDataType(NodeId(0, EnumDataTypeId_NodeId));
		argument.setName(_T("TwoStateItemNodeIds"));
		argument.setDescription(LocalizedText(_T("The NodeIds of TwoStateItem variables whose semantic is to be changed"), _T("en")));
		argument.setValueRank(1);
		inputArguments.push_back(argument);
	}
	{
		Argument argument;
		argument.setDataType(NodeId(0, EnumDataTypeId_LocalizedText));
		argument.setName(_T("TrueText"));
		argument.setDescription(LocalizedText(_T("The text that is to be set as 'true' value"), _T("en")));
		argument.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(argument);
	}
	{
		Argument argument;
		argument.setDataType(NodeId(0, EnumDataTypeId_LocalizedText));
		argument.setName(_T("FalseText"));
		argument.setDescription(LocalizedText(_T("The text that is to be set as 'false' value"), _T("en")));
		argument.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(argument);
	}
	{
		Argument argument;
		argument.setDataType(NodeId(0, EnumDataTypeId_Boolean));
		argument.setName(_T("SummarizeChanges"));
		argument.setDescription(LocalizedText(_T("If true, changes to the TwoStateItems are summarized and reported as single SemanticChange event"), _T("en")));
		argument.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(argument);
	}

	Value inputArgumentsValue;
	inputArgumentsValue.setArgumentArray(inputArguments);

	DataValue inputArgumentsDataValue;
	inputArgumentsDataValue.setValue(inputArgumentsValue);

	VariablePtr inputArgumentsVar = Variable::create();
	AddressSpaceRoot::instance()->createNodeId(inputArgumentsVar, EnumIdentifierType_Guid, getApplicationModule()->getNameSpaceIndex());
	inputArgumentsVar->setDataValue(inputArgumentsDataValue);
	inputArgumentsVar->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
	inputArgumentsVar->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
	inputArgumentsVar->setVariableType(Statics::VariableTypeId_PropertyType);
	inputArgumentsVar->setAccessLevel(EnumAccessLevel_CurrentRead);
	addReference(Statics::ReferenceTypeId_HasProperty, inputArgumentsVar);

	// Add the nodes to the address space
	insertTree();
	parent->addReference(Statics::ReferenceTypeId_HasComponent, this);
}

ChangeTwoStateItemSemanticsMethod::~ChangeTwoStateItemSemanticsMethod()
{

}

EnumStatusCode ChangeTwoStateItemSemanticsMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	EnumStatusCode requestStatus = EnumStatusCode_Good;
	_UNUSED(transaction);

	// Extract provided arguments
	std::vector<NodeId> twoStateItemIds;
	LocalizedText trueText, falseText;
	bool summarizeChanges = false;
	requestStatus = extractArguments(request, twoStateItemIds, trueText, falseText, summarizeChanges);

	// Fulfill request
	if(StatusCode::isSUCCEEDED(requestStatus))
	{
#if TB5_ALARMS
		if(summarizeChanges)
		{
			AddressSpaceRoot::instance()->beginSemanticsChange();
		}
#endif
		for(size_t i = 0; i < twoStateItemIds.size() && StatusCode::isSUCCEEDED(requestStatus); i++)
		{
			BaseNodePtr twoStateItemBase = AddressSpaceRoot::instance()->getNode(&twoStateItemIds[i]);
			if(twoStateItemBase.isNotNull() && twoStateItemBase->isInstanceOfUaType(Statics::VariableTypeId_TwoStateDiscreteType, true))
			{
				BaseNode* pBase = twoStateItemBase;
				TwoStateItemPtr twoStateItem = static_cast<TwoStateItem*>(pBase);

				// modify semantics
				twoStateItem->setTrueText(trueText);
				twoStateItem->setFalseText(falseText);

				// re-set the current item value to notify the semantics change by status code flags
				DataValue currentValue = twoStateItem->getDataValue();
				Value toggledState = !currentValue.getValue()->getBoolean();
				currentValue.setValue(toggledState);

				twoStateItem->setDataValueWithChangedSemantics(currentValue);
			}
			else
			{
				_tprintf(_T("Error while processing ChangeTwoStateSemantics method: Node with NodeId %s is not of type TwoStateDiscrete"),
					twoStateItemIds[i].toString().c_str());
				requestStatus = EnumStatusCode_BadInvalidArgument;
			}
		}

#if TB5_ALARMS
		if(summarizeChanges)
		{
			LocalizedText semanticChangeEventMessage(_T("Semantic change detected"), _T("en"));
			AddressSpaceRoot::instance()->endSemanticsChange(&semanticChangeEventMessage, 100);
		}
#endif
	}

	request->setStatusCode(requestStatus);
	request->complete();
	return EnumStatusCode_Good;
}

EnumStatusCode ChangeTwoStateItemSemanticsMethod::extractArguments(Server::MethodCallRequest* request, std::vector<NodeId>& twoStateItemIds, LocalizedText& trueText, LocalizedText& falseText, bool& summarizeChanges)
{
	EnumStatusCode requestStatus = EnumStatusCode_Good;

	std::vector<Value> inputArguments = request->getInputArguments();
	if(inputArguments.size() == 4)
	{
		requestStatus = inputArguments[0].getNodeIdArray(twoStateItemIds);
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			requestStatus = inputArguments[1].getLocalizedText(trueText);
		}
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			requestStatus = inputArguments[2].getLocalizedText(falseText);
		}
		if(StatusCode::isSUCCEEDED(requestStatus))
		{
			if(inputArguments[3].getDataType() != EnumDataTypeId_Boolean)
			{
				requestStatus = EnumStatusCode_BadInvalidArgument;
			}
			else
			{
				summarizeChanges = inputArguments[3].getBoolean();
			}
		}
	}
	else
	{
		requestStatus = EnumStatusCode_BadInvalidArgument;
	}

	return requestStatus;
}
