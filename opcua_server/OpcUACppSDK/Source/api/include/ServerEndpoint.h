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

#ifndef SERVERENDPOINT_H
#define SERVERENDPOINT_H
#include "Base.h"
#include "Enums.h"
#include "InnerUserTokenPolicy.h"
#include "UserTokenPolicy.h"
#include "IUserIdentityToken.h"
#include "InnerSignatureData.h"
#include "SignatureData.h"
#include "InnerApplicationDescription.h"
#include "ApplicationDescription.h"
#include "INodeId.h"
#include "ISignedSoftwareCertificate.h"
#include "PkiStoreConfiguration.h"

namespace SoftingOPCToolbox5
{
	// Forward declarations
	namespace Server
	{
		class Session;

		class DeleteSessionRequest;
		typedef ObjectPointer<DeleteSessionRequest> DeleteSessionRequestPtr;
	}

	namespace Server
	{
		/*! An endpoint defines a physical address by means of a URL that clients may
		* use to connect to the OPC UA server. A server may provide multiple endpoints.
		*
		* An endpoint further defines the supported set of
		* @li	message security settings a client may choose from when connecting to the endpoint
		*		(i.e. whether and how messages need to be signed and/or encrypted)
		* @li	user authentication mechanisms (anonymous, user/password, X.509, ...) */
		class TBC_EXPORT Endpoint
			: public Base
		{
		public:
			/*! Constructs a new Endpoint instance */
			static ObjectPointer<Endpoint> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory. */
			Endpoint(TBHandle otbHandle);

		protected:
			/*! Default constructor */
			Endpoint();

		public:
			/*! Destructor */
			virtual ~Endpoint();

			/*! Returns the sessions that belong to this endpoint. */
			std::vector<ObjectPointer<Server::Session> > getSessions();
			/*! @overload */
			std::vector<ObjectPointer<const Server::Session> > getSessions() const;


			//////////////////////////////////////////////////////////////////////////
			// Endpoint configuration
			//////////////////////////////////////////////////////////////////////////
			/*! Makes the endpoint available to clients. Only after calling this method clients are
			* able to connect to the server using this endpoint.
			*
			* The endpoint URL as well as the security configurations must be set up before calling this method.
			*
			* @see close()
			*
			* @note This method implicitly initializes the SDK namespace to the next free namespace index if the SDK namespace
			*		was not yet set (see AddressSpaceRoot::getToolkitNamespaceIndex). */
			EnumStatusCode open();

			/*! Returns whether the endpoint is open, i.e. whether it is accessible by clients.
			*
			* @see Endpoint::open() Endpoint::close() */
			bool getIsOpen() const;

			/*! Makes the endpoint unavailable to clients. All sessions related to that endpoint will be closed,
			* and all subscriptions belonging to these sessions will be deleted.
			*
			* @see open()
			*
			* @note This method does not return before all attached sessions, subscription and monitored items are deleted,
			*       which will invoke callbacks to the application. */
			EnumStatusCode close();

			/*! Returns the URL of the endpoint.
			*
			* @see setUrl() */
			tstring getUrl() const;
			/*! Sets the URL of the endpoint.
			* The URL has the format "<transport>://<host or IP>[:<port>][/<url-path>]".
			* Supported transport are "opc.tcp" and "https"
			* Omitted port means default port 4840.
			*
			* @note The URL can not be changed while the endpoint is open (see Endpoint::open() and Endpoint::close()).
			*
			* @note Ensure that the host name can be resolved in the network if the host name is specified.
			*
			* @note Some clients do not provide a HTTPS certificate for the services GetEndpoints and FindServers. The SDK rejects any HTTPS connection where no SSL certificate is used.
			*       Those clients may fail in connecting to a server developed with the SDK. */
			EnumStatusCode setUrl(const tstring& url);

			/*! Adds a new message security configuration for this endpoint. A security configuration specifies
			* a set of message security settings which a client may use when connecting to this endpoint.
			*
			* Depending on the security needs a server may choose from one of the following security policies
			* which define the algorithms that are used to sign and/or encrypt exchanged messages:
			* @li	SecurityPolicyUri_None
			* @li	SecurityPolicyUri_Basic128Rsa15 (deprecated - shall no longer be used)
			* @li	SecurityPolicyUri_Basic256 (deprecated - shall no longer be used)
			* @li	SecurityPolicyUri_Basic256Sha256
			* @li	SecurityPolicyUri_Aes128Sha256RsaOaep
			* @li	SecurityPolicyUri_Aes256Sha256RsaPss
			*
			* @note The security configurations can not be modified while the endpoint is open (see Endpoint::open() and Endpoint::close()). */
			EnumStatusCode addSecurityConfiguration(EnumMessageSecurityMode messageSecurityMode, const tstring& securityPolicy);

			/*! Sets multiple message security configurations for this endpoint and replaces all existing configurations.
			*
			* @note The security configurations can not be modified while the endpoint is open (see Endpoint::open() and Endpoint::close()).
			* @see addSecurityConfiguration() */
			EnumStatusCode setSecurityConfigurations(const std::vector<EnumMessageSecurityMode>& securityModes, const std::vector<tstring>& securityPolicies);

			/*! Gets all configured message security configurations of this endpoint.
			*
			* @see addSecurityConfiguration() */
			EnumStatusCode getSecurityConfigurations(std::vector<EnumMessageSecurityMode>& securityModes, std::vector<tstring>& securityPolicies) const;


			/*! Adds an accepted user authentication mechanism for this endpoint by means of a user token policy.
			*
			* @note	Please note that every endpoint must define at least one user token policy.
			*		If a server application does not require user authentication, it may want to
			*		define a policy that permits anonymous access to the server.
			*
			* @note The user token policies can not be modified while the endpoint is open (see Endpoint::open() and Endpoint::close()).
			*
			* @see onAuthenticateUser() */
			EnumStatusCode addUserTokenPolicy(const IUserTokenPolicy* policy);

			/*! Sets multiple user token policies for this endpoint and replaces all existing policies.
			*
			* @note The user token policies can not be modified while the endpoint is open (see Endpoint::open() and Endpoint::close()).
			* @see addUserTokenPolicy() */
			EnumStatusCode setUserTokenPolicies(const std::vector<UserTokenPolicy>& securityPolicies);

			/*! Get all user token policies of this endpoint.
			*
			* @see addUserTokenPolicy() */
			std::vector<UserTokenPolicy> getUserTokenPolicies() const;

			/*! Configures rejection of repeated connection attempts
			*
			* A server may monitor repeated attempts for denied user authentication.
			* In case of repeated denied attempt it is likely that a client tries to guess user credentials.
			*
			* In this case SDK provides means to lock continued attempts from a specific host.
			*
			* @param maxFailedAttempts	After this number of failed attempts the client is denied automatically for a while
			* @param validationPeriod	The number of failed attempts within this period are counted (in ms)
			* @param lockPeriod			If number of failed attempts exceed configured number, this period the client is locked (in ms)
			*							A lock period of 0 means no locking is done (default). */
			EnumStatusCode configureRepeatedConnectAttempts(OTUInt32 maxFailedAttempts, OTUInt32 validationPeriod, OTUInt32 lockPeriod);

			/*! Returns whether the endpoint is restricted to a single network interface or
			* listens to all network interfaces of this machine.
			*
			* @see setRestrictToSingleInterface */
			bool isRestrictedToSingleInterface();

			/*! Sets whether to restrict the accepted connections to a single network interface (IP address).
			* By default the endpoint accepts connections on all network interfaces of the machine at the port specified in the URL.
			* If listening is restricted to a single IP address (IPv4 or IPv6) a client can connect only to the IP address in the URL.
			*
			* @param restrictToSingleInterface	@li true: A client can connect only to the IP address (IPv4 or IPv6) specified in the URL
			*									@li false: A client can connect to all network interfaces of the machine
			*
			* @note The endpoint restriction can not be changed while the endpoint is open (see Endpoint::open() and Endpoint::close()).
			*
			* @note @p Endpoints @p on @p same @p port
			* @note
			*       If multiple endpoints (of the same server or different servers on the same machine) are using the same port all endpoints should be restricted to a single interface.
			* @note
			*       If more than one endpoint is restricted to a specific interface all endpoints can be opened as long they use different interfaces (IP addresses).
			* @note
			*       If more than one endpoint is NOT restricted to a specific interface the endpoint opened first succeeds, all others will fail to open.
			* @note
			*       If there is a mixture in restriction the behavior is operating system dependent:
			*       @li On Windows a mixture of restricted / not restricted endpoints on the same port is possible.
			*           The unrestricted endpoint will serve all interfaces not used by open restricted endpoint.
			*           If the restricted endpoint is closed, the unrestricted endpoint will also serve this interface.
			*           This behavior is independent on the order of opening.
			*       @li On Linux a mixture of restricted / not restricted endpoints on the same port is NOT possible.
			*           The endpoint opened first (restricted or not) succeeds, the other will fail to open. */
			EnumStatusCode setRestrictToSingleInterface(bool restrictToSingleInterface);

			/*! Sets the location of the application instance certificate and its private key file used for this endpoint.
			*
			* This certificate is used for authentication in secured connections for this endpoint.
			* If not specified, the application instance certificate specified in Applcation::setInstanceCertificate() is used.
			*
			* @param applicationInstanceCertificate The location of the application instance certificate.
			* @param privateKeyFile                 The location of the certificate's private key file.
			* @param password                       The pass phrase that was used to protect the private key.
			*
			* @note The certificate is used only during opening the endpoint, thus the application instance certificate must be set before opening the endpoint
			*       and changes on a open endpoint only take place after close at the next open.
			*
			* @note The URL in the alternate subject of the certificate shall match the URL in the namespace array at position 1.
			*		Thus a specific certificate of an endpoint shall use the same URL as in the certificate used in Application::setInstanceCertificate(). */
			EnumStatusCode setInstanceCertificate(const tstring& applicationInstanceCertificate, const tstring& privateKeyFile, const tstring& password);

			/*! Gets the location of the configured application instance certificate for this endpoint.
			*
			* @see setInstanceCertificate */
			tstring getInstanceCertificate() const;

			/*! Gets the certificate information of the application instance certificate for this endpoint.
			*
			* @see setInstanceCertificate */
			ByteString getInstanceCertificateData() const;

			/*! Gets the location of the private key file for this endpoint.
			*
			* @see setInstanceCertificate */
			tstring getPrivateKeyFile() const;

			/*! Gets the information for certificate validation of server application instance certificates used for this endpoint.
			*
			* @param pkiStoreConfiguration			The PKI store configuration used for this session.
			* @return								The status code if configuration could be retrieved. */
			EnumStatusCode getPkiStoreConfiguration(IPkiStoreConfiguration* pkiStoreConfiguration) const;

			/*! Sets the information for certificate validation of server application instance certificates used for this endpoint.
			*
			* If not specified, the PKI store configuration specified in Application::setPkiStoreConfiguration() is used.
			*
			* @param pkiStoreConfiguration			The PKI store configuration used for this session.

			* @note This method must be called before opening this endpoint. */
			EnumStatusCode setPkiStoreConfiguration(const IPkiStoreConfiguration* pkiStoreConfiguration);

			/*! Configure the endpoint for reverse connectivity to a client.
			*
			* The reverse connect feature allows a server to initiate the connection to a client.
			* The server tries cyclically to open physical connections (sockets) to the client and and send a "reverse hello" message.
			* A client listening for such sockets and "reverse hello" messages can use these to create a "secure channel",
			* e.g. to connect a Client::Session.
			*
			* @param clientUrl			The URL where the client listens for reverse connections.
			* @param maxConnections		The maximum number of connections allowed for this client.
			*
			* @note This method can be invoked at any time, before or after opening this endpoint.
			* @note Every client URL can be configured only once.
			*       To alter an existing configuration remove and re-add it with new settings. */
			EnumStatusCode addReverseConnect(const tstring clientUrl, OTUInt32 maxConnections = 2);

			/*! Removes a reverse connectivity configuration.
			*
			* Once the reverse connect configuration is removed, no new physical connections to the client are established.
			*
			* @param clientUrl			Client application URL.
			*
			* @note This method can be invoked at any time, before or after opening / closing this endpoint.
			*		Removing the configuration on an open endpoint will keep existing sessions (logical connections)
			*		alive until the session timeout expires or the endpoint is closed.
			*
			* @note All open connections (secure channels) which use this configuration are closed immediately. */
			EnumStatusCode removeReverseConnect(const tstring clientUrl);

			//////////////////////////////////////////////////////////////////////////
			// User authentication
			//////////////////////////////////////////////////////////////////////////

			/*! This method is called whenever the server needs to verify the identity of a user.
			* An authenticated user must be assigned to a user group that holds the user access rights
			* for variables and methods (see IAccessPattern).
			*
			* This method shall return EnumStatusCode_Good, if the server accepts the passed user identity.
			* If the server could not verify the user identity or does not accept the token type,
			* one of the following status codes may be returned:
			* @li EnumStatusCode_BadIdentityTokenInvalid for invalid (i.e. undefined) identity tokens
			* @li EnumStatusCode_BadIdentityTokenRejected for token types that the server does not support
			* @li EnumStatusCode_BadUserAccessDenied, if the user identity could not be verified
			*
			* @param userIdentityToken			The user token that is used for authentication
			* @param tokenSignature				This parameter can be ignored
			* @param session					The session for which the user wants to authenticate
			* @param applicationDescription		The application description provided by the client. The application description
			*									may be required if authentication is based on applications rather than users.
			* @param [out] userGroupIndex		The index of a user group to which the user is assigned
			*									Note: The user group index is used only if Application::enableUserAccessPattern is invoked.
			*									Otherwise you shall assign roles to the session while validating users.
			*
			* @note	Please note that this method is also called when a client provides an anonymous user identity.
			*		By default, only anonymous access is granted; all other token types are rejected.
			*
			* @note This method is invoked with the endpoint locked.
			*       Do not lock any application specific mutex within this callback which is involved with API methods regarding this object.
			*
			* @see validateUserCertificate() IAccessPattern setUserCertificateTrustListLocation() */
			virtual EnumStatusCode onAuthenticateUser(const IUserIdentityToken* userIdentityToken, const ISignatureData* tokenSignature,
				Server::Session* session, const IApplicationDescription* applicationDescription, OTUInt32* userGroupIndex);

			/*! Verifies that the given @p x509Certificate provided by the onAuthenticateUser() callback method
			* is trusted. In order for the certificate to be trusted
			* @li for self-signed certificates: the certificate must be available in the user certificate trust list or
			* @li for certificates signed by a CA: the CA chain must be available in the user certificate trust list.
			*
			* @see X509IdentityToken setUserCertificateTrustListLocation() onAuthenticateUser() */
			EnumStatusCode validateUserCertificate(const IByteString* pX509Certificate) const;

			/*! @deprecated Please use getUserPkiConfiguration() instead. */
			DEPRECATED(tstring getUserCertificateTrustListLocation() const);
			/*! @deprecated Please use getUserPkiConfiguration() instead. */
			DEPRECATED(EnumStatusCode getUserCertificateTrustListLocation(const tstring& certificateTrustListLocation));
			/*! @deprecated Please use setUserPkiConfiguration() instead. */
			DEPRECATED(EnumStatusCode setUserCertificateTrustListLocation(const tstring& certificateTrustListLocation));

			/*! @deprecated Please use getUserPkiConfiguration() instead. */
			DEPRECATED(tstring getUserCertificateIssuerListLocation() const);
			/*! @deprecated Please use setUserPkiConfiguration() instead. */
			DEPRECATED(EnumStatusCode setUserCertificateIssuerListLocation(const tstring& issuerListLocation));

			/*! @deprecated Please use getUserPkiConfiguration() instead. */
			DEPRECATED(tstring getUserCertificateRevocationListLocation() const);
			/*! @deprecated Please use setUserPkiConfiguration() instead. */
			DEPRECATED(EnumStatusCode setUserCertificateRevocationListLocation(const tstring& certificateRevocationListLocation));


			/*! Gets the information for user certificate validation if users authenticate via certificate.
			*
			* @see setUserPkiConfiguration() */
			PkiStoreConfiguration getUserPkiConfiguration() const;
			/*! Sets the information for user certificate validation if users authenticate via certificate.
			*
			* @see IPkiStoreConfiguration
			*
			* @note This method must be called only if a server application exposes a user authentication via certificate. */
			EnumStatusCode setUserPkiConfiguration(const IPkiStoreConfiguration* pkiConfiguration);


			//////////////////////////////////////////////////////////////////////////
			// Callbacks
			//////////////////////////////////////////////////////////////////////////

			/*! Callback that is called when a client wants to create a session.
			* This callback enables a server application to deny the creation of a session.
			* If the function is overridden and the result is not good, the session
			* will not be created.
			*
			* @note This method is invoked with the endpoint locked.
			*       Do not lock any application specific mutex within this callback which is involved with API methods regarding this object. */
			virtual EnumStatusCode onCreateSession(Server::Session* session);

			/*! Callback that is called when a session is deleted on the server.
			* It allows an application to count the number of running sessions (which may be required for
			* a denial of a further session).
			*
			* The response to the client is sent after the request is completed (i.e. complete method is called).
			*
			* @note	When overriding this method you mustn't use blocking calls within this method since
			* this blocks the UA communication.
			* Processing and completion of the request can be executed within a separate thread if blocking calls are necessary. */
			virtual void handleDeleteSession(Server::DeleteSessionRequestPtr deleteSessionRequest);

			/*! Callback that will be triggered when a client tries to open a new secure channel or to renew an existing secure channel.
			*
			* This callback enables a server application to deny the creation of a secure channel.
			* If the function is overridden and the result is not good, the secure channel
			* will not be created.
			*
			* @param secureChannelId				The new or renewed ID for the secure channel which was assigned from the stack
			* @param clientCertificate				The certificate of the client that opened the secure channel.
			*										The ByteString can represent a single certificate or a certificate chain.
			* @param clientCertificateThumbprint	The thumbprint of the client certificate
			* @param requestType					Type of the request
			* @param securityPolicy					Requested security policy
			* @param securityMode					Requested message security mode
			* @param requestedLifetime				Requested lifetime of the secure channel
			*
			* @note This callback won't be invoked if the request is already rejected due to security reasons by the stack. */
			virtual EnumStatusCode onOpeningSecureChannel(OTUInt32 secureChannelId,
				const IByteString& clientCertificate,
				const tstring& clientCertificateThumbprint,
				EnumOpenRequest requestType,
				const tstring& securityPolicy,
				EnumMessageSecurityMode securityMode,
				double requestedLifetime);

			/*! Callback that is triggered when a client tries to create or renew a new secure channel.
			*
			* The default implementation raises a matching audit event.
			* This callback is invoked for every CreateSecureChannelRequest, no matter if the creation is accepted or rejected.
			*
			* @param result							The security validation status for the OpenSecureChannel request
			* @param secureChannelId				The string representation of the new or renewed ID for the secure channel
			*										which was assigned from the stack
			* @param clientCertificate				The certificate of the client that opened the secure channel
			*										The ByteString can represent a single certificate or a certificate chain.
			* @param clientCertificateThumbprint	The thumbprint of the client certificate
			* @param requestType					Type of the request
			* @param securityPolicy					Requested security policy
			* @param securityMode					Requested message security mode
			* @param requestedLifetime				Requested lifetime of the secure channel */
			virtual void onSecureChannelOpened(EnumStatusCode result,
				const tstring& secureChannelId,
				const IByteString* clientCertificate,
				const tstring& clientCertificateThumbprint,
				EnumOpenRequest requestType,
				const tstring& securityPolicy,
				EnumMessageSecurityMode securityMode,
				double requestedLifetime);

			/*! Callback that will be triggered when a client session was created.
			*
			* The default implementation produces a matching audit event.
			*
			* @param result							The status code of the request
			* @param auditEntryId					The human readable ID given by the client request
			* @param secureChannelId				The ID of the secure channel
			* @param clientCertificate				The certificate of the client creating the session
			*										The ByteString can represent a single certificate or a certificate chain.
			* @param clientCertificateThumbprint	The thumbprint of the client certificate
			* @param session						The newly created session (can be NULL in case of failure) */
			virtual void onSessionCreated(EnumStatusCode result,
				const tstring& auditEntryId,
				const tstring& secureChannelId,
				const IByteString* clientCertificate,
				const tstring& clientCertificateThumbprint,
				const Session* session);

			/*! Callback that will be triggered when a client session was activated.
			*
			* The default implementation produces a matching audit event.
			*
			* @param result						The status code of the request
			* @param auditEntryId				The human readable ID given by the client request
			* @param sessionId					The ID of the session.
			* @param signedSoftwareCertificates	The software certificates provided by the client that activated the session
			* @param userIdentityToken			The identity of the user activating the session */
			virtual void onSessionActivated(EnumStatusCode result,
				const tstring& auditEntryId,
				const INodeId* sessionId,
				const std::vector<const ISignedSoftwareCertificate*> signedSoftwareCertificates,
				const IUserIdentityToken* userIdentityToken);

			/*! Callback that will be triggered when a client used an endpoint URL
			* which does not match the exposed URLs.
			*
			* The default implementation produces a matching audit event.
			* The server does not reject the creation of a session when URLs do not match.
			*
			* @param result							The status code of the request
			* @param auditEntryId					The human readable ID given by the client request
			* @param secureChannelId				The ID of the secure channel
			* @param clientCertificate				The certificate of the client that created the session
			*										The ByteString can represent a single certificate or a certificate chain.
			* @param clientCertificateThumbprint	The thumbprint of the client certificate
			* @param session						The newly created session (can be NULL in case of failure)
			* @param endpointUrl					The URL of the CreateSession service call */
			virtual void onUrlMismatch(EnumStatusCode result,
				const tstring& auditEntryId,
				const tstring& secureChannelId,
				const IByteString* clientCertificate,
				const tstring& clientCertificateThumbprint,
				const Session* session,
				const tstring& endpointUrl);

			/*! Callback that will be triggered when the certificate that has been provided by the client
			* when establishing a connection could not be validated.
			*
			* The default implementation produces a matching audit event.
			*
			* @param error					The result of the certificate validation
			* @param certificate			The application instance certificate of the client
			*								The ByteString can represent a single certificate or a certificate chain.
			* @param securityPolicy			The security policy the client used to establish the secure channel
			* @param messageSecurityMode	The message security the client used to establish the secure channel */
			virtual EnumStatusCode onInvalidCertificate(
				EnumStatusCode error,
				const ByteString& certificate,
				const tstring& securityPolicy,
				EnumMessageSecurityMode messageSecurityMode);

		private:
			/*! For internal use only */
			void initMembers();

			/*! Forbid use assignment operator */
			Endpoint& operator=(const Endpoint&);
			/*! Forbid use of copy constructor */
			Endpoint(const Endpoint&);
		};
		typedef ObjectPointer<Endpoint> EndpointPtr;
		typedef ObjectPointer<const Endpoint> EndpointConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif	// SERVERENDPOINT_H
