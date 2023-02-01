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

#ifndef _PubSubGroupDataTypeStruct_H_
#define _PubSubGroupDataTypeStruct_H_

#include "Enums.h"
#include "EndpointDescriptionStruct.h"
#include "KeyValuePairStruct.h"

#ifdef SOOS_WINDOWS
#pragma pack(push,8)
#endif

//lint -sem(PubSubGroupDataTypeStruct::clear, cleanup)
//lint -sem(PubSubGroupDataTypeStruct::init, initializer)
typedef struct TBCORE_IF PubSubGroupDataTypeStruct
{
	OTChar* pName;

	OTBoolean enabled;

	EnumMessageSecurityMode securityMode;

	OTChar* pSecurityGroupId;

	EndpointDescriptionStruct* pSecurityKeyServices;
	OTUInt32 securityKeyServices_count;

	OTUInt32 maxNetworkMessageSize;

	KeyValuePairStruct* pGroupProperties;
	OTUInt32 groupProperties_count;


	PubSubGroupDataTypeStruct();
	PubSubGroupDataTypeStruct(const PubSubGroupDataTypeStruct&);

	EnumStatusCode set(const PubSubGroupDataTypeStruct&);
	EnumStatusCode get(PubSubGroupDataTypeStruct&) const;

	const PubSubGroupDataTypeStruct& operator=(const PubSubGroupDataTypeStruct& cp);
	bool operator==(const PubSubGroupDataTypeStruct& toCompare) const;
	bool operator!=(const PubSubGroupDataTypeStruct& toCompare) const;
	bool operator<(const PubSubGroupDataTypeStruct& toCompare) const;
	bool operator>(const PubSubGroupDataTypeStruct& toCompare) const;

	int compare(const PubSubGroupDataTypeStruct* pTarget) const;

	void clear();
	~PubSubGroupDataTypeStruct();
} PubSubGroupDataTypeStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
