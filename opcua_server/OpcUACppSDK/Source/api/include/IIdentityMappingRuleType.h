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

#ifndef IIDENTITYMAPPINGRULETYPE_H
#define IIDENTITYMAPPINGRULETYPE_H

#include "IdentityMappingRuleTypeStruct.h"

namespace SoftingOPCToolbox5
{
	/*! Defines a single rule for selecting a UserIdentityToken. */
	class TBC_EXPORT IIdentityMappingRuleType
	{
	public:
		/*! Destructor */
		virtual ~IIdentityMappingRuleType(){};

		/*! Returns the type of criteria contained in the rule. */
		virtual EnumIdentityMappingType getCriteriaType() const = 0;
		/*! Sets the type of criteria. */
		virtual void setCriteriaType(EnumIdentityMappingType criteriaType) = 0;

		/*! Returns the criteria which the UserIdentityToken must meet for a Session to be mapped to the Role.
		* The meaning of the criteria depends on the mappingType.
		* For EnumIdentityMappingType_Anonymous and EnumIdentityMappingType_AuthenticatedUser the criteria shall be empty. */
		virtual tstring getCriteria() const = 0;
		/*! Sets the criteria which the UserIdentityToken must meet for a Session to be mapped to the Role.
		* The meaning of the criteria depends on the mappingType.
		* For EnumIdentityMappingType_Anonymous and EnumIdentityMappingType_AuthenticatedUser the criteria shall be empty. */
		virtual void setCriteria(const tstring& criteria) = 0;

		/*! Copies the content of @p cp to this instance */
		virtual EnumStatusCode set(const IIdentityMappingRuleType* cp) = 0;
		/*! @overload */
		virtual EnumStatusCode set(const IIdentityMappingRuleType& cp) = 0;

		/*! Compares this instance to the given @p other instance.
		*
		* @li Returns <0, if "this < other"
		* @li Returns 0, if "this == other"
		* @li Returns >0, if "this > other" */
		virtual int compare(const IIdentityMappingRuleType* other) const = 0;
		/*! @overload */
		virtual int compare(const IIdentityMappingRuleType& other) const = 0;

		/*! Compares two instances for equality */
		virtual bool operator==(const IIdentityMappingRuleType& other) const = 0;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IIdentityMappingRuleType& other) const = 0;

		/*! For internal use only */
		virtual void* getInternHandle() const = 0;
	}; // end IIdentityMappingRuleType
} // end namespace

#endif	// IIDENTITYMAPPINGRULETYPE_H
