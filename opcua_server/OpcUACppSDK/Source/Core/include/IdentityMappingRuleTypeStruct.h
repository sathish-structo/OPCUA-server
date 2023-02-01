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

#ifndef IDENTITYMAPPINGRULESTRUCT_H
#define IDENTITYMAPPINGRULESTRUCT_H

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

/*! The IdentityMappingRuleType structure defines a single rule for selecting a UserIdentityToken. */
//lint -sem(IdentityMappingRuleTypeStruct::clear, cleanup)
//lint -sem(IdentityMappingRuleTypeStruct::init, initializer)
typedef struct TBCORE_IF IdentityMappingRuleTypeStruct
{
	/*! The type of criteria contained in the rule. */
	EnumIdentityMappingType m_criteriaType;

	/*! The criteria which the UserIdentityToken must meet for a Session to be mapped to the Role.
	The meaning of the criteria depends on the mappingType. The criteria area "" for EnumIdentityMappingType_Anonymous and EnumIdentityMappingType_AuthenticatedUser. */
	OTChar* m_pCriteria;

	IdentityMappingRuleTypeStruct();
	IdentityMappingRuleTypeStruct(const IdentityMappingRuleTypeStruct& cp);
	EnumStatusCode setCriteria(const OTChar* pCriteria);

	EnumStatusCode set(const IdentityMappingRuleTypeStruct&);
	EnumStatusCode get(IdentityMappingRuleTypeStruct&) const;
#ifdef OT_COMPILATION
	EnumStatusCode set(const OpcUa_IdentityMappingRuleType&);
	EnumStatusCode get(OpcUa_IdentityMappingRuleType&) const;
	EnumStatusCode getOpcUaExtensionObject(OpcUa_ExtensionObject& extensionObject) const;
#endif


	const IdentityMappingRuleTypeStruct& operator=(const IdentityMappingRuleTypeStruct& cp);
	bool operator==(const IdentityMappingRuleTypeStruct& toCompare) const;
	bool operator!=(const IdentityMappingRuleTypeStruct& toCompare) const;
	bool operator<(const IdentityMappingRuleTypeStruct& toCompare) const;
	bool operator>(const IdentityMappingRuleTypeStruct& toCompare) const;

	int compare(const IdentityMappingRuleTypeStruct* pTarget) const;
	tstring toString() const;

	void clear(void);
	~IdentityMappingRuleTypeStruct(void);

}IdentityMappingRuleTypeStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif	// IDENTITYMAPPINGRULESTRUCT_H
