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

#ifndef INNERENDPOINTTYPE_H
#define INNERENDPOINTTYPE_H

#include "IEndpointType.h"

namespace SoftingOPCToolbox5
{
	/*! For internal use only */
	class TBC_EXPORT InnerEndpointType
		: public IEndpointType
	{
	public:
		/*! Destructor */
		virtual ~InnerEndpointType();

		/*! Returns the URL for the Endpoint. */
		virtual tstring getEndpointUrl() const;
		/*! Sets the URL for the Endpoint. */
		virtual void setEndpointUrl(const tstring& endpointUrl);

		/*! Returns the type of message security. */
		virtual EnumMessageSecurityMode getSecurityMode() const;
		/*! Sets the type of message security. */
		virtual void setSecurityMode(EnumMessageSecurityMode securityMode);

		/*! Returns the URI of the SecurityPolicy. */
		virtual tstring getSecurityPolicyUri() const;
		/*! Sets the URI of the SecurityPolicy. */
		virtual void setSecurityPolicyUri(const tstring& securityPolicyUri);

		/*! Returns the URI of the Transport Profile. */
		virtual tstring getTransportProfileUri() const;
		/*! Sets the URI of the Transport Profile. */
		virtual void setTransportProfileUri(const tstring& transportProfileUri);

		/*! For internal use only */
		void initInner(EndpointTypeStruct* pHandle);

		virtual EnumStatusCode set(const IEndpointType* pSrc);
		virtual EnumStatusCode set(const IEndpointType& pSrc);

		virtual int compare(const IEndpointType* other) const;
		virtual int compare(const IEndpointType& other) const;

		virtual bool operator==(const IEndpointType& other) const;
		virtual bool operator!=(const IEndpointType& other) const;

		virtual void* getInternHandle() const;

	private:
		EndpointTypeStruct* m_pContent;
	};
} // end namespace

#endif	// INNERENDPOINTTYPE_H
