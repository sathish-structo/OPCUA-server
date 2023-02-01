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
#include "TestSemanticChangeEventMethod.h"
#include "ServerAddressSpaceRoot.h"
#include "Argument.h"
#include "Statics.h"
#include "ServerMethodCallRequest.h"
#include "ApplicationModule.h"


TestSemanticChangeEventMethod::TestSemanticChangeEventMethod(void)
{
}


TestSemanticChangeEventMethod::~TestSemanticChangeEventMethod(void)
{
}

ObjectPointer<TestSemanticChangeEventMethod> TestSemanticChangeEventMethod::create( const NodeId& changeEventNodeId, Server::BaseNodePtr parent )
{
	ObjectPointer<TestSemanticChangeEventMethod> pSemanticChangeEventMethod = new TestSemanticChangeEventMethod();
	if(!pSemanticChangeEventMethod)
	{
		return NULL;
	}
	pSemanticChangeEventMethod->setNodeId(changeEventNodeId);
	pSemanticChangeEventMethod->setBrowseName(QualifiedName(_T("SemanticChange"), 2));
	pSemanticChangeEventMethod->setDisplayName(LocalizedText(_T("SemanticChange"), _T("en")));
	pSemanticChangeEventMethod->init(NULL);
	pSemanticChangeEventMethod->setDescription(LocalizedText(_T("Method to call SemanticChangeEvent manually."), _T("en")));

	parent->addReference(Statics::ReferenceTypeId_HasComponent, pSemanticChangeEventMethod);

	pSemanticChangeEventMethod->setExecutable(true);
	// no specific user access

	Argument argument;
	argument.setDataType(Statics::DataTypeId_SemanticChangeStructureDataType);
	argument.setValueRank(1);
	argument.setName(_T("Changes"));
	std::vector<Argument> inputArguments;
	inputArguments.push_back(argument);
	NodeId argNodeId(2, _T("SemanticChangeInputArg"));
	pSemanticChangeEventMethod->setInputArguments(&argNodeId, inputArguments);
	// no output arguments

	return pSemanticChangeEventMethod;
}

EnumStatusCode TestSemanticChangeEventMethod::handleExecuteRequest( Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request )
{
	_UNUSED(transaction);
	EnumStatusCode result;
	std::vector<Value> inputValues = request->getInputArguments();
	std::vector<EnumStatusCode> inputArgumentResults;

	if(inputValues.size() == 1)
	{
		std::vector<SemanticChangeStructureDataType> semanticChangeArguments;
		result = inputValues[0].getSemanticChangeStructureDataTypeArray(semanticChangeArguments);

		if(StatusCode::isGood(result))
		{
			Server::ServerSemanticChangeEventPtr pSemanticChangeEvent = Server::ServerSemanticChangeEvent::create();

			inputArgumentResults.push_back(EnumStatusCode_Good);
			result = pSemanticChangeEvent->setChanges(semanticChangeArguments);
			NodeId id(EnumNumericNodeId_Server);
			pSemanticChangeEvent->report(&id);

		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
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
