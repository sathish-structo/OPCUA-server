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

#ifndef __RELATIVEPATHSTRUCT_H_
#define __RELATIVEPATHSTRUCT_H_

#include "RelativePathElementStruct.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

/*! Structure that defines a sequence of References and BrowseNames to follow. */
//lint -sem(RelativePathStruct::clear, cleanup)
//lint -sem(RelativePathStruct::init, initializer)
typedef struct TBCORE_IF RelativePathStruct
{
	/*! A sequence of References and BrowseNames to follow.
	* Each element in the sequence is processed by finding the targets and then using those targets as the starting
	* nodes for the next element. The targets of the final element are the target of the RelativePath. */
	OTUInt32 elements_count;
	RelativePathElementStruct* pElements;


	RelativePathStruct();
	RelativePathStruct(const RelativePathStruct& other);

	EnumStatusCode addElement(const RelativePathElementStruct* pRelativePathElement);

	EnumStatusCode set(const RelativePathStruct&);
	EnumStatusCode get(RelativePathStruct&) const;
#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_RelativePath&);
	EnumStatusCode get(OpcUa_RelativePath&) const;
	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject) const;

	/*! Accepts a string which is a sequence of parsable RelativePathElement strings.
	*
	* For example "/1:First.2:Second<Organizes>0:Acknowledge".
	*
	* @li It uses the characters '/', '.' and '<' to split the string into the separate RelativePathElements,
	*     those 3 characters clearly identify the start of every RelativePathElement.
	* @li The character "&" is used as escape character, every character of "/.<>:#!&" (without the quotes)
	*     within the <ReferenceName> or <TargetName> must be escaped (and only those are allowed to be escaped).
	*
	* @note The ReferenceType of the RelativePathElements are NodeIds while the BNF provides only QualifiedNames.
	*       The output parameter pathElementReferenceTypes returns the referenceNames as QualifiedNames and the
	*       caller must make sure to find and patch the RelativePathElements with the correct NodeIds.
	*       RelativePathElements need to be patched by the related NodeIds.
	*       If the simple references '/' or '.' are used, then the NodeId is already assigned during parse and
	*       a NULL-QualifiedName is returned for this element.
	* @note The TargetName can be empty, but the ReferenceName must be set.
	* @note The BNF syntax for RelativePath is defined in the specification. */
	EnumStatusCode parse(const tstring& relativePathString, std::vector<QualifiedNameStruct>& pathElementReferenceTypes);

#endif	// OT_COMPILATION

	const RelativePathStruct& operator=(const RelativePathStruct& cp);
	bool operator==(const RelativePathStruct& toCompare) const;
	bool operator!=(const RelativePathStruct& toCompare) const;
	bool operator<(const RelativePathStruct& toCompare) const;
	bool operator>(const RelativePathStruct& toCompare) const;

	int compare(const RelativePathStruct* pTarget) const;

	void clear();
	~RelativePathStruct();

}RelativePathStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
