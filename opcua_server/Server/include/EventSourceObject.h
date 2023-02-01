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

#ifndef _EVENTSOURCEOBJECT_H_
#define _EVENTSOURCEOBJECT_H_

#include "ServerObject.h"
#include "ClientSession.h"
#include "Application.h"

using namespace SoftingOPCToolbox5;

class EventSourceObject
	: public Server::Object
{
public:

	/*! @brief Constructor */
	EventSourceObject();

	/*! @brief Destructor */
	~EventSourceObject();

	void initialize(Object* parent,
		const INodeId& nodeId,
		const ILocalizedText& displayName,
		const IQualifiedName& browseName);

private:
	Server::Object* m_parent;

};

typedef ObjectPointer<EventSourceObject> EventSourceObjectPtr;
typedef ObjectPointer<const EventSourceObject> EventSourceObjectConstPtr;

#endif
