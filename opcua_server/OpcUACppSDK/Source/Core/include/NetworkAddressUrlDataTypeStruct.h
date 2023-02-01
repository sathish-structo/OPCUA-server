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

#ifndef _NetworkAddressUrlDataTypeStruct_H_
#define _NetworkAddressUrlDataTypeStruct_H_

#include "NetworkAddressDataTypeStruct.h"

#ifdef SOOS_WINDOWS
#pragma pack(push,8)
#endif

struct NetworkAddressUrlDataTypeInt;

//lint -sem(NetworkAddressUrlDataTypeStruct::init, initializer)
struct TBCORE_IF NetworkAddressUrlDataTypeStruct : public NetworkAddressDataTypeStruct
{
	/*! Access the address string for the communication relation in the form on an URL String. */
	const OTChar* getUrl() const;

	/*! Set the address string for the communication relation in the form on an URL String. */
	EnumStatusCode setUrl(const OTChar* pUrl);

	/*! Access the name of the network interface used for the communication relation. */
	const OTChar* getNetworkInterface() const;

	/*! Set the name of the network interface used for the communication relation. */
	EnumStatusCode setNetworkInterface(const OTChar* pNetworkInterface);

	NetworkAddressUrlDataTypeStruct();
	NetworkAddressUrlDataTypeStruct(const NetworkAddressUrlDataTypeStruct&);
	NetworkAddressUrlDataTypeStruct(const NetworkAddressDataTypeStruct& cp, bool copyContent);

	EnumStatusCode set(const NetworkAddressUrlDataTypeStruct&);
	EnumStatusCode set(const NetworkAddressDataTypeStruct&);
	EnumStatusCode get(NetworkAddressUrlDataTypeStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_NetworkAddressUrlDataType&);
	EnumStatusCode get(OpcUa_NetworkAddressUrlDataType&) const;

	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject) const;
#endif

	const NetworkAddressUrlDataTypeStruct& operator=(const NetworkAddressUrlDataTypeStruct& cp);
	bool operator==(const NetworkAddressUrlDataTypeStruct& toCompare) const;
	bool operator!=(const NetworkAddressUrlDataTypeStruct& toCompare) const;
	bool operator<(const NetworkAddressUrlDataTypeStruct& toCompare) const;
	bool operator>(const NetworkAddressUrlDataTypeStruct& toCompare) const;

	int compare(const NetworkAddressUrlDataTypeStruct* pTarget) const;

	void clear(void);
	~NetworkAddressUrlDataTypeStruct(void);

protected:
	void init(void);
};

typedef struct NetworkAddressUrlDataTypeStruct NetworkAddressUrlDataTypeStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
