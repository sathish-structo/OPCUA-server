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

#ifndef ENDPOINTTYPESTRUCT_H
#define ENDPOINTTYPESTRUCT_H

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

/*! Structure describing engineering units. */
//lint -sem(EndpointTypeStruct::clear, cleanup)
//lint -sem(EndpointTypeStruct::init, initializer)
typedef struct TBCORE_IF EndpointTypeStruct
{
	/*! The URL for the Endpoint. */
	OTChar* m_endpointUrl;

	/*! The type of message security. */
	EnumMessageSecurityMode m_securityMode;

	/*! The URI of the SecurityPolicy. */
	OTChar* m_securityPolicyUri;
#
	/*! The URI of the Transport Profile. */
	OTChar* m_transportProfileUri;


	EndpointTypeStruct();
	EndpointTypeStruct(const EndpointTypeStruct& cp);
	EnumStatusCode setEndpointUrl(const OTChar* pValue);
	EnumStatusCode setSecurityPolicyUri(const OTChar* pValue);
	EnumStatusCode setTransportProfileUri(const OTChar* pValue);

	EnumStatusCode set(const EndpointTypeStruct&);
	EnumStatusCode get(EndpointTypeStruct&) const;
#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_EndpointType&);
	EnumStatusCode get(OpcUa_EndpointType&) const;
	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject) const;
#endif


	const EndpointTypeStruct& operator=(const EndpointTypeStruct& cp);
	bool operator==(const EndpointTypeStruct& toCompare) const;
	bool operator!=(const EndpointTypeStruct& toCompare) const;
	bool operator<(const EndpointTypeStruct& toCompare) const;
	bool operator>(const EndpointTypeStruct& toCompare) const;

	int compare(const EndpointTypeStruct* pTarget) const;
	tstring toString() const;

	void clear(void);
	~EndpointTypeStruct(void);

}EndpointTypeStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif	// ENDPOINTTYPESTRUCT_H
