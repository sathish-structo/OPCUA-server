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

#ifndef EXPANDEDNODEID_H
#define EXPANDEDNODEID_H

#include "IExpandedNodeId.h"
#include "NodeId.h"
#include "InnerNodeId.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

namespace SoftingOPCToolbox5
{
	/*! The ExpandedNodeId data type extends the NodeId data type by elements that allow addressing across server borders.
	* ExpandedNodeIds allow the namespace to be specified explicitly as a string or with an index in the server's
	* namespace table. */
	class TBC_EXPORT ExpandedNodeId
		: public IExpandedNodeId
	{
	public:
		/*! Default constructor */
		ExpandedNodeId();
		/*! Constructs an ExpandedNodeId with a numeric identifier.
		*
		* @param namespaceIndex		The index in the server's namespace table
		* @param identifier			The numeric identifier */
		ExpandedNodeId(OTUInt16 namespaceIndex, OTUInt32 identifier);
		/*! Constructs an ExpandedNodeId with a string identifier.
		*
		* @param namespaceIndex		The index in the server's namespace table
		* @param identifier			The string identifier */
		ExpandedNodeId(OTUInt16 namespaceIndex, const tstring& identifier);
		/*! Constructs a copy of the given instance */
		ExpandedNodeId(const IExpandedNodeId*);
		/*! @overload */
		ExpandedNodeId(const IExpandedNodeId&);
		/*! @overload */
		ExpandedNodeId(const ExpandedNodeId&);
		/*! @overload */
		ExpandedNodeId(const INodeId*);
		/*! @overload */
		ExpandedNodeId(const INodeId&);

		/*! Destructor */
		virtual ~ExpandedNodeId();

		/*! Assigns the contents of @p cp to this instance */
		ExpandedNodeId& operator=(const IExpandedNodeId*);
		/*! @overload */
		ExpandedNodeId& operator=(const IExpandedNodeId&);
		/*! @overload */
		ExpandedNodeId& operator=(const ExpandedNodeId&);

		/* see IExpandedNodeId::set */
		virtual EnumStatusCode set(const IExpandedNodeId* pSrc);
		/*! @overload */
		virtual EnumStatusCode set(const IExpandedNodeId& pSrc);
		/*! @overload */
		virtual EnumStatusCode set(const INodeId* pSrc);
		/*! @overload */
		virtual EnumStatusCode set(const INodeId& pSrc);

		/* see IExpandedNodeId::compare */
		virtual int compare(const IExpandedNodeId*) const;
		/*! @overload */
		virtual int compare(const IExpandedNodeId&) const;
		/*! @overload */
		virtual int compare(const INodeId* other) const;
		/*! @overload */
		virtual int compare(const INodeId& other) const;

		/*! Compares two instances for equality */
		virtual bool operator==(const IExpandedNodeId& other) const;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IExpandedNodeId& other) const;
		/*! Compares two instances for equality */
		virtual bool operator==(const INodeId& other) const;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const INodeId& other) const;

		/* see IExpandedNodeId::getServerIndex */
		virtual OTUInt32 getServerIndex() const;
		/* see IExpandedNodeId::setServerIndex */
		virtual void setServerIndex(OTUInt32 value);

		/* see IExpandedNodeId::getNamespaceUri */
		virtual tstring getNamespaceUri() const;
		/* see IExpandedNodeId::setNamespaceUri */
		virtual void setNamespaceUri(const tstring& value);

		/* see IExpandedNodeId::getNamespaceIndex */
		virtual OTUInt16 getNamespaceIndex() const;
		/* see IExpandedNodeId::setNamespaceIndex */
		virtual void setNamespaceIndex(OTUInt16 value);

		/* see IExpandedNodeId::getIdentifierType */
		virtual EnumIdentifierType getIdentifierType() const;

		/* see IExpandedNodeId::getIdentifier */
		virtual Value getIdentifier() const;
		/* see INodeId::getNumericIdentifier */
		virtual EnumStatusCode getNumericIdentifier(OTUInt32* pId) const;
		/* see INodeId::getStringIdentifier */
		virtual EnumStatusCode getStringIdentifier(tstring* pId) const;
		/* see INodeId::getGuidIdentifier */
		virtual EnumStatusCode getGuidIdentifier(Guid* pId) const;
		/* see INodeId::getOpaqueIdentifier */
		virtual EnumStatusCode getOpaqueIdentifier(ByteString* pId) const;

		/* see IExpandedNodeId::setNumeric */
		virtual EnumStatusCode setNumeric(OTUInt16 namespaceIndex, OTUInt32 identifier);
		/* see IExpandedNodeId::setString */
		virtual EnumStatusCode setString(OTUInt16 namespaceIndex, const tstring& identifier);
		/* see IExpandedNodeId::setGuid */
		virtual EnumStatusCode setGuid(OTUInt16 namespaceIndex, const IGuid* identifier);
		/* see IExpandedNodeId::setOpaque */
		virtual EnumStatusCode setOpaque(OTUInt16 namespaceIndex, const ByteString& identifier);

		/* see IExpandedNodeId::isNull */
		virtual bool isNull() const;
		/* see IExpandedNodeId::setNull */
		virtual EnumStatusCode setNull();

		/* see IExpandedNodeId::getNodeId */
		virtual NodeId getNodeId() const;

		/* see IExpandedNodeId::toString */
		virtual tstring toString() const;

		/*! Parses the given string and constructs an ExpandedNodeId from it.
		*
		* The string must conform to the following syntax:
		* svr=&lt;serverindex&gt;;ns=&lt;namespaceindex&gt;;&lt;type&gt;=&lt;value&gt;
		* or
		* svr=&lt;serverindex&gt;;nsu=&lt;uri&gt;;&lt;type&gt;=&lt;value&gt;
		*
		* @li serverindex		The ServerIndex formatted as a base 10 number. If the ServerIndex is 0 then the entire 'svr=0;' clause shall be omitted.
		* @li namespaceindex	The NamespaceIndex formatted as a base 10 number. If the NamespaceIndex is 0 then the entire 'ns=0;' clause shall be omitted.
		*						The NamespaceIndex shall not be present if the URI is present.
		* @li uri				The NamespaceUri formatted as a string. Any reserved characters in the URI shall be replaced with a '%%' (a single percent sign) followed by its 8
		*						bit ANSI value encoded as two hexadecimal digits (case insensitive). For example, the character ';' would be replaced by '%3B'.
		*						The reserved characters are ';' and '%%' (a single percent sign).
		*						If the NamespaceUri is null or empty, then 'nsu=;' clause shall be omitted.
		* @li type				A flag that specifies the identifier type ("i": numeric, "s": string, "g": GUID, "b": opaque)
		* @li value				The Identifier encoded as string. For identifiers of type GUID, the actual value must be enclosed
		*						by curly braces (e.g. "{09087e75-8e5e-499b-954f-f2a9603db28a}").
		* In case of an error, a null NodeId is returned.
		*
		* Examples:
		* @li i=13
		* @li ns=10;i=1
		* @li ns=10;s=Hello:World
		* @li ns=1;g={09087e75-8e5e-499b-954f-f2a9603db28a}
		* @li nsu=ExampleNamespace;s=Example Identifier
		* @li nsu=Token1%3BToken2;s=Hello:World
		* @li svr=4;ns=4;i=1234
		* @li svr=4;nsu=MyNamespace4;i=1234
		*
		* @note The parser of the ExpandedNodeId is a superset of the parser for the NodeId, thus every valid string representation for a NodeId can be parsed
		*       by this parser.
		* @note Opaque NodeIds have an identifier which is - according to specification - a string encoded by Base64 encoding (e.g. "ns=1;b=M/RbKBsRVkePCePcx24oRA==").
		*       This parse method however expects a "hex-dump" encoded string, where each byte is encoded as two characters [0-9,A-F] (e.g. "ns=1;b=0D16FADC")
		* @see NodeId::parse() */
		static ExpandedNodeId parse(const tstring& identifier);
		/*! @overload */
		static EnumStatusCode parse(const tstring& identifier, IExpandedNodeId* pExpandedNodeId);

		/*! Clears the contents of this object to enable its re-use. */
		void clear();

		/*! For internal use only */
		virtual void* getInternHandle() const;

	private:
		/*! For internal use only */
		void initMembers();

		InnerNodeId nodeId;
		ExpandedNodeIdStruct m_content;
	};
} // end namespace

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif	// EXPANDEDNODEID_H
