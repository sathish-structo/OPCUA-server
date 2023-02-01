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

#ifndef INNERENUMDEFINITION_H
#define INNERENUMDEFINITION_H

#include "IEnumDefinition.h"
#include "EnumField.h"
#include "IEnumField.h"

namespace SoftingOPCToolbox5
{
	/*! For internal use only */
	class TBC_EXPORT InnerEnumDefinition
		: public IEnumDefinition
	{
	public:
		/*! Destructor */
		virtual ~InnerEnumDefinition();

		/*! Gets the Fields member of the class */
		virtual std::vector<EnumField> getFields() const;
		/*! Adds a Field to the Fields member of the class */
		virtual EnumStatusCode addField(const IEnumField& field);

		/*! For internal use only */
		void initInner(EnumDefinitionStruct* pHandle);

		virtual EnumStatusCode set(const IEnumDefinition* pSrc);
		virtual EnumStatusCode set(const IEnumDefinition& pSrc);

		virtual int compare(const IEnumDefinition* other) const;
		virtual int compare(const IEnumDefinition& other) const;

		virtual bool operator==(const IEnumDefinition& other) const;
		virtual bool operator!=(const IEnumDefinition& other) const;

		virtual void* getInternHandle() const;

	private:
		EnumDefinitionStruct* m_pContent;
	};
} // end namespace

#endif
