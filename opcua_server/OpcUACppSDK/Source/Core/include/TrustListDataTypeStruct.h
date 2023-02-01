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

#ifndef _TrustListDataTypeStruct_H_
#define _TrustListDataTypeStruct_H_

#include "ByteStringStruct.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

//lint -sem(TrustListDataTypeStruct::clear, cleanup)
//lint -sem(TrustListDataTypeStruct::init, initializer)
typedef struct TBCORE_IF TrustListDataTypeStruct
{
	EnumTrustListMasks specifiedLists;

	// certificates shall be in DER format
	ByteStringStruct* pTrustedCertificates;
	OTUInt32 trustedCertificates_count;

	// CRLs shall be in DER format
	ByteStringStruct* pTrustedCrls;
	OTUInt32 trustedCrls_count;

	// certificates shall be in DER format
	ByteStringStruct* pIssuerCertificates;
	OTUInt32 issuerCertificates_count;

	// CRLs shall be in DER format
	ByteStringStruct* pIssuerCrls;
	OTUInt32 issuerCrls_count;


	TrustListDataTypeStruct();
	TrustListDataTypeStruct(const TrustListDataTypeStruct&);

	EnumStatusCode set(const TrustListDataTypeStruct&);
	EnumStatusCode get(TrustListDataTypeStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_TrustListDataType&);
	EnumStatusCode get(OpcUa_TrustListDataType&) const;

	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject) const;
	EnumStatusCode decode(const ByteStringStruct& encodedContent);
	EnumStatusCode encode(ByteStringStruct& encodedContent);
#endif

	const TrustListDataTypeStruct& operator=(const TrustListDataTypeStruct& cp);
	bool operator==(const TrustListDataTypeStruct& toCompare) const;
	bool operator!=(const TrustListDataTypeStruct& toCompare) const;
	bool operator<(const TrustListDataTypeStruct& toCompare) const;
	bool operator>(const TrustListDataTypeStruct& toCompare) const;

	int compare(const TrustListDataTypeStruct* pTarget) const;

	void clear(void);
	// certificates shall be in DER format
	EnumStatusCode setTrustedCertificates(const std::vector<ByteStringStruct> &certificates);
	std::vector<ByteStringStruct> getTrustedCertificates() const;
	// CRLs shall be in DER format
	EnumStatusCode setTrustedCrls(const std::vector<ByteStringStruct>& crls);
	std::vector<ByteStringStruct> getTrustedCrls() const;
	// certificates shall be in DER format
	EnumStatusCode setIssuerCertificates(const std::vector<ByteStringStruct>& certificates);
	std::vector<ByteStringStruct> getIssuerCertificates() const;
	// CRLs shall be in DER format
	EnumStatusCode setIssuerCrls(const std::vector<ByteStringStruct>& crls);
	std::vector<ByteStringStruct> getIssuerCrls() const;
	~TrustListDataTypeStruct(void);

	EnumStatusCode validateContent();

} TrustListDataTypeStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
