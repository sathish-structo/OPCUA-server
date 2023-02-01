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
#include "EventSourceObject.h"
#include "Statics.h"

/**
 * @brief Constructor
 */
EventSourceObject::EventSourceObject()
{
}

/**
 * @brief Destructor
 */
EventSourceObject::~EventSourceObject()
{
}

void EventSourceObject::initialize(Object* parent,
									const INodeId& nodeId,
									const ILocalizedText& displayName,
									const IQualifiedName& browseName)
{
	m_parent = parent;

	setBrowseName(browseName);
	setDisplayName(displayName);
	setNodeId(nodeId);
	setEventNotifier(EnumEventNotifier_SubscribeToEvents);
	Server::Object::init(NULL);

	m_parent->addReference(Statics::ReferenceTypeId_Organizes, this);
}
