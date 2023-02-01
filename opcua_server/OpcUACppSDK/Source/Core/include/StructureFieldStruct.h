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

#ifndef STRUCTUREFIELDSTRUCT_H
#define STRUCTUREFIELDSTRUCT_H

#include "LocalizedTextStruct.h"
#include "NodeIdStruct.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

//lint -sem(StructureFieldStruct::clear, cleanup)
//lint -sem(StructureFieldStruct::init, initializer)
typedef struct TBCORE_IF StructureFieldStruct
{
	OTChar* pName;

	LocalizedTextStruct description;


	NodeIdStruct dataType;

	OTInt32 valueRank;

	OTUInt32* pArrayDimensions;
	OTUInt32 arrayDimensions_count;

	OTUInt32 maxStringLength;

	OTBoolean isOptional;


	StructureFieldStruct();
	StructureFieldStruct(const StructureFieldStruct&);

	EnumStatusCode set(const StructureFieldStruct&);
	EnumStatusCode get(StructureFieldStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_StructureField&);
	EnumStatusCode get(OpcUa_StructureField&, const std::vector<tstring>& preferredLocales) const;

	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject, const std::vector<tstring>& preferredLocales) const;
#endif

	const StructureFieldStruct& operator=(const StructureFieldStruct& cp);
	bool operator==(const StructureFieldStruct& toCompare) const;
	bool operator!=(const StructureFieldStruct& toCompare) const;
	bool operator<(const StructureFieldStruct& toCompare) const;
	bool operator>(const StructureFieldStruct& toCompare) const;

	int compare(const StructureFieldStruct* pTarget) const;

	void clear();

	// access functions for members
	const OTChar* getName() const;
	EnumStatusCode setName(const OTChar* pNewName);
	EnumStatusCode setArrayDimensions(OTUInt32 noOfArrayDimensions, const OTUInt32* pNewArrayDimensions);
	EnumStatusCode getArrayDimensions(OTUInt32* pNoOfArrayDimensions, OTUInt32** ppArrayDimensions) const;
#ifdef OT_COMPILATION
	std::vector<OTUInt32> getArrayDimensions() const;
#endif

	tstring toString() const;

	~StructureFieldStruct();
} StructureFieldStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif	// STRUCTUREFIELDSTRUCT_H
