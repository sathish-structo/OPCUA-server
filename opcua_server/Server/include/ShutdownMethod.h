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
#ifndef _SHUTDOWNMETHOD_H_
#define _SHUTDOWNMETHOD_H_

#include "ServerMethod.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		class Object;
		typedef ObjectPointer<Object> ObjectPtr;
	}
}

class ShutdownMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	void initialize(SoftingOPCToolbox5::Server::ObjectPtr parentObject, OTUInt16 namespaceIndex);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* /* transaction */ ,SoftingOPCToolbox5::Server::MethodCallRequest* request);
private:
	void setCustomInputArguments();
};

#endif // _SHUTDOWNMETHOD_H_
