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
#include "ShutdownMethod.h"

#include "ServerObject.h"
#include "Statics.h"
#include "Argument.h"
#include "ServerMethodCallRequest.h"

#include "ApplicationModule.h"

using namespace SoftingOPCToolbox5;

void ShutdownMethod::initialize(Server::ObjectPtr parentObject, OTUInt16 namespaceIndex)
{
	tstring strMethodId(_T("shutDownMethodId"));
	if(StatusCode::isGood(setNodeId(NodeId(namespaceIndex, strMethodId))))
	{
		tstring strMethodName(_T("ShutdownMethod"));
		setBrowseName(QualifiedName(strMethodName, namespaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T(""));
		setDisplayName(&ltStr);
		setDescription(LocalizedText(_T("Shut down this server"), _T("")));

		setExecutable(true);
		setCustomInputArguments();
		insertTree();
		parentObject->addReference(Statics::ReferenceTypeId_HasComponent, this);
	}
}

void ShutdownMethod::setCustomInputArguments()
{
	vector<Argument> inputArgs;

	// define input args for the method ......
	Argument param;
	param.setDataType(NodeId(EnumNumericNodeId_UInt32));
	param.setName(_T("Delay"));
	param.setDescription(LocalizedText(_T("Delay in milliseconds until the server is shut down."), _T("")));
	param.setValueRank(ValueRank_Scalar);
	inputArgs.push_back(param);

	param.clear();
	param.setDataType(NodeId(EnumNumericNodeId_String));
	param.setName(_T("Password"));
	param.setDescription(LocalizedText(_T("Required password to shut down the server."), _T("")));
	param.setValueRank(ValueRank_Scalar);
	inputArgs.push_back(param);

	NodeId inputArgNodeId(this->getNodeId()->getNamespaceIndex(), _T("shutDownMethod_InputArguments"));
	(void)setInputArguments(&inputArgNodeId, inputArgs);
}

EnumStatusCode ShutdownMethod::handleExecuteRequest(Server::MethodCallTransaction* /* transaction */ , Server::MethodCallRequest* request)
{
	EnumStatusCode functionRetunCode = EnumStatusCode_Good;
	std::vector<Value> inputArgs = request->getInputArguments();

	std::vector<Value> outputArgs;

	vector<EnumStatusCode> inputStatusCodes;

	size_t expectedParams = 2;
	OTUInt32 delay = 0;
	tstring password;

	if(inputArgs.size() < expectedParams)
	{
		functionRetunCode = EnumStatusCode_BadArgumentsMissing;
	}
	else if(inputArgs.size() > expectedParams)
	{
		functionRetunCode = EnumStatusCode_BadInvalidArgument;
	}
	else
	{
		// delay
		if(inputArgs[0].getDataType() == EnumDataTypeId_UInt32)
		{
			inputStatusCodes.push_back(EnumStatusCode_Good);
			delay = inputArgs[0].getUInt32();
		}
		else // wrong type!
		{
			inputStatusCodes.push_back(EnumStatusCode_BadTypeMismatch);
		}
		// password
		if(inputArgs[1].getDataType() == EnumDataTypeId_String)
		{
			password = inputArgs[1].getString();
			if(password == _T("now"))
			{
				inputStatusCodes.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputStatusCodes.push_back(EnumStatusCode_BadSecurityChecksFailed);
			}
		}
		else // wrong type!
		{
			inputStatusCodes.push_back(EnumStatusCode_BadTypeMismatch);
		}
	}

	for(size_t i = 0; i < inputStatusCodes.size(); i++)
	{
		if(StatusCode::isBad(inputStatusCodes[i]))
		{
			functionRetunCode = EnumStatusCode_BadInvalidArgument;
		}
	}

	if(StatusCode::isGood(functionRetunCode))
	{
		getApplicationModule()->shutDown(delay);
	}

	// fill request results
	request->setInputArgumentResults(inputStatusCodes);
	request->setOutputArguments(outputArgs);
	request->setStatusCode(functionRetunCode);

	// and complete it
	request->complete();

	return EnumStatusCode_Good;
}
