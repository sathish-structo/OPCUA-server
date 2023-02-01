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

#ifndef IENDPOINTTYPE_H
#define IENDPOINTTYPE_H

#include "EndpointTypeStruct.h"

namespace SoftingOPCToolbox5
{
	/*! Describes an Endpoint. */
	class TBC_EXPORT IEndpointType
	{
	public:
		/*! Destructor */
		virtual ~IEndpointType(){};

		/*! Returns the URL for the Endpoint. */
		virtual tstring getEndpointUrl() const = 0;
		/*! Sets the URL for the Endpoint. */
		virtual void setEndpointUrl(const tstring& endpointUrl) = 0;

		/*! Returns the type of message security. */
		virtual EnumMessageSecurityMode getSecurityMode() const = 0;
		/*! Sets the type of message security. */
		virtual void setSecurityMode(EnumMessageSecurityMode securityMode) = 0;

		/*! Returns the URI of the SecurityPolicy. */
		virtual tstring getSecurityPolicyUri() const = 0;
		/*! Sets the URI of the SecurityPolicy. */
		virtual void setSecurityPolicyUri(const tstring& securityPolicyUri) = 0;

		/*! Returns the URI of the Transport Profile. */
		virtual tstring getTransportProfileUri() const = 0;
		/*! Sets the URI of the Transport Profile. */
		virtual void setTransportProfileUri(const tstring& transportProfileUri) = 0;

		/*! Copies the content of @p cp to this instance */
		virtual EnumStatusCode set(const IEndpointType* cp) = 0;
		/*! @overload */
		virtual EnumStatusCode set(const IEndpointType& cp) = 0;

		/*! Compares this instance to the given @p other instance.
		*
		* @li Returns <0, if "this < other"
		* @li Returns 0, if "this == other"
		* @li Returns >0, if "this > other" */
		virtual int compare(const IEndpointType* other) const = 0;
		/*! @overload */
		virtual int compare(const IEndpointType& other) const = 0;

		/*! Compares two instances for equality */
		virtual bool operator==(const IEndpointType& other) const = 0;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const IEndpointType& other) const = 0;

		/*! For internal use only */
		virtual void* getInternHandle() const = 0;
	}; // end IEndpointType
} // end namespace

#endif	// IENDPOINTTYPE_H
