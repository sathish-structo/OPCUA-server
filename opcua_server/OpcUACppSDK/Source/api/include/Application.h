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

#ifndef APPLICATION_H
#define APPLICATION_H

#include "Base.h"
#include "Enums.h"
#include "InnerApplicationDescription.h"
#include "ApplicationDescription.h"
#include "InnerMonitoringChange.h"
#include "MonitoringChange.h"
#include "EndpointDescription.h"
#include "AccessPattern.h"
#include "IDataValue.h"
#include "INodeId.h"
#include "PkiStoreConfiguration.h"
#include "ServerVariable.h"
#include "ApiMacros.h"

namespace SoftingOPCToolbox5
{
	// Forward declarations
	namespace Client
	{
		class Session;
		typedef ObjectPointer<Session> SessionPtr;
		class RedundantSession;
		typedef ObjectPointer<RedundantSession> RedundantSessionPtr;
		class ReverseListener;
	}
	namespace Server
	{
		class Endpoint;
		typedef ObjectPointer<Endpoint> EndpointPtr;
		class BaseNode;
	}

	namespace PubSub
	{
		class Connection;
		typedef ObjectPointer<Connection> ConnectionPtr;
		class PublishedDataSet;
		typedef ObjectPointer<PublishedDataSet> PublishedDataSetPtr;
		class SubscribedDataSet;
		typedef ObjectPointer<SubscribedDataSet> SubscribedDataSetPtr;
	}
	/*! Root class for all server and client applications. The Application is a singleton which may exist
	* only once.
	*
	* The application class is used for server implementations as well as for client implementations where the
	* initialization defines whether an application is an OPC UA server, an OPC UA client, or both.
	* After the initialization the application type can not be changed anymore. */
	class TBC_EXPORT Application
		: public Base
	{
	public:
		/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
		Application(TBHandle otbHandle);

	public:
		/*! Destructor */
		virtual ~Application();

		/*! Returns the singleton instance */
		static ObjectPointer<Application> instance();
		/*! @overload */
		static ObjectPointer<const Application> constInstance();

		/////////////////////////////////////////////////////////////////////////
		// Common methods of class Application
		/////////////////////////////////////////////////////////////////////////

		/*! Initializes the application instance with the given @p description.
		*
		* This method initializes the communication stack as well as SDK-internal static variables.
		* A bad return code signals a principle problem (e.g. DLL can not be loaded) and the installation shall
		* be checked for errors.
		*
		* This shall be the first call on the application object after the configuration of the application object.
		*
		* @param description	An object describing the application instance. The description defines
		*						whether the application is a server or client (or both).
		*
		* @see uninitialize() start() */
		EnumStatusCode initialize(const IApplicationDescription* description);
		/*! @overload
		*
		* Initializes the application with the settings from an XML configuration file
		*
		* @param configFile			The full path to the configuration XML file to load
		* @param privateKeyPassword	The pass phrase that was used to protect the private key
		* @param extensions			List of unknown extension XML elements found in the configuration file. Not returned if NULL. */
		EnumStatusCode initialize(const tstring& configFile, const tstring& privateKeyPassword, std::vector<XmlElement>* extensions = NULL);

		/*! Uninitializes the application. This must be the last call on the application instance.
		* Every call on the application after that call (except of initialize()) will return an error.
		*
		* A server application will close all connected Sessions and Endpoints.
		* A client application will disconnect all Sessions and child objects (e.g. Subscriptions).
		*
		* @see initialize() stop() */
		EnumStatusCode uninitialize();

		/*! Starts the application.
		*
		* Within this method SDK internal threads are spawned and
		* Please note that you have to initialize the application and setInstanceCertificate before this method can be called.
		*
		* @see initialize() stop()
		*
		* @note UA communication cannot be performed before calling this method. */
		EnumStatusCode start();

		/*! Stops the application. This method shall be called before calling uninitialize() to
		* close all running connections.
		*
		* For server applications, this method closes all open Sessions and Endpoints.
		* For client applications, this method closes all Sessions and Subscriptions.
		*
		* @see uninitialize() start() */
		EnumStatusCode stop();

		/*! Activates the given feature in case of a valid license code.
		*
		* @param product The product feature to activate
		* @param licenseKey The license key for the given @p product feature */
		EnumStatusCode activateLicense(EnumFeature product, const tstring& licenseKey);

		/*! Creates a new self-signed certificate for this application.
		*
		* This certificate is used for authentication in secured connections, no matter whether your
		* application is a client, a server or both.
		*
		* @param securityProfileUri     The security profile used for certificate creation.
		* @param certificateFileName    The path and name of the file to store the certificate (in DER format).
		* @param privateKeyFileName     The path and name of the file to store the private key (in PEM format).
		* @param privateKeyPassword     The password to encrypt the private key.
		* @param validityDuration       The duration of the certificate validity period in seconds.
		*                               The start time of the validity period is the creation time of the certificate.
		* @param productName            The product name to be used as the CommonName (CN) in the certificate.
		* @param organizationName       The name of the organization that executes the server. This is usually not the vendor of the server.
		*                               This name is used as Organization (O) in the certificate.
		* @param hostNames              A comma separated list, containing the IP addresses and host names, that can be used to reach the server.
		*                               Each list entry has to be preceded by "IP:" or "DNS:".
		*                               Example: "DNS:MyMachine.MyDomain, IP:192.168.0.1"
		*
		* @note The initialize() method has to be invoked first, to specify application URI and other information. */
		EnumStatusCode createSelfSignedCertificate(const tstring& securityProfileUri,
			const tstring& certificateFileName,
			const tstring& privateKeyFileName,
			const tstring& privateKeyPassword,
			OTUInt32 validityDuration,
			const tstring& productName,
			const tstring& organizationName,
			const tstring& hostNames) const;

		/*! @deprecated Please use setInstanceCertificate() instead.
		* Sets the location of the application instance certificate. */
		DEPRECATED(EnumStatusCode loadCertificate(const tstring& applicationInstanceCertificate) const);

		/*! @deprecated Please use setInstanceCertificate() instead.
		* Sets the location of the private key of the application instance certificate. */
		DEPRECATED(EnumStatusCode loadPrivateKey(const tstring& privateKeyFile, const tstring& password) const);

		/*! Sets the location of the application instance certificate and its private key file.
		* This certificate is used for authentication in secured connections, no matter whether your
		* application is a client, a server or both. For server applications with secured connections, you need to additionally
		* define the location of the folder that contains the client certificates to accept (see setPkiStoreConfiguration())
		*
		* The certificate is loaded during start().
		*
		* @param applicationInstanceCertificate The location of the application instance certificate.
		* @param privateKeyFile                 The location of the certificate's private key file.
		* @param password                       The pass phrase that was used to protect the private key.
		*
		* @note The location of the application instance certificate must be set before starting the application,
		*       while the application is started; any changes on the path or on the file don't have an effect
		*       until the application is stopped and restarted.
		* @note Client applications which don't use any security don't need an application instance certificate
		*       and this method call can be left out. */
		EnumStatusCode setInstanceCertificate(const tstring& applicationInstanceCertificate, const tstring& privateKeyFile, const tstring& password) const;

		/*! Gets the location of the configured application instance certificate.
		*
		* @see setInstanceCertificate */
		tstring getInstanceCertificate() const;

		/*! Gets the certificate information of the application instance certificate.
		*
		* @see setInstanceCertificate */
		ByteString getInstanceCertificateData() const;

		/*! Gets the location of the private key file.
		*
		* @see setInstanceCertificate */
		tstring getPrivateKeyFile() const;

		/*! Gets the password of the private key.
		*
		* @see setInstanceCertificate */
		tstring getPrivateKeyPassword() const;

		/*! Sets the location of the discovery server's application instance certificate. The certificate is used
		* to authenticate the discovery server. In client applications, the
		* certificate is used during findServers(). In server applications, the certificate is used
		* for authentication during server registration.
		*
		* @param discoveryServerCertificate The location of the discovery server's application instance certificate
		*
		* @see findServers() enableServerRegistration() */
		EnumStatusCode loadDiscoveryServerCertificate(const tstring& discoveryServerCertificate) const;

		/*! Sets the location of the server's DH parameter file.  The DH parameter file is used
		* by the HTTPS endpoint to generate single use DH keys for each TLS session.
		* The DH parameter file is necessary to ensure Perfect Forward Secrecy.
		*
		* @param dhParamFileLocation The location of the server's DH parameter file
		*
		* @note Setting this file is no longer necessary as the Stack ensures the Perfect Forward Secrecy via Elliptic Curves.
		*       This is only necessary if the Elliptic Curve Implementation of OpenSSL is disabled */
		EnumStatusCode setDhParamFileLocation(const tstring& dhParamFileLocation) const;

		/*! Parses an Endpoint URL and splits it into the single portions.
		*
		* The URL has the format "<transport>://<host or IP>[:<port>][/<url-path>]".
		*
		* @param url		The endpoint URL to parse
		* @param transport				Returns the used transport protocol
		* @param host					Returns the used hostname or IP address as string
		* @param port					Returns the used port number
		* @param path					Returns the used optional path including all slashes
		* @param pImpliciteDefaultPort	If the port part ":port" is omitted, then true, else false will be returned. */
		static bool parseEndpointURL(tstring url, tstring& transport, tstring& host, OTUInt16& port, tstring& path, bool* pImpliciteDefaultPort = NULL);

#if TB5_CLIENT
		/////////////////////////////////////////////////////////////////////////
		// Client-only methods of class Application
		/////////////////////////////////////////////////////////////////////////

		/*! Adds the given Session to the application. The session shall be created and configured before adding it
		* to the application.
		* A Session object can only be used after adding it to the application. A bad result code will be returned
		* in case of an invalid configuration of the given Session.
		*
		* @param session The session to add
		*
		* @see Client::Session
		* @note Client-only method. */
		EnumStatusCode addSession(Client::SessionPtr session);

		/*! Returns the sessions that belong to this application.
		*
		* @see addSession() removeSession() Client::Session
		* @note Client-only method.
		* @note The order of the returned values is not defined and can be random */
		std::vector<ObjectPointer<Client::Session> > getSessions();
		/*! @overload */
		std::vector<ObjectPointer<const Client::Session> > getSessions() const;

		/*! Removes the given session from the application. All child objects of that session will be also removed
		* and are not available anymore (e.g. Subscriptions).
		*
		* @param session The session to remove. The session must be disconnected from the server.
		*
		* @see Client::Session
		*
		* @note	Please note that the session must be disconnected (see Client::Session::disconnect()) before
		*		it can be removed from the application.
		* @note Client-only method. */
		EnumStatusCode removeSession(Client::SessionPtr session);

		/*! Adds the given RedundantSession to the application. The session shall be created and configured before adding it
		* to the application.
		* A RedundantSession object can only be used after adding it to the application. A bad result code will be returned
		* in case of an invalid configuration of the given RedundantSession.
		*
		* @param redundantSession The RedundantSession to add
		*
		* @see Client::RedundantSession
		* @note Client-only method. */
		EnumStatusCode addRedundantSession(Client::RedundantSessionPtr redundantSession);

		/*! Returns the RedundantSessions that belong to this application.
		*
		* @see addRedundantSession() removeRedundantSession() Client::RedundantSession
		* @note Client-only method.
		* @note The order of the returned values is not defined and can be random */

		std::vector<ObjectPointer<Client::RedundantSession> > getRedundantSessions();
		/*! @overload */
		std::vector<ObjectPointer<const Client::RedundantSession> > getRedundantSessions() const;

		/*! Removes the given RedundantSession from the application. All child objects of that session will be also removed
		* and are not available anymore (e.g. Subscriptions).
		*
		* @param redundantSession The RedundantSession to remove. The RedundantSession must be disconnected from the server.
		*
		* @see Client::RedundantSession
		*
		* @note	Please note that the RedundantSession must be disconnected (see Client::RedundantSession::disconnect()) before
		*		it can be removed from the application.
		* @note Client-only method. */
		EnumStatusCode removeRedundantSession(Client::RedundantSessionPtr redundantSession);

		/*! Retrieves the list of registered servers from a dedicated discovery server.
		* This method will first establish a connection with the discovery server, and then invokes the "FindServers" service.
		*
		* The establishment of connection is executed synchronously.
		* Keep in mind that this call can block up to the configured network timeout (@see setDefaultNetworkTimeout).
		* Do not invoke this method in a thread context that has to serve other tasks in parallel.
		*
		* @param discoveryServerUrl				The URL of the discovery server
		* @param serverURIs						A list of servers to return. All registered servers are returned if
		*										this list is empty.
		* @param[out] applicationDescriptions	A list of the registered servers.
		* @param pkiStoreConfiguration		The PKI store configuration used for this service (optional, if not specified the general application setting is used)
		* @param pInstanceCertificateFile	The filename of instance certificate used for this service (optional, if not specified the general application setting is used)
		* @param pPrivateKeyFile			The filename private key used for this service (optional, if not specified the general application setting is used)
		* @param pPassword					The password for private key used for this service (optional, if not specified the general application setting is used)
		* @param hostnameResoultion			The resolving of hostname part of the URL
		*
		* @note Client-only method. */
		EnumStatusCode findServers(const tstring& discoveryServerUrl, const std::vector<tstring>& serverURIs,
			std::vector<ApplicationDescription>& applicationDescriptions,
			const IPkiStoreConfiguration* pkiStoreConfiguration = NULL,
			const tstring* pInstanceCertificateFile = NULL, const tstring* pPrivateKeyFile = NULL, const tstring* pPassword = NULL,
			EnumHostnameResolution hostnameResoultion = EnumHostnameResolution_PreferIpV4) const;

		/*! Retrieves the list of endpoints from the server located at @p serverUrl by invoking
		* the server's "GetEndpoints" service.
		*
		* This method will first establish an unsecured connection with the server, and then invoke
		* the "GetEndpoints" service.
		*
		* The establishment of connection is executed synchronously.
		* Keep in mind that this can block up to the configured network timeout (@see setDefaultNetworkTimeout).
		* Do not invoke this method in a thread context that has to serve other tasks in parallel.
		*
		* The returned list of endpoints may be restricted by defining transport profiles which
		* endpoints have to support. The OPC UA specification currently defines the following transport
		* profiles:
		* @li	TransportProfileUri_UaTcp_UaSc_UaBinary
		* @li	TransportProfileUri_Https_UaBinary
		* @li	TransportProfileUri_Https_UaXml
		* @li	TransportProfileUri_SoapHttp_UaSc_UaBinary
		* @li	TransportProfileUri_SoapHttp_WsSc_UaBinary
		* @li	TransportProfileUri_SoapHttp_WsSc_UaXml
		* @note Currently only TransportProfileUri_UaTcp_UaSc_UaBinary and TransportProfileUri_Https_UaBinary are supported.
		*
		* @param serverUrl					The disconvery endpoint URL of the server to ask for endpoint dscriptions. You may use findServers() to
		*									determine server URLs.
		* @param transportProfiles			A list of transport profiles that the endpoints should support. An empty
		*									list indicates that no restriction is applied.
		* @param[out] endpointDescriptions	The list of endpoints that the server provides
		* @param pkiStoreConfiguration		The PKI store configuration used for this service (optional, if not specified the general application setting is used)
		* @param pInstanceCertificateFile	The filename of instance certificate used for this service (optional, if not specified the general application setting is used)
		* @param pPrivateKeyFile			The filename private key used for this service (optional, if not specified the general application setting is used)
		* @param pPassword					The password for private key used for this service (optional, if not specified the general application setting is used)
		* @param hostnameResolution			The resolving of hostname part of the URL
		* @param reverseListener			The reverse listener used for this server as configured in ReverseListener
		* @param serverUri					The server URI as specified in reverse listener
		*
		* @note Client-only method.
		*
		* @note If using a reverse listener for this call, you have to specify the server URL and server URI identical to the configuration
		*		as used in ReverseListener::setServerInformation */
		EnumStatusCode getEndpointsFromServer(const tstring& serverUrl, const std::vector<tstring>& transportProfiles,
			std::vector<EndpointDescription>& endpointDescriptions,
			const IPkiStoreConfiguration* pkiStoreConfiguration = NULL,
			const tstring* pInstanceCertificateFile = NULL, const tstring* pPrivateKeyFile = NULL, const tstring* pPassword = NULL,
			EnumHostnameResolution hostnameResolution = EnumHostnameResolution_PreferIpV4,
			const Client::ReverseListener* reverseListener = NULL, const tstring &serverUri = _T("")) const;

#endif	// TB5_CLIENT
		/*! Returns the maximum number of monitored items created at server side per service call.
		*
		* @see setMaxMonitoredItemsPerService() */
		OTUInt32 getMaxMonitoredItemsPerService() const;
		/*! Sets the maximum of monitored items to be created within one service call.
		*
		* The number is limited by the message size, depending on the request size (e.g. filter, NodeId, ...).
		* On the other hand when creating too many monitored items at server side with one single call, the request may take
		* very long and other requests are not invoked.
		*
		* @param maxMonitoredItemsPerService The maximum number of monitored items to be created (see above for further explanation)
		*
		* @note Default value is 1000.
		* @note Client-only method. */
		EnumStatusCode setMaxMonitoredItemsPerService(OTUInt32 maxMonitoredItemsPerService);


		/////////////////////////////////////////////////////////////////////////
		// Server-only methods of class Application
		/////////////////////////////////////////////////////////////////////////

		/*! Adds the given endpoint to the application. The endpoint has to be configured properly
		* before it is added to the Application. An Endpoint may be added only once to an application.
		*
		* @param endpoint The Endpoint to add
		*
		* @see Server::Endpoint
		* @note Server-only method. */
		EnumStatusCode addEndpoint(Server::EndpointPtr endpoint);

		/*! Returns the endpoints that belong to this application.
		*
		* @see addEndpoint() removeEndpoint() Server::Endpoint
		* @note Server-only method.
		* @note The order of the returned values is not defined and can be random */
		std::vector<ObjectPointer<Server::Endpoint> > getEndpoints();
		/*! @overload */
		std::vector<ObjectPointer<const Server::Endpoint> > getEndpoints() const;

		/*! Removes the given endpoint from the application.
		*
		* @param endPoint The Endpoint to remove
		*
		* @see Server::Endpoint
		* @note Server-only method. */
		EnumStatusCode removeEndpoint(Server::EndpointPtr endPoint);

		/*! Callback method that will be called in case of any change in MonitoredItems,
		* e.g. if an item is added, removed or changed. Overriding this method, the server is able to adapt the I/O
		* to external data sources to the new monitoring requirements.
		*
		* For example a server that has to retrieve data also by subscriptions from devices or other servers must
		* be able to adapt these subscriptions to the requirements from the clients.
		*
		* @param nodes		The monitored nodes that are affected by the change.
		* @param changes	Describes the changes regarding monitoring of the affected @p nodes. This vector
		*					has the same size as the @p nodes vector.
		*
		* @note Server-only method. */
		virtual EnumStatusCode handleMonitoringChange(const std::vector<Server::BaseNode*>& nodes, const std::vector<Server::MonitoringChange>& changes);

		/*! Enables the registration of the server with a discovery server. In order to keep
		* the list of registered servers up-to-date, the discovery server requires each registered
		* server to re-register periodically.
		*
		* @param discoveryServerUrl		The URL of the discovery server
		* @param securityPolicy			The security policy used to connect to the discovery server.
		*								See Client::Session::setSecurityPolicy() for further reference on security policies.
		* @param messageSecurityMode	The message security mode used to connect to the discovery server.
		*								See Client::Session::setMessageSecurityMode() for further reference.
		* @param refreshPeriod			The period in milliseconds in which the (re-)registration of the server application
		*								is performed. A period of 10 minutes (i.e. 600000ms) is recommended.
		* @param serverCapabilities		The set of Server capabilities supported by the Server.
		*
		* @note The first registration operation will be scheduled to be executed immediately.
		* @note Server-only method. */
		EnumStatusCode enableServerRegistration(const tstring& discoveryServerUrl,
			const tstring& securityPolicy,
			EnumMessageSecurityMode messageSecurityMode,
			OTUInt32 refreshPeriod,
			const std::vector<tstring>& serverCapabilities = std::vector<tstring>(1, _T("NA")));

		/*! Disables the cyclic registration of the server with a discovery server.
		*
		* @see enableServerRegistration()
		* @note Server-only method. */
		EnumStatusCode disableServerRegistration();

		/*! Allows to use the former way of access patterns to configure user access.
		*
		* By enabling the access pattern configuration, the new configuration via RolePermissions is not possible.
		*
		* @note This method should be called after Application::initialize() and before using any AccessPattern related methods.
		* @note This method must be called before Application::start()
		* @note Once enabled, the usage of AccessPattern cannot be reset
		* @note Server-only method - has no effect on client applications
		*
		* @see Server::BaseNode::setRolePermissions(), Server::Session::setRoles() */
			DEPRECATED(EnumStatusCode enableUserAccessPattern());

		/*! Adds or updates an access pattern which defines the user access rights for
		* variables and methods.
		*
		* An error is returned if the access pattern's key is invalid or if the
		* application is not a server.
		*
		* @note Server-only method.
		* @note The access field of all set access patterns should always be large enough to handle all user groups,
		*		not configured user groups don't get any user access rights and the SDK will produce log error messages.
		*
		* @note This method is deprecated. Please use BaseNode::setRolePermissions instead.
		*		If you still like to use this method, please take care that Application::enableUserAccessPattern() is invoked
		*		first to enable the former way of user access right configuration.
		* @note This method implicitly initializes the SDK namespace to the next free namespace index if the SDK namespace
		*		was not yet set (see AddressSpaceRoot::getToolkitNamespaceIndex). */
		DEPRECATED(EnumStatusCode setAccessPattern(const IAccessPattern* accessPattern));

		/*! Returns an array of the currently defined AccessPatterns.
		*
		* @note Server-only method. */
		DEPRECATED(EnumStatusCode getAccessPatterns(std::vector<AccessPattern>& accessPatterns) const);

		/*! Returns a BYTE value describing the access level of the specified user or users group, within the specified
		* AccessPattern.
		*
		* @note Server-only method.
		*
		* @note This method is deprecated. Please use BaseNode::getUserPermissions instead. */
		DEPRECATED(EnumStatusCode getUserAccessLevel(OTUInt32 accessPatternKey, OTUInt32 userGroupIdx, OTUInt8* userAccessLevel) const);

		/*! Returns whether the given group of services is enabled for a
		* server application.
		*
		* @see setEnableServiceGroup()
		* @note Server-only method.
		* @note By default, all services are enabled in a server application. */
		bool isServiceGroupEnabled(EnumServiceGroup serviceGroup) const;
		/*! Enables or disables a specific group of services in a server application.
		* This method can be used to limit the functionality of the server
		* application.
		*
		* @param serviceGroup The group of services to enable/disable
		* @param enable If true, the given group of services is enabled
		*
		* @note Server-only method.
		* @note By default, all services are enabled in a server application. */
		EnumStatusCode setEnableServiceGroup(EnumServiceGroup serviceGroup, bool enable);

		/*! Returns if the creation of the default addresspace shall be suppressed or not.
		*
		* @see suppressAddresspaceCreation()
		* @note Server-only method */
		bool getSuppressAddresspaceCreation() const;
		/*! Suppresses or enables the creation of the default addresspace.
		* The default behavior is that the default addresspace will be created.
		*
		* @note Server-only method
		* @note This method must be called before initialize() */
		EnumStatusCode suppressAddresspaceCreation(bool suppress);

		/*! Configures rejection of repeated connection attempts
		*
		* A server may monitor repeated attempts for denied user authentication.
		* In case of repeated denied attempt it is likely that a client tries to guess user credentials.
		*
		* In this case SDK provides means to lock continued attempts from a specific host.
		* Here the default for all endpoints to be created can be configured.
		*
		* @param maxFailedAttempts	After this number of failed attempts the client is denied automatically for a while
		* @param validationPeriod	The number of failed attempts within this period are counted (in ms)
		* @param lockPeriod			If number of failed attempts exceed configured number, this period the client is locked (in ms)
		*							A lock period of 0 means no locking is done (default).
		*
		* @note Server-only method */
		EnumStatusCode configureRepeatedConnectAttempts(OTUInt32 maxFailedAttempts, OTUInt32 validationPeriod, OTUInt32 lockPeriod);

		/////////////////////////////////////////////////////////////////////////
		// Common Attributes of class Application
		/////////////////////////////////////////////////////////////////////////

		/*! Returns the application description that has been provided with the initialization
		* of the application.
		*
		* @see initialize() */
		const IApplicationDescription* getDescription() const;

		/*! Returns the list of preferred locales
		*
		* @param[out] preferredLocales	An ordered list of preferred locales
		*
		* @see setPreferredLocales() */
		EnumStatusCode getPreferredLocales(std::vector<tstring>* preferredLocales) const;
		/*! Sets the list of preferred locales. This ordered list contains locales conforming to RFC 3066.
		* The first list entry contains the locale with the highest priority.
		*
		* For client applications, this list is passed to the server to retrieve the best matching
		* localized text value in requests (e.g. when a client reads the DisplayName attribute
		* of a node). The preferred locales in application is used as default for new client session.
		*
		* For server applications, the provided list of preferred locales is used to decide which
		* localized text value to deliver to the client if it has not provided a list of preferred locales
		* or if the server does not have a matching translation for the requested locales.
		*
		* @param preferredLocales	An ordered list of preferred locales. Locales must adhere to RFC 3066.
		*							The first entry contains the locale with the highest priority.
		*
		* @see Client::Session::setPreferredLocales() */
		EnumStatusCode setPreferredLocales(const std::vector<tstring>& preferredLocales);

		/*! Returns the timeout for a service group.
		*
		* @see setServiceTimeout()
		* @note Client-only method. */
		OTUInt32 getServiceTimeout(EnumTimeoutGroup group) const;
		/*! Sets the timeout for a specific service groups.
		*
		* If a server does not respond to a service within the timeout a EnumStatusCode_BadTimeout is returned.
		*
		* For a server, this timeout is only a hint and can be used to cancel long running operations to free resources.
		*
		* The timeouts set in the Application object are used as default when creating a session.
		*
		* @param group		A specific timeout group
		* @param timeout	The timeout for this group
		*
		* @see EnumTimeoutGroup for mapping of timeout groups and methods
		* @note Client-only method. */
		EnumStatusCode setServiceTimeout(EnumTimeoutGroup group, OTUInt32 timeout);

		/*! Returns the maximum number of piped publish request.
		*
		* @see setMaxPipedPublishRequests() */
		OTUInt32 getMaxPipedPublishRequests() const;
		/*! Sets the number of publish requests which are supported.
		*
		* For client applications the provided number of @p maxPipedPublishRequests limits the number
		* of publish requests the client sends to a server. At any point in time, the client will not fill
		* the server's publish request queue with more than the given number of requests.
		*
		* For server applications @p maxPipedPublishRequests sets the maximum number of received publish requests
		* a server session store in its publish request queue. If this number has been reached and more publish requests
		* are received from a client, the oldest publish requests from the queue will be answered with status
		* code EnumStatusCode_BadTooManyPublishRequests.
		* A server shall support at least the number of supported/expected subscriptions per session.
		*
		* @param maxPipedPublishRequests The maximum number of publish requests (see above for further explanation)
		*
		* @note Default value is 5. */
		EnumStatusCode setMaxPipedPublishRequests(OTUInt32 maxPipedPublishRequests);

		/*! Returns the major version of the underlying SDK.
		*
		* @see getToolboxMinorVersion() getToolboxPatchVersion() */
		unsigned short getToolboxMajorVersion() const;

		/*! Returns the minor version of the underlying SDK.
		*
		* @see getToolboxMajorVersion() getToolboxPatchVersion() */
		unsigned short getToolboxMinorVersion() const;

		/*! Returns the patch version of the underlying SDK.
		*
		* @see getToolboxMajorVersion() getToolboxMinorVersion() */
		unsigned short getToolboxPatchVersion() const;

		/*! Returns the build number of the underlying SDK. */
		OTUInt32 getToolboxBuildNumber() const;

		/*! Returns the evaluation time (in milliseconds) for unlicensed SDK installations.
		* After the demo time has elapsed, the SDK stops working.
		*
		* @see setDemoTime() */
		OTUInt32 getDemoTime() const;
		/*! Sets the evaluation time (in milliseconds) for unlicensed SDK installations.
		*
		* The demo time is used to stop functionality after demo time (in ms) is expired if no license is activated.
		* The demo time cannot be set to a value greater than 90 minutes (90 * 60 * 1000).
		*
		* @note Default value is 90 * 60 * 1000, i.e. 90 minutes. */
		EnumStatusCode setDemoTime(OTUInt32 demoTime);

		/*! Returns the channel sharing configuration.
		*
		* @see setChannelSharing() */
		bool getChannelSharing() const;
		/*! Sets the channel sharing configuration.
		*
		* The delivered stack has a hard-coded maximum of 50 secure channel. Increasing this limit needs re-compilation of the deliverables.
		*
		* Channel sharing is used to share one secure channel between multiple client sessions.
		* A secure channel can be shared only if URL, security configuration, ... is identical for client sessions.
		*
		* @note The effect at server side is server implementation dependent.
		*       The server can handle each secure channel within a separate thread and increase performance by parallel execution.
		*       However exhausting work at server side should be executed asynchronously in general, so parallel execution can be implemented for a single secure channel as well.
		*
		* @note Default value is false, i.e. channels are not shared. */
		EnumStatusCode setChannelSharing(bool channelSharing);

		/*! Returns whether the own certificate is sent as certificate chain
		*
		* @see setSendOwnCertificateAsChain */
		bool getSendOwnCertificateAsChain();
		/*! Configures whether the SDK sends its own certificate or user certificates as chain.
		*
		* In case the own certificate or a user certificate is signed by a Certificate Authority (CA), it is recommended to send the entire
		* certificate chain instead of just the leaf certificate.
		* This recommendation applies for the services that are used to initiate an OPC UA communication so that other OPC UA applications
		* are able to validate and setup the full trust chain.
		*
		* When enabled, the SDK will search automatically for the issuer(s) of the own application instance certificate (if any)
		* to send the full certificate chain during the communication initiating services.
		* The issuers of the own certificate must be stored either at the issuer list location of the application PKI store or in the same
		* directory as the own application instance certificate.
		*
		* The SDK uses this setting for the following services:
		* Client:
		* @li OpenSecureChannelRequest
		* @li CreateSessionRequest
		*
		* Server:
		* @li GetEndpointsResponse
		* @li CreateSessionResponse
		*
		* The default value is false
		*
		* @note This automatic support is just for the application instance certificate, chains for X509 user certificates must be created manually by the client application.
		* @note This setting has no effect for self-signed certificates
		* @note This setting affects only new connections, already connected Client::Sessions and already opened Server::Endpoints are not affected
		* @see IX509IdentityToken */
		void setSendOwnCertificateAsChain(bool sendAsChain);
		
		/*! Validates an application instance certificate.
		*
		* The certificate is validated with the given PKI store.
		* The PKI store configuration specifies the validation options.
		*
		* @see IPkiStoreConfiguration
		*
		* @param pkiStoreConfiguration	The PKI store configuration used for validation.
		* @param pCertificate			The certificate to be validated.
		* @param pApplicationUri		The application URI of the application the certificate belongs to.
		*								This parameter can be NULL if the PKI store configuration does not request URI check or if a user certificates is validated.
		*								@see IPkiStoreConfiguration::checkUrlMatch()
		* @note The rejected certificates location within @p pkiStoreConfiguration will be ignored by this method. */
		EnumStatusCode validateCertificate(const IPkiStoreConfiguration* pkiStoreConfiguration, const IByteString *pCertificate, const tstring* pApplicationUri);

		/////////////////////////////////////////////////////////////////////////
		// Client-only Attributes of class Application
		/////////////////////////////////////////////////////////////////////////

		/*! Returns the default timeout (in milliseconds) to be used when trying to establish connections over the network.
		*
		* @see setDefaultNetworkTimeout()
		* @note Client-only method. */
		OTUInt32 getDefaultNetworkTimeout() const;
		/*! Sets the default timeout to be used when trying to establish connections over the network.
		* This value will be used as default value for all new sessions, but may be overridden for each session.
		*
		* @param defaultNetworkTimeout The default network timeout in milliseconds
		*
		* @see Client::Session::setNetworkTimeout()
		* @note Default value is 60000, i.e. 60s.
		* @note Client-only method. */
		EnumStatusCode setDefaultNetworkTimeout(OTUInt32 defaultNetworkTimeout);

		/*! @deprecated use Application::getServiceTimeout to get service specific timeouts. */
		DEPRECATED(OTUInt32 getDefaultRequestTimeoutHint() const);
		/*! @deprecated use Application::setServiceTimeout to set service specific timeouts.
		*
		* Sets the request timeout (in milliseconds) used for all service calls issued on this session.
		*
		* @param defaultRequestTimeoutHint	Timeout in milliseconds for a service */
		DEPRECATED(EnumStatusCode setDefaultRequestTimeoutHint(OTUInt32 defaultRequestTimeoutHint));

		/*! Returns the default interval in (in ms) when a client tries to re-connect of an interrupted session.
		*
		* @see setDefaultAutoReconnectInterval()
		* @note Client-only method. */
		OTUInt32 getDefaultAutoReconnectInterval() const;
		/*! Sets the default interval (in ms) when a client tries to re-connect of an interrupted session.
		*
		* All client sessions will use this value when created.
		* A value of 0 means no re-connect is done at all. In this case the application is responsible to repair interrupted sessions.
		* Minimum supported interval is 1000 ms, Typical values are e.g. 10000, 20000.
		*
		* @see Client::Session::setAutoReconnectInterval()
		* @note Default value is 0, i.e. automatic re-connect of sessions is disabled.
		* @note Client-only method. */
		EnumStatusCode setDefaultAutoReconnectInterval(OTUInt32 defaultAutoReconnectInterval);

		/*! Returns the default interval (in ms) for testing the connection to a server.
		*
		* @see setDefaultConnectionMonitoringInterval()
		* @note Client-only method. */
		OTUInt32 getDefaultConnectionMonitoringInterval() const;
		/*! Sets the default interval (in ms) for testing the connection to a server.
		*
		* All client sessions will use this value when created.
		* If connection break is recognized, the status of the session changes to disconnected.
		* Interval 0 means no connection test at all.
		* Minimum supported interval is 100 ms, typical intervals are between 5000 and 10000.
		*
		* @see Client::Session::setConnectionMonitoringInterval()
		* @note Default value is 0, i.e. monitoring of connections is disabled.
		* @note Client-only method. */
		EnumStatusCode setDefaultConnectionMonitoringInterval(OTUInt32 defaultConnectionMonitoringInterval);


		/*! Sets the information for certificate validation of client instance certificates.
		*
		* @see IPkiStoreConfiguration
		*
		* @note The PKI store configuration shall be set for all server and client applications.
		*		The only exception is if the application does not use any security related functionality
		*		including encryption / decryption of user authentication tokens. */
		EnumStatusCode setPkiStoreConfiguration(const IPkiStoreConfiguration* pkiStoreConfiguration);
		/*! Gets the information for certificate validation of client instance certificates.
		*
		* @see setPkiStoreConfiguration() */
		PkiStoreConfiguration getPkiStoreConfiguration() const;

		/*! Configures the SDK to use a GDS for certificate management.
		*
		* A GDS application maintains both application instance and user certificate (gdsGetCertificateStatus, gdsCreateSignedCertificateAsync) and
		* trust relationship (content of PKI store trusted, issuer, CRL, ...).
		*
		* UA servers can use the pull or push management model for maintenance of certificates via GDS.
		* UA clients can use the pull model only.
		* For GDS support of the pull model the SDK needs some information to be configured via this method.
		*
		* @param gdsUrl				Endpoint URL where the GDS can be accessed.
		* @param userIdentity		The user identity to authenticate at GDS as a trusted user.
		* @param securityProfileUri	The security profile used for certificate creation.
		* @param updateCycle		The period in milliseconds how often the SDK accesses the GDS for the pull management model is accessed to check whether new trust list information is available.
		*							The period will be revised to be within a reasonable range between 5 minutes and 7 days.
		*							A cycle time of 0 means that automatic update is disabled; an application may still use other GDS specific pull-model methods.
		*							The automatic update covers ONLY the certificate and PKI store for the application instance certificates.
		*							Other certificates and PKI stores (e.g. for users, specific PKI stores, ...) must be updated via
		*							Application implementation.
		*							Please implement renewal via gdsGetCertificateStatus, gdsCreateSignedCertificateAsync or gdsGetTrustListAsync by your own.
		*
		* @note	This configuration is not required for a pure push management model, where the GDS or another application takes care to register this server application.
		*
		* @note	The PKI store needs to be configured before GDS configuration (see setPkiStoreConfiguration).
		*
		* @note The PKI configuration needs to be configured using PkiStoreConfiguration::setBasePath (other configurations will not work). */
		EnumStatusCode gdsSetConfiguration(const tstring& gdsUrl, const IUserIdentityToken& userIdentity, tstring securityProfileUri, OTUInt32 updateCycle);
		/*! Gets the GDS configuration.
		*
		* @see setGDSConfiguration() */
		EnumStatusCode gdsGetConfiguration(tstring& gdsUrl, UserIdentityToken& userIdentity, tstring& securityProfileUri, OTUInt32& updateCycle) const;

		/*! Get the GDS assigned a NodeId of an application.
		*
		* A GDS application assigns a NodeId when an application is registered. This NodeId identifies
		* the application whenever a UA method at the GDS shall be invoked.
		*
		* @return The GDS assigned NodeId. If not yet registered, a NULL NodeId is returned. */
		NodeId gdsGetApplicationId() const;

		/*! Registers the application at GDS.
		
		* A GDS application assigns a NodeId when an application is registered. This NodeId identifies
		* the application whenever a UA method at the GDS shall be invoked.
		
		* SDK registers the application at GDS when starting. If a connection to GDS cannot be established
		* at start up, an application can register at GDS at any time.
		*
		* @note Whenever any SDK method for GDS is invoked, SDK tries to register automatically if not yet done.
		*		An explicit invocation of gdsRegisterApplication is not needed in this case.
		*		Explicit registration is only needed if an application likes to invoke GDS methods by its own to
		*		have a valid GdsApplicationId available.
		*
		* @see gdsGetApplicationId */
		EnumStatusCode gdsRegisterApplication() const;

		/*! Retrieves the certificate groups from GDS for this application.
		
		* A GDS application may provide multiple certificate groups for an application.
		* At least a certificate group "DefaultApplicationGroup" should be provided.
		* Note that this DefaultApplicationGroup is NOT the NodeId EnumNumericNodeId_ServerConfiguration_CertificateGroups_DefaultApplicationGroup
		* of the default namespace, but the NodeId with numeric identifier 615 in the GDS namespace.
		*
		* A GDS can provide much more certificate groups, e.g. DefaultUserTokenGroup or other specific certificate groups.
		*
		* @note Whenever any SDK method for GDS is invoked, SDK tries to register automatically if not yet done.
		*		An explicit invocation of gdsRegisterApplication is not needed in this case.
		*		Explicit registration is only needed if an application likes to invoke GDS methods by its own to
		*		have a valid GdsApplicationId available.
		*
		* @param certificateGroups	The NodeIds of the supported certificate groups.
		* @param displayNames		The display names of the nodes.
		* @param certificateTypes	The certificate types supported by the groups.
		*
		* @note The sizes of the vectors are the same. */
		EnumStatusCode gdsGetCertificateGroups(std::vector<NodeId> &certificateGroups, std::vector<LocalizedText> &displayNames,
			std::vector<std::vector<NodeId> > &certificateTypes) const;

		/*! Starts creation of a new GDS signed certificate.
		*
		* An application instance certificate can be used for secured connections, no matter whether your
		* application is a client, a server or both.
		* A user certificate can be used by a client application to authenticate a user via X509 certificate at a server.
		*
		* It is recommended to create new certificate / private key at a different location than provided to SDK e.g. in setInstanceCertificate.
		* In case of unexpected errors (or power fails) it is possible that the old configuration is already deleted and not yet written.
		*
		* The results are notified via gdsCreateSignedCertificateCompleted.
		* In case of a successful creation, the new certificate and private key can be used to replace the old ones.
		*
		* @param pCookie				A cookie which will be returned in gdsCreateSignedCertificateCompleted.
		* @param securityProfileUri		The security profile used for certificate creation.
		* @param privateKeyPassword		The password to encrypt the private key.
		* @param commonName				The name to be used as the CommonName (CN) in the certificate.
		*								For instance certificates this is the product name, for user certificates this is the user name.
		* @param organizationName		The name of the organization that executes the server. This is usually not the vendor of the server.
		*								This name is used as Organization (O) in the certificate.
		* @param hostNames				A comma separated list, containing the IP addresses and host names, that can be used to reach the server.
		*								Each list entry has to be preceded by "IP:" or "DNS:".
		*								Example: "DNS:MyMachine.MyDomain, IP:192.168.0.1"
		*								For user certificates this parameter is ignored.
		* @param isUserCertificate		If true, an user certificate shall be created.
		* @param certificateGroup		The certificate group where the certificate shall belong to.
		*								Application instance certificates are typically created in DefaultApplicationGroup (NULL NodeId).
		*
		* @note The application needs to be started first.
		*		Also GDS and PKI has to be configured first.
		*
		* @note A GDS may delay requests until a GDS administrator has accepted the request.
		*		In this case the callback will return with the status BadNothingToDo and a pending request ID, which can be finished later.
		*		The sign request is marked as pending when the GDS doesn't complete the sign request within 5 tries where each retry has a delay of 1 second.
		*
		* @note If this method returns a bad status code, the callback will not be invoked. */
		EnumStatusCode gdsCreateSignedCertificateAsync(
			void* pCookie,
			const tstring& securityProfileUri,
			const tstring& privateKeyPassword,
			const tstring& commonName,
			const tstring& organizationName,
			const tstring& hostNames,
			bool isUserCertificate = false,
			const NodeId& certificateGroup = NodeId()) const;

		/*! Callback method for gdsCreateSignedCertificateAsync and gdsFinishCertificateAsync.
		*
		* This method is invoked when the creation of signed certificate is completed or when the GDS does not finish the request
		* within a certain period.
		* The default implementation does nothing.
		* If an application instance certificate was created, an application can use gdsReplaceInstanceCertificate to replace
		* the existing public and private key.
		*
		* @param pCookie				The cookie provided in gdsCreateSignedCertificateAsync.
		* @param creationResult			The result of the certificate creation.
		* @param certificate			The data of the new certificate.
		* @param privateKey				The data of the new private key.
		* @param privateKeyPassword		The password of the private key (provided in gdsCreateSignedCertificateAsync).
		* @param isUserCertificate		The information passed in gdsCreateSignedCertificateAsync.
		* @param certificateGroup		The information passed in gdsCreateSignedCertificateAsync.
		* @param issuerCertificates		The certificate(s) used to sign the new certificate.
		*								An application can store these certificates into the "trusted" or "issuer" folder if needed.
		* @param pendingRequestId		If the creationResult is EnumStatusCode_BadNothingToDo the creation request is pending.
		*								In this case the pending request ID (from GDS) can be used to finish certificate creation later via
		*								gdsFinishCertificateAsync
		*
		* @see gdsCreateSignedCertificateAsync */
		virtual void gdsCreateSignedCertificateCompleted(
			void* pCookie,
			EnumStatusCode creationResult,
			const IByteString& certificate,
			const IByteString& privateKey,
			const tstring& privateKeyPassword,
			bool isUserCertificate,
			const INodeId& certificateGroup,
			const std::vector<ByteString> issuerCertificates,
			const INodeId& pendingRequestId);

		/*! Tries to finish a pending sign certificate request on the GDS.
		*
		* A GDS may delay sign requests due to any long lasting operation, e.g. wait until a GDS administrator accepts this application
		* for certificate deployment.
		*
		* The results are notified via gdsCreateSignedCertificateCompleted.
		* In case of a successful creation, the new certificate and private key can be used to replace the old ones.
		*
		* @param pCookie				A cookie which will be returned in the next response of gdsCreateSignedCertificateCompleted.
		* @param pendingRequestId		The pending request ID returned from the previous response of gdsCreateSignedCertificateCompleted.
		*
		* @note The next response of gdsCreateSignedCertificateCompleted may indicate that the sign request is still not completed by the GDS.
		*		This method call can be repeated until the request is complete.
		*
		* @note Finishing is only possible if a valid request ID is used, which was reported from the previous response at gdsCreateSignedCertificateCompleted.
		*		If an error occurs here, a new gdsCreateSignedCertificateAsync shall be invoked.
		*
		* @note If this method returns a bad status code, the callback will not be invoked. */
		EnumStatusCode gdsFinishCertificateAsync(
			void* pCookie,
			const NodeId& pendingRequestId) const;

		/*! Method to replace own certificate and private key after new creation.
		*
		* This method can be invoked during the callback gdsCreateSignedCertificateCompleted.
		*
		* @param certificate			The data of the new certificate.
		* @param privateKey				The data of the new private key.
		* @param privateKeyPassword		The password to encrypt the private key.
		* @param issuerCertificates		The certificate(s) used to sign the new certificate.
		*								The SDK will store these certificates in the same folder as the certificate folder provided in setInstanceCertificate.
		*
		* @note While replacing the PKI content all server endpoints using this PKI store will be closed and re-opened to avoid concurrent file access.
		*		This will cause a communication interruption of all sessions connected to these endpoints.
		*
		* @note Client sessions will stay open using the old certificate. If the the server re-news the secure channel (and fails due to certificate validation)
		*		a new secure channel will be created which uses the new certificate.
		*
		* @see gdsCreateSignedCertificateAsync */
		EnumStatusCode gdsReplaceInstanceCertificate(const ByteString& certificate,
			const ByteString& privateKey,
			const tstring& privateKeyPassword,
			const std::vector<ByteString>& issuerCertificates);

		/*! Method to store certificate and private key to files.
		*
		* This method can be invoked in gdsCreateSignedCertificateCompleted.
		* For application instance certificates gdsReplaceInstanceCertificate should be used. Other certificates can be stored using this method.
		*
		* @param certificate			The data of the new certificate (in DER file format).
		* @param certificateFile		The file where the certificate shall be stored.
		* @param privateKey				The data of the new private key (in PEM file format).
		* @param privateKeyFile			The file where the private key shall be stored.
		*
		* @note The formats of certificate and private key are provided by gdsCreateSignedCertificateAsync as expected.
		*		The private key data is already protected by password (if specified).
		*
		* @see gdsCreateSignedCertificateAsync */
		EnumStatusCode storeCertificate(const ByteString& certificate, const tstring& certificateFile,
			const ByteString& privateKey, const tstring& privateKeyFile);

		/*! Retrieves the status of the certificate from GDS.

		* A GDS application may provide multiple certificate groups for an application.
		* At least a certificate group "DefaultApplicationGroup" should be provided.
		* Note that this DefaultApplicationGroup is NOT the NodeId EnumNumericNodeId_ServerConfiguration_CertificateGroups_DefaultApplicationGroup
		* of the default namespace, but the NodeId with numeric identifier 615 in the GDS namespace.
		*
		* A GDS can provide much more certificate groups, e.g. DefaultUserTokenGroup or other specific certificate groups.
		*
		* @note Whenever any SDK method for GDS is invoked, SDK tries to register automatically if not yet done.
		*		An explicit invocation of gdsRegisterApplication is not needed in this case.
		*		Explicit registration is only needed if an application likes to invoke GDS methods by its own to
		*		have a valid GdsApplicationId available.
		*
		* @param needsUpdate		The information whether the certificate shall be updated.
		*							In case a certificate needs an update, please invoke gdsCreateSignedCertificateAsync to create a new certificate.
		* @param certificateGroup	The NodeIds of the certificate group which shall be used.
		*							See gdsGetCertificateGroups which groups are available.
		*							If a NULL NodeId is provided then the CertificateManager shall select the DefaultApplicationGroup.
		* @param certificateType	The NodeIds of the certificate type which shall be used.
		*							If a NULL NodeId is provided, the type shall be calculated from the certificate group.
		*
		* @note If the certificate group is default application group, then the SDK will perform additional local checks to evaluate
		*		if an update is necessary.
		*		This includes a check if the application ever got its initial certificate after registration and if the own
		*		certificate is self signed.
		*		In this case UA method on the GDS may not be invoked. */
		EnumStatusCode gdsGetCertificateStatus(bool& needsUpdate, const NodeId& certificateGroup = NodeId(), const NodeId& certificateType = NodeId()) const;

		/*! Starts reading the requested trust list from GDS.
		*
		*  When retrieving of trust list is completed, the results are notified via gdsGetTrustListCompleted.
		*
		* @param pCookie				A cookie which will be returned in gdsCreateSignedCertificateCompleted
		* @param certificateGroup		The certificate group where the trust list shall be read from.
		*								If a NULL NodeId is provided then the CertificateManager shall return the TrustListId for a suitable
		*								default group for the Application.
		*
		* @note If this method returns a bad status code, the callback will not be invoked. */
		EnumStatusCode gdsGetTrustListAsync(
			void* pCookie,
			const NodeId& certificateGroup = NodeId()) const;

		/*! Callback method for gdsGetTrustListAsync.
		*
		* This method is invoked when the reading of GDS trust list is completed.
		* Default implementation does nothing.
		*
		* @param pCookie				The cookie provided in gdsGetTrustListAsync.
		* @param certificateGroup		The certificate group provided in gdsGetTrustListAsync.
		* @param getResult				The result of getting the trust list.
		* @param trustedCertificates	A list of trusted certificates in the PKI store.
		* @param trustedCrls			A list of trusted CRLs in the PKI store.
		* @param issuerCertificates		A list of issuer certificates in the PKI store.
		* @param issuerCrls				A list of issuer CRLs in the PKI store.
		* @param lastUpdateTime			The last time when this trust list was updated on the GDS.
		*
		* @note You can use the read trust list to replace your PKI store via replacePkiContent.
		*
		* @see gdsGetTrustListAsync */
		virtual void gdsGetTrustListCompleted(
			void* pCookie,
			const INodeId& certificateGroup,
			EnumStatusCode getResult,
			const std::vector<ByteString>& trustedCertificates,
			const std::vector<ByteString>& trustedCrls,
			const std::vector<ByteString>& issuerCertificates,
			const std::vector<ByteString>& issuerCrls,
			const IDateTime& lastUpdateTime);

		/*! Replaces the contents of all trust-related PKI folders with new certificates / byte strings.
		*
		* Replacement is implemented in a fail-save way. A backup is created before replacement.
		* A power-fail at any time will either keep a full backup or a complete PKI store.
		* In case of a power-fail the backup will be restored within setBasepath.
		*
		* @param pkiStore					The PKI store where the content shall be replaced
		* @param trustedCertificates		The list of trusted certificates in DER encoding
		* @param trustedCrls				The list of trusted CRLs in DER encoding
		* @param issuerCertificates			The list of issuer certificates in DER encoding
		* @param issuerCrls					The list of issuer CRLs in DER encoding
		*									For replacing user PKI store endpoints will stay open during replacement.
		* @param lastUpdateTime				The time when the source content was last updated, the default time is now.
		*
		* @note PKI store must be configured via PkiStoreConfiguration::setBasePath.
		*
		* @note Partial replacement / merging is not possible.
		*
		* @note Already existing folders with the name "sdk_pki_backup" or "sdk_temp_pki_backup" are deleted by the SDK.
		*
		* @note While replacing PKI content all affected endpoints will be closed temporarily to avoid concurrent access. */
		EnumStatusCode replacePkiContent(const PkiStoreConfiguration& pkiStore,
			const std::vector<ByteString>& trustedCertificates,
			const std::vector<ByteString>& trustedCrls,
			const std::vector<ByteString>& issuerCertificates,
			const std::vector<ByteString>& issuerCrls,
			const DateTime& lastUpdateTime = DateTime::getUtcNow());
		/*! @overload
		*
		* @param pkiStore			The PKI store where the content shall be replaced
		* @param trustListContent	The raw encoded TrustListDataType in binary encoding as it can be read from a TrustListType
		*							GDS address space via OPC UA file access.
		* @param lastUpdateTime		The time when the source content was last updated, the default time is now.
		*
		* @note While replacing the PKI content all server endpoints using this PKI store will be closed and re-opened to avoid concurrent file access.
		*		This will cause a communication interruption of all sessions connected to these endpoints. */
		EnumStatusCode replacePkiContent(const PkiStoreConfiguration& pkiStore,
			const ByteString& trustListContent,
			const DateTime& lastUpdateTime = DateTime::getUtcNow());

		/*! Returns the time when the folder contents of the application PKI store was last updated.
		*
		* The time gets updated whenever one of the following operations take place:
		* @li An application assigns a PkiStoreConfiguration via setPkiStoreConfiguration()
		* @li An application invokes replacePkiContent()
		* @li The SDK is configured for automatic GDS Pull handling and updates the PKI store automatically
		* @li A server application with GDS Push support receives a PKI store update from a GDS
		*
		* In case of an initial or new PkiStoreConfiguration, the update time is calculated from the files on the file system.
		* It takes the oldest file modification time as reference, thus the oldest time since the PKI store was completely updated.
		*
		* In case the file contents are replaced via SDK operations, then either the provided time or current time is used.
		*
		* @note The SDK does not detect if any manual file modifications on the file system take place after setPkiStoreConfiguration().
		* @note A manual copy of a file does usually NOT update the file modification time, so the last modification time usually
		*		resembles the creation time of the original file. */
		DateTime getPkiStoreLastUpdateTime() const;

		/*! Returns the default queue size for DataChange-MonitoredItems.
		*
		* @see setDefaultDataChangeMonitoringQueueSize()
		* @note Server-only method. */
		OTUInt32 getDefaultDataChangeMonitoringQueueSize() const;
		/*! Sets the default queue size for DataChange-MonitoredItems.
		*
		* The queue size determines the number of data value changes that the
		* server may queue. This number is used when a client does not request
		* a specific queue size.
		*
		* @param defaultMonitoringQueueSize The queue size of the MonitoredItems data change queue.
		*
		* @see setMaxDataChangeMonitoringQueueSize()
		* @note Default value is 1.
		* @note Server-only method. */
		EnumStatusCode setDefaultDataChangeMonitoringQueueSize(OTUInt32 defaultMonitoringQueueSize);

		/*! Returns the maximum allowed queue size for DataChange-MonitoredItems.
		*
		* @see setMaxDataChangeMonitoringQueueSize()
		* @note Server-only method. */
		OTUInt32 getMaxDataChangeMonitoringQueueSize() const;
		/*! Sets the maximum allowed queue size for DataChange-MonitoredItems.
		*
		* @param maxMonitoringQueueSize The maximum queue size of the MonitoredItems data change queue
		*
		* @see setDefaultDataChangeMonitoringQueueSize()
		* @note Default value is 1000.
		* @note Server-only method. */
		EnumStatusCode setMaxDataChangeMonitoringQueueSize(OTUInt32 maxMonitoringQueueSize);

		/*! Returns the maximum allowed queue size for Event-MonitoredItems.
		*
		* @see setMaxEventMonitoringQueueSize()
		* @note Server-only method. */
		OTUInt32 getMaxEventMonitoringQueueSize() const;
		/*! Sets the maximum allowed queue size for Event-MonitoredItems. This queue size
		* will be used for all MonitoredItem queues that contain events.
		*
		* @param maxMonitoringQueueSize The maximum queue size of the MonitoredItems data change queue
		*
		* @note Default value is 1000.
		* @note Server-only method. */
		EnumStatusCode setMaxEventMonitoringQueueSize(OTUInt32 maxMonitoringQueueSize);

		/*! Returns the minimum acceptable session timeout (in ms) for new created sessions.
		*
		* @see setMinSessionTimeout()
		* @note Server-only method. */
		OTUInt32 getMinSessionTimeout() const;
		/*! Sets the minimum acceptable session timeout (in ms) for new created sessions.
		*
		* @note Default value is 1000, i.e. 1 second
		* @note Server-only method. */
		EnumStatusCode setMinSessionTimeout(OTUInt32 minSessionTimeout);

		/*! Returns the maximum acceptable session timeout (in ms) for new created sessions.
		*
		* @see setMaxSessionTimeout()
		* @note Server-only method. */
		OTUInt32 getMaxSessionTimeout() const;
		/*! Sets the maximum acceptable session timeout (in ms) for new created sessions.
		*
		* @note Default value is 600000, i.e. 10 minutes
		* @note Server-only method. */
		EnumStatusCode setMaxSessionTimeout(OTUInt32 maxSessionTimeout);

		/*! Returns the minimum interval (in milliseconds) that may be set for updating diagnostic information in the
		* address space of a server.
		*
		* @see setMinDiagnosticsUpdateInterval()
		* @note Server-only method. */
		OTUInt32 getMinDiagnosticsUpdateInterval() const;
		/*! Sets the minimum interval (in milliseconds) that may be set for updating diagnostic information in the
		* address space of a server.
		*
		* @param minDiagnosticsUpdateInterval The minimal update interval in milliseconds for diagnostic information
		*
		* @see Server::Diagnostics
		* @note Default value is 100.
		* @note Server-only method. */
		EnumStatusCode setMinDiagnosticsUpdateInterval(OTUInt32 minDiagnosticsUpdateInterval);

		/*! Returns the maximum interval (in milliseconds) that may be set for updating diagnostic information in the
		* address space of a server.
		*
		* @see setMaxDiagnosticsUpdateInterval()
		* @note Server-only method. */
		OTUInt32 getMaxDiagnosticsUpdateInterval() const;
		/*! Sets the maximum interval (in milliseconds) that may be set for updating diagnostic information in the
		* address space of a server.
		*
		* @param maxDiagnosticsUpdateInterval The maximum update interval in milliseconds for diagnostic information
		*
		* @see Server::Diagnostics
		* @note Default value is 24 * 60 * 60 * 1000, i.e. 24 hours.
		* @note Server-only method. */
		EnumStatusCode setMaxDiagnosticsUpdateInterval(OTUInt32 maxDiagnosticsUpdateInterval);

		/*! Returns the maximum number of continuation points per session for browse services
		*
		* @see setMaxBrowseContinuationPoints()
		* @note Server-only method. */
		OTUInt16 getMaxBrowseContinuationPoints() const;
		/*! Sets the maximum number of continuation points per session for browse services
		*
		* When processing browse requests the client can restrict the number of results per request (see BrowseTransaction).
		* Also the server is allowed to return less results than the maximum requested.
		* In this case a continuation point is returned to the client, which can retrieve further results from the server.
		* Those continuation points can consume memory or other system resources, so the number shall be restricted (per session).
		*
		* @param numberOfBrowseContinuationPoints The maximum number of nodes continuation points per session for browse services.
		*
		* @note Default value is 100.
		*
		* @note Server-only method. */
		EnumStatusCode setMaxBrowseContinuationPoints(OTUInt16 numberOfBrowseContinuationPoints);

		/*! Returns the maximum number of continuation points per session for history read services
		*
		* @see setMaxHistoryContinuationPoints()
		* @note Server-only method. */
		OTUInt16 getMaxHistoryContinuationPoints() const;
		/*! Sets the maximum number of continuation points per session for history read services
		*
		* When processing history read requests the client can restrict the number of results per request.
		* (see HistoryReadRawTransaction, HistoryReadProcessedTransaction, HistoryReadEventTransaction).
		* Also the server is allowed to return less results than the maximum requested.
		* In this case a continuation point is returned to the client, which can retrieve further results from the server.
		* Those continuation points can consume memory or other system resources, so the number shall be restricted (per session).
		*
		* @param numberOfHistoryContinuationPoints The maximum number of nodes continuation points per session for history read services.
		*
		* @note Default value is 100.
		*
		* @note A single history read transaction can consume more than one continuation point.
		* The number of necessary continuation points can be up to the number of nodes within a transaction.
		*
		* @note Server-only method. */
		EnumStatusCode setMaxHistoryContinuationPoints(OTUInt16 numberOfHistoryContinuationPoints);

		/*! Returns the maximum number of continuation points per session for query services
		*
		* @see setMaxQueryContinuationPoints()
		* @note Server-only method. */
		OTUInt16 getMaxQueryContinuationPoints() const;
		/*! Sets the maximum number of continuation points per session for query services
		*
		* When processing a query the client can restrict the number of results per request.
		* Also the server is allowed to return less results than the maximum requested (see setMaxDataSetsToReturn).
		* In this case a continuation point is returned to the client, which can retrieve further results from the server.
		* Those continuation points can consume memory or other system resources, so the number shall be restricted (per session).
		*
		* @param numberOfQueryContinuationPoints The maximum number of nodes continuation points per session for query services.
		*
		* @note Default value is 10.
		*
		* @note A single query can consume more than one continuation point.
		*
		* @note Server-only method. */
		EnumStatusCode setMaxQueryContinuationPoints(OTUInt16 numberOfQueryContinuationPoints);

		/*! Returns the maximum number of data sets a server should return in a response to a query request.
		*
		* @see setMaxDataSetsToReturn()
		* @note Server-only method. */
		OTUInt32 getMaxDataSetsToReturn() const;
		/*! Sets the maximum number of data sets a server should return in a response to a query request.
		*
		* @note Default value is 0, i.e. the server does not limit the number of query data sets to return to a client.
		* @note Server-only method. */
		EnumStatusCode setMaxDataSetsToReturn(OTUInt32 maxDataSetsToReturn);

		/*! Returns the maximum number of references a server should return in a response to a Browse or BrowseNext request.
		*
		* @see setMaxReferencesToReturn()
		* @note Server-only method. */
		OTUInt32 getMaxReferencesToReturn() const;
		/*! Sets the maximum number of references a server should return in a response to a Browse or BrowseNext request.
		*
		* @note Default value is 0, i.e. the server does not limit the number of references to return to a client.
		* @note Server-only method. */
		EnumStatusCode setMaxReferencesToReturn(OTUInt32 maxReferencesToReturn);

		/*! Returns the maximum number of nodes a server shall analyze per query request
		*
		* @see setMaxNodesToAnalyzePerQueryRequest()
		* @note Server-only method. */
		OTUInt32 getMaxNodesToAnalyzePerQueryRequest() const;
		/*! Sets the maximum number of nodes a server shall analyze per query request (QueryFirst/QueryNext).
		*
		* When processing a query request, a server walks over its address space and decides
		* for each encountered node whether is it to be included in the query response or not (depending on the
		* filter provided by a client). This process may be very time consuming and may impose high load on the server.
		* Thus, in order to keep the server responsible, the number of nodes analyzed for each query request
		* should be restricted by a server application.
		*
		* @param numberOfNodesAnalyzedPerQueryRequest The maximum number of nodes in the address space which are to be analyzed per query request. The value 0 is not permitted.
		*
		* @note Default value is 100.
		* @note Server-only method. */
		EnumStatusCode setMaxNodesToAnalyzePerQueryRequest(OTUInt32 numberOfNodesAnalyzedPerQueryRequest);

		/*! Returns the minimum allowed publishing interval (in ms) for Subscriptions.
		*
		* @see setMinPublishingInterval()
		* @note Server-only method. */
		double getMinPublishingInterval() const;

		/*! Sets the minimum allowed publishing interval (in ms) for Subscriptions.
		*
		* The publishing interval requested from a client can be revised by a server. A very small publishing interval may cause high CPU load.
		* The SDK revises the requested publish interval into the defined range set.
		* Allowed range: 1.0 - 1.7976931348623158e+308
		*
		* A server application can revise the publishing interval programmatically when Server::Subscription::onCreate (onModify) is invoked,
		* using Server::Subscription::setPublishingInterval() (within the defined range).
		*
		* @param minPublishingInterval The minimum acceptable publishing interval (in ms) for Subscriptions. The value 0.0 is not permitted.
		*
		* @note Default value is 10.
		* @note Server-only method. */
		EnumStatusCode setMinPublishingInterval(double minPublishingInterval);

		/*! Returns the maximum allowed publishing interval (in ms) for Subscriptions.
		* @see setMaxPublishingInterval()
		* @note Server-only method. */
		double getMaxPublishingInterval() const;

		/*! Sets the maximum allowed publishing interval (in ms) for Subscriptions.
		*
		* The publishing interval requested from a client can be revised by a server. A very small publishing interval may cause high CPU load.
		* The SDK revises the requested publish interval into the defined range set.
		* Allowed range: 1.0 - 1.7976931348623158e+308
		*
		* A server application can revise the publishing interval programmatically when Server::Subscription::onCreate (onModify) is invoked,
		* using Server::Subscription::setPublishingInterval() (within the defined range).
		*
		* @param maxPublishingInterval The maximum acceptable publishing interval (in ms) for Subscriptions. The value 0.0 is not permitted.
		*
		* @note Default value is 10 * 60 * 1000, i.e. 10 minutes.
		* @note Server-only method. */
		EnumStatusCode setMaxPublishingInterval(double maxPublishingInterval);

		/*! Returns the minimum sampling interval (in ms) that shall be accepted for DataChange-MonitoredItems.
		*
		* @see setMinSamplingInterval()
		* @note Server-only method. */
		double getMinSamplingInterval() const;
		/*! Sets the minimum sampling interval (in ms) that shall be accepted for DataChange-MonitoredItems.
		*
		* The sampling interval requested from a client can be revised by a server.
		* The SDK revises the requested sampling interval into the defined range set.
		* Allowed range: 1.0 - 2147483647.0.
		*
		* A server application can revise the sampling interval programmatically when Server::Subscription::handleCreateMonitoredItems (handleModifyMonitoredItems) is invoked,
		* via Server::MonitoredItem::setSamplingInterval() (within the defined range).
		*
		* @param minSamplingInterval The minimum acceptable sampling interval (in ms) for DataChange-MonitoredItems.
		*
		* @note Default value is 0.
		* @note Server-only method. */
		EnumStatusCode setMinSamplingInterval(double minSamplingInterval);

		/*! Returns the maximum sampling interval (in ms) that shall be accepted for DataChange-MonitoredItems.
		*
		* @see setMaxSamplingInterval()
		* @note Server-only method. */
		double getMaxSamplingInterval() const;
		/*! Sets the maximum sampling interval (in ms) that shall be accepted for DataChange-MonitoredItems.
		*
		* The sampling interval requested from a client can be revised by a server.
		* The SDK revises the requested sampling interval into the defined range set.
		* Allowed range: 1.0 - 2147483647.0.
		*
		* A server application can revise the sampling interval programmatically when Server::Subscription::handleCreateMonitoredItems (handleModifyMonitoredItems) is invoked,
		* via Server::MonitoredItem::setSamplingInterval() (within the defined range).
		*
		* @param maxSamplingInterval The maximum acceptable sampling interval (in ms) for DataChange-MonitoredItems.
		*
		* @note Default value is 10000, i.e. 10s.
		* @note Server-only method. */
		EnumStatusCode setMaxSamplingInterval(double maxSamplingInterval);

		/*! Returns whether the server emits certain types of audit events (see setEnableAuditEvents() for details)
		*
		* @see setEnableAuditEvents()
		* @note Server-only method. */
		bool getEnableAuditEvents() const;
		/*! Sets whether the server shall emit the following audit events if the corresponding events occur:
		* @li AuditUrlMismatchEvent:		This audit event is emitted when a client creates a session with the server
		*									and the host part of the client-provided endpoint URL does not match
		*									one of the server's host names.
		* @li AuditCreateSessionEvent:		This audit event is emitted when a client creates a session with the server.
		* @li AuditActivateSessionEvent:	This audit event is emitted when a client activates a session with the server.
		* @li AuditCancelEvent:				This audit event is emitted when the server has processed a 'cancel' request
		*									from the client.
		*
		* @note Server-only method.
		* @note Default value is 'false'. */
		EnumStatusCode setEnableAuditEvents(bool enableAuditEvents);

		/*! Returns the name of the manufacturer of this application
		*
		* @note Server-only method.
		* @see setManufacturerName */
		tstring getManufacturerName() const;
		/*! Sets the name of the manufacturer of this application. This name can be set by the user implementation.
		* The manufacturer name is exposed as "ManufacturerName" variable in the
		* AddressSpace (Server/ServerStatus/BuildInfo/ManufacturerName).
		*
		* @note Default value is an empty string.
		* @note The manufacturer name must be set before the application is initialized (see initialize())
		* @note Server-only method. */
		EnumStatusCode setManufacturerName(const tstring& manufacturerName);

		/*! Returns the major version of this application.
		*
		* @see setMajorVersion()
		* @note Server-only method. */
		unsigned short getMajorVersion() const;
		/*! Sets the major version of this application. This version can be set by the user implementation.
		* The major version is exposed as part of the "SoftwareVersion" variable in the
		* AddressSpace (Server/ServerStatus/BuildInfo/SoftwareVersion).
		*
		* @see setMinorVersion() setPatchVersion()
		* @note Default value is 0.
		* @note The major version must be set before the application is initialized (see initialize())
		* @note Server-only method. */
		EnumStatusCode setMajorVersion(unsigned short majorVersion);

		/*! Returns the minor version of this application.
		*
		* @see setMinorVersion()
		* @note Server-only method. */
		unsigned short getMinorVersion() const;
		/*! Sets the minor version of this application. This version can be set by the user implementation.
		* The minor version is exposed as part of the "SoftwareVersion" variable in the
		* AddressSpace (Server/ServerStatus/BuildInfo/SoftwareVersion).
		*
		* @see setMajorVersion() setPatchVersion()
		* @note Default value is 0.
		* @note The minor version must be set before the application is initialized (see initialize())
		* @note Server-only method. */
		EnumStatusCode setMinorVersion(unsigned short minorVersion);

		/*! Returns the patch version of this application.
		*
		* @see setPatchVersion()
		* @note Server-only method. */
		unsigned short getPatchVersion() const;
		/*! Sets the patch version of this application. This version can be set by the user implementation.
		* The patch version is exposed as part of the "SoftwareVersion" variable in the
		* AddressSpace (Server/ServerStatus/BuildInfo/SoftwareVersion).
		*
		* @see setMajorVersion() setMinorVersion()
		* @note Default value is 0.
		* @note The patch version must be set before the application is initialized (see initialize())
		* @note Server-only method. */
		EnumStatusCode setPatchVersion(unsigned short patchVersion);

		/*! Returns the build number of this application.
		*
		* @see setBuildNumber()
		* @note Server-only method. */
		OTUInt32 getBuildNumber() const;
		/*! Sets the build number of this application. This number can be set by the user implementation.
		* The build number is exposed as "BuildNumber" variable in the
		* AddressSpace (Server/ServerStatus/BuildInfo/BuildNumber).
		*
		* @note Default value is 0.
		* @note The build number must be set before the application is initialized (see initialize())
		* @note Server-only method. */
		EnumStatusCode setBuildNumber(OTUInt32 buildNumber);

		/*! Returns the build date of this application
		*
		* @see setBuildDate()
		* @note Server-only method. */
		DateTime getBuildDate() const;
		/*! Sets the build date of this application. The date can be set by the user implementation.
		* The build date is exposed as "BuildDate" variable in the AddressSpace (Server/ServerStatus/BuildInfo/BuildDate)
		*
		* @note Default value is January 1, 1601, 00:00 (UTC).
		* @note The build date must be set before the application is initialized (see initialize())
		* @note Server-only method. */
		EnumStatusCode setBuildDate(const DateTime& buildDate);

		/*! Returns the redundancy mode (according to OPC UA) that the server provides to clients.
		*
		* @see setServerRedundancyMode()
		* @note Server-only method. */
		EnumRedundancySupport getServerRedundancyMode() const;
		/*! Sets the redundancy mode (according to OPC UA) that the server provides to clients.
		*
		* The supported redundancy mode is exposed as "RedundancySupport" variable in the
		* address space (Server/ServerRedundancy/RedundancySupport).
		*
		* @note Server-only method.
		* @note If any non-transparent redundancy mode is configured, also the application descriptions of all
		* redundancy partners has to be provided @see setRedundantServerArray */
		EnumStatusCode setServerRedundancyMode(EnumRedundancySupport redundancyMode);

		/*! Gets the application description of all servers that are part of the redundant array of servers.
		*
		* @see setRedundantServerArray()
		* @note Server-only method. */
		EnumStatusCode getRedundantServerArray(std::vector<ApplicationDescription>& redundantServerDescriptions) const;
		/*! Sets the application description of all servers that are part of the redundant array of servers.
		*
		* If the redundant array consists of three servers, for example, each server needs to provide
		* the application descriptions of all three servers to this method (including the description of the itself).
		*
		* The SDK uses the provided information for two purposes:
		* @li	The FindServer service of each server in the redundant array returns the provided
		*		list of application descriptions. This allows clients that know the discovery endpoint of only one server
		*		to retrieve the discovery endpoint for any other server in the redundant array.
		* @li 	The application URI contained in each application description is exposed in the address
		*		space as "ServerUriArray" variable (Server/ServerRedundancy/ServerUriArray). This
		*		allows clients to identify the URIs of all servers that are part of the redundant server array.
		*
		* @note This setting applies only servers that support non-transparent redundancy.
		* @note Server-only method. */
		EnumStatusCode setRedundantServerArray(const std::vector<ApplicationDescription>& redundantServerDescriptions);

		/*! Return the configured server profile URIs.
		*
		* @param serverProfilesUris		The profiles / facets the server application supports
		*
		* @see addServerProfileURI */
		EnumStatusCode getServerProfileURIs(std::vector<tstring>& serverProfilesUris) const;
		/*! Adds a supported profile or facet URI to an server application.
		*
		* All supported profiles and facets will be displayed in the ServerCapablities / ServerProfileArray node.
		* A server shall expose all profiles / facets, which are supported by this server.
		*
		* Typical profiles are
		* @li	http://opcfoundation.org/UA-Profile/Transport/https-uabinary
		* @li	http://opcfoundation.org/UA-Profile/Server/StandardUA2017
		*
		* Additional facets would be e.g.
		* @li	http://opcfoundation.org/UA-Profile/Server/HistoricalRawData
		* @li	http://opcfoundation.org/UA-Profile/Server/AggregateHistorical
		* @li	http://opcfoundation.org/UA-Profile/Server/ACExclusiveAlarming
		* @li	http://opcfoundation.org/UA-Profile/Server/VisibleRedundancy
		* @li	http://opcfoundation.org/UA-Profile/Server/FileAccess
		* @li ...
		*
		* @see https://opcfoundation-onlineapplications.org/ProfileReporting/ for other facets URIs.
		*
		* @param serverProfileUri		A profile / facet the server application supports
		*
		* By default the profiles are empty, a compliant server should add at least one profile. */
		EnumStatusCode addServerProfileURI(const tstring& serverProfileUri);

		/*! Adds a PubSub connection to the application
		*
		* @param pubSubConnection The connection to add
		*
		* @note The name of the Connection must be valid and unique among all other Connections */
		EnumStatusCode addPubSubConnection(PubSub::ConnectionPtr pubSubConnection);

		/*! Removes a PubSub connection from the application
		*
		* @param pubSubConnection The connection to be removed */
		EnumStatusCode removePubSubConnection(PubSub::ConnectionPtr pubSubConnection);

		/*! Gets all PubSub connections
		*
		* @note The order of the returned values is not defined and can be random */
		std::vector<PubSub::ConnectionPtr> getPubSubConnections() const;

		/*! Adds a PubSub PublishedDataSet to the application
		*
		* Adding a PublishedDataSet to the application is not mandatory to publish data but it is recommended.
		* Added PublishedDataSets will be kept alive by the SDK and can be exposed in a Server address space,
		* even when the PublishedDataSets are not attached to any DataSetWriter.
		* PublishedDataSets, which are attached to a DataSetWriter will be kept alive and can be exposed in a
		* Server address space, even without being added to the Application.
		*
		* @param publishedDataSet The PublishedDataSet to add
		*
		* @note The name of the PublishedDataSet must be valid and unique among all other PublishedDataSets */
		EnumStatusCode addPubSubPublishedDataSet(PubSub::PublishedDataSetPtr publishedDataSet);

		/*! Removes a PubSub PublishedDataSet from the application
		*
		* @param publishedDataSet The PublishedDataSet to be removed */
		EnumStatusCode removePubSubPublishedDataSet(PubSub::PublishedDataSetPtr publishedDataSet);

		/*! Gets all PubSub PublishedDataSets
		*
		* @note The order of the returned values is not defined and can be random */
		std::vector<PubSub::PublishedDataSetPtr> getPubSubPublishedDataSets() const;

		/*! Adds a PubSub SubscribedDataSet to the application
		*
		* Adding a SubscribedDataSet to the application is not mandatory to publish data but it is recommended.
		* Added SubscribedDataSets will be kept alive by the SDK and can be exposed in a Server address space,
		* even when the SubscribedDataSets are not attached to any DataSetWriter.
		* SubscribedDataSets, which are attached to a DataSetWriter will be kept alive and can be exposed in a
		* Server address space, even without being added to the Application.
		*
		* @param subscribedDataSet The SubscribedDataSet to add
		*
		* @note The name of the SubscribedDataSet must be valid and unique among all other SubscribedDataSets */
		EnumStatusCode addPubSubSubscribedDataSet(PubSub::SubscribedDataSetPtr subscribedDataSet);

		/*! Removes a PubSub SubscribedDataSet from the application
		*
		* @param subscribedDataSet The SubscribedDataSet to be removed */
		EnumStatusCode removePubSubSubscribedDataSet(PubSub::SubscribedDataSetPtr subscribedDataSet);

		/*! Gets all PubSub SubscribedDataSets
		*
		* @note The order of the returned values is not defined and can be random */
		std::vector<PubSub::SubscribedDataSetPtr> getPubSubSubscribedDataSets() const;

		/*! Exposes PubSub nodes to the address space
		*
		* All PubSub nodes including their properties, added to the application will be shown at (or hidden from) the address space.
		*
		* @note Server-only method
		*
		* @note Default value is false.
		*
		* @note This method implicitly initializes the SDK namespace to the next free namespace index if the SDK namespace
		*		was not yet set (see AddressSpaceRoot::getToolkitNamespaceIndex). */
		void setShowPubSubNodes(bool showNodes);
		/*! Gets the setting
		*
		* @see setShowPubSubNodes */
		bool getShowPubSubNodes();

		/*! Allows to ignore various security related checks and validations
		*
		* See EnumSecurityCheck what validations can be disabled.
		* If the option is used in a client session, this setting is the default for all sessions created afterwards.
		* Client session specific validations can be adapted at session side.
		*
		* @param checksToDisable The bits of checks that shall not be performed
		*
		* @note If you need to enable any security check, please contact the vendor of the UA application causing this need.
		*       The vendor shall fix the issue as soon as possible - the work-around for this issue may be removed in later releases of the SDK. */
		DEPRECATED(EnumStatusCode disableSecurityChecks(EnumSecurityCheck checksToDisable));

	private:
		/*! Default constructor */
		Application();
		/*! For internal use only */
		void initMembers();

		InnerApplicationDescription m_Description;

		/*! Forbid use assignment operator */
		Application& operator=(const Application&);
		/*! Forbid use of copy constructor */
		Application(const Application&);
	};
	typedef ObjectPointer<Application> ApplicationPtr;
	typedef ObjectPointer<const Application> ApplicationConstPtr;
} // toolbox namespace end

#endif	// APPLICATION_H
