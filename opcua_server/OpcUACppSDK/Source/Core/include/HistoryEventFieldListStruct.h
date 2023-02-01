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

#ifndef _HistoryEventFieldListStruct_H_
#define _HistoryEventFieldListStruct_H_

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

#include "ValueStruct.h"

//lint -sem(HistoryEventFieldListStruct::clear, cleanup)
//lint -sem(HistoryEventFieldListStruct::init, initializer)
typedef struct TBCORE_IF HistoryEventFieldListStruct
{
	/* The list of event fields */
	ValueStruct* pEventFields;
	OTUInt32 eventFields_count;


	HistoryEventFieldListStruct();
	HistoryEventFieldListStruct(const HistoryEventFieldListStruct&);

	EnumStatusCode set(const HistoryEventFieldListStruct&);
	EnumStatusCode get(HistoryEventFieldListStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_HistoryEventFieldList&, OTUInt32 reserved = 0);
	EnumStatusCode get(OpcUa_HistoryEventFieldList&, const std::vector<tstring>& preferredLocales) const;

	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject, const std::vector<tstring>& preferredLocales) const;
#endif

	const HistoryEventFieldListStruct& operator=(const HistoryEventFieldListStruct& cp);
	bool operator==(const HistoryEventFieldListStruct& toCompare) const;
	bool operator!=(const HistoryEventFieldListStruct& toCompare) const;
	bool operator<(const HistoryEventFieldListStruct& toCompare) const;
	bool operator>(const HistoryEventFieldListStruct& toCompare) const;

	int compare(const HistoryEventFieldListStruct* pTarget) const;

	void clear(void);
	~HistoryEventFieldListStruct(void);
} HistoryEventFieldListStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
