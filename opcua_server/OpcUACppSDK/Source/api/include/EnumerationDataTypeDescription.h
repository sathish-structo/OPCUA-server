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

#ifndef ENUMERATION_DATATYPE_DESCRIPTION_H
#define ENUMERATION_DATATYPE_DESCRIPTION_H

#include "Base.h"
#include "NodeId.h"
#include "DataTypeDescription.h"
#include "EnumField.h"

namespace SoftingOPCToolbox5
{
	/*! This class stores the information of an enumeration data type, it holds a list of EnumField.
	*
	* Any "normal" enumeration DataType has to reside below the abstract "Enumeration" DataType.
	* Subtypes of enumerations can only restrict the enumeration values of its super-type.
	* The enumeration values have to be specified completely.
	*
	* A "special" enumeration data type is an OptionSet, which describes a bitset, i.e. multiple bits can be set to represent different flags.
	* In this case the enumerated values specify the bit position to be set if enabled (e.g. enumerated value == 0, the first bit is set, i.e. the value is 0x01)
	* The withs of the bitset can be defined via the width of any unsigned number data type.
	*
	* @see EnumField */
	class TBC_EXPORT EnumerationDataTypeDescription
		: public DataTypeDescription
	{
	public:
		/*! Constructs a new EnumerationDataTypeDescription instance */
		static ObjectPointer<EnumerationDataTypeDescription> create();

		/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
		EnumerationDataTypeDescription(TBHandle otbHandle);

		/*! Destructor */
		virtual ~EnumerationDataTypeDescription();

	protected:
		/*! Default constructor */
		EnumerationDataTypeDescription();

	public:
		/*! Sets the list of EnumField
		*
		* @param	enumeratedValues	The EnumeratedValues of the EnumerationDataTypeDescription
		*
		* @note		The integer representations of all given EnumeratedValues must be different.
		*			It is recommended to use different names as well. */
		EnumStatusCode setEnumeratedValues(const std::vector<EnumField>& enumeratedValues);

		/*! Returns the list of EnumField
		*
		* @param	enumeratedValues	The EnumeratedValues of the EnumerationDataTypeDescription */
		EnumStatusCode getEnumeratedValues(std::vector<EnumField>& enumeratedValues) const;

		/*! Gets the definition of the enumerated data type.
		*
		* The EnumDefinition just contain the list of enumerated fields, so this is more an overload to the getEnumeratedValues. */
		EnumStatusCode getDefinition(EnumDefinition& definition) const;

		/*! Get the transport data type
		*
		* Normal enumerations have to use transport data type Int32.
		* Those enumerations have to reside below the abstract "Enumeration" DataType node.
		*
		* Enumerations used as "OptionSet" have to use transport data types Byte, UInt16, UInt32, or UInt64.
		* The enumerated values of OptionSets identify the bit number which can be set.
		*
		* @note Use a value of the transport data type to set or get a value in the Value for this data type. */
		EnumDataTypeId getTransportDataType() const;

		/*! Determines, whether this EnumerationDataTypeDescription has an enumeration representation for the given integer */
		bool isEnumValue(OTInt64 integerRepresentation) const;

		/*! Returns the EnumValueType, that represents the given integer value
		* @note An error is returned, if there is no matching representation */
		EnumStatusCode getEnumeratedValue(OTInt64 integerRepresentation, EnumField& enumeratedValue) const;

	private:
		/*! EnumerationDataTypeDescription cannot be copied */
		EnumerationDataTypeDescription(const EnumerationDataTypeDescription&);
		/*! EnumerationDataTypeDescription cannot be copied */
		EnumerationDataTypeDescription& operator=(const EnumerationDataTypeDescription&);
	};
	typedef ObjectPointer<EnumerationDataTypeDescription> EnumerationDataTypeDescriptionPtr;
	typedef ObjectPointer<const EnumerationDataTypeDescription> EnumerationDataTypeDescriptionConstPtr;
} // end namespace

#endif	// ENUMERATED_DATATYPE_DESCRIPTION_H
