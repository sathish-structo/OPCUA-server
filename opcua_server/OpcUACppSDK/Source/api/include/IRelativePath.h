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

#ifndef IRELATIVEPATH_H
#define IRELATIVEPATH_H

#include "RelativePathStruct.h"
#include "InnerRelativePathElement.h"
#include "RelativePathElement.h"

namespace SoftingOPCToolbox5
{
	/*! The RelativePath data type defines a sequence of browse names and references to follow. */
	class TBC_EXPORT IRelativePath
	{
	public:
		/*! Destructor */
		virtual ~IRelativePath(){};

		/*! The methods getElementCount() and getElement() are provided for efficient access to data.
		* The semantics of these methods are documented in getElements(). */
		virtual OTUInt32 getElementCount() const = 0;
		/*! The methods getElementCount() and getElement() are provided for efficient access to data.
		* The semantics of these methods are documented in getElements(). */
		virtual const IRelativePathElement* getElement(OTUInt32 index) const = 0;
		/*! A sequence of references and browse names to follow.
		*
		* Each element in the sequence is processed by finding the targets and then using those targets
		* as the starting nodes for the next element. The targets of the final element are the target of
		* the RelativePath. */
		virtual std::vector<RelativePathElement> getElements() const = 0;
		/*! Adds an element to the list of references and browse names to follow.
		*
		* @see getElements() */
		virtual EnumStatusCode addElement(const IRelativePathElement* pValue) = 0;
		/*! @overload */
		virtual EnumStatusCode addElement(const IRelativePathElement& value) = 0;

		/*! Copies the content of @p cp to this instance */
		virtual EnumStatusCode set(const IRelativePath*) = 0;
		/*! @overload */
		virtual EnumStatusCode set(const IRelativePath&) = 0;

		/*! Compares this instance to the given @p other instance.
		*
		* @li Returns <0, if "this < other"
		* @li Returns 0, if "this == other"
		* @li Returns >0, if "this > other" */
		virtual int compare(const IRelativePath* other) const = 0;
		/*! @overload */
		virtual int compare(const IRelativePath& other) const = 0;

		/*! Compares two instances for equality */
		virtual bool operator==(const IRelativePath& other) const = 0;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IRelativePath& other) const = 0;

		/*! For internal use only */
		virtual void* getInternHandle() const = 0;
	}; // end IRelativePath
} // end namespace
#endif	// IRELATIVEPATH_H
