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

#ifndef IDENTITYMAPPINGRULETYPE_H
#define IDENTITYMAPPINGRULETYPE_H

#include "IIdentityMappingRuleType.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

namespace SoftingOPCToolbox5
{
	/*! The IdentityMappingRuleType structure defines a single rule for selecting a UserIdentityToken. */
	class TBC_EXPORT IdentityMappingRuleType
		: public IIdentityMappingRuleType
	{
	public:
		/*! Default constructor */
		IdentityMappingRuleType();
		/*! Constructs a copy of the given instance @p cp */
		IdentityMappingRuleType(const IIdentityMappingRuleType* cp);
		/*! @overload */
		IdentityMappingRuleType(const IIdentityMappingRuleType& cp);
		/*! @overload */
		IdentityMappingRuleType(const IdentityMappingRuleType& cp);

		/*! Destructor */
		virtual ~IdentityMappingRuleType();

		/*! Assigns the contents of @p cp to this instance */
		IdentityMappingRuleType& operator=(const IIdentityMappingRuleType* cp);
		/*! @overload */
		IdentityMappingRuleType& operator=(const IIdentityMappingRuleType& cp);
		/*! @overload */
		IdentityMappingRuleType& operator=(const IdentityMappingRuleType& cp);

		/*! @see IIdentityMappingRuleType::compare(IIdentityMappingRuleType*) */
		virtual int compare(const IIdentityMappingRuleType* other) const;
		/*! @overload */
		virtual int compare(const IIdentityMappingRuleType& other) const;

		/*! Compares two instances for equality */
		virtual bool operator==(const IIdentityMappingRuleType& other) const;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IIdentityMappingRuleType& other) const;

		/*! Copies the content of @p cp to this instance */
		virtual EnumStatusCode set(const IIdentityMappingRuleType* pSrc);
		/*! @overload */
		virtual EnumStatusCode set(const IIdentityMappingRuleType& pSrc);

		/*! see IIdentityMappingRuleType::getCriteriaType */
		virtual EnumIdentityMappingType getCriteriaType() const;
		/*! see IIdentityMappingRuleType::setCriteriaType */
		virtual void setCriteriaType(EnumIdentityMappingType criteriaType);

		/*! see IIdentityMappingRuleType::getCriteria */
		virtual tstring getCriteria() const;
		/*! see IIdentityMappingRuleType::setCriteria */
		virtual void setCriteria(const tstring& criteria);

		/*! For internal use only */
		virtual void* getInternHandle() const;
		/*! For internal use only */
		void initMembers();
		/*! Clears the contents of this object to enable its re-use. */
		void clear();

	private:
		IdentityMappingRuleTypeStruct m_content;
	};
} // end namespace

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif	// IDENTITYMAPPINGRULETYPE_H
