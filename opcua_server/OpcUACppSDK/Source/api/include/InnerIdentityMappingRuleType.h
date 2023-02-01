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

#ifndef INNERIDENTITYMAPPINGRULETYPE_H
#define INNERIDENTITYMAPPINGRULETYPE_H

#include "IIdentityMappingRuleType.h"

namespace SoftingOPCToolbox5
{
	/*! For internal use only */
	class TBC_EXPORT InnerIdentityMappingRuleType
		: public IIdentityMappingRuleType
	{
	public:
		/*! Destructor */
		virtual ~InnerIdentityMappingRuleType();

		/*! Returns the type of criteria contained in the rule. */
		virtual EnumIdentityMappingType getCriteriaType() const;
		/*! Sets the type of criteria. */
		virtual void setCriteriaType(EnumIdentityMappingType criteriaType);

		/*! Returns the criteria which the UserIdentityToken must meet for a Session to be mapped to the Role. */
		virtual tstring getCriteria() const;
		/*! Sets the criteria which the UserIdentityToken must meet for a Session to be mapped to the Role. */
		virtual void setCriteria(const tstring& criteria);

		/*! For internal use only */
		void initInner(IdentityMappingRuleTypeStruct* pHandle);

		virtual EnumStatusCode set(const IIdentityMappingRuleType* pSrc);
		virtual EnumStatusCode set(const IIdentityMappingRuleType& pSrc);

		virtual int compare(const IIdentityMappingRuleType* other) const;
		virtual int compare(const IIdentityMappingRuleType& other) const;

		virtual bool operator==(const IIdentityMappingRuleType& other) const;
		virtual bool operator!=(const IIdentityMappingRuleType& other) const;

		virtual void* getInternHandle() const;

	private:
		IdentityMappingRuleTypeStruct* m_pContent;
	};
} // end namespace

#endif	// INNERIDENTITYMAPPINGRULETYPE_H
