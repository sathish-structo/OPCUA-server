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

#ifndef SERVERMULTISTATEITEM_H
#define SERVERMULTISTATEITEM_H

#include "ServerDiscreteItem.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		/*! This class represents a DiscreteItem that can have more than two states. Each state is described by a LocalizedText.
		*
		* Properties of MultiStateItem:
		* @li EnumStrings (mandatory):	A string lookup table corresponding to sequential numeric values (0, 1, 2, etc.).
		*								Example: {"OPEN", "CLOSE", "IN TRANSIT"}.
		*								Here the string "OPEN" corresponds to 0, "Close" to 1 and "IN TRANSIT" to 2. */
		class TBC_EXPORT MultiStateItem
			: public DiscreteItem
		{
		public:
			/*! A value which indicates that the MultiStateItem is in an invalid state. */
			static const OTUInt32 INVALID_STATE = ((OTUInt32)(-1));

			/*! Constructs a new MultiStateItem instance
			*
			* After construction the object has not a single state.
			* The value is set to INVALID_STATE:
			* Use addState() to add additional states and setState() to set the state to a valid value. */
			static ObjectPointer<MultiStateItem> create();

			/*! Constructor with mandatory attributes
			*
			* @param nodeId						NodeId of the MultiStateItem to be created
			* @param displayName				The display name of the MultiStateItem to be created
			* @param browseName					The browse name of the MultiStateItem to be created
			* @param enumStringsPropertyNodeId	optional NodeId of the EnumStrings property of the MultiStateItem to be created.
			*									(If no NodeId is specified, a nodeId will be generated) */
			static ObjectPointer<MultiStateItem> create(const INodeId& nodeId,
				const ILocalizedText& displayName,
				const IQualifiedName& browseName,
				const INodeId* enumStringsPropertyNodeId = NULL);

		protected:
			/*! Default constructor */
			MultiStateItem();

			/*! Constructor with mandatory attributes is protected (for derived types only).
			*
			* Please use MultiStateItem::create(const INodeId&, const ILocalizedText&, const IQualifiedName&) to construct an instance of this type. */
			MultiStateItem(const INodeId& nodeId,
				const ILocalizedText& displayName,
				const IQualifiedName& browseName,
				const INodeId* enumStringsPropertyNodeId = NULL);

		public:
			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			MultiStateItem(TBHandle otbHandle);

			/*! Destructor */
			virtual ~MultiStateItem();

			/*! Adds a new state to the MultiStateItem.
			* The given text is added to the property EnumStrings and the ID of the new state is returned.
			* The first added state will return 0, the next 1, ... */
			OTUInt32 addState(const ILocalizedText& stateText);

			/*! Sets the actual state of the MultiStateItem. */
			EnumStatusCode setState(OTUInt32 actualState);

			/*! Validates whether an input value is valid to be used for the Value attribute of this Variable.
			*
			* Overridden method that additionally validates the range according to configured states.
			* This method may be called when handling a write request to the Value attribute.
			*
			* @param dataValue		The data value to validate
			* @param pIndexRange	The optional index range when only a subset shall be updated.
			*						Use NULL or an empty NumericRange for no index range checks.
			*
			* @see Variable::validateValue */
			virtual EnumStatusCode validateValue(const IValue& value, const INumericRange* pIndexRange);

			/*! @see BaseNode::init() */
			virtual EnumStatusCode init(const INodeId* typeId);

		private:
			/*! Forbid use assignment operator */
			MultiStateItem& operator=(const MultiStateItem&);
			/*! Forbid use of copy constructor */
			MultiStateItem(const MultiStateItem&);
		};

		typedef ObjectPointer<MultiStateItem> MultiStateItemPtr;
		typedef ObjectPointer<const MultiStateItem> MultiStateItemConstPtr;

	} // end Server namespace
} // toolbox namespace end
#endif	// SERVERMULTISTATEITEM_H
