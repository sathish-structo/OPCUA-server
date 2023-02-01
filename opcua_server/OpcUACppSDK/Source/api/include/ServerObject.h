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

#ifndef SERVEROBJECT_H
#define SERVEROBJECT_H
#include "Base.h"
#include "Enums.h"
#include "ServerBaseNode.h"
#include "ServerHistoryReadEventRequest.h"
#include "ServerHistoryReadEventTransaction.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		/*! The Object class is used to define an Object node in the address space of a server.
		*
		* An Object is used to represent systems, system components, real-world objects and software
		* objects in the address space of a server. An object consists of other objects, variables and/or methods.
		*
		* Example: A device could be represented as an object whereas its process variables could be
		* mapped as variables in the address space. */
		class TBC_EXPORT Object
			: public BaseNode
		{
		public:
			/*! Constructs a new Object instance */
			static ObjectPointer<Object> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			Object(TBHandle otbHandle);

		protected:
			/*! Default constructor */
			Object();
			/*! For internal use only */
			Object(bool suppressCoreInstanceCreate);

		public:
			/*! Destructor */
			virtual ~Object();

			/*! Returns the 'EventNotifier' attribute of this object.
			*
			* @see setEventNotifier() */
			EnumEventNotifier getEventNotifier() const;
			/*! Sets the 'EventNotifier' attribute of this object.
			*
			* The EventNotifier attribute is used to indicate if the node can be used
			* to subscribe to events or to read/write historical events.
			*
			* By default, the event notifier attribute contains 0.
			*
			* @see EnumEventNotifier */
			EnumStatusCode setEventNotifier(EnumEventNotifier eventNotifier);

			/*! Returns the NodeId of the object's type.
			* The object type is assigned during BaseNode::init(). */
			EnumStatusCode getTypeNodeId(NodeId* pTypeNodeId) const;

#if TB5_HISTORY
			/*! Callback method called by the default implementation of Server::HistoryReadEventTransaction::handleRequests().
			*
			* This method must be overridden by server applications to provide historical events
			* to clients.
			*
			* @param transaction	The transaction of the request to handle
			* @param request		The request to handle
			*
			* @note	When overriding this method you mustn't use blocking calls within this method since
			* this blocks the UA communication. */
			virtual EnumStatusCode handleHistoryReadEventRequest(Server::HistoryReadEventTransaction* transaction, Server::HistoryReadEventRequest* request);
#endif

			/*! Callback method that is triggered when a UA method is invoked in context of this object by a client.
			* This method should be overridden by server applications.
			*
			* @param transaction	The transaction which contains the single call request
			* @param request		The call request which shall be handled.
			*
			* @note The request is pre-processed by the SDK.
			*		In case of errors (e.g. access restrictions, input arguments, ...) the status code of the request is set.
			*		In case of invalid input arguments the input argument results is set by SDK.
			*
			* @note	When overriding this method you mustn't use blocking calls within this method since
			*		this blocks the UA communication.
			*
			* @note This callback won't be invoked for methods where the callback Method::handleExecute() is overridden,
			*		unless the overriding implementation invokes Method::executeRequest().
			*
			* @note The method does not have a return value - us request->setStatusCode to pass a result to the client. */
			virtual void handleCallRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);

		private:
			/*! Forbid use assignment operator */
			Object& operator=(const Object&);
			/*! Forbid use of copy constructor */
			Object(const Object&);
		};
		typedef ObjectPointer<Object> ObjectPtr;
		typedef ObjectPointer<const Object> ObjectConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif	// SERVEROBJECT_H
