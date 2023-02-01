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

#ifndef ENDPOINTTYPE_H
#define ENDPOINTTYPE_H

#include "IEndpointType.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

namespace SoftingOPCToolbox5
{
	/*! The EndpointType structure describes an Endpoint. */
	class TBC_EXPORT EndpointType
		: public IEndpointType
	{
	public:
		/*! Default constructor */
		EndpointType();
		/*! Constructs a copy of the given instance @p cp */
		EndpointType(const IEndpointType* cp);
		/*! @overload */
		EndpointType(const IEndpointType& cp);
		/*! @overload */
		EndpointType(const EndpointType& cp);

		/*! Destructor */
		virtual ~EndpointType();

		/*! Assigns the contents of @p cp to this instance */
		EndpointType& operator=(const IEndpointType* cp);
		/*! @overload */
		EndpointType& operator=(const IEndpointType& cp);
		/*! @overload */
		EndpointType& operator=(const EndpointType& cp);

		/*! @see IEndpointType::compare(IEndpointType*) */
		virtual int compare(const IEndpointType* other) const;
		/*! @overload */
		virtual int compare(const IEndpointType& other) const;

		/*! Compares two instances for equality */
		virtual bool operator==(const IEndpointType& other) const;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IEndpointType& other) const;

		/*! Copies the content of @p cp to this instance */
		virtual EnumStatusCode set(const IEndpointType* pSrc);
		/*! @overload */
		virtual EnumStatusCode set(const IEndpointType& pSrc);

		/*! see IEndpointType::getEndpointUrl. */
		virtual tstring getEndpointUrl() const;
		/*! see IEndpointType::setEndpointUrl. */
		virtual void setEndpointUrl(const tstring& endpointUrl);

		/*! see IEndpointType::getSecurityMode. */
		virtual EnumMessageSecurityMode getSecurityMode() const;
		/*! see IEndpointType::setSecurityMode. */
		virtual void setSecurityMode(EnumMessageSecurityMode securityMode);

		/*! see IEndpointType::getSecurityPolicyUri. */
		virtual tstring getSecurityPolicyUri() const;
		/*! see IEndpointType::setSecurityPolicyUri. */
		virtual void setSecurityPolicyUri(const tstring& securityPolicyUri);

		/*! see IEndpointType::getTransportProfileUri. */
		virtual tstring getTransportProfileUri() const;
		/*! see IEndpointType::setTransportProfileUri. */
		virtual void setTransportProfileUri(const tstring& transportProfileUri);

		/*! For internal use only */
		virtual void* getInternHandle() const;
		/*! For internal use only */
		void initMembers();
		/*! Clears the contents of this object to enable its re-use. */
		void clear();

	private:
		EndpointTypeStruct m_content;
	};
} // end namespace

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif	// ENDPOINTTYPE_H
