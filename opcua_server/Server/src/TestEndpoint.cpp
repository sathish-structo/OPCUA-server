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
#include "stdafx.h"
#include "TestEndpoint.h"
#include "ServerSession.h"
#include "ServerDeleteSessionRequest.h"
#include "ApplicationModule.h"
#include "ServerFile.h"
#include "ToolkitTrace.h"

#define TRACE_SESSIONS

#define USE_PERMISSION_ROLES

DeleteSessionWorkItem::DeleteSessionWorkItem(const SoftingOPCToolbox5::Server::DeleteSessionRequestPtr request, SoftingOPCToolbox5::Server::EndpointPtr endpoint)
{
	m_request = request;
	m_endpoint = endpoint;
	m_session = request->getSession();
}

DeleteSessionWorkItem::DeleteSessionWorkItem()
{
}

DeleteSessionWorkItem::~DeleteSessionWorkItem()
{
}

void DeleteSessionWorkItem::execute()
{
#ifdef TRACE_SESSIONS
	NodeId sessionId = m_session->getId();
	Value id = sessionId.getIdentifier();
	OTUInt32 numericId;
	id.getUInt32(&numericId);
	_tprintf(_T("Session %s (Id: %u) deleted\n"), m_session->getName().c_str(), numericId);
#endif

	m_request->complete();
}

DeleteSessionAction::DeleteSessionAction(const Server::DeleteSessionRequestPtr request, Server::EndpointPtr endpoint)
{
	m_request = request;
	m_endpoint = endpoint;
	m_session = request->getSession();
}

DeleteSessionAction::~DeleteSessionAction()
{
}

void DeleteSessionAction::execute()
{
	DeleteSessionWorkItem* workItem = new DeleteSessionWorkItem(m_request, m_endpoint);
	if(!getApplicationModule()->addWorkItem(workItem))
	{
		workItem->execute();
		delete workItem;
	}
}

void DeleteSessionAction::cleanUp()
{
	execute();
}

TestEndpoint::TestEndpoint(): Endpoint()
{
	init();
}

TestEndpoint::TestEndpoint(TBHandle otbHandle): Endpoint(otbHandle)
{
	init();
}

void TestEndpoint::init()
{
	// Granted users:
	UserEntry userEntry;
	userEntry.password = ByteString(_T("pwd"));
	userEntry.groupIndex = ApplicationModule::s_UserGroup_Group1;
	userEntry.m_userRoles.clear();
	userEntry.m_userRoles.push_back(ApplicationModule::s_RoleIdGroup1);
	userEntry.accountEnabled = true;
	m_users[_T("usr")] = userEntry;

	userEntry.password = ByteString(_T("pwd2"));
	userEntry.groupIndex = ApplicationModule::s_UserGroup_Group2;
	userEntry.m_userRoles.clear();
	userEntry.m_userRoles.push_back(ApplicationModule::s_RoleIdGroup2);
	userEntry.accountEnabled = true;
	m_users[_T("usr2")] = userEntry;

	userEntry.password = ByteString(_T("query"));
	userEntry.groupIndex = ApplicationModule::s_UserGroup_Query;
	userEntry.m_userRoles.clear();
	userEntry.m_userRoles.push_back(ApplicationModule::s_RoleIdQuery);
	userEntry.accountEnabled = true;
	m_users[_T("query")] = userEntry;

	userEntry.password = ByteString(_T("eve"));
	userEntry.groupIndex = ApplicationModule::s_UserGroup_Default;
	userEntry.m_userRoles.clear();
	userEntry.m_userRoles.push_back(ApplicationModule::s_RoleIdDefault);
	userEntry.accountEnabled = false;
	m_users[_T("eve")] = userEntry;

	userEntry.password = ByteString(_T("admin"));
	userEntry.groupIndex = ApplicationModule::s_UserGroup_Default;
	userEntry.m_userRoles.clear();
	userEntry.m_userRoles.push_back(NodeId(EnumNumericNodeId_WellKnownRole_SecurityAdmin));
	userEntry.accountEnabled = true;
	m_users[_T("SecurityAdmin")] = userEntry;

	m_isDefaultEndpoint = true;
	m_denySecureChannelOpen = false;
}

TestEndpoint::~TestEndpoint()
{
}

