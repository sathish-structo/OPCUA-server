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
#ifndef OPERATIONLIMITMETHODS_H
#define OPERATIONLIMITMETHODS_H

#include "ServerMethod.h"

class GetOperationLimitMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	EnumStatusCode initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
};
typedef SoftingOPCToolbox5::ObjectPointer<GetOperationLimitMethod> GetOperationLimitMethodPtr;

class SetOperationLimitMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	EnumStatusCode initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
};
typedef SoftingOPCToolbox5::ObjectPointer<SetOperationLimitMethod> SetOperationLimitMethodPtr;

#endif	// OPERATIONLIMITMETHODS_H
