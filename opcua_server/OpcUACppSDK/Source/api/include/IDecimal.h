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

#ifndef IDECIMAL_H
#define IDECIMAL_H

#include "DecimalStruct.h"
#include "ByteString.h"

namespace SoftingOPCToolbox5
{
	/*! The Decimal data type describes a high-precision signed decimal number.
	*
	* It consists of an arbitrary precision integer unscaled value and an integer scale.
	* The scale is the inverse power of ten that is applied to the unscaled value.
	*
	* I.e. the decimal number is "value * 10^(-scale)". */
	class TBC_EXPORT IDecimal
	{
	public:
		/*! Destructor */
		virtual ~IDecimal(){};

		/*! Set the value and scale of the decimal. Value is provided as an 8 bit unsigned integer.
		*
		* @param value				The source value to set.
		* @param scale				The source scale to set.
		* @param changeBitLength	The decimal value's bit length is adjusted to fit the provided data type size if 'true'.
		*
		* @note	Set will fail with status code BadNumericOverflow if changeBitLength is 'false' and
		*		the current value's bit length is too small for the provided value. */
		virtual EnumStatusCode set(OTUInt8 value, OTInt16 scale, OTBoolean changeBitLength = true) = 0;

		/*! Get the value and scale of the decimal. Value is converted to an 8 bit unsigned integer.
		*
		* @param value				The target variable to store the value.
		* @param scale				The target variable to store the scale.
		*
		* @note	Get will fail with status code BadNumericOverflow if the current value
		*		doesn't fit the target variable or if it's negative. */
		virtual EnumStatusCode get(OTUInt8& value, OTInt16& scale) const = 0;

		/*! Set the value and scale of the decimal. Value is provided as a 16 bit unsigned integer.
		*
		* @param value				The source value to set.
		* @param scale				The source scale to set.
		* @param changeBitLength	The decimal value's bit length is adjusted to fit the provided data type size if 'true'.
		*
		* @note	Set will fail with status code BadNumericOverflow if changeBitLength is 'false' and
		*		the current value's bit length is too small for the provided value. */
		virtual EnumStatusCode set(OTUInt16 value, OTInt16 scale, OTBoolean changeBitLength = true) = 0;

		/*! Get the value and scale of the decimal. Value is converted to a 16 bit unsigned integer.
		*
		* @param value				The target variable to store the value.
		* @param scale				The target variable to store the scale.
		*
		* @note	Get will fail with status code BadNumericOverflow if the current value
		*		doesn't fit the target variable or if it's negative. */
		virtual EnumStatusCode get(OTUInt16& value, OTInt16& scale) const = 0;

		/*! Set the value and scale of the decimal. Value is provided as a 32 bit unsigned integer.
		*
		* @param value				The source value to set.
		* @param scale				The source scale to set.
		* @param changeBitLength	The decimal value's bit length is adjusted to fit the provided data type size if 'true'.
		*
		* @note	Set will fail with status code BadNumericOverflow if changeBitLength is 'false' and
		*		the current value's bit length is too small for the provided value. */
		virtual EnumStatusCode set(OTUInt32 value, OTInt16 scale, OTBoolean changeBitLength = true) = 0;

		/*! Get the value and scale of the decimal. Value is converted to a 32 bit unsigned integer.
		*
		* @param value				The target variable to store the value.
		* @param scale				The target variable to store the scale.
		*
		* @note	Get will fail with status code BadNumericOverflow if the current value
		*		doesn't fit the target variable or if it's negative. */
		virtual EnumStatusCode get(OTUInt32& value, OTInt16& scale) const = 0;

		/*! Set the value and scale of the decimal. Value is provided as a 64 bit unsigned integer.
		*
		* @param value				The source value to set.
		* @param scale				The source scale to set.
		* @param changeBitLength	The decimal value's bit length is adjusted to fit the provided data type size if 'true'.
		*
		* @note	Set will fail with status code BadNumericOverflow if changeBitLength is 'false' and
		*		the current value's bit length is too small for the provided value. */
		virtual EnumStatusCode set(OTUInt64 value, OTInt16 scale, OTBoolean changeBitLength = true) = 0;

