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

#ifndef IREADERGROUPMESSAGEDATATYPE_H
#define IREADERGROUPMESSAGEDATATYPE_H

struct ReaderGroupMessageDataTypeStruct;

namespace SoftingOPCToolbox5
{
	/*! The interface to ReaderGroupMessageDataType, which is an abstract base type without fields
	* for message mapping specific ReaderGroup parameters. */
	class TBC_EXPORT IReaderGroupMessageDataType
	{
	public:
		/*! Destructor */
		virtual ~IReaderGroupMessageDataType(){};

		/*! Returns the concrete data type of an instance of this abstract class. */
		virtual EnumDataTypeId getDataType() const = 0;

		/*! Compares this instance to the given @p other instance.
		*
		* @li Returns -1, if "this < other"
		* @li Returns 0, if "this == other"
		* @li Returns 1, if "this > other" */
		virtual int compare(const IReaderGroupMessageDataType* other) const = 0;
		/*! @overload */
		virtual int compare(const IReaderGroupMessageDataType& other) const = 0;

		/*! Compares two instances for equality */
		virtual bool operator==(const IReaderGroupMessageDataType& other) const = 0;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IReaderGroupMessageDataType& other) const = 0;

		/*! Copies the content of @p cp to this instance */
		virtual EnumStatusCode set(const IReaderGroupMessageDataType* cp) = 0;
		/*! @overload */
		virtual EnumStatusCode set(const IReaderGroupMessageDataType& cp) = 0;

		/*! For internal use only */
		virtual ReaderGroupMessageDataTypeStruct* getInternHandle() = 0;
		/*! For internal use only */
		virtual const ReaderGroupMessageDataTypeStruct* getInternHandle() const = 0;
	}; // end IReaderGroupMessageDataType
} // end namespace

#endif
