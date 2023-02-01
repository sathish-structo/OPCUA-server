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
#include "OperationLimitMethods.h"

#include "Statics.h"
#include "Argument.h"
#include "ServerMethodCallRequest.h"
#include "ServerAddressSpaceRoot.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

EnumStatusCode GetOperationLimitMethod::initialize(BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("GetOperationLimit");
	EnumStatusCode result = setNodeId(NodeId(namespaceIndex, methodName));
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to set the NodeId of %s\n"), methodName.c_str());
		return result;
	}
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Gets the current operation limit of a service type"), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	NodeId inputArgsNodeId(namespaceIndex, _T("GetOperationLimit_InputArguments"));
	std::vector<Argument> inputArguments;
	Argument currentArgument;
	currentArgument.setName(_T("ServiceType"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("The service type as string, valid values are e.g. \"Read\", \"Write\", \"Browse\" ..."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);
	result = this->setInputArguments(&inputArgsNodeId, inputArguments);
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
		return result;
	}

	NodeId outputArgsNodeId(namespaceIndex, _T("GetOperationLimit_OutputArguments"));
	std::vector<Argument> outputArguments;
	currentArgument.setName(_T("OperationLimit"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_UInt32));
	currentArgument.setDescription(LocalizedText(_T("The current operation limit"), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	outputArguments.push_back(currentArgument);
	result = this->setOutputArguments(&outputArgsNodeId, outputArguments);
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to set the OutputArguments of %s\n"), methodName.c_str());
		return result;
	}

	result = init(NULL);
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return result;
	}
	return result;
}

EnumStatusCode GetOperationLimitMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArguments = request->getInputArguments();
	std::vector<Value> outputArguments;
	std::vector<EnumStatusCode> inputArgumentResults;
	EnumStatusCode result = validateInputArguments(inputArguments, inputArgumentResults);

	EnumServiceType serviceType = EnumServiceType_Invalid;
	OTUInt32 operationLimit = 0;

	if(StatusCode::isGood(result) && inputArguments.size() == 1)
	{
		tstring serviceTypeString = inputArguments[0].getString();
		serviceType = getEnumServiceTypeFromString(serviceTypeString.c_str());
		if(serviceType < 0)	// failed to parse the string
		{
			// on success, the vector should be empty yet and there is only this one inputArgument
			inputArgumentResults.push_back(EnumStatusCode_BadInvalidArgument);
			result = EnumStatusCode_BadInvalidArgument;
		}
	}

	if(StatusCode::isGood(result))
	{
		operationLimit = AddressSpaceRoot::instance()->getOperationLimit(serviceType);
	}

	outputArguments.push_back(Value(operationLimit));

	if(StatusCode::isBad(validateOutputArguments(outputArguments)))
	{
		_tprintf(_T("The validation of OutputArguments for %s failed\n"), getNodeId()->toString().c_str());
		result = EnumStatusCode_BadInternalError;
	}
	request->setOutputArguments(outputArguments);

	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

EnumStatusCode SetOperationLimitMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("SetOperationLimit");
	EnumStatusCode result = setNodeId(NodeId(namespaceIndex, methodName));
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to set the NodeId of %s\n"), methodName.c_str());
		return result;
	}
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Sets the operation limit of a service type"), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	NodeId inputArgsNodeId(namespaceIndex, _T("SetOperationLimit_InputArguments"));
	std::vector<Argument> inputArguments;
	Argument currentArgument;
	currentArgument.setName(_T("ServiceType"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("The service type as string, valid values are e.g. \"Read\", \"Write\", \"Browse\" ..."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);
	currentArgument.setName(_T("OperationLimit"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_UInt32));
	currentArgument.setDescription(LocalizedText(_T("The new operation limit"), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);
	result = this->setInputArguments(&inputArgsNodeId, inputArguments);
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
		return result;
	}

	result = init(NULL);
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return result;
	}
	return result;
}

EnumStatusCode SetOperationLimitMethod::handleExecuteRequest(MethodCallTransaction* transaction, MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArguments = request->getInputArguments();
	std::vector<EnumStatusCode> inputArgumentResults;
	EnumStatusCode result = validateInputArguments(inputArguments, inputArgumentResults);

	EnumServiceType serviceType = EnumServiceType_Invalid;
	OTUInt32 operationLimit = 0;

	if(StatusCode::isGood(result) && inputArguments.size() == 2)
	{
		tstring serviceTypeString = inputArguments[0].getString();
		serviceType = getEnumServiceTypeFromString(serviceTypeString.c_str());
		if(serviceType < 0)	// failed to parse the string
		{
			// on success, the vector should be empty yet and there is only this one inputArgument
			if(inputArgumentResults.empty())
			{
				inputArgumentResults = std::vector<EnumStatusCode>(2, EnumStatusCode_Good);
			}
			inputArgumentResults[0] = EnumStatusCode_BadInvalidArgument;
			result = EnumStatusCode_BadInvalidArgument;
		}

		operationLimit = inputArguments[1].getUInt32();
	}

	if(StatusCode::isGood(result))
	{
		result = AddressSpaceRoot::instance()->setOperationLimit(serviceType, operationLimit);
	}

	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}