		/*! Get the value and scale of the decimal. Value is converted to a 64 bit unsigned integer.
		*
		* @param value				The target variable to store the value.
		* @param scale				The target variable to store the scale.
		*
		* @note	Get will fail with status code BadNumericOverflow if the current value
		*		doesn't fit the target variable or if it's negative. */
		virtual EnumStatusCode get(OTUInt64& value, OTInt16& scale) const = 0;

		/*! Set the value and scale of the decimal. Value is provided as an 8 bit signed integer.
		*
		* @param value				The source value to set.
		* @param scale				The source scale to set.
		* @param changeBitLength	The decimal value's bit length is adjusted to fit the provided data type size if 'true'.
		*
		* @note	Set will fail with status code BadNumericOverflow if changeBitLength is 'false' and
		*		the current value's bit length is too small for the provided value. */
		virtual EnumStatusCode set(OTInt8 value, OTInt16 scale, OTBoolean changeBitLength = true) = 0;

		/*! Get the value and scale of the decimal. Value is converted to an 8 bit signed integer.
		*
		* @param value				The target variable to store the value.
		* @param scale				The target variable to store the scale.
		*
		* @note	Get will fail with status code BadNumericOverflow if the current value
		*		doesn't fit the target variable. */
		virtual EnumStatusCode get(OTInt8& value, OTInt16& scale) const = 0;

		/*! Set the value and scale of the decimal. Value is provided as a 16 bit signed integer.
		*
		* @param value				The source value to set.
		* @param scale				The source scale to set.
		* @param changeBitLength	The decimal value's bit length is adjusted to fit the provided data type size if 'true'.
		*
		* @note	Set will fail with status code BadNumericOverflow if changeBitLength is 'false' and
		*		the current value's bit length is too small for the provided value. */
		virtual EnumStatusCode set(OTInt16 value, OTInt16 scale, OTBoolean changeBitLength = true) = 0;

		/*! Get the value and scale of the decimal. Value is converted to a 16 bit signed integer.
		*
		* @param value				The target variable to store the value.
		* @param scale				The target variable to store the scale.
		*
		* @note	Get will fail with status code BadNumericOverflow if the current value
		*		doesn't fit the target variable. */
		virtual EnumStatusCode get(OTInt16& value, OTInt16& scale) const = 0;

		/*! Set the value and scale of the decimal. Value is provided as a 32 bit signed integer.
		*
		* @param value				The source value to set.
		* @param scale				The source scale to set.
		* @param changeBitLength	The decimal value's bit length is adjusted to fit the provided data type size if 'true'.
		*
		* @note	Set will fail with status code BadNumericOverflow if changeBitLength is 'false' and
		*		the current value's bit length is too small for the provided value. */
		virtual EnumStatusCode set(OTInt32 value, OTInt16 scale, OTBoolean changeBitLength = true) = 0;

		/*! Get the value and scale of the decimal. Value is converted to a 32 bit signed integer.
		*
		* @param value				The target variable to store the value.
		* @param scale				The target variable to store the scale.
		*
		* @note	Get will fail with status code BadNumericOverflow if the current value
		*		doesn't fit the target variable. */
		virtual EnumStatusCode get(OTInt32& value, OTInt16& scale) const = 0;

		/*! Set the value and scale of the decimal. Value is provided as a 64 bit signed integer.
		*
		* @param value				The source value to set.
		* @param scale				The source scale to set.
		* @param changeBitLength	The decimal value's bit length is adjusted to fit the provided data type size if 'true'.
		*
		* @note	Set will fail with status code BadNumericOverflow if changeBitLength is 'false' and
		*		the current value's bit length is too small for the provided value. */
		virtual EnumStatusCode set(OTInt64 value, OTInt16 scale, OTBoolean changeBitLength = true) = 0;

