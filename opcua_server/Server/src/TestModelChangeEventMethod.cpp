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
#include "TestModelChangeEventMethod.h"
#include "ServerAddressSpaceRoot.h"
#include "Argument.h"
#include "Statics.h"
#include "ServerMethodCallRequest.h"
#include "ApplicationModule.h"


TestModelChangeEventMethod::TestModelChangeEventMethod()
{


}


TestModelChangeEventMethod::~TestModelChangeEventMethod(void)
{
}

ObjectPointer<TestModelChangeEventMethod> TestModelChangeEventMethod::create( const NodeId& changeEventNodeId, Server::BaseNodePtr parent )
{
	ObjectPointer<TestModelChangeEventMethod> pModelChangeEventMethod = new TestModelChangeEventMethod();
	if(!pModelChangeEventMethod)
	{
		return NULL;
	}
	pModelChangeEventMethod->setNodeId(changeEventNodeId);
	pModelChangeEventMethod->setBrowseName(QualifiedName(_T("ModelChange"), 2));
	pModelChangeEventMethod->setDisplayName(LocalizedText(_T("ModelChange"), _T("en")));
	pModelChangeEventMethod->init(NULL);
	pModelChangeEventMethod->setDescription(LocalizedText(_T("Method to call ModelChangeEvent manually."), _T("en")));

	parent->addReference(Statics::ReferenceTypeId_HasComponent, pModelChangeEventMethod);

	pModelChangeEventMethod->setExecutable(true);
	Argument argument;
	argument.setDataType(Statics::DataTypeId_ModelChangeStructureDataType);
	argument.setValueRank(1);
	argument.setName(_T("Changes"));
	std::vector<Argument> inputArguments;
	inputArguments.push_back(argument);
	NodeId argNodeId(2, _T("ModelChangeInputArg"));
	pModelChangeEventMethod->setInputArguments(&argNodeId, inputArguments);
	// no output arguments

	return pModelChangeEventMethod;
}

EnumStatusCode TestModelChangeEventMethod::handleExecuteRequest( Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request )
{
	_UNUSED(transaction);
	EnumStatusCode result;
	std::vector<Value> inputValues = request->getInputArguments();
	std::vector<EnumStatusCode> inputArgumentResults;
	if(inputValues.size() == 1)
	{
		std::vector<ModelChangeStructureDataType> modelChangeArguments;
		if(StatusCode::isGood(inputValues[0].getModelChangeStructureDataTypeArray(modelChangeArguments)))
		{
			Server::ServerGeneralModelChangeEventPtr pModelChangeEvent = Server::ServerGeneralModelChangeEvent::create();

			inputArgumentResults.push_back(EnumStatusCode_Good);
			result = pModelChangeEvent->setChanges(modelChangeArguments);
			if(StatusCode::isGood(result))
			{
				NodeId id(EnumNumericNodeId_Server);
				pModelChangeEvent->report(&id);
			}
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}
	}
	else
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return result;
}

#endif
