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

#ifndef SERVEROBJECTTYPE_H
#define SERVEROBJECTTYPE_H
#include "Base.h"
#include "Enums.h"
#include "ServerBaseNode.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		/*! The ObjectType class is used to define an ObjectType node in the address space of a server.
		*
		* An ObjectType provides definitions for an Object in the address space, i.e. it defines a
		* blueprint for objects (instances of the ObjectType). Clients use the ObjectType to learn
		* about the references, components, and properties that an Object instance provides.
		*
		* All object types finally derive from the object type "BaseObjectType".
		*
		* Example: The ObjectType "FolderType" is a well-known object type that a server may use
		* to organize information (e.g. variables and/or objects).
		*
		* @note Each Object must be assigned to an ObjectType using BaseNode::init(). */
		class TBC_EXPORT ObjectType
			: public BaseNode
		{
		public:
			/*! Constructs a new ObjectType instance */
			static ObjectPointer<ObjectType> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			ObjectType(TBHandle otbHandle);

		protected:
			/*! Default constructor */
			ObjectType();

		public:
			/*! Destructor */
			virtual ~ObjectType();

			/*! Returns the base type of this object type. If there is no base type, a NULL pointer is returned. */
			ObjectPointer<Server::ObjectType> getBaseType();
			/*! @overload */
			ObjectPointer<const Server::ObjectType> getBaseType() const;

			/*! If true, indicates that objects of this type may NOT be exposed in the address space of the server. */
			bool getIsAbstract() const;
			/*! Indicates whether objects of this type may be exposed in the address space of the server.
			* If the object type is abstract, an object of this type must not be exposed in the address space.
			* However, the object type may still be used as event type in event notifications (see EventNotification). */
			EnumStatusCode setIsAbstract(bool isAbstract);

			/*! Callback method that is triggered when a UA method is invoked in context of this object type by a client.
			* This method should be overridden by server applications.
			*
			* @param transaction	The transaction which contains the single call request
			* @param request		The call request which shall be handled
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
			ObjectType& operator=(const ObjectType&);
			/*! Forbid use of copy constructor */
			ObjectType(const ObjectType&);
		};
		typedef ObjectPointer<ObjectType> ObjectTypePtr;
		typedef ObjectPointer<const ObjectType> ObjectTypeConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif	// SERVEROBJECTTYPE_H
