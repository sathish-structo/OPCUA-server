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

#ifndef SERVERBROWSECONTINUATIONPOINT_H
#define SERVERBROWSECONTINUATIONPOINT_H

#include "Base.h"
#include "ServerBaseNode.h"
#include "ServerContinuationPoint.h"
#include "ByteString.h"
#include "ViewDescription.h"
#include "NodeId.h"

namespace SoftingOPCToolbox5
{
	// Forward declarations
	namespace Server
	{
		class ReferenceIterator;
		typedef ObjectPointer<ReferenceIterator> ReferenceIteratorPtr;
	}

	namespace Server
	{
		/*! A continuation point for Browse requests.
		*
		* @see ContinuationPoint BrowseTransaction BrowseRequest BrowseNextTransaction BrowseNextRequest */
		class TBC_EXPORT BrowseContinuationPoint
			: public ContinuationPoint
		{
		public:
			/*! Constructs a new BrowseContinuationPoint instance */
			static ObjectPointer<BrowseContinuationPoint> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			BrowseContinuationPoint(TBHandle otbHandle);

		protected:
			/*! Default constructor */
			BrowseContinuationPoint();

		public:
			/*! Destructor */
			virtual ~BrowseContinuationPoint();

			/*! Returns the type of this continuation point (Browse).
			*
			* @return The continuation point type */
			virtual EnumContinuationPointType getType() const;

			/*! Stores some context information to know where to continue the request during a BrowseNext.
			*
			* This is the default context information which is assigned and expected to be set by the default implementations of
			* Server::BrowseTransaction and ServerBrowseNextTransaction.
			* Setting this default context can be omitted if the entire default implementations of Server::BrowseTransaction
			* and ServerBrowseNextTransaction are overwritten.
			*
			* @note	To store additional or different context information for this ContinuationPoint, create
			*		and use a derived BrowseContinuationPoint which holds the additional data as additional members. */
			EnumStatusCode setReferenceIteratorContext(Server::ReferenceIteratorPtr referenceIterator);

			/*! Gets the previously stored context information to continue an already started BrowseRequest during BrowseNext.
			*
			* @return Returns NULL if no context information was set
			*
			* @see setReferenceIteratorContext */
			Server::ReferenceIteratorPtr getReferenceIteratorContext();

			/*! @deprecated Please store custom data to a custom derived BrowseContinuationPoint class and
			* ensure to free the custom data via the destructor.
			*
			* Interprets and returns the context information as ByteString.
			*
			* @return The context information interpreted as ByteString */
			DEPRECATED(ByteString getContextAsByteString() const);
			/*! @deprecated Please store custom data to a custom derived BrowseContinuationPoint class and
			* ensure to free the custom data via the destructor.
			*
			* Stores the provided @p byteString as context information for this
			* continuation point.
			*
			* Any previously stored context information is deleted. */
			DEPRECATED(EnumStatusCode setContextFromByteString(const ByteString& byteString));

			/*! Returns the description of the view to browse.
			*
			* The view describes if the browse shall be performed on the entire address space or with certain
			* restriction, where only a part of the address space is visible.
			* An empty ViewDescription value indicates the entire address space.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a BrowseRequest
			* @see IViewDescription, Server::View, Server::BrowseRequest::setContinuationPoint() */
			EnumStatusCode getView(ViewDescription& view) const;

			/*! Returns the maximum number of references to return for each starting node.
			* This value applies for every request of this transaction.
			*
			* A client application can request this to restrict the number of returned references.
			* When the number of requests is reached, a ContinuationPoint shall be created so that the client
			* can continue the request via the 'BrowseNext' service.
			*
			* The value 0 indicates that the client is imposing no limitation.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a BrowseRequest
			* @see Server::ContinuationPoint, Server::BrowseNextTransaction, Server::BrowseRequest::setContinuationPoint() */
			OTUInt32 getRequestedMaxReferencesPerNode() const;

			/*! Returns the NodeId of the node from where the browse shall be performed.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a BrowseRequest
			* @see Server::BrowseRequest::setContinuationPoint() */
			EnumStatusCode getNodeIdToBrowse(NodeId& nodeId) const;

			/*! Returns a pointer to the Node from where the browse shall be performed.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a BrowseRequest
			* @see Server::BrowseRequest::setContinuationPoint() */
			ObjectPointer<Server::BaseNode> getNodeToBrowse();
			/*! @overload */
			ObjectPointer<const Server::BaseNode> getNodeToBrowse() const;

			/*! Returns the direction of references to follow, whether to search in forward, backward
			* or both directions.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a BrowseRequest
			* @see	EnumBrowseDirection, Server::BrowseRequest::setContinuationPoint() */
			EnumBrowseDirection getBrowseDirection() const;

			/*! Returns the NodeId of the reference type to follow.
			* Only found instances of this type or its subtypes shall be returned.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a BrowseRequest
			* @see	getIncludeSubtypes(), Server::BrowseRequest::setContinuationPoint() */
			EnumStatusCode getReferenceTypeId(NodeId& nodeId) const;

			/*! Returns whether to follow subtypes of the provided reference type.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a BrowseRequest
			* @see	getReferenceTypeId(), Server::BrowseRequest::setContinuationPoint() */
			bool getIncludeSubtypes() const;

			/*! Returns the node classes that are to be followed.
			* This option restricts the returned results to certain node classes.
			*
			* @note	This value is a bit mask and it may be a combination of several values.
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a BrowseRequest
			* @see	EnumNodeClass, Server::BrowseRequest::setContinuationPoint() */
			EnumNodeClass getNodeClassMask() const;

			/*! Returns the result mask that is to be applied to the followed nodes.
			* The result mask describes, which information shall be returned for the found nodes additional to their NodeId.
			*
			* @note	This value is a bit mask and it may be an combination of several values.
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a BrowseRequest
			* @see	EnumResultMask, Server::BrowseRequest::setContinuationPoint() */
			EnumResultMask getResultMask() const;

		private:
			/*! For internal use only */
			void initMembers();

			/*! Forbid use assignment operator */
			BrowseContinuationPoint& operator=(const BrowseContinuationPoint&);
			/*! Forbid use of copy constructor */
			BrowseContinuationPoint(const BrowseContinuationPoint&);
		};
		typedef ObjectPointer<Server::BrowseContinuationPoint> BrowseContinuationPointPtr;
		typedef ObjectPointer<const Server::BrowseContinuationPoint> BrowseContinuationPointConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif	// SERVERBROWSECONTINUATIONPOINT_H
