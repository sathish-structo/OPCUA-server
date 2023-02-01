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

#ifndef _ServiceCounterDataTypeStruct_H_
#define _ServiceCounterDataTypeStruct_H_

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif


//lint -sem(ServiceCounterDataTypeStruct::clear, cleanup)
//lint -sem(ServiceCounterDataTypeStruct::init, initializer)
typedef struct TBCORE_IF ServiceCounterDataTypeStruct
{
	/*! @brief The number of Service requests that have been received. */
	OTUInt32 totalCount;

	/*! @brief The total number of Service requests that were rejected. */
	OTUInt32 errorCount;


	ServiceCounterDataTypeStruct();
	ServiceCounterDataTypeStruct(const ServiceCounterDataTypeStruct&);
	ServiceCounterDataTypeStruct(OTUInt32 TotalCount, OTUInt32 ErrorCount);

	EnumStatusCode set(const ServiceCounterDataTypeStruct&);
	EnumStatusCode get(ServiceCounterDataTypeStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_ServiceCounterDataType&);
	EnumStatusCode get(OpcUa_ServiceCounterDataType&) const;

	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject) const;
#endif

	const ServiceCounterDataTypeStruct& operator=(const ServiceCounterDataTypeStruct& cp);
	bool operator==(const ServiceCounterDataTypeStruct& toCompare) const;
	bool operator!=(const ServiceCounterDataTypeStruct& toCompare) const;
	bool operator<(const ServiceCounterDataTypeStruct& toCompare) const;
	bool operator>(const ServiceCounterDataTypeStruct& toCompare) const;

	int compare(const ServiceCounterDataTypeStruct* pTarget) const;

	tstring toString() const;

	void clear(void);
	~ServiceCounterDataTypeStruct(void);
} ServiceCounterDataTypeStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
