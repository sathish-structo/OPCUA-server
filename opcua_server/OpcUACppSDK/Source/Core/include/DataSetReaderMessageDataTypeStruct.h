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

#ifndef DATASETREADERMESSAGEDATATYPESTRUCT_H
#define DATASETREADERMESSAGEDATATYPESTRUCT_H

#include "Enums.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

struct DataSetReaderMessageDataTypeInt;

//lint -sem(DataSetReaderMessageDataTypeStruct::clear, cleanup)
//lint -sem(DataSetReaderMessageDataTypeStruct::init, initializer)
typedef struct TBCORE_IF DataSetReaderMessageDataTypeStruct
{
	DataSetReaderMessageDataTypeStruct();
	DataSetReaderMessageDataTypeStruct(const DataSetReaderMessageDataTypeStruct&);

	EnumDataTypeId getDataType() const;

	EnumStatusCode set(const DataSetReaderMessageDataTypeStruct&);
	EnumStatusCode get(DataSetReaderMessageDataTypeStruct&) const;

#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_ExtensionObject& source);
	EnumStatusCode get(OpcUa_ExtensionObject& target) const;
#endif

	const DataSetReaderMessageDataTypeStruct& operator=(const DataSetReaderMessageDataTypeStruct& cp);
	bool operator==(const DataSetReaderMessageDataTypeStruct& toCompare) const;
	bool operator!=(const DataSetReaderMessageDataTypeStruct& toCompare) const;
	bool operator<(const DataSetReaderMessageDataTypeStruct& toCompare) const;
	bool operator>(const DataSetReaderMessageDataTypeStruct& toCompare) const;

	int compare(const DataSetReaderMessageDataTypeStruct* pTarget) const;

	void clear();
	~DataSetReaderMessageDataTypeStruct();

	DataSetReaderMessageDataTypeInt *getContent() const;
protected:
	// Internal object
	DataSetReaderMessageDataTypeInt *pContent;
	// Flag set in subclasses when the content is not owned
	bool bNoRelease;
} DataSetReaderMessageDataTypeStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif	// DATASETREADERMESSAGEDATATYPESTRUCT_H
