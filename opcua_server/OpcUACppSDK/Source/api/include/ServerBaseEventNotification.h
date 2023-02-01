/*****************************************************************************
*                                                                            *
*                     SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                          *
*                                D-85540 Haar                                *
*                        Phone: ++49-89-4 56 56-0                            *
*                          Fax: ++49-89-4 56 56-3 99                         *
*                                                                            *
*                            SOFTING CONFIDENTIAL                            *
*                                                                            *
*                     Copyright (C) SOFTING IA GmbH 2022                     *
*                             All Rights Reserved                            *
*                                                                            *
* NOTICE: All information contained herein is, and remains the property of   *
*   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- *
*   lectual and technical concepts contained herein are proprietary to       *
*   SOFTING Industrial Automation GmbH and its suppliers and may be covered  *
*   by German and Foreign Patents, patents in process, and are protected by  *
*   trade secret or copyright law. Dissemination of this information or      *
*   reproduction of this material is strictly forbidden unless prior         *
*   written permission is obtained from SOFTING Industrial Automation GmbH.  *
******************************************************************************
******************************************************************************
*                                                                            *
* PROJECT_NAME             OPC UA C++ Server & Client SDK                    *
*                                                                            *
* VERSION                  6.00.0                                            *
*                                                                            *
* DATE                     13.6.2022                                            *
*                                                                            *
*****************************************************************************/

#ifndef SERVERBASEEVENTNOTIFICATION_H
#define SERVERBASEEVENTNOTIFICATION_H
#if TB5_ALARMS
#include "Base.h"
#include "Enums.h"
#include "InnerNodeId.h"
#include "NodeId.h"
#include "ServerObject.h"
#include "TimeZone.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		/*! Base class for simple events that are not represented by nodes in the address space.
		*
		* In general, those event types are almost anything except conditions.
		* These simple events can be described by a flat, unstructured set of properties. */
		class TBC_EXPORT BaseEventNotification
			: public Base
		{
		public:
			/*! Constructs a new event notification object for the given event type */
			static ObjectPointer<BaseEventNotification> create(const INodeId* eventTypeNode);

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			BaseEventNotification(TBHandle otbHandle);

		protected:
			/*! Constructs a new event notification object for the given event type */
			BaseEventNotification(const INodeId* eventTypeNode);

			/*! For internal use only */
			BaseEventNotification(bool suppressCoreInstanceCreate);

		public:
			/*! Destructor */
			virtual ~BaseEventNotification();

			/*! Initializes the BaseEventNotification instance with the given event type or event instance node.
			* If the node is of a wrong type the function call will return an error.
			*
			* @note	The event type is already defined in the constructor. Unless derived classes
			*		override init(), there is no need to call this method again. */
			virtual EnumStatusCode init(const INodeId* eventTypeNode);

			/*! @see BaseEvent::getEventId() */
			virtual ByteString getEventId() const;
			/*! @see BaseEvent::setEventId() */
			virtual EnumStatusCode setEventId(const IByteString* eventId);

			/*! @see BaseEvent::getEventType() */
			virtual NodeId getEventType() const;
			/*! @see BaseEvent::setEventType() */
			virtual EnumStatusCode setEventType(const INodeId* eventTypeNode);
			/*! @see BaseEvent::setEventType() */
			virtual EnumStatusCode setEventType(const INodeId& eventTypeNode);

			/*! @see BaseEvent::getSourceNode() */
			virtual NodeId getSourceNode() const;
			/*! @see BaseEvent::setSourceNode() */
			virtual EnumStatusCode setSourceNode(const INodeId* sourceNode);
			/*! @see BaseEvent::setSourceNode() */
			virtual EnumStatusCode setSourceNode(const INodeId& sourceNode);

			/*! @see BaseEvent::getSourceName() */
			virtual tstring getSourceName() const;
			/*! @see BaseEvent::setSourceName() */
			virtual EnumStatusCode setSourceName(const tstring& sourceName);

			/*! @see BaseEvent::getTime() */
			virtual DateTime getTime() const;
			/*! @see BaseEvent::setTime() */
			virtual EnumStatusCode setTime(const IDateTime* time);
			/*! @see BaseEvent::setTime() */
			virtual EnumStatusCode setTime(const IDateTime& time);

			/*! @see BaseEvent::getReceiveTime() */
			virtual DateTime getReceiveTime() const;
			/*! @see BaseEvent::setReceiveTime() */
			virtual EnumStatusCode setReceiveTime(const IDateTime* receiveTime);
			/*! @see BaseEvent::setReceiveTime() */
			virtual EnumStatusCode setReceiveTime(const IDateTime& receiveTime);

			/*! @see BaseEvent::getLocalTime() */
			virtual TimeZone getLocalTime() const;
			/*! @see BaseEvent::setLocalTime() */
			virtual EnumStatusCode setLocalTime(const ITimeZone* localTime);
			/*! @see BaseEvent::setLocalTime() */
			virtual EnumStatusCode setLocalTime(const ITimeZone& localTime);

			/*! @see BaseEvent::getMessage() */
			virtual LocalizedText getMessage() const;
			/*! @see BaseEvent::setMessage() */
			virtual EnumStatusCode setMessage(const ILocalizedText* message);
			/*! @see BaseEvent::setMessage() */
			virtual EnumStatusCode setMessage(const ILocalizedText& message);

			/*! @see BaseEvent::getSeverity() */
			virtual OTUInt16 getSeverity() const;
			/*! @see BaseEvent::setSeverity() */
			virtual EnumStatusCode setSeverity(OTUInt16 severity);

			/*! @see BaseEvent::getProperty() */
			virtual Value getProperty(const IQualifiedName& propertyName) const;
			/*! @see BaseEvent::setProperty() */
			virtual EnumStatusCode setProperty(const IQualifiedName& propertyName, const IValue* propertyValue);
			/*! @see BaseEvent::setProperty() */
			virtual EnumStatusCode setProperty(const IQualifiedName& propertyName, const IValue& propertyValue);
			/*! @see BaseEvent::removeProperty() */
			virtual EnumStatusCode removeProperty(const IQualifiedName& propertyName);

			/*! @see BaseEvent::report() */
			virtual EnumStatusCode report(const INodeId* reportingNode);

		private:
			/*! Forbid use assignment operator */
			BaseEventNotification& operator=(const BaseEventNotification&);
			/*! Forbid use of copy constructor */
			BaseEventNotification(const BaseEventNotification&);
		};
		typedef ObjectPointer<BaseEventNotification> BaseEventNotificationPtr;
		typedef ObjectPointer<const BaseEventNotification> BaseEventNotificationConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif
#endif	// SERVERBASEEVENTNOTIFICATION_H
