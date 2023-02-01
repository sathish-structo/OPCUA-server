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

#ifndef PUBLISHEDDATASETSOURCEDATATYPE_H
#define PUBLISHEDDATASETSOURCEDATATYPE_H

#include "IPublishedDataSetSourceDataType.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

namespace SoftingOPCToolbox5
{
	class InnerPublishedDataSetSourceDataType;

	/*! The PublishedDataSetSourceDataType is an abstract base type without fields
	* for the definition of the PublishedDataSet source.
	*
	* Concrete subclasses are PublishedDataItemsDataType and PublishedEventsDataType, which are used e.g. in
	* @li structure class PublishedDataSetDataType */
	class TBC_EXPORT PublishedDataSetSourceDataType
	{
	public:
		/*! Default constructor */
		PublishedDataSetSourceDataType();
		/*! Constructs a copy of the given instance @p cp */
		PublishedDataSetSourceDataType(const IPublishedDataSetSourceDataType* cp);
		/*! @overload */
		PublishedDataSetSourceDataType(const IPublishedDataSetSourceDataType& cp);
		/*! @overload */
		PublishedDataSetSourceDataType(const PublishedDataSetSourceDataType& cp);
		/*! For internal use only */
		PublishedDataSetSourceDataType(const PublishedDataSetSourceDataTypeStruct& cp);

		/*! Destructor */
		virtual ~PublishedDataSetSourceDataType();

		/*! Assigns the contents of @p cp to this instance */
		PublishedDataSetSourceDataType& operator=(const IPublishedDataSetSourceDataType* cp);
		/*! @overload */
		PublishedDataSetSourceDataType& operator=(const IPublishedDataSetSourceDataType& cp);
		/*! @overload */
		PublishedDataSetSourceDataType& operator=(const PublishedDataSetSourceDataType& cp);

		/*! Assigns the contents of @p pSrc to this instance */
		EnumStatusCode set(const IPublishedDataSetSourceDataType* pSrc);
		/*! @overload */
		EnumStatusCode set(const IPublishedDataSetSourceDataType& pSrc);
		/*! @overload */
		EnumStatusCode set(const PublishedDataSetSourceDataType* pSrc);
		/*! @overload */
		EnumStatusCode set(const PublishedDataSetSourceDataType& pSrc);
		/*! For internal use only */
		EnumStatusCode set(const PublishedDataSetSourceDataTypeStruct* pSrc);
		/*! For internal use only */
		EnumStatusCode set(const PublishedDataSetSourceDataTypeStruct& pSrc);

		/*! Access the contained object */
		const IPublishedDataSetSourceDataType& getInterface() const;

		/*! Clears the contents of this object to enable its re-use. */
		void clear();

	private:
		/*! For internal use only */
		void destroyContent();

		IPublishedDataSetSourceDataType* m_pContent;
	};
} // end namespace

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