EnumStatusCode TestEndpoint::onAuthenticateUser(const IUserIdentityToken* userIdentityToken,
										const ISignatureData* signature,
										Server::Session* session,
										const IApplicationDescription* clientDescription,
										unsigned int* userGroupIndex)
{
	_UNUSED(signature);
	_UNUSED(clientDescription);

	/*_tprintf(_T("Authenticating user\r\n"));
	_tprintf(_T("===================\r\n"));
	tstring policyId = userIdentityToken->getPolicyId();
	_tprintf(_T("Policy id: %s\r\n"),policyId.c_str());
	*/

	EnumStatusCode authenticationResult = EnumStatusCode_Good;

	switch(userIdentityToken->getType())
	{
	case EnumUserTokenType_Anonymous:
		if(ApplicationModule::s_accessPatternEnabled)
		{
			*userGroupIndex = ApplicationModule::s_UserGroup_Default;
		}
		else
		{
			_UNUSED(userGroupIndex);
			std::vector<NodeId> roles;
			roles.push_back(ApplicationModule::s_RoleIdDefault);
			std::vector<NodeId> rolesAssignedBySDK;
			rolesAssignedBySDK = session->getRoles();
			if(rolesAssignedBySDK != roles)
			{
				// actually this could be an assertion, BUT if
				// - a client activates a session while 
				// - the server re-imports the standard address space
				// - with option "replace existing nodes"
				// then there is a time gap where the server / capabilities / roleset does not yet contain the server application role nodes
				// the application roles (e.g. ApplicationModule::s_RoleIdDefault) will be references later, when import is completed
				// In this gap a activates session will have NO assigned roles!!!
				// This seems to be a very strange use case - and if it is allowed, then the server should close its endpoints while doing it
				Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("SDK has assigned wrong roles"), _T(""));
			}
		}

		authenticationResult = EnumStatusCode_Good;
		break;

	case EnumUserTokenType_UserName:
		{
			UserNameIdentityToken usernameToken;
			userIdentityToken->getUserNameIdentityToken(usernameToken);

			tstring userName = usernameToken.getUserName();
			ByteString pwd = usernameToken.getPassword();
			//string textPwd;
			//textPwd.append((const char*)pwd.getDataPointer(), pwd.getSize());
			//_tprintf(_T("Username : %s\r\n"),userName.c_str());
			//printf("Password : %s\r\n", textPwd.c_str());
			authenticationResult = authenticateUsrPwd(session, userName, pwd, userGroupIndex);
		}
		break;

	case EnumUserTokenType_Certificate:
		{
			X509IdentityToken x509IdentityToken;
			authenticationResult = userIdentityToken->getX509IdentityToken(x509IdentityToken);

			if(StatusCode::isGood(authenticationResult))
			{
				authenticationResult = validateUserCertificate(x509IdentityToken.getCertificate());
			}

			if(StatusCode::isGood(authenticationResult))
			{
				if(ApplicationModule::s_accessPatternEnabled)
				{
					*userGroupIndex = ApplicationModule::s_UserGroup_Group1;
				}
				else
				{
					// this assignment of session roles needs to be done manually
					// there is no criteria available to describe this role assignment.
					std::vector<NodeId> roles;
					roles.push_back(ApplicationModule::s_RoleIdGroup1);
					session->setRoles(roles);
				}
			}
			else
			{
				authenticationResult = EnumStatusCode_BadIdentityTokenRejected;
			}
		}
		break;

	case EnumUserTokenType_IssuedToken:
		{
			IssuedIdentityToken issuedIdentityToken;
			userIdentityToken->getIssuedIdentityToken(issuedIdentityToken);

			// For now the test server accepts only our handcrafted XML fragment
			// provided by the test client
			tstring xmlFragment = issuedIdentityToken.getData()->getStringFromUtf8();
			if(xmlFragment == _T("<?xml version='1.0'?><tokendata></tokendata>"))
			{
				if(ApplicationModule::s_accessPatternEnabled)
				{
					*userGroupIndex = ApplicationModule::s_UserGroup_Default;
				}
				else
				{
					// this assignment of session roles needs to be done manually
					// there is no criteria available to describe this role assignment.
					std::vector<NodeId> roles;
					roles.push_back(ApplicationModule::s_RoleIdDefault);
					session->setRoles(roles);
				}
			}
			else
			{
				authenticationResult = EnumStatusCode_BadIdentityTokenRejected;
			}
		}
		break;

	default:
		authenticationResult = EnumStatusCode_BadIdentityTokenInvalid;
		break;
	}

	return authenticationResult;
}

EnumStatusCode TestEndpoint::authenticateUsrPwd(Server::Session* session, tstring& userName, ByteString& password, unsigned int* userGroupIndex)
{
	EnumStatusCode authenticationResult = EnumStatusCode_BadIdentityTokenRejected;
	if(userName.empty())
	{
		authenticationResult = EnumStatusCode_BadIdentityTokenInvalid;
	}
	else
	{
		authenticationResult = EnumStatusCode_BadUserAccessDenied;

		if(m_users.find(userName) != m_users.end())
		{
			UserEntry userEntry = m_users[userName];
			if(userEntry.password.compare(password))
			{
				if(userEntry.accountEnabled)
				{
					if(ApplicationModule::s_accessPatternEnabled)
					{
						*userGroupIndex = userEntry.groupIndex;
					}
					else
					{
						std::vector<NodeId> rolesAssignedBySDK;
						rolesAssignedBySDK = session->getRoles();
						if(rolesAssignedBySDK != userEntry.m_userRoles)
						{
							// actually this could be an assertion, BUT if
							// - a client activates a session while 
							// - the server re-imports the standard address space
							// - with option "replace existing nodes"
							// then there is a time gap where the server / capabilities / roleset does not yet contain the server application role nodes
							// the application roles (e.g. ApplicationModule::s_RoleIdDefault) will be references later, when import is completed
							// In this gap a activates session will have NO assigned roles!!!
							// This seems to be a very strange use case - and if it is allowed, then the server should close its endpoints while doing it
							Trace::instance()->writeLine(EnumTraceLevel_Error, EnumTraceGroup_User1, _T("SDK has assigned wrong roles"), _T(""));
						}
					}
					authenticationResult = EnumStatusCode_Good;
				}
			}
		}
	}
	return authenticationResult;
}

