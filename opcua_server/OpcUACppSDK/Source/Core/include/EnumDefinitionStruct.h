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

#ifndef _EnumDefinitionStruct_H_
#define _EnumDefinitionStruct_H_

#include "EnumFieldStruct.h"

#ifdef SOOS_WINDOWS
#pragma pack(push,8)
#endif

//lint -sem(EnumDefinitionStruct::clear, cleanup)
//lint -sem(EnumDefinitionStruct::init, initializer)
typedef struct TBCORE_IF EnumDefinitionStruct
{
	EnumFieldStruct* pFields;
	OTUInt32 fields_count;


	EnumDefinitionStruct();
	EnumDefinitionStruct(const EnumDefinitionStruct&);

	EnumStatusCode set(const EnumDefinitionStruct&);
	EnumStatusCode get(EnumDefinitionStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_EnumDefinition&);
	EnumStatusCode get(OpcUa_EnumDefinition&, const std::vector<tstring>& preferredLocales) const;

	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject, const std::vector<tstring>& preferredLocales) const;
#endif

	const EnumDefinitionStruct& operator=(const EnumDefinitionStruct& cp);
	bool operator==(const EnumDefinitionStruct& toCompare) const;
	bool operator!=(const EnumDefinitionStruct& toCompare) const;
	bool operator<(const EnumDefinitionStruct& toCompare) const;
	bool operator>(const EnumDefinitionStruct& toCompare) const;

	int compare(const EnumDefinitionStruct* pTarget) const;

	tstring toString() const;

	void clear(void);
	~EnumDefinitionStruct(void);
} EnumDefinitionStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
