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

#ifndef IDATASETREADERMESSAGEDATATYPE_H
#define IDATASETREADERMESSAGEDATATYPE_H

struct DataSetReaderMessageDataTypeStruct;

namespace SoftingOPCToolbox5
{
	/*! The interface to DataSetReaderMessageDataType, which is an abstract base type without fields
	* for message mapping specific DataSetReader parameters. */
	class TBC_EXPORT IDataSetReaderMessageDataType
	{
	public:
		/*! Destructor */
		virtual ~IDataSetReaderMessageDataType(){};

		/*! Returns the concrete data type of an instance of this abstract class. */
		virtual EnumDataTypeId getDataType() const = 0;

		/*! Compares this instance to the given @p other instance.
		*
		* @li Returns -1, if "this < other"
		* @li Returns 0, if "this == other"
		* @li Returns 1, if "this > other" */
		virtual int compare(const IDataSetReaderMessageDataType* other) const = 0;
		/*! @overload */
		virtual int compare(const IDataSetReaderMessageDataType& other) const = 0;

		/*! Compares two instances for equality */
		virtual bool operator==(const IDataSetReaderMessageDataType& other) const = 0;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IDataSetReaderMessageDataType& other) const = 0;

		/*! Copies the content of @p cp to this instance */
		virtual EnumStatusCode set(const IDataSetReaderMessageDataType* cp) = 0;
		/*! @overload */
		virtual EnumStatusCode set(const IDataSetReaderMessageDataType& cp) = 0;

		/*! For internal use only */
		virtual DataSetReaderMessageDataTypeStruct* getInternHandle() = 0;
		/*! For internal use only */
		virtual const DataSetReaderMessageDataTypeStruct* getInternHandle() const = 0;
	}; // end IDataSetReaderMessageDataType
} // end namespace

#endif
