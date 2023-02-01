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

#ifndef _MdnsDiscoveryConfigurationStruct_H_
#define _MdnsDiscoveryConfigurationStruct_H_


#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

//lint -sem(MdnsDiscoveryConfigurationStruct::clear, cleanup)
//lint -sem(MdnsDiscoveryConfigurationStruct::init, initializer)
typedef struct TBCORE_IF MdnsDiscoveryConfigurationStruct
{
	/* The name of the Server when it is announced via mDNS. */
	OTChar* pMdnsServerName;

	/* The set of Server capabilities supported by the Server. */
	OTChar** ppServerCapabilities;
	OTUInt32 serverCapabilities_count;


	MdnsDiscoveryConfigurationStruct();
	MdnsDiscoveryConfigurationStruct(const MdnsDiscoveryConfigurationStruct&);

	EnumStatusCode set(const MdnsDiscoveryConfigurationStruct&);
	EnumStatusCode get(MdnsDiscoveryConfigurationStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_MdnsDiscoveryConfiguration&);
	EnumStatusCode get(OpcUa_MdnsDiscoveryConfiguration&) const;

	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject) const;
#endif

	const MdnsDiscoveryConfigurationStruct& operator=(const MdnsDiscoveryConfigurationStruct& cp);
	bool operator==(const MdnsDiscoveryConfigurationStruct& toCompare) const;
	bool operator!=(const MdnsDiscoveryConfigurationStruct& toCompare) const;
	bool operator<(const MdnsDiscoveryConfigurationStruct& toCompare) const;
	bool operator>(const MdnsDiscoveryConfigurationStruct& toCompare) const;

	int compare(const MdnsDiscoveryConfigurationStruct* pTarget) const;

	void clear(void);
	~MdnsDiscoveryConfigurationStruct(void);
} MdnsDiscoveryConfigurationStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
