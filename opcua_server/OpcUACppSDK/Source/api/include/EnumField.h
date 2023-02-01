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

#ifndef ENUMFIELD_H
#define ENUMFIELD_H

#include "IEnumField.h"
#include "LocalizedText.h"
#include "ILocalizedText.h"
#include "InnerLocalizedText.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

namespace SoftingOPCToolbox5
{
	/*! The EnumField data type contains the metadata for a field of a custom Enumeration or OptionSet DataType. */
	class TBC_EXPORT EnumField
		: public IEnumField
	{
	public:
		/*! Default constructor */
		EnumField();
		/*! Constructs a copy of the given instance @p cp */
		EnumField(const IEnumField* cp);
		/*! @overload */
		EnumField(const IEnumField& cp);
		/*! @overload */
		EnumField(const EnumField& cp);

		/*! Destructor */
		virtual ~EnumField();

		/*! Assigns the contents of @p cp to this instance */
		EnumField& operator=(const IEnumField* cp);
		/*! @overload */
		EnumField& operator=(const IEnumField& cp);
		/*! @overload */
		EnumField& operator=(const EnumField& cp);

		/* see IEnumField::compare(IEnumField*) */
		virtual int compare(const IEnumField* other) const;
		/*! @overload */
		virtual int compare(const IEnumField& other) const;

		/*! Compares two instances for equality */
		virtual bool operator==(const IEnumField& other) const;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IEnumField& other) const;

		/* see IEnumField::set(IEnumField*) */
		virtual EnumStatusCode set(const IEnumField* pSrc);
		/*! @overload */
		virtual EnumStatusCode set(const IEnumField& pSrc);

		/* see IEnumField::setName */
		virtual void setName(const tstring& name);
		/* see IEnumField::getName */
		virtual tstring getName() const;

		/* see IEnumField::setValue */
		virtual void setValue(const OTInt64& value);
		/* see IEnumField::getValue */
		virtual OTInt64 getValue() const;

		/* see IEnumField::setDisplayName */
		virtual EnumStatusCode setDisplayName(const ILocalizedText* displayName);
		/*! @overload */
		virtual EnumStatusCode setDisplayName(const LocalizedText& displayName);
		/* see IEnumField::getDisplayName */
		virtual const ILocalizedText* getDisplayName() const;

		/* see IEnumField::setDescription */
		virtual EnumStatusCode setDescription(const ILocalizedText* description);
		/*! @overload */
		virtual EnumStatusCode setDescription(const LocalizedText& description);
		/* see IEnumField::getDescription */
		virtual const ILocalizedText* getDescription() const;

		/*! For internal use only */
		virtual void* getInternHandle() const;
		/*! For internal use only */
		void initMembers();
		/*! Clears the contents of this object to enable its re-use. */
		void clear();

	private:
		InnerLocalizedText m_displayName;
		InnerLocalizedText m_description;

		EnumFieldStruct m_content;
	};
} // end namespace

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
