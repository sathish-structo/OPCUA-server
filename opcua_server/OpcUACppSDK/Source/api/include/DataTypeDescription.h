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

#ifndef DATATYPEDESCRIPTION_H
#define DATATYPEDESCRIPTION_H

#include "Base.h"
#include "NodeId.h"
#include "QualifiedName.h"

namespace SoftingOPCToolbox5
{
	/*! This is the base class of a user defined data type.
	*
	* A server application can define and expose data types by attaching is to a DaatType node.
	* Before assigning to a data type node, the data type cannot be used. */
	class TBC_EXPORT DataTypeDescription
		: public Base
	{
	protected:
		/*! Default constructor */
		DataTypeDescription();
	public:
		/*! Destructor */
		virtual ~DataTypeDescription();

		/*! Indicates the concrete subtype of this DataTypeDescription instance.
		* After checking for the correct concrete type, this class can safely be cast to
		* its concrete subtype.
		* @see static_pointer_cast */
		EnumDataTypeDescriptionType getDescriptionType() const;

		/*! Returns the Name of the DataTypeDescription
		*
		* The name of the data type description is assigned at server side via Server::DataType::setDataTypeDescription().
		* Before this, an empty QualifiedName will be returned.
		* On client side the name is provided by SDK with ClientSession::updateDataTypes(). */
		QualifiedName getName() const;

		/*! Gets the NodeId of the data type node
		*
		* @note For server application, the data type NodeId is available only after assigning the data type description to the data type node.
		* @see Server::DataType::setDataTypeDescription() */
		NodeId getDataTypeId() const;
	private:
		/*! DataTypeDescription cannot be copied */
		DataTypeDescription(const DataTypeDescription&);
		/*! DataTypeDescription cannot be copied */
		DataTypeDescription& operator=(const DataTypeDescription&);
	};
	typedef ObjectPointer<DataTypeDescription> DataTypeDescriptionPtr;
	typedef ObjectPointer<const DataTypeDescription> DataTypeDescriptionConstPtr;
} // end namespace

#endif	// DATATYPEDESCRIPTION_H
