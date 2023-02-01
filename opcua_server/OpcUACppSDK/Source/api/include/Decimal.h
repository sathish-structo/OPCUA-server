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

#ifndef DECIMAL_H
#define DECIMAL_H

#include "IDecimal.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

namespace SoftingOPCToolbox5
{
	/*! The Decimal data type describes a high-precision signed decimal number.
	*
	* It consists of an arbitrary precision integer unscaled value and an integer scale.
	* The scale is the inverse power of ten that is applied to the unscaled value.
	*
	* I.e. the decimal number is "value * 10^(-scale)". */
	class TBC_EXPORT Decimal
		: public IDecimal
	{
	public:
		/*! Default constructor.
		*
		* @note	The decimal is initialized with a 0 bit length value. The methods used to
		*		set the value without changing the bit length will fail on this new object. */
		Decimal();
		/*! Constructs a copy of the given instance @p cp */
		Decimal(const IDecimal* cp);
		/*! @overload */
		Decimal(const IDecimal& cp);
		/*! @overload */
		Decimal(const Decimal& cp);

		/*! Destructor */
		virtual ~Decimal();

		/*! Assigns the contents of @p cp to this instance */
		Decimal& operator=(const IDecimal* cp);
		/*! @overload */
		Decimal& operator=(const IDecimal& cp);
		/*! @overload */
		Decimal& operator=(const Decimal& cp);

		/*! @see IDecimal::compare(IDecimal*) const */
		virtual int compare(const IDecimal* other) const;
		/*! @overload */
		virtual int compare(const IDecimal& other) const;

		/*! Compares two instances for equality */
		virtual bool operator==(const IDecimal& other) const;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IDecimal& other) const;

		/*! Copies the content of @p cp to this instance */
		virtual EnumStatusCode set(const IDecimal* pSrc);
		/*! @overload */
		virtual EnumStatusCode set(const IDecimal& pSrc);

		/*! @see IDecimal::set(OTUInt8, OTInt16, OTBoolean) */
		virtual EnumStatusCode set(OTUInt8 value, OTInt16 scale, OTBoolean changeBitLength = true);
		/*! @see IDecimal::get(OTUInt8&, OTInt16&) const */
		virtual EnumStatusCode get(OTUInt8& value, OTInt16& scale) const;

		/*! @see IDecimal::set(OTUInt16, OTInt16, OTBoolean) */
		virtual EnumStatusCode set(OTUInt16 value, OTInt16 scale, OTBoolean changeBitLength = true);
		/*! @see IDecimal::get(OTUInt16&, OTInt16&) const */
		virtual EnumStatusCode get(OTUInt16& value, OTInt16& scale) const;

		/*! @see IDecimal::set(OTUInt32, OTInt16, OTBoolean) */
		virtual EnumStatusCode set(OTUInt32 value, OTInt16 scale, OTBoolean changeBitLength = true);
		/*! @see IDecimal::get(OTUInt32&, OTInt16&) const */
		virtual EnumStatusCode get(OTUInt32& value, OTInt16& scale) const;

		/*! @see IDecimal::set(OTUInt64, OTInt16, OTBoolean) */
		virtual EnumStatusCode set(OTUInt64 value, OTInt16 scale, OTBoolean changeBitLength = true);
		/*! @see IDecimal::get(OTUInt64&, OTInt16&) const */
		virtual EnumStatusCode get(OTUInt64& value, OTInt16& scale) const;

		/*! @see IDecimal::set(OTInt8, OTInt16, OTBoolean) */
		virtual EnumStatusCode set(OTInt8 value, OTInt16 scale, OTBoolean changeBitLength = true);
		/*! @see IDecimal::get(OTInt8&, OTInt16&) const */
		virtual EnumStatusCode get(OTInt8& value, OTInt16& scale) const;

		/*! @see IDecimal::set(OTInt16, OTInt16, OTBoolean) */
		virtual EnumStatusCode set(OTInt16 value, OTInt16 scale, OTBoolean changeBitLength = true);
		/*! @see IDecimal::get(OTInt16&, OTInt16&) const */
		virtual EnumStatusCode get(OTInt16& value, OTInt16& scale) const;

		/*! @see IDecimal::set(OTInt32, OTInt16, OTBoolean) */
		virtual EnumStatusCode set(OTInt32 value, OTInt16 scale, OTBoolean changeBitLength = true);
		/*! @see IDecimal::get(OTInt32&, OTInt16&) const */
		virtual EnumStatusCode get(OTInt32& value, OTInt16& scale) const;

		/*! @see IDecimal::set(OTInt64, OTInt16, OTBoolean) */
		virtual EnumStatusCode set(OTInt64 value, OTInt16 scale, OTBoolean changeBitLength = true);
		/*! @see IDecimal::get(OTInt64&, OTInt16&) const */
		virtual EnumStatusCode get(OTInt64& value, OTInt16& scale) const;

		/*! @see IDecimal::set(const ByteString&, OTInt16) */
		virtual EnumStatusCode set(const ByteString& value, OTInt16 scale);
		/*! @see IDecimal::get(ByteString&, OTInt16&) const */
		virtual EnumStatusCode get(ByteString& value, OTInt16& scale) const;

		/*! @see IDecimal::set(const tstring&, const tstring&, OTBoolean) */
		virtual EnumStatusCode set(const tstring& value, const tstring& scale, OTBoolean changeBitLength = true);

		/*! @see IDecimal::get(tstring&, tstring&) const */
		virtual EnumStatusCode get(tstring& value, tstring& scale) const;

		/*! @see IDecimal::getValueSize() const */
		virtual OTInt32 getValueSize() const;

		/*! @see IDecimal::isNegative() const */
		virtual OTBoolean isNegative() const;

		/*! @see IDecimal::setZero() */
		virtual void setZero();

		/*! @see IDecimal::getScale() const */
		virtual OTInt16 getScale() const;

		/*! @see IDecimal::setScale() */
		virtual EnumStatusCode setScale(OTInt16 scale);

		/* @see IDecimal::toString const */
		virtual tstring toString() const;

		/*! Initialize the decimal to the specified bit length of the unscaled value (8 * byteSize).
		* It will initialize the scale and value if provided. By default the scale and value are set to 0.
		* @param byteSize			The size of the value buffer (in bytes).
		* @param value				The source unscaled value to set.
		* @param scale				The source scale to set.
		*
		* @note	This method will set the raw binary data of the decimal's unscaled value.
		*		The application has to provide it in the correct binary format:
		*		A 2's complement signed integer with least significant byte first. */
		EnumStatusCode init(OTInt32 byteSize, const OTUInt8* value = NULL, OTInt16 scale = (OTInt16)0);

		/*! Clears the contents of this object to enable its re-use. */
		void clear();

		/*! For internal use only */
		virtual void* getInternHandle() const;

	private:
		DecimalStruct m_content;
	};
} // end namespace

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif	// DECIMAL_H
