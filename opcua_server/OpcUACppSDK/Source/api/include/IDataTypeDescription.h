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

#ifndef IDATATYPEDESCRIPTION_H
#define IDATATYPEDESCRIPTION_H

#include "DataTypeDescriptionStruct.h"
#include "NodeId.h"
#include "INodeId.h"
#include "QualifiedName.h"
#include "IQualifiedName.h"

namespace SoftingOPCToolbox5
{
	/*! This is the base class of a user defined data type. */
	class TBC_EXPORT IDataTypeDescription
	{
	public:
		/*! Destructor */
		virtual ~IDataTypeDescription(){};

		/*! Sets the DataTypeId member in the class
		*
		* @param dataType	The NodeId of the DataType. */
		virtual EnumStatusCode setDataTypeId(const INodeId* dataType) = 0;
		/*! @overload */
		virtual EnumStatusCode setDataTypeId(const NodeId& dataType) = 0;
		/*! Gets the DataTypeId member of the class */
		virtual const INodeId* getDataTypeId() const = 0;

		/*! Sets the Name member in the class
		*
		* @param name	A unique name for the data type. */
		virtual EnumStatusCode setName(const IQualifiedName* name) = 0;
		/*! @overload */
		virtual EnumStatusCode setName(const QualifiedName& name) = 0;
		/*! Gets the Name member of the class */
		virtual const IQualifiedName* getName() const = 0;

		/*! For internal use only */
		virtual void* getInternHandle() const = 0;
	}; // end IDataTypeDescription
} // end namespace

#endif
