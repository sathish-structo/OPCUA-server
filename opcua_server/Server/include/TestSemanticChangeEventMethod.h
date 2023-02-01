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
#ifndef _TESTSEMANTICCHANGEEVENTMETHOD_H_
#define _TESTSEMANTICCHANGEEVENTMETHOD_H_
#if TB5_ALARMS
#include "ServerMethod.h"
#include "ServerSemanticChangeEvent.h"
#include "SemanticChangeStructureDataType.h"

using namespace SoftingOPCToolbox5;
class TestSemanticChangeEventMethod :
	public Server::Method
{
public:
	~TestSemanticChangeEventMethod(void);
	/*! Creates a new method object in the address space. 
		*
		* The parent object is given, the own node ID are provided by the containing RedModule in class variables.
		* The other information (strings, ...) are hard coded
		*/
		static ObjectPointer<TestSemanticChangeEventMethod> create(const NodeId& changeEventNodeId, Server::BaseNodePtr parent);

	/*! Overwritten handler. 
		*
		* The handler passes the nodes and data values to the containing RedModule.
		* The containing module can deny the request (e.g. if its own server state is not secondary)
		*/
		virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);
private:
	TestSemanticChangeEventMethod(void);
};
#endif
#endif // _TESTSEMANTICCHANGEEVENTMETHOD_H_
