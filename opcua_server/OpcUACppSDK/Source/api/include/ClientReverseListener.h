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

#ifndef CLIENTREVERSELISTENER_H
#define CLIENTREVERSELISTENER_H
#if TB5_CLIENT
#include "Base.h"

namespace SoftingOPCToolbox5
{
	namespace Client
	{
		/*! The ReverseListener class listens for physical connections initiated by an OPC UA server.
		*
		* The reverse connect feature allows a server to initiate physical connections (sockets) to a client.
		* The server opens the socket and sends a "reverse hello" to the client.
		* The client can use this reverse established physical connection to create a "secure channel",
		* i.e. to connect a client session (logical connection).
		*
		* By this means it is possible to establish a connection to a server running behind a firewall which would
		* block incoming requests from a client outside the firewall.
		*
		* @note The client should start the ReverseListener as early as possible so that servers can
		*       establish physical connections before the client tries to use these for sessions */
		class TBC_EXPORT ReverseListener
			: public Base
		{
		public:
			/*! Constructs a new ReverseListener instance */
			static ObjectPointer<ReverseListener> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory. */
			ReverseListener(TBHandle otbHandle);

		protected:
			/*! Default constructor */
			ReverseListener();

		public:
			/*! Destructor */
			virtual ~ReverseListener();

			/////////////////////////////////////////////////////////////////////////
			// Attributes of class ReverseListener
			/////////////////////////////////////////////////////////////////////////

			/*! Sets the URL of the client.
			*
			* The client URL includes the IP address and the port where the client is listening for incoming connections from a server.
			*
			* @param clientUrl	The URL where the client shall listen
			*
			* @note Currently only URLs with transport TCP (opc.tcp://) are supported. */
			EnumStatusCode setClientUrl(const tstring& clientUrl);
			/*! Gets the URL of the client. */
			tstring getClientUrl();

			/*! Sets the information for the servers which shall be allowed to connect.
			*
			* As different servers in different networks may use similar IP addresses, the endpointUrls alone may be ambiguous.
			* It requires a combination of server URI and endpoint URL to clearly identify an endpoint of a server in a remote network.
			*
			* @param serverUris			The application URIs of the servers.
			*							The application URI of a specific server is part of its ApplicationDescription.
			* @param serverEndpointUrls	The endpoints of the servers which shall be used.
			*							@see Client::Session::setUrl
			*
			* @note The number of serverEndpointUrls must be the same as the number of serverUris.
			*
			* @note To accept several endpoints from the same server provide several entries with identical serverUris but different endpointURLs */
			EnumStatusCode setServerInformation(const std::vector<tstring>& serverUris, const std::vector<tstring>& serverEndpointUrls);
			/*! Gets the configured information for the servers which shall be allowed to connect. */
			EnumStatusCode getServerInformation(std::vector<tstring>& serverUris, std::vector<tstring>& serverEndpointUrls) const;

			/*! Sets the maximum of client channels that will be used.
			*
			* The listener reserves this number of sockets for connections using this reverse listener.
			* This number of channels is shared among all reverse connecting servers.
			* If the number is exceeded, then new connections from servers will fail, thus also
			* the client cannot establish more connections (e.g. as Sessions) via this reverse listener.
			*
			* The default value is 10
			*
			* @param maxClientChannels	The maximum channels which will be used for client sessions.
			*
			* @note The total number of channels is limited by compilation. */
			EnumStatusCode setMaxClientChannels(OTUInt32 maxClientChannels);
			/*! Gets the current maximum of client channels that will be supported. */
			OTUInt32 getMaxClientChannels() const;

			/*! Sets the maximum of anonymous connections.
			*
			* Once a server opens a physical connection (socket) to the client, the connection will be "anonymous" until
			* the server sends the reverse hello. By sending the reverse hello, the server identified itself and the connection
			* is no longer anonymous. Identified connections will be added to the "waiting connections" from this server until
			* a client session will use it.
			*
			* The number of anonymous channels is shared for all incoming reverse connections from all reverse connecting servers to this listener.
			* If this number is exceeded, then the oldest anonymous connection to this listener is closed.
			*
			* A good guide value is the number of expected reverse connecting servers + 1.
			*
			* The default value is 2.
			*
			* @param maxAnonymousConnections	The maximum anonymous connections
			*
			* @note Once an anonymous connection is established, the server should immediately send the reverse hello message,
			*       so the connection should be anonymous only for a very short time and every server should have at most one
			*       anonymous connection open at the same time */
			EnumStatusCode setMaxAnonymousConnections(OTUInt32 maxAnonymousConnections);
			/*! Gets the current maximum of anonymous connections. */
			OTUInt32 getMaxAnonymousConnections() const;

			/*! Sets the maximum of waiting connections per server.
			*
			* The waiting connections are connections which are mapped to a server (the "reverse hello" was sent),
			* but not yet used by a client session.
			*
			* This limit applies for every server endpoint that is configured via setServerInformation().
			* The used number of waiting connections of one server endpoint does not affect the number of waiting connections
			* of any other server endpoint.
			*
			* The default value is 5
			*
			* @param maxWaitingConnections	The maximum waiting connections per server */
			EnumStatusCode setMaxWaitingConnections(const std::vector<OTUInt32> &maxWaitingConnections);
			/*! Gets the currently used maximum of waiting connections per server. */
			EnumStatusCode getMaxWaitingConnections(std::vector<OTUInt32>& maxWaitingConnections) const;

			/*! Sets the listener to be bound to a specific interface.
			*
			* By default the reverse listener will listen on all network interfaces for incoming connections.
			* If listening is restricted to a single IP address (IPv4 or IPv6) a server can connect only to the IP address in the URL.
			*
			* @param restrictToSingleInterface	@li true: A server can connect only to the IP address (IPv4 or IPv6) specified in the URL
			*									@li false: A server can connect to all network interfaces of the machine
			*
			* @note If listening on a specific interface the client URL shall contain an IP address (not a name to be resolved). */
			EnumStatusCode setRestrictToSingleInterface(bool restrictToSingleInterface);
			/*! Gets the current configuration of listening on a specific interface. */
			bool getRestrictToSingleInterface() const;

			/*! Starts the reverse listener. */
			EnumStatusCode start();

			/*! Stops the reverse listener. */
			EnumStatusCode stop();

			/*! Callback for configured connections.
			*
			* A reverse listener is configured to accept only connections from a configured server (see setServerInformation).
			* In case any of these configured servers connected to the reverse listener, this callback will be invoked.
			*
			* @param serverUri		The server URI of the server which has opened the connection
			* @param endpointUrl	The endpoint URL of the server which opened the connection
			*
			* @note This callback is meant for information purposes. Typically an application does not need to implement a specific behaviour. */
			virtual void onServerConnectionOpened(const tstring& serverUri, const tstring& endpointUrl);
			/*! Callback for rejected connections.
			*
			* A reverse listener is configured to accept only connections from a configured server (see setServerInformation).
			* In case any other server tries to connect to the reverse listener, this callback will be invoked.
			*
			* @param serverUri		The server URI of the server which was rejected
			* @param endpointUrl	The endpoint URL of the server which was rejected
			*
			* @note Re-configuration of reverse listener to allow additional servers to connect will disconnect any already
			*		connected sessions using this reverse listener. */
			virtual void onServerConnectionRejetced(const tstring& serverUri, const tstring& endpointUrl);
		private:
			/*! For internal use only */
			void initMembers();

			/*! Forbid use of assignment operator */
			ReverseListener& operator=(const ReverseListener&);
			/*! Forbid use of copy constructor */
			ReverseListener(const ReverseListener&);
		};
		typedef ObjectPointer<ReverseListener> ReverseListenerPtr;
		typedef ObjectPointer<const ReverseListener> ReverseListenerConstPtr;
	} // end Client namespace
} // toolbox namespace end
#endif	// TB5_CLIENT

#endif	// CLIENTREVERSELISTENER_H
