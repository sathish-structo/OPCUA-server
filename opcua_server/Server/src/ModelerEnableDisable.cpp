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

#include "ModelerEnableDisable.h"
#include "ServerAddressSpaceRoot.h"
#include "Statics.h"
#include "ServerMethodCallRequest.h"
#include "ServerBaseNode.h"
#include "Argument.h"
#include "DataValue.h"
#include "ApplicationModule.h"

using namespace SoftingOPCToolbox5;

extern ApplicationModule* g_ServerModule;

EnumStatusCode EnableModelerMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	EnumStatusCode result = EnumStatusCode_Good;

	std::vector<Value> inputArguments = request->getInputArguments();
	if (0 == inputArguments.size())
	{
		EnumModelDesignerStatus mcResult = EnumModelDesignerStatus_Good;
		mcResult = Server::AddressSpaceRoot::instance()->enableModeling(Server::AddressSpaceRoot::instance()->getServerObject());
		if(StatusCode::isBad(mcResult))
		{
			switch(mcResult)
			{
			case EnumModelDesignerStatus_BadInvalidState:
				result = EnumStatusCode_BadInvalidState;
				break;
			case EnumModelDesignerStatus_BadNodeIdExists:
				result = EnumStatusCode_BadNodeIdExists;
				break;
			case EnumModelDesignerStatus_BadFileOpenError:
				result = EnumStatusCode_BadFileNotFound;
				break;
			default:
				result = EnumStatusCode_BadInternalError;
			}
		}
		else
		{
			AddressSpaceRoot::instance()->addNamespace(_T("http://softing.com/ModelDesignerTests"), NULL);
		}
	}
	else
	{
		result = EnumStatusCode_BadInvalidArgument;
	}
	request->setStatusCode(result);
	request->complete();
	return result;
}

bool EnableModelerMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("EnableModeler");
	setNodeId(NodeId(namespaceIndex, methodName));
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Enables the Modeler Root"), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	/*if(StatusCode::isBad(this->setInputArguments(&inputArgsNodeId, inputArguments)))
	{
		_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
		return false;
	}*/
	if(StatusCode::isBad(init(NULL)))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return false;
	}
	return true;
}

EnumStatusCode DisableModelerMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	EnumStatusCode result = EnumStatusCode_Good;

	std::vector<Value> inputArguments = request->getInputArguments();
	if (0 == inputArguments.size())
	{
		EnumModelDesignerStatus mcResult = EnumModelDesignerStatus_Good;
		mcResult = Server::AddressSpaceRoot::instance()->disableModeling();
		if(StatusCode::isBad(mcResult))
		{
			switch(mcResult)
			{
			case EnumModelDesignerStatus_BadInvalidState:
				result = EnumStatusCode_BadInvalidState;
				break;
			case EnumModelDesignerStatus_BadNodeIdExists:
				result = EnumStatusCode_BadNodeIdExists;
				break;
			case EnumModelDesignerStatus_BadFileOpenError:
				result = EnumStatusCode_BadFileNotFound;
				break;
			default:
				result = EnumStatusCode_BadInternalError;
			}
		}
	}
	else
	{
		result = EnumStatusCode_BadInvalidArgument;
	}
	request->setStatusCode(result);
	request->complete();
	return result;
}

bool DisableModelerMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("DisableModeler");
	setNodeId(NodeId(namespaceIndex, methodName));
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Disables the Modeler Root"), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	/*NodeId inputArgsNodeId(namespaceIndex, _T("ImportNamespaces_InputArguments"));
	std::vector<Argument> inputArguments;*/

	//if(StatusCode::isBad(this->setInputArguments(&inputArgsNodeId, inputArguments)))
	//{
	//	_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
	//	return false;
	//}
	if(StatusCode::isBad(init(NULL)))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return false;
	}
	return true;
}

