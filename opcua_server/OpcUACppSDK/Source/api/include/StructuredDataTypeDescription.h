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

#ifndef STRUCTUREDDATATYPEDESCRIPTION_H
#define STRUCTUREDDATATYPEDESCRIPTION_H

#include "Base.h"
#include "NodeId.h"
#include "DataTypeDescription.h"
#include "StructureField.h"

namespace SoftingOPCToolbox5
{
	/*! This class stores the information of a structured data type or union, it holds a list of StructureFields.
	*
	* @see FieldDescription */
	class TBC_EXPORT StructuredDataTypeDescription
		: public DataTypeDescription
	{
	public:
		/*! Constructs a new StructuredDataTypeDescription instance */
		static ObjectPointer<StructuredDataTypeDescription> create();

		/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
		StructuredDataTypeDescription(TBHandle otbHandle);

		/*! Destructor */
		virtual ~StructuredDataTypeDescription();

	protected:
		/*! Default constructor */
		StructuredDataTypeDescription();

	public:
		/*! Defines the fields of the structured data type or union
		*
		* @param	fields	The fields of the StructuredDataTypeDescription
		*
		* @note	The list of fields contain only the fields defined by this data type.
		*		In case of a derived structure, the fields of the base type shall not be specified in the description, but have to be provided in the Value.
		*
		* @note	If this StructuredDataTypeDescription is configured to describe a union, then the "IsOptional" flag of the fields is ignored.
		*
		* @note	All fields must have different names, including the fields of all base types.
		*
		* @see setUnion */
		EnumStatusCode setFields(const std::vector<StructureField>& fields);

		/*! Returns the list of FieldDescriptions
		*
		* @param	fields	The fields of the StructuredDataTypeDescription
		*
		* @see setFields */
		EnumStatusCode getFields(std::vector<StructureField>& fields) const;

		/*! Gets the definition of the structured data type.
		*
		* A structured data type is completely described by its definition.
		* Beside the StructureFields, the definition also contains
		* @li The default encoding ID, which is used for encoding / decoding values
		* @li The base type of the structure, where the structure inherits fields from
		* @li The StructureType, which gives more information about the structure
		*
		* All these information cannot be set directly, but are automatically calculated once a server exposes a data type.
		* @li The StructureType depends on existence of optional fields.
		* @li The base type depends on the hierarchy of data type node, where the definition is assigned to.
		* @li The encoding ID is provided in Server::DataType::setDataTypeDescription
		*
		* @note The member DefaultEncodingId of definition shall match the NodeId provided when the description is assigned to a data type node
		*		via Server::DataType::setDataTypeDescription().
		*
		* @note The member BaseDataType of definition is currently ignored.
		*		The base data type is calculated with the HasSubtype reference to the Server::DataType node. */
		EnumStatusCode getDefinition(StructureDefinition& definition) const;

		/*! Gets the NodeId of the encoding node
		*
		* @note For server application, the encoding NodeId is available only after assigning the data type description to the data type node.
		* @see Server::DataType::setDataTypeDescription()
		*
		* @note The encoding NodeId may be the Null NodeId, especially for enumeration data types, where the encoding is implicitly done as Int32. */
		NodeId getEncodingId() const;

		/*! Returns whether this StructuredDataTypeDescription describes a union (true) or a structure (false). */
		bool isUnion() const;

		/*! Defines if this StructuredDataTypeDescription describes a union (true) or a structure (false).
		*
		* The default value is "false" to describe a structure.
		*
		* @note Unions must be direct subtypes of data type "Union", so unions cannot be subtypes of other unions.
		*
		* @see setFields */
		EnumStatusCode setUnion(bool descriptionIsUnion);

	private:
		/*! StructuredDataTypeDescription cannot be copied - not implemented by intension */
		StructuredDataTypeDescription(const StructuredDataTypeDescription&);
		/*! StructuredDataTypeDescription cannot be copied - not implemented by intension */
		StructuredDataTypeDescription& operator=(const StructuredDataTypeDescription&);
	};
	typedef ObjectPointer<StructuredDataTypeDescription> StructuredDataTypeDescriptionPtr;
	typedef ObjectPointer<const StructuredDataTypeDescription> StructuredDataTypeDescriptionConstPtr;
} // end namespace

#endif	// STRUCTUREDDATATYPEDESCRIPTION_H
