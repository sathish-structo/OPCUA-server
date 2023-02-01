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

#include "Nodeset2ExportImport.h"
#include "ServerAddressSpaceRoot.h"
#include "Statics.h"
#include "ServerMethodCallRequest.h"
#include "ServerBaseNode.h"
#include "Argument.h"
#include "DataValue.h"

using namespace SoftingOPCToolbox5;

EnumStatusCode NodeSet2SetNodeExtensionsMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	EnumStatusCode result = EnumStatusCode_Good;

	std::vector<XmlElement> extensions;
	NodeId nodeId;
	Server::BaseNodePtr node;
	std::vector<tstring> stringExtensions;

	std::vector<Value> inputArguments = request->getInputArguments();
	if (inputArguments.size() < 2)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	else if (inputArguments.size() > 2)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isGood(result))
	{
		// 2 input arguments
		if(StatusCode::isBad(inputArguments[0].getNodeId(nodeId)))
		{
			result = EnumStatusCode_BadInvalidArgument;
		}
		if(StatusCode::isBad(inputArguments[1].getStringArray(stringExtensions)))
		{
			result = EnumStatusCode_BadInvalidArgument;
		}
	}
	if(StatusCode::isGood(result))
	{
		node = Server::AddressSpaceRoot::instance()->getNode(&nodeId);
		if(node.isNull())
		{
			result = EnumStatusCode_BadInvalidArgument;
		}
	}

	if(StatusCode::isGood(result))
	{
		// convert strings to XmlElements
		for (size_t i=0; i< stringExtensions.size(); i++)
		{
			XmlElement extension;
			extension.setUtf8String(stringExtensions[i]);
			extensions.push_back(extension);
		}

		result = node->setExtensions(extensions);
	}

	request->setStatusCode(result);
	request->complete();

	return result;
}

bool NodeSet2SetNodeExtensionsMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("SetNodeExtensions");
	setNodeId(NodeId(namespaceIndex, methodName));
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Sets extensions to a node"), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	NodeId inputArgsNodeId(namespaceIndex, _T("SetNodeExtensions_InputArguments"));
	std::vector<Argument> inputArguments;
	Argument currentArgument;

	currentArgument.setName(_T("NodeId"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_NodeId));
	currentArgument.setDescription(LocalizedText(_T("NodeId of the node"), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("Extensions"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("Extensions"), _T("")));
	currentArgument.setValueRank(1);
	std::vector<OTUInt32> dimensions;
	dimensions.push_back(0);
	currentArgument.setArrayDimensions(dimensions);
	inputArguments.push_back(currentArgument);

	if(StatusCode::isBad(this->setInputArguments(&inputArgsNodeId, inputArguments)))
	{
		_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
		return false;
	}
	if(StatusCode::isBad(init(NULL)))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return false;
	}
	return true;
}

EnumStatusCode NodeSet2GetNodeExtensionsMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	EnumStatusCode result = EnumStatusCode_Good;

	std::vector<XmlElement> extensions;
	NodeId nodeId;
	Server::BaseNodePtr node;
	std::vector<tstring> stringExtensions;

	std::vector<Value> inputArguments = request->getInputArguments();
	if (inputArguments.size() < 1)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	else if (inputArguments.size() > 1)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isGood(result))
	{
		// 1 input argument
		if(StatusCode::isBad(inputArguments[0].getNodeId(nodeId)))
		{
			result = EnumStatusCode_BadInvalidArgument;
		}
	}
	if(StatusCode::isGood(result))
	{
		node = Server::AddressSpaceRoot::instance()->getNode(&nodeId);
		if(node.isNull())
		{
			result = EnumStatusCode_BadInvalidArgument;
		}
	}

	if(StatusCode::isGood(result))
	{
		result = node->getExtensions(extensions);
	}
	if(StatusCode::isGood(result))
	{
		// convert XmlElements to strings
		for (size_t i=0; i< extensions.size(); i++)
		{
			tstring extension = extensions[i].getStringFromUtf8();
			stringExtensions.push_back(extension);
		}
	}
	std::vector<Value> outputArguments;
	Value extensionsValue;
	extensionsValue.setStringArray(stringExtensions);
	outputArguments.push_back(extensionsValue);
	request->setOutputArguments(outputArguments);

	request->setStatusCode(result);
	request->complete();

	return result;
}

bool NodeSet2GetNodeExtensionsMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("GetNodeExtensions");
	setNodeId(NodeId(namespaceIndex, methodName));
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Gets extensions from a node"), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	NodeId inputArgsNodeId(namespaceIndex, _T("GetNodeExtensions_InputArguments"));
	std::vector<Argument> inputArguments;
	NodeId outputArgsNodeId(namespaceIndex, _T("GetNodeExtensions_OutputArguments"));
	std::vector<Argument> outputArguments;
	Argument currentArgument;

	currentArgument.setName(_T("NodeId"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_NodeId));
	currentArgument.setDescription(LocalizedText(_T("NodeId of the node"), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	if(StatusCode::isBad(this->setInputArguments(&inputArgsNodeId, inputArguments)))
	{
		_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
		return false;
	}

	currentArgument.setName(_T("Extensions"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("Extensions"), _T("")));
	currentArgument.setValueRank(1);
	std::vector<OTUInt32> dimensions;
	dimensions.push_back(0);
	currentArgument.setArrayDimensions(dimensions);
	outputArguments.push_back(currentArgument);

	if(StatusCode::isBad(this->setOutputArguments(&outputArgsNodeId, outputArguments)))
	{
		_tprintf(_T("Failed to set the OutputArguments of %s\n"), methodName.c_str());
		return false;
	}

	if(StatusCode::isBad(init(NULL)))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return false;
	}
	return true;
}

