/*****************************************************************************
*                                                                            *
* PROJECT_NAME             OPC UA C++ Server & Client SDK           *
*                                                                            *
* VERSION                  6.00.0                                            *
*                                                                            *
* DATE                     13.6.2022                                        *
*                                                                            *
*                                                                            *
* NOTICE:                                                                    *
*   This code and information is provided "as is" without warranty of        *
*   any kind, either expressed or implied, including but not limited         *
*   to the implied warranties of merchantability and/or fitness for a        *
*   particular purpose.                                                      *
*****************************************************************************/

#ifndef __TESTENDPOINT_H__
#define __TESTENDPOINT_H__

#include "ServerEndpoint.h"
#include "ServerSession.h"
#include "Util_TimerThread.h"
#include "Util_WorkItem.h"

using namespace SoftingOPCToolbox5;

class DeleteSessionWorkItem : public Util_WorkItem
{
public:
	DeleteSessionWorkItem(const SoftingOPCToolbox5::Server::DeleteSessionRequestPtr request, SoftingOPCToolbox5::Server::EndpointPtr endpoint);
	~DeleteSessionWorkItem();
	virtual void execute();
private:
	DeleteSessionWorkItem();
	SoftingOPCToolbox5::Server::DeleteSessionRequestPtr m_request;
	SoftingOPCToolbox5::Server::EndpointPtr m_endpoint;
	SoftingOPCToolbox5::Server::SessionPtr m_session;
};
/*! Action to delete a Subscription */
class DeleteSessionAction
	: public Util_ScheduledAction
{
public:
	DeleteSessionAction(const SoftingOPCToolbox5::Server::DeleteSessionRequestPtr request, SoftingOPCToolbox5::Server::EndpointPtr endpoint);
	virtual ~DeleteSessionAction();
	virtual void execute();

	virtual void cleanUp();
private:
	DeleteSessionAction();
	SoftingOPCToolbox5::Server::DeleteSessionRequestPtr m_request;
	SoftingOPCToolbox5::Server::EndpointPtr m_endpoint;
	SoftingOPCToolbox5::Server::SessionPtr m_session;
};

class TestEndpoint:
	public SoftingOPCToolbox5::Server::Endpoint
{
public:
	TestEndpoint();
	TestEndpoint(TBHandle otbHandle);
	void init();
	virtual ~TestEndpoint();
	virtual EnumStatusCode onAuthenticateUser(const IUserIdentityToken* userIdentityToken,
		const ISignatureData* signature,
		Server::Session* session,
		const IApplicationDescription* clientDescription,
		unsigned int* userGroupIndex);
	virtual EnumStatusCode onInvalidCertificate(EnumStatusCode error,
		const ByteString& certificate,
		const tstring& securityPolicy,
		EnumMessageSecurityMode messageSecurityMode);
	EnumStatusCode authenticateUsrPwd(Server::Session* session, tstring& userName, ByteString& password, unsigned int* userGroupIndex);

	virtual EnumStatusCode onCreateSession(Server::Session* session);
	virtual void handleDeleteSession(Server::DeleteSessionRequestPtr deleteSessionRequest);

	virtual EnumStatusCode onOpeningSecureChannel(OTUInt32 secureChannelId,
		const IByteString& clientCertificate,
		const tstring& clientCertificateThumbprint,
		EnumOpenRequest requestType,
		const tstring& securityPolicy,
		EnumMessageSecurityMode securityMode,
		double requestedLifetime);

	/*! Returns whether this is a default endpoint (opened during startup)
	* or not (added during runtime) */
	bool isDefaultEndpoint();
	/*! Define whether this is a default endpoint (opened during startup)
	* or not (added during runtime)
	* Default state is true */
	void setIsDefaultEndpoint(bool isDefault);

	/*! Set flag to deny secure channel open.
	* Default state is false (do not deny) */
	void setDenySecureChannelOpen(bool deny);

private:
	class UserEntry
	{
	public:
		UserEntry();
		UserEntry(const UserEntry &other);
		virtual ~UserEntry();
		const TestEndpoint::UserEntry& operator=(const UserEntry& other);

		unsigned int groupIndex;
		std::vector<NodeId> m_userRoles;
		ByteString password;
		bool accountEnabled;
	} ;

	std::map<tstring, UserEntry> m_users;
	//! only non default sessions are allowed to be closed during runtime
	bool m_isDefaultEndpoint;
	bool m_denySecureChannelOpen;
};

#endif
