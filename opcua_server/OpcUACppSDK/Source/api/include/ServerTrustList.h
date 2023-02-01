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

#ifndef SERVERTRUSDTLIST_H
#define SERVERTRUSDTLIST_H
#include "Base.h"
#include "Enums.h"
#include "ServerFile.h"
#include "PkiStoreConfiguration.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		class MethodCallTransaction;
		class MethodCallRequest;
		/*! The ServerTrustList class is used to represents the TrustList of a server which supports GDS push management.
		*
		* A TrustList object represents the content of a PKI store in the servers address space.
		* Typically only a session with role EnumNumericNodeId_WellKnownRole_SecurityAdmin shall be able to access this content.
		*
		* The TrustList contains mandatory properties to describe how GDS can access the server.
		* All properties will be updated by SDK.
		* @li LastUpdateTime
		* @li UpdateFrequency (optional)
		*
		* The ServerTrustList handles UA methods which are invoked by GDS to read the trust list content. */
		class TBC_EXPORT TrustList
			: public File
		{
		public:
			/*! Constructs a new TrustList instance */
			static ObjectPointer<TrustList> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			TrustList(TBHandle otbHandle);

			virtual void handleCallRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);

			/*! Set the PKI store configuration to be represented.
			*
			* Only the paths are relevant - check options are not represented.
			*
			* @param pkiStoreConfiguration	The PKI store configuration to be used. */
			EnumStatusCode setPkiStoreConfiguration(const PkiStoreConfiguration& pkiStoreConfiguration);
			/*! Get the represented PKI store configuration.
			*
			* @see setPkiStoreConfiguration */
			EnumStatusCode getPkiStoreConfiguration(PkiStoreConfiguration& pkiStoreConfiguration) const;


		protected:
			/*! Default constructor */
			TrustList();

		public:
			/*! Destructor */
			virtual ~TrustList();

		private:
			/*! Forbid use assignment operator */
			TrustList& operator=(const TrustList&);
			/*! Forbid use of copy constructor */
			TrustList(const TrustList&);
		};
		typedef ObjectPointer<TrustList> TrustListPtr;
		typedef ObjectPointer<const TrustList> TrustListConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif	// SERVERTRUSDTLIST_H
