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

#ifndef NODEIDSTRUCT_H
#define NODEIDSTRUCT_H

struct ByteStringStruct;
struct GuidStruct;

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

//lint -sem(NodeIdStruct::clear, cleanup)
//lint -sem(NodeIdStruct::init, initializer)
typedef struct TBCORE_IF NodeIdStruct
{
	OTUInt16    namespaceIndex;
	unsigned char  identifierType;
	union _identifier
	{
		OTUInt32       numeric;
		OTChar*           pString;
		GuidStruct*       pGuid;
		ByteStringStruct* pByteString;
	} identifier;

	NodeIdStruct(void);
	NodeIdStruct(const NodeIdStruct& cp);
	NodeIdStruct(EnumNumericNodeId id);

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_NodeId& cp);
	EnumStatusCode get(OpcUa_NodeId& target) const;
#endif

	EnumStatusCode set(const NodeIdStruct& aSource);
	EnumStatusCode get(NodeIdStruct& aDestination) const;

	const NodeIdStruct& operator=(const NodeIdStruct& cp);
	bool operator==(const NodeIdStruct& toCompare) const;
	bool operator!=(const NodeIdStruct& toCompare) const;
	bool operator < (const NodeIdStruct& toCompare) const;
	bool operator > (const NodeIdStruct& toCompare) const;

	int compare(const NodeIdStruct* pTarget, bool ignoreNamespace = false) const;

	EnumStatusCode setNumeric_API(unsigned short nsIndex, OTUInt32 numeric);
	void setNumeric(OTUInt16 nsIndex, OTUInt32 numeric);
	void setNumeric(OTUInt32 numeric);
	EnumStatusCode setString_API(unsigned short nsIndex, const OTChar* string);
	EnumStatusCode setGuid_API(unsigned short nsIndex, const GuidStruct* guid);
	EnumStatusCode setOpaque_API(unsigned short nsIndex, const ByteStringStruct* byteString);


	tstring toString() const;
	/*! Parse only the identifier of a NodeId */
	static EnumStatusCode parseIdentifier(OTUInt16 namespaceIndex, const tstring& identifierString, NodeIdStruct& nodeId);
	static EnumStatusCode parse(const tstring& nodeIdString, NodeIdStruct& nodeId);

	void clear(void);
	~NodeIdStruct(void);

	EnumStatusCode setNull_API(void);
	bool isNull_API(void) const;
	bool isNull() const;

}NodeIdStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif	// NODEIDSTRUCT_H
