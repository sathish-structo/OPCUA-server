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

#ifndef _WriterGroupDataTypeStruct_H_
#define _WriterGroupDataTypeStruct_H_

#include "WriterGroupTransportDataTypeStruct.h"
#include "WriterGroupMessageDataTypeStruct.h"
#include "DataSetWriterDataTypeStruct.h"
#include "Enums.h"
#include "EndpointDescriptionStruct.h"
#include "KeyValuePairStruct.h"

#ifdef SOOS_WINDOWS
#pragma pack(push,8)
#endif

//lint -sem(WriterGroupDataTypeStruct::clear, cleanup)
//lint -sem(WriterGroupDataTypeStruct::init, initializer)
typedef struct TBCORE_IF WriterGroupDataTypeStruct
{
	OTUInt16 writerGroupId;

	OTDouble publishingInterval;

	OTDouble keepAliveTime;

	OTUInt8 priority;

	OTChar** ppLocaleIds;
	OTUInt32 localeIds_count;

	WriterGroupTransportDataTypeStruct transportSettings;

	WriterGroupMessageDataTypeStruct messageSettings;

	DataSetWriterDataTypeStruct* pDataSetWriters;
	OTUInt32 dataSetWriters_count;

	OTChar* pName;

	OTBoolean enabled;

	EnumMessageSecurityMode securityMode;

	OTChar* pSecurityGroupId;

	EndpointDescriptionStruct* pSecurityKeyServices;
	OTUInt32 securityKeyServices_count;

	OTUInt32 maxNetworkMessageSize;

	KeyValuePairStruct* pGroupProperties;
	OTUInt32 groupProperties_count;


	WriterGroupDataTypeStruct();
	WriterGroupDataTypeStruct(const WriterGroupDataTypeStruct&);

	EnumStatusCode set(const WriterGroupDataTypeStruct&);
	EnumStatusCode get(WriterGroupDataTypeStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_WriterGroupDataType&);
	EnumStatusCode get(OpcUa_WriterGroupDataType&, const std::vector<tstring>& preferredLocales) const;

	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject, const std::vector<tstring>& preferredLocales) const;
#endif

	const WriterGroupDataTypeStruct& operator=(const WriterGroupDataTypeStruct& cp);
	bool operator==(const WriterGroupDataTypeStruct& toCompare) const;
	bool operator!=(const WriterGroupDataTypeStruct& toCompare) const;
	bool operator<(const WriterGroupDataTypeStruct& toCompare) const;
	bool operator>(const WriterGroupDataTypeStruct& toCompare) const;

	int compare(const WriterGroupDataTypeStruct* pTarget) const;

	tstring toString() const;

	void clear();
	~WriterGroupDataTypeStruct();
} WriterGroupDataTypeStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
