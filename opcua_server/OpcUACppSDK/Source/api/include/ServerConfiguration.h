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

#ifndef SERVERCONFIGURATION_H
#define SERVERCONFIGURATION_H
#include "Base.h"
#include "Enums.h"
#include "ServerObject.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		class MethodCallTransaction;
		class MethodCallRequest;

		/*! The ServerConfiguration class is used to represents the configuration of a server which supports GDS push management.
		*
		* There is always exactly one instance in the server address space.
		* Due to this fact an application cannot create other instances; the only creation is done by SDK on initializing or via XML import.
		* If you need to overwrite any method handling, you need to instantiate your derived class via Creator::createServerConfiguration.
		*
		* The ServerConfiguration contains mandatory properties to describe how GDS can access the server:
		* @li ServerCapabilities
		* @li SupportedPrivateKeyFormats (only PEM formats are supported by SDK)
		* @li MaxTrustListSize
		* @li MulticastDnsEnabled (multicast DNS is not supported by SDK)
		*
		* The ServerConfiguration handles UA methods which are invoked by GDS:
		* @li UpdateCertificate
		* @li ApplyChanges
		* @li CreateSigningRequest
		* @li GetRejectedList
		*
		* @note The properties SupportedPrivateKeyFormats and MulticastDnsEnabled are handled by SDK only. */
		class TBC_EXPORT Configuration
			: public Object
		{
		public:
			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			Configuration(TBHandle otbHandle);

			/*! Sets the ServerCapabilities property of the server configuration.
			*
			* A full list of capabilities can be found at http://www.opcfoundation.org/UA/schemas/1.04/ServerCapabilities.csv
			* SDK sets as default "DA" as property.
			*
			* @param serverCapabilities		 The capabilities of a server. */
			EnumStatusCode setServerCapabilities(const std::vector<tstring>& serverCapabilities);
			/*! Gets the ServerCapabilities property of the server configuration.
			*
			* @see setServerCapabilities */
			EnumStatusCode getServerCapabilities(std::vector<tstring>& serverCapabilities) const;

			/*! Sets the MaxTrustListSize property of the server configuration.
			*
			* All trusted and issuer certificates are exposed as a file in a trust list object.
			* This property specifies the maximum of this representation.
			*
			* @param maxTrustListSize		Maximum of the file representation.
			*								Default is 64 kB (65535 byte). */
			EnumStatusCode setMaxTrustListSize(OTUInt32 maxTrustListSize);
			/*! Gets the ServerCapabilities property of the server configuration.
			*
			* @see setMaxTrustListSize */
			EnumStatusCode getMaxTrustListSize(OTUInt32& maxTrustListSize) const;

			virtual void handleCallRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);


		protected:
			/*! Default constructor */
			Configuration();

		public:
			/*! Destructor */
			virtual ~Configuration();

		private:
			/*! Forbid use assignment operator */
			Configuration& operator=(const Configuration&);
			/*! Forbid use of copy constructor */
			Configuration(const Configuration&);
		};
		typedef ObjectPointer<Configuration> ConfigurationPtr;
		typedef ObjectPointer<const Configuration> ConfigurationConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif	// SERVERCONFIGURATION_H
