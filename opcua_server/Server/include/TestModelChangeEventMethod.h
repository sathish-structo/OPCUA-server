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
#ifndef _TESTMODELCHANGEEVENTMETHOD_H_
#define _TESTMODELCHANGEEVENTMETHOD_H_
#if TB5_ALARMS

#include "ServerGeneralModelChangeEvent.h"
#include "ModelChangeStructureDataType.h"
#include "ServerMethod.h"

using namespace SoftingOPCToolbox5;
class TestModelChangeEventMethod: public Server::Method
{
public:
	~TestModelChangeEventMethod(void);

	/*! Creates a new method object in the address space. 
		*
		* The parent object is given, the own node ID are provided by the containing RedModule in class variables.
		* The other information (strings, ...) are hard coded
		*/
		static ObjectPointer<TestModelChangeEventMethod> create(const NodeId& changeEventNodeId, Server::BaseNodePtr parent);

	/*! Overwritten handler. 
		*
		* The handler passes the nodes and data values to the containing RedModule.
		* The containing module can deny the request (e.g. if its own server state is not secondary)
		*/
		virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);
private:
	TestModelChangeEventMethod();
	
};

#endif
#endif // _TESTMODELCHANGEEVENTMETHOD_H_
