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

#ifndef DECIMALSTRUCT_H
#define DECIMALSTRUCT_H

#include "ByteStringStruct.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

/*!
* @brief A high-precision signed decimal number.
*/
//lint -sem(DecimalStruct::clear, cleanup)
//lint -sem(DecimalStruct::init, initializer)
typedef struct TBCORE_IF DecimalStruct
{
	/*! A signed integer representing the power of ten used to scale the value. */
	OTInt16 m_scale;

	/*! A 2-complement signed integer representing the unscaled value.
	The integer is encoded with the least significant byte first.*/
	ByteStringStruct m_value;

	DecimalStruct();
	DecimalStruct(const DecimalStruct& cp);
	~DecimalStruct();
	
	// Store value of common integer types, get value as common integer types.
	// * If changeBitLength is false the setters will try to fit the value in the current bit size,
	//   fail if overflow.
	// * If changeBitLength is true the setters will clear contents and initialize the Decimal
	//   to a suitable bit length.
	// Getters will fail if the current value doesn't fit in the target variable.
	// Setters will always set the unscaled value from provided source, scale will be set to 0.
	// Getters will return the unscaled value if scale is requested.
	EnumStatusCode set(OTUInt8 value, OTBoolean changeBitLength = true);
	EnumStatusCode get(OTUInt8& value, OTInt16* scale = NULL) const;
	EnumStatusCode set(OTUInt16 value, OTBoolean changeBitLength = true);
	EnumStatusCode get(OTUInt16& value, OTInt16* scale = NULL) const;
	EnumStatusCode set(OTUInt32 value, OTBoolean changeBitLength = true);
	EnumStatusCode get(OTUInt32& value, OTInt16* scale = NULL) const;
	EnumStatusCode set(OTUInt64 value, OTBoolean changeBitLength = true);
	EnumStatusCode get(OTUInt64& value, OTInt16* scale = NULL) const;
	EnumStatusCode set(OTInt8 value, OTBoolean changeBitLength = true);
	EnumStatusCode get(OTInt8& value, OTInt16* scale = NULL) const;
	EnumStatusCode set(OTInt16 value, OTBoolean changeBitLength = true);
	EnumStatusCode get(OTInt16& value, OTInt16* scale = NULL) const;
	EnumStatusCode set(OTInt32 value, OTBoolean changeBitLength = true);
	EnumStatusCode get(OTInt32& value, OTInt16* scale = NULL) const;
	EnumStatusCode set(OTInt64 value, OTBoolean changeBitLength = true);
	EnumStatusCode get(OTInt64& value, OTInt16* scale = NULL) const;
	EnumStatusCode set(const tstring& value, const tstring& scale, OTBoolean changeBitLength = true);
	EnumStatusCode get(tstring& value, tstring& scale) const;

	// Set value and size to 0, current bit length is preserved.
	void setZero();

	// Check if stored value negative
	OTBoolean isNegative() const;

	EnumStatusCode set(const DecimalStruct&);
	EnumStatusCode get(DecimalStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_Decimal&);
	EnumStatusCode get(OpcUa_Decimal&) const;
	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject) const;
#endif
	
	const DecimalStruct& operator=(const DecimalStruct& cp);
	bool operator==(const DecimalStruct& toCompare) const;
	bool operator!=(const DecimalStruct& toCompare) const;
	bool operator<(const DecimalStruct& toCompare) const;
	bool operator>(const DecimalStruct& toCompare) const;

	int compare(const DecimalStruct* pTarget) const;
	tstring toString() const;

	EnumStatusCode init(OTInt32 byteSize, const OTUInt8* value = NULL, OTInt16 scale = (OTInt16)0);
	void clear(void);

	EnumStatusCode prepareSet(OTUInt32 minByteLength, OTBoolean changeBitLength);
}DecimalStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif	// DECIMALSTRUCT_H
