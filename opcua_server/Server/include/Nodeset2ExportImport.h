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
#ifndef NODESET2_EXPORT_IMPORT_H
#define NODESET2_EXPORT_IMPORT_H

#include "ServerMethod.h"

/*! An OPC UA method to set extensions to a specified node */
class NodeSet2SetNodeExtensionsMethod :
	public SoftingOPCToolbox5::Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);
	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
};
typedef SoftingOPCToolbox5::ObjectPointer<NodeSet2SetNodeExtensionsMethod> NodeSet2SetNodeExtensionsMethodPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const NodeSet2SetNodeExtensionsMethod> NodeSet2SetNodeExtensionsMethodConstPtr;

/*! An OPC UA method to get extensions from a specified node */
class NodeSet2GetNodeExtensionsMethod :
	public SoftingOPCToolbox5::Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);
	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
};
typedef SoftingOPCToolbox5::ObjectPointer<NodeSet2GetNodeExtensionsMethod> NodeSet2GetNodeExtensionsMethodPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const NodeSet2GetNodeExtensionsMethod> NodeSet2GetNodeExtensionsMethodConstPtr;

#endif //NODESET2_EXPORT_IMPORT_H
