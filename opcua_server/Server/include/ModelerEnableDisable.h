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
#ifndef MODELERENABLEDISABLE_H
#define MODELERENABLEDISABLE_H

#include "ServerMethod.h"

// forward declaration
class EnableModelerMethod;
class DisableModelerMethod;

typedef SoftingOPCToolbox5::ObjectPointer<EnableModelerMethod> EnableModelerMethodPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const EnableModelerMethod> EnableModelerMethodConstPtr;

/*! An OPC UA method to Export namespaces to a xml file */
class EnableModelerMethod:
	public SoftingOPCToolbox5::Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);
	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
};


typedef SoftingOPCToolbox5::ObjectPointer<DisableModelerMethod> DisableModelerMethodPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const DisableModelerMethod> DisableModelerMethodConstPtr;

/*! An OPC UA method to Import namespaces from a xml file */
class DisableModelerMethod :
	public SoftingOPCToolbox5::Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);
	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
};

#endif //MODELERENABLEDISABLE_H
