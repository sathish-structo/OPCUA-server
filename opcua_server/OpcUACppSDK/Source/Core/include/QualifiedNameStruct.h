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

#ifndef __QUALIFIEDNAMESTRUCT_H_
#define __QUALIFIEDNAMESTRUCT_H_

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif


/*! A name with an additional namespace index. This type of name is used for specifying BrowseNames (that
* must contain the namespace). */
//lint -sem(QualifiedNameStruct::clear, cleanup)
//lint -sem(QualifiedNameStruct::init, initializer)
typedef struct TBCORE_IF QualifiedNameStruct
{
	/*! String containing the name without namespaceindex. */
	OTChar* pName;

	/*! The namespaceindex to use building a BrowsePath. */
	unsigned short namespaceIndex;


	QualifiedNameStruct();
	EnumStatusCode setName(const OTChar* pValue);

	EnumStatusCode set(const QualifiedNameStruct&);
	EnumStatusCode get(QualifiedNameStruct&) const;
#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_QualifiedName&);
	EnumStatusCode get(OpcUa_QualifiedName&) const;
#endif	// OT_COMPILATION
	QualifiedNameStruct(const OTChar* name, unsigned short namespecIndex);
	void setNameAndIndex_API(const OTChar* name, unsigned short index);
	bool isNull() const;
	tstring toString() const;
	/*! Accepts a string of the form "[<NamespaceIndex>:]<Name>".
	*
	* @li The optional "ns" before the colon must consist of 1 to any numbers, parsable as UInt16.
	* @li The character "&" is used as escape character, every character of "/.<>:#!&" (without the quotes)
	*     within the name must be escaped (and only those are allowed to be escaped).
	*
	* @note This BrowseName representation is defined as part of the BNF for RelativePath.
	*
	* @note This parser shall NOT be used for the value cast when casting from string to QualifiedName,
	*       the value cast specifies a different logic */ 
	EnumStatusCode parse(const tstring& browseNameString);
	QualifiedNameStruct(const QualifiedNameStruct& cp);

	const QualifiedNameStruct& operator=(const QualifiedNameStruct& cp);
	bool operator==(const QualifiedNameStruct& toCompare) const;
	bool operator!=(const QualifiedNameStruct& toCompare) const;
	bool operator<(const QualifiedNameStruct& toCompare) const;
	bool operator>(const QualifiedNameStruct& toCompare) const;

	int compare(const QualifiedNameStruct* pTarget) const;

	void clear(void);
	~QualifiedNameStruct(void);

}QualifiedNameStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
