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
#include "TestServerSession.h"
#include "TestServerSession.h"

#include "ServerMethodCallRequest.h"
#include "ServerMethodCallTransaction.h"
#include "ServerSession.h"
#include "TestServerBrowseTransaction.h"
#include "ServerBaseNode.h"
#include "ServerBrowseContinuationPoint.h"
#include "DynamicNodeCache.h"

// #define TRACE_SESSIONS
#define TRACE_REGISTERNODES

/**
* @brief Constructor
* @param otbHandle OTB handle
*/
TestServerSession::TestServerSession(void* otbHandle)
: Server::Session(otbHandle), m_maxEventQueueSize(0)
{
	// in the TestEndpoint::onCreateSession() an addref on the session object keeps tis instance alive
#ifdef TRACE_SESSIONS
	_tprintf(_T("TestServerSession::TestServerSession %p\n"), this);
#endif //TRACE_SESSIONS
}

/**
* @brief Destructor
*/
TestServerSession::~TestServerSession()
{
	// in the TestEndpoint::onDeleteSession() an release on the session object allows desctuction
#ifdef TRACE_SESSIONS
	_tprintf(_T("TestServerSession::~TestServerSession %p\n"), this);
#endif //TRACE_SESSIONS
}

EnumStatusCode TestServerSession::onRegisterNodes(const std::vector<NodeId> &registeredNodes)
{
#ifdef TRACE_REGISTERNODES
	for (size_t i=0; i < registeredNodes.size(); i++)
	{
		_tprintf(_T("Node registered: %s\n"), registeredNodes[i].toString().c_str());
	}
#endif
	return EnumStatusCode_Good;
}
EnumStatusCode TestServerSession::onUnregisterNodes(const std::vector<NodeId> &registeredNodes)
{
#ifdef TRACE_REGISTERNODES
	for (size_t i=0; i < registeredNodes.size(); i++)
	{
		_tprintf(_T("Node unregistered: %s\n"), registeredNodes[i].toString().c_str());
	}
#endif
	return EnumStatusCode_Good;
}
