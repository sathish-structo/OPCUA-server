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

#ifndef _FieldTargetDataTypeStruct_H_
#define _FieldTargetDataTypeStruct_H_

#include "GuidStruct.h"
#include "NumericRangeStruct.h"
#include "NodeIdStruct.h"
#include "Enums.h"
#include "ValueStruct.h"

#ifdef SOOS_WINDOWS
#pragma pack(push,8)
#endif

//lint -sem(FieldTargetDataTypeStruct::clear, cleanup)
//lint -sem(FieldTargetDataTypeStruct::init, initializer)
typedef struct TBCORE_IF FieldTargetDataTypeStruct
{
	GuidStruct dataSetFieldId;

	NumericRangeStruct receiverIndexRange;

	NodeIdStruct targetNodeId;

	EnumAttributeId attributeId;

	NumericRangeStruct writeIndexRange;

	EnumOverrideValueHandling overrideValueHandling;

	ValueStruct overrideValue;


	FieldTargetDataTypeStruct();
	FieldTargetDataTypeStruct(const FieldTargetDataTypeStruct&);

	EnumStatusCode set(const FieldTargetDataTypeStruct&);
	EnumStatusCode get(FieldTargetDataTypeStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_FieldTargetDataType&);
	EnumStatusCode get(OpcUa_FieldTargetDataType&, const std::vector<tstring>&) const;

	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject, const std::vector<tstring>&) const;
#endif

	const FieldTargetDataTypeStruct& operator=(const FieldTargetDataTypeStruct& cp);
	bool operator==(const FieldTargetDataTypeStruct& toCompare) const;
	bool operator!=(const FieldTargetDataTypeStruct& toCompare) const;
	bool operator<(const FieldTargetDataTypeStruct& toCompare) const;
	bool operator>(const FieldTargetDataTypeStruct& toCompare) const;

	int compare(const FieldTargetDataTypeStruct* pTarget) const;

	tstring toString() const;

	void clear(void);
	~FieldTargetDataTypeStruct(void);
} FieldTargetDataTypeStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