		/*! Get the value and scale of the decimal. Value is converted to a 64 bit signed integer.
		*
		* @param value				The target variable to store the value.
		* @param scale				The target variable to store the scale.
		*
		* @note	Get will fail with status code BadNumericOverflow if the current value
		*		doesn't fit the target variable. */
		virtual EnumStatusCode get(OTInt64& value, OTInt16& scale) const = 0;

		/*! Set the value and scale of the decimal. Value is provided as a ByteString.
		*
		* @param value				The source value to set.
		* @param scale				The source scale to set.
		*
		* @note	This method will set the raw binary data of the decimal's unscaled value.
		*		The application has to provide it in the correct binary format:
		*		A 2's complement signed integer with least significant byte first.
		*		Value's bit length is always changed. */
		virtual EnumStatusCode set(const ByteString& value, OTInt16 scale) = 0;

		/*! Get the value and scale of the decimal. Decimal's raw unscaled value is returned in a ByteString.
		*
		* @param value				The target variable to store the value.
		* @param scale				The target variable to store the scale. */
		virtual EnumStatusCode get(ByteString& value, OTInt16& scale) const = 0;

		/*! Set the value and scale of the decimal from strings.
		* Both value and scale should be base 10 representations of integers.
		*
		* @param value				The source value to set.
		* @param scale				The source scale to set.
		* @param changeBitLength	The decimal value's bit length is adjusted to fit the provided value if 'true'.
		*
		* @note	Set will fail with status code BadNumericOverflow if changeBitLength is 'false' and
		*		the current value's bit length is too small for the provided value. */
		virtual EnumStatusCode set(const tstring& value, const tstring& scale, OTBoolean changeBitLength = true) = 0;

		/*! Get the value and scale of the decimal as strings.
		* Both value and scale will be returned as base 10 representations of integers.
		*
		* @param value				The target value to set.
		* @param scale				The target scale to set. */
		virtual EnumStatusCode get(tstring& value, tstring& scale) const = 0;

		/*! Get the size (in bytes) of the decimal's value.
		*
		* @note	When the decimal is initialized by a setter from an unsigned source value, getValueSize() will return
		*		1 byte more than the base data type's size. Since 2's complement requires 1 bit more to store full range of unsigned types,
		*		the setter will use this extra byte when a change of bit length is allowed. */
		virtual OTInt32 getValueSize() const = 0;

		/*! Verify the sign of the stored value. Returns 'true' when the value < 0. */
		virtual OTBoolean isNegative() const = 0;

		/*! Set the value and scale of the decimal to 0. Value's bit length is preserved. */
		virtual void setZero() = 0;

		/*! Get the scale of the decimal. */
		virtual OTInt16 getScale() const = 0;

		/*! Set the scale of the decimal. */
		virtual EnumStatusCode setScale(OTInt16 scale) = 0;

		/*! Copies the content of @p cp to this instance */
		virtual EnumStatusCode set(const IDecimal* cp) = 0;
		/*! @overload */
		virtual EnumStatusCode set(const IDecimal& cp) = 0;

		/*! Compares this instance to the given @p other instance.
		*
		* @li Returns <0, if "this < other"
		* @li Returns 0, if "this == other"
		* @li Returns >0, if "this > other"
		* @note	A binary comparison is performed between the two instances, the decimals compare equal when the scales are equal and
		*		the values are same in bit size and binary equal. */
		virtual int compare(const IDecimal* other) const = 0;
		/*! @overload */
		virtual int compare(const IDecimal& other) const = 0;

		/*! Compares two instances for equality */
		virtual bool operator==(const IDecimal& other) const = 0;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IDecimal& other) const = 0;

		/*! Returns a string representation of the Decimal for debugging purposes */
		virtual tstring toString() const = 0;

		/*! For internal use only */
		virtual void* getInternHandle() const = 0;
	}; // end IDecimal
} // end namespace

#endif	// IDECIMAL_H
