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

#ifndef CLIENTCERTIFICATECHANGE_H
#define CLIENTCERTIFICATECHANGE_H
#include "Base.h"
#include "Enums.h"
#include "ByteString.h"

namespace SoftingOPCToolbox5
{
	namespace Client
	{
		/*! The CertificateChangeTransaction notifies a client application if the servers certificate has been changed.
		*
		* A servers certificate may change while a client session is connected, e.g. if a certificate renewal is necessary.
		* Typically the session is disconnected for a short time and re-connected after the server's certificate change.
		* A client application normally is notified about untrusted certificates during the connect process (or it
		* validates the servers certificate after the getEndpointsFromServer).
		* If a changed certificate is detected during a re-connect (see Client::Session:setAutoReconnectInterval) the session
		* may stay disconnected forever.
		*
		* Using this transaction a client application can decide how such a changed certificate shall be treated.
		*
		* @note This callback is even called when the session is configured with a NULL certificate and when the initial connect detects the real server certificate.
		*       Such a use case can be beneficial as every certificate validation can be done by the handling of this CertificateChangeTransaction. */
		class TBC_EXPORT CertificateChangeTransaction
			: public Base
		{
		public:
			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			CertificateChangeTransaction(TBHandle otbHandle);

		public:
			/*! Destructor */
			virtual ~CertificateChangeTransaction();

			/*! Returns the new server certificate */
			ByteString getCertificate();

			/*! The validation result of the sessions PKI store.
			*
			* A changed certificate may be already trusted, e.g. if the signing CA is trusted.
			* Nevertheless the application is notified about the change. */
			EnumStatusCode getValidationResult() const;


			/*! Completes the transaction.
			*
			* @param result		The result how to continue.
			*					A bad result will deny the actual running (re-) connect to the server.
			*					If you decide to deny the connection to the server with the new certificate you may change this session
			*					to disable auto-reconnect via Client::Session:setAutoReconnectInterval(0).
			*					Otherwise you will by notified cyclically on every re-connect cycle.
			*					If the status EnumStatusCode_BadConnectionRejected is set, then the SDK will store the rejected certificate
			*					into the RejectedCertificates folder (if this path was assigned in the PkiStoreConfiguration).
			*
			* @note If the server's new certificate is not yet trusted, you have to store it in the trusted folder of the PKI store.
			*		Otherwise the continued connect will still fail! */
			void complete(EnumStatusCode result);

		private:
			/*! Transactions are always created by the SDK core. */
			CertificateChangeTransaction();
			/*! Forbid use assignment operator */
			CertificateChangeTransaction& operator=(const CertificateChangeTransaction&);
			/*! Forbid use of copy constructor */
			CertificateChangeTransaction(const CertificateChangeTransaction&);
		};

		typedef ObjectPointer<CertificateChangeTransaction> CertificateChangeTransactionPtr;
		typedef ObjectPointer<const CertificateChangeTransaction> CertificateChangeTransactionConstPtr;

	} // end client namespace
} // toolbox namespace end

#endif	// CLIENTCERTIFICATECHANGE_H