EnumStatusCode TestEndpoint::onInvalidCertificate(EnumStatusCode error,
											const ByteString& certificate,
											const tstring& securityPolicy,
											EnumMessageSecurityMode messageSecurityMode)
{
	// call base class to enable audit events
	Server::Endpoint::onInvalidCertificate(error, certificate, securityPolicy, messageSecurityMode);

	_tprintf(_T("Certificate validation error: %s\n"), getEnumStatusCodeString(error));
	_tprintf(_T("Security policy: %s\n"), securityPolicy.c_str());
	_tprintf(_T("Security mode: %s\n"), getEnumMessageSecurityModeString(messageSecurityMode));
	_tprintf(_T("Certificate length: %d\n"), certificate.getSize());
	return EnumStatusCode_Good;
}

EnumStatusCode TestEndpoint::onCreateSession(Server::Session* session)
{
#ifdef TRACE_SESSIONS
	NodeId sessionId = session->getId();
	Value id = sessionId.getIdentifier();
	OTUInt32 numericId;
	tstring clientIp = session->getClientIpAddress();
	id.getUInt32(&numericId);
	_tprintf(_T("Session %s (Id: %u) from IP %s created%s%s\n"),
		session->getName().c_str(),
		numericId,
		clientIp.c_str(),
		m_isDefaultEndpoint ? _T("") : _T(" on secondary Endpoint "),
		m_isDefaultEndpoint ? _T("") : getUrl().c_str());
#endif

	return Endpoint::onCreateSession(session);
}

void TestEndpoint::handleDeleteSession(Server::DeleteSessionRequestPtr deleteSessionRequest)
{
	DeleteSessionAction* pAction = new DeleteSessionAction(deleteSessionRequest, this);
	OTUInt32 delay = getApplicationModule()->getServiceDelay(EnumNumericNodeId_CloseSessionRequest);
	
	if(delay != 0)
	{
		// service shall be executed asynchronously with a delay
		pAction->setAutoDelete(true);
		pAction->setTimeout(delay);
		if(getApplicationModule()->addAction(pAction))
		{
			// ownership successfully transferred to the TimerThread
			pAction = NULL;
		}
	}

	if(pAction)
	{
		// either no delay is set or add action failed
		// ==> execute synchronous
		pAction->execute();
		delete pAction;
	}
}

EnumStatusCode TestEndpoint::onOpeningSecureChannel(OTUInt32 secureChannelId,
	const IByteString& clientCertificate,
	const tstring& clientCertificateThumbprint,
	EnumOpenRequest requestType,
	const tstring& securityPolicy,
	EnumMessageSecurityMode securityMode,
	double requestedLifetime)
{
	_UNUSED(secureChannelId);
	_UNUSED(clientCertificate);
	_UNUSED(clientCertificateThumbprint);
	_UNUSED(requestType);
	_UNUSED(securityPolicy);
	_UNUSED(securityMode);
	_UNUSED(requestedLifetime);

	// Return a bad status code if the deny flag is set, use a status which can be recognized
	return (m_denySecureChannelOpen ? EnumStatusCode_BadSecureChannelClosed : EnumStatusCode_Good);
}

bool TestEndpoint::isDefaultEndpoint()
{
	return m_isDefaultEndpoint;
}

void TestEndpoint::setIsDefaultEndpoint(bool isDefault)
{
	m_isDefaultEndpoint = isDefault;
}

void TestEndpoint::setDenySecureChannelOpen(bool deny)
{
	m_denySecureChannelOpen = deny;
}

TestEndpoint::UserEntry::UserEntry()
{
	groupIndex = 0;
	m_userRoles.clear();
	password.clear();
	accountEnabled = false;
}

TestEndpoint::UserEntry::UserEntry(const UserEntry& other)
{
	*this = other;
}

const TestEndpoint::UserEntry& TestEndpoint::UserEntry::operator=(const UserEntry& other)
{
	groupIndex = other.groupIndex;
	m_userRoles = other.m_userRoles;
	password = other.password;
	accountEnabled = other.accountEnabled;

	return *this;
}

TestEndpoint::UserEntry::~UserEntry()
{
}
