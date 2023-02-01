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
#ifndef _TESTSERVER_SESSION_H_
#define _TESTSERVER_SESSION_H_

#include "ServerSession.h"

using namespace SoftingOPCToolbox5;

/**
 * @brief Overloaded session
 */
class TestServerSession
	: public Server::Session
{
public:

	/*! @brief Constructor */
	TestServerSession(void* otbHandle);

	virtual ~TestServerSession();

	virtual EnumStatusCode onRegisterNodes(const std::vector<NodeId> &registeredNodes);
	virtual EnumStatusCode onUnregisterNodes(const std::vector<NodeId> &registeredNodes);

	virtual OTUInt32 getMaxEventQueueSize() {return m_maxEventQueueSize;};
	virtual void setMaxEventQueueSize(OTUInt32 maxSize) {m_maxEventQueueSize = maxSize;};
private:
	OTUInt32 m_maxEventQueueSize;

};

typedef ObjectPointer<TestServerSession> TestServerSessionPtr;
typedef ObjectPointer<const TestServerSession> TestServerSessionConstPtr;


#endif
