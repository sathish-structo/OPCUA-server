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
#if TB5_CLIENT
#include "RedModule.h"
#include "ApplicationModule.h"
#include "ServerMultiStateItem.h"
#include "Constants.h"
#include "UserIdentityToken.h"
#include "ServerReferenceIterator.h"
#include "Argument.h"
#include "Application.h"
#include "ServerMethodCallRequest.h"

using namespace SoftingOPCToolbox5;


tstring RedModule::RedLinkEndpoint ::c_redLinkPolicyId = _T("RedLinkUser_Policy");
tstring RedModule::RedLinkEndpoint ::c_redLinkUser = _T("RedLinkUser");
tstring RedModule::RedLinkEndpoint::c_redLinkPwd = _T("RedLinkPwd");

OTUInt32 RedModule::c_RedStateStartUp;
OTUInt32 RedModule::c_RedStateShutDown;
OTUInt32 RedModule::c_RedStatePrimNoSync;
OTUInt32 RedModule::c_RedStatePrimSyncronizing;
OTUInt32 RedModule::c_RedStatePrimInSync;
OTUInt32 RedModule::c_RedStateSecondaryNotSync;
OTUInt32 RedModule::c_RedStateSecondarySynchronizing;
OTUInt32 RedModule::c_RedStateSecondaryInSync;

NodeId* RedModule::c_RedundancyInfoNodeId;
NodeId* RedModule::c_ForceSecondaryNodeId;
NodeId* RedModule::c_RedundancySwitchNodeId;
NodeId* RedModule::c_RedundancySyncNodeId;

OTUInt32 RedModule::c_waitTillBecomePrimaryAgain = 5; // 5 cycles

///////////////////////////////////////
// class RedModule
///////////////////////////////////////
RedModule::RedModule() :  m_syncReceivedCount(0), m_waitTillBecomePrimaryAgain(0), m_pAppModule(NULL), m_pRedAction(NULL)
{
}

RedModule::~RedModule()
{
}

bool RedModule::init(ApplicationModule* pAppModule, const ApplicationDescription& applicationDescription, const std::vector<Server::EndpointPtr>& localEndpoints, EnumRedundancySupport redMode, const std::vector<tstring> &redUrls)
{
	m_localEndpoints = localEndpoints;
	m_pAppModule = pAppModule;
	m_redMode = redMode;

	if(localEndpoints.empty())
	{
		_REPORT_ASSERTION(_T("It is expected to have endpoints"));
		return false;
	}
	if (m_redMode == EnumRedundancySupport_None)
	{
		// simply open the local endpoints if no redundancy is configured
		for(size_t i = 0; i < localEndpoints.size(); i++)
		{
			EnumStatusCode openResult = localEndpoints[i]->open();
			if(StatusCode::isBad(openResult))
			{
				_tprintf(_T("Failed to open endpoint '%s': %s\n"), localEndpoints[i]->getUrl().c_str(), getEnumStatusCodeString(openResult));
				return false;
			}
		}
		return true;
	}

	c_ForceSecondaryNodeId = new NodeId(1, _T("ForceSecondary"));
	c_RedundancySyncNodeId = new NodeId(1, _T("RedundancySync"));
	c_RedundancySwitchNodeId = new NodeId(1, _T("RedundancySwitch"));
	c_RedundancyInfoNodeId = new NodeId(1, _T("RedundancyInfo"));

	NodeId serviceLevelNodeId(EnumNumericNodeId_Server_ServiceLevel);
	m_serviceLevelVar = Server::AddressSpaceRoot::instance()->getVariable(&serviceLevelNodeId);

	ApplicationPtr pApp = Application::instance();

	{
		UTIL_SINGLE_LOCK(Util_Sync, RedModuleLock, m_sync);

		// first open the redundancy link endpoint
		m_redLinkEndpoint = RedLinkEndpoint::create();
		m_redLinkEndpoint->addSecurityConfiguration(EnumMessageSecurityMode_None, SecurityPolicyUri_None);
		UserTokenPolicy userTokenPolicy;
		userTokenPolicy.setPolicyId(RedLinkEndpoint::c_redLinkPolicyId);
		userTokenPolicy.setTokenType(EnumUserTokenType_UserName);
		userTokenPolicy.setSecurityPolicyUri(SecurityPolicyUri_None);
		m_redLinkEndpoint->addUserTokenPolicy(&userTokenPolicy);

		// use the URL of the first endpoint
		m_redLinkEndpoint->setUrl(buildRedLinkUrl(localEndpoints[0]->getUrl()));

		// store list of redundancy partners
		bool foundSelf = false;
		for(size_t i = 0; i < redUrls.size(); i++)
		{
			bool isLocalEndpoint = false;
			for(size_t j = 0; j < localEndpoints.size(); j++)
			{
				if(localEndpoints[j]->getUrl() == redUrls[i])
				{
					isLocalEndpoint = true;
					break;
				}
			}

			if(isLocalEndpoint)
			{
				RedPartnerInfo redInfo;
				redInfo.url = redUrls[i];
				m_redPartnerInfos.push_back(redInfo);
			}
			else
			{
				foundSelf = true;
			}
		}
		_ASSERTION(foundSelf, _T("Could not find this server instance amongst the list of redundancy partners specified with /redurls."));
		if(!foundSelf)
		{
			_tprintf(_T("==================================================================\n"));
			_tprintf(_T("Warning:\n"));
			_tprintf(_T("Could not find this server instance amongst the list of redundancy partners specified with /redurls.\n"));
			_tprintf(_T("==================================================================\n"));
		}


		// set redundancy information
		{
			std::vector<ApplicationDescription> redundantServerDescriptions;

			redundantServerDescriptions.reserve(redUrls.size());
			for (size_t i=0; i< redUrls.size(); i++)
			{
				// initially use original application description
				ApplicationDescription appDesc;
				appDesc.set(applicationDescription);

				tstring transport;
				tstring redHostName;
				OTUInt16 redPort = 0;
				tstring urlPath;
				if (!Application::parseEndpointURL(redUrls[i], transport, redHostName, redPort, urlPath))
				{
					_tprintf(_T("Error in parsing URL %s\n"), redUrls[i].c_str());
					redundantServerDescriptions.clear();
					break;
				}
				appDesc.setApplicationUri(buildAppUri(redHostName, SERVER_APP_IDENTIFIER));
				std::vector<tstring> discUrls;
				discUrls.push_back(redUrls[i]);
				appDesc.setDiscoveryUrls(discUrls);
				redundantServerDescriptions.push_back(appDesc);
			}

			pApp->setServerRedundancyMode(redMode);
			pApp->setRedundantServerArray(redundantServerDescriptions);


			// add some redundancy information 
			m_redInfo = Server::Object::create();
			m_redInfo->setNodeId(c_RedundancyInfoNodeId);
			m_redInfo->setBrowseName(QualifiedName(_T("RedundancyInfo"), 1));
			m_redInfo->setDisplayName(LocalizedText(_T("RedundancyInfo"), _T("en")));
			m_redInfo->init(Statics::ObjectTypeId_FolderType);
			AddressSpaceRoot::instance()->getServerObject()->addReference(Statics::ReferenceTypeId_Organizes, m_redInfo);

			m_pRedState = Server::MultiStateItem::create(NodeId(1, _T("RedState")), 
				LocalizedText(_T("RedundancyState"), _T("en")), 
				QualifiedName(_T("RedundancyState"), 1));
			c_RedStateStartUp = m_pRedState->addState(LocalizedText(_T("Redundancy Startup"), _T("en")));
			c_RedStateShutDown = m_pRedState->addState(LocalizedText(_T("Redundancy Shutdown"), _T("en")));
			c_RedStateSecondaryNotSync = m_pRedState->addState(LocalizedText(_T("Secondary - Not synchronized"), _T("en")));
			c_RedStateSecondarySynchronizing = m_pRedState->addState(LocalizedText(_T("Secondary - Synchronizing"), _T("en")));
			c_RedStateSecondaryInSync = m_pRedState->addState(LocalizedText(_T("Secondary - In Sync"), _T("en")));
			c_RedStatePrimNoSync = m_pRedState->addState(LocalizedText(_T("Primary - No secondary"), _T("en")));
			c_RedStatePrimSyncronizing = m_pRedState->addState(LocalizedText(_T("Primary - Synchronizing"), _T("en")));
			c_RedStatePrimInSync = m_pRedState->addState(LocalizedText(_T("Primary - In Sync"), _T("en")));

			m_pRedState->setAccessLevel(EnumAccessLevel_CurrentRead);

			m_redInfo->addReference(Statics::ReferenceTypeId_HasProperty, m_pRedState);
			m_currentState = c_RedStateStartUp;

			m_redForceSecondaryMethod = RedForceSecondaryMethod::create(this, m_redInfo);
			m_redundancySwitchMethod = RedSwitchMethod::create(this, m_redInfo);
			m_redundancySyncMethod = RedSyncMethod::create(this, m_redInfo);

			// get the nodes to be synchronized
			m_nodesToSync.clear();
			addChildVariables(NodeId(2, _T("Static")), m_nodesToSync, m_variablesToSync);
			addChildVariables(NodeId(2, _T("Dynamic")), m_nodesToSync, m_variablesToSync);
		}

		for (size_t i=0; i<m_redPartnerInfos.size(); i++)
		{
			m_redPartnerInfos[i].session = RedSyncSession::create(this, i);
			m_redPartnerInfos[i].session->setUrl(buildRedLinkUrl(m_redPartnerInfos[i].url));
			m_redPartnerInfos[i].session->setAutoReconnectInterval(5000);
			m_redPartnerInfos[i].session->setConnectionMonitoringInterval(5000);
			m_redPartnerInfos[i].session->setSecurityConfiguration(EnumMessageSecurityMode_None, SecurityPolicyUri_None);
			m_redPartnerInfos[i].session->setSessionTimeout(20000.0);

			UserIdentityToken userToken;
			UserNameIdentityToken userNameToken;
			userNameToken.setPolicyId(RedLinkEndpoint::c_redLinkPolicyId);
			userNameToken.setUserName(RedLinkEndpoint ::c_redLinkUser);
			userNameToken.setPassword(ByteString(RedLinkEndpoint ::c_redLinkPwd));
			userToken.setUserNameIdentityToken(&userNameToken);
			m_redPartnerInfos[i].session->setUserIdentityToken(&userToken);
			pApp->addSession(m_redPartnerInfos[i].session);
		}
	}

	// no lock during connect
	for (size_t i=0; i<m_redPartnerInfos.size(); i++)
	{
		m_redPartnerInfos[i].session->connect(true); // try to connect
		m_redPartnerInfos[i].session->readRedState(); // and read synchronously redundancy state 
	}

	{
		UTIL_SINGLE_LOCK(Util_Sync, RedModuleLock, m_sync);
		// start up phase done - calculate redundancy state
		changeState(c_RedStateSecondaryNotSync, true, false);
		changeState(calcNewState(), true, false);
	}

	// open red link endpoint now red state in set correctly
	if(StatusCode::isBad(m_redLinkEndpoint->open()))
	{
		printf("ERROR: failed to open redundancy sync endpoint\r\n");
	}

	// now everything is updated, we can start the check action
	m_pRedAction = new RedundancyAction(this, 1000, true);
	m_pAppModule->addAction(m_pRedAction);

	return true;
}

/**
* @brief Uninitializes this instance (during shut down).
* @return void
*/
void RedModule::end()
{
	// in case of no redundancy do nothing
	if (m_redMode != EnumRedundancySupport_None)
	{
		size_t i;
		ApplicationPtr pApp = Application::instance();

		if (m_pAppModule && m_pRedAction)
		{
			m_pAppModule->removeAction(m_pRedAction);
			delete m_pRedAction;
		}

		if (m_redLinkEndpoint.isNotNull())
		{
			// close redundancy link endpoint
			m_redLinkEndpoint->close();
			pApp->removeEndpoint(m_redLinkEndpoint);
			m_redLinkEndpoint.reset();
		}

		changeState(c_RedStateShutDown, true, false);

		// wait for synchronizing task to be finished

		// disconnect all connections to redundancy partners
		{
			UTIL_SINGLE_LOCK(Util_Sync, RedModuleLock, m_sync);
			for (i=0; i<m_redPartnerInfos.size(); i++)
			{
				m_redPartnerInfos[i].session->disconnect();
				pApp->removeSession(m_redPartnerInfos[i].session);
			}
			m_redPartnerInfos.clear();
		}

		m_serviceLevelVar.reset();
		m_redForceSecondaryMethod.reset();
		m_redundancySwitchMethod.reset();
		// getApplicationModule()->removeAction(&m_dynamicVariableChangeAction);

		delete c_ForceSecondaryNodeId;
		delete c_RedundancySwitchNodeId;
		delete c_RedundancySyncNodeId;
		delete c_RedundancyInfoNodeId;
		c_ForceSecondaryNodeId = NULL;
		c_RedundancySwitchNodeId = NULL;
		c_RedundancySyncNodeId = NULL;
		c_RedundancyInfoNodeId = NULL;
	}

}

tstring RedModule::buildRedLinkUrl(const tstring &url)
{
	tstring transport;
	tstring hostname;
	OTUInt16 port = 0;
	tstring urlPath;
	Client::SessionPtr session = Client::Session::create();
	(void)Application::parseEndpointURL(url, transport, hostname, port, urlPath);
	port += 1000;
	tstringstream ss;
	ss << transport << _T("://") << hostname << _T(":") << port << _T("/RedLink");
	return ss.str();
}

bool RedModule::isPrimary()
{
	return isPrimary(m_currentState);
}

bool RedModule::isPrimary(OTUInt32 redState)
{
	return ((redState == c_RedStatePrimNoSync) || 
		(redState == c_RedStatePrimInSync) || 
		(redState == c_RedStatePrimSyncronizing));
}

bool RedModule::isSecondary()
{
	return isSecondary(m_currentState);
}

bool RedModule::isSecondary(OTUInt32 redState)
{
	return ((redState == c_RedStateSecondaryNotSync) || 
		(redState == c_RedStateSecondarySynchronizing) ||
		(redState == c_RedStateSecondaryInSync));
}

void RedModule::syncReceived(const std::vector<const INodeId*> &nodes, const std::vector<const IDataValue*> &dataValues)
{
	EnumStatusCode result = EnumStatusCode_Good;
	if (!isSecondary())
		return;

	std::vector<EnumStatusCode> setResults;
	result = Server::AddressSpaceRoot::instance()->setVariableDataValues(nodes, dataValues, &setResults);

	// check results
	_ASSERTION(StatusCode::isGood(result), _T("setVariableDataValues failed"));
	_ASSERTION(setResults.size() == nodes.size(), _T("Invalid count of setResults returned"));
	bool allVarsSet = true;
	size_t i;
	for (i=0; i < setResults.size(); i++)
	{
		if (StatusCode::isBad(setResults[i]))
		{
			allVarsSet = false;
		}
	}
	_ASSERTION(allVarsSet, _T("synchronizing of some variables failed"));
	_UNUSED(allVarsSet);
	_UNUSED(result);
	UTIL_SINGLE_LOCK(Util_Sync, RedModuleLock, m_sync);
	if (isSecondary())
	{
		OTUInt32 newState = c_RedStateSecondarySynchronizing; // may be better ;-)

		if (m_syncReceivedCount > 5)
		{
			newState = c_RedStateSecondaryInSync;
		}
		else
		{
			m_syncReceivedCount++;
		}

		changeState(newState, true, true);
	}
}

void RedModule::onRedInfoChanged(size_t index, OTUInt32 redState, OTUInt8 serviceLevel)
{
	UTIL_SINGLE_LOCK(Util_Sync, RedModuleLock, m_sync);
	m_redPartnerInfos[index].redState = redState;
	m_redPartnerInfos[index].serviceLevel = serviceLevel;
}

EnumStatusCode RedModule::changeState(OTUInt32 newState, bool force, bool inCallback)
{
	EnumStatusCode result = EnumStatusCode_Good;

	bool endpointTargetOpen = false;
	{

		UTIL_SINGLE_LOCK(Util_Sync, RedModuleLock, m_sync);
		bool changeAllowed = force;

		if (m_currentState != newState)
		{
			if (m_waitTillBecomePrimaryAgain != 0)
			{
				// never allow to switch back when just switched from primary to secondary
				changeAllowed = false;
			}
			else
			{
				if (!changeAllowed)
				{
					// check whether allowed - a switch to primary (or startup / shutdown) is always allowed
					changeAllowed = isPrimary(newState) || (newState == c_RedStateShutDown) || (newState == c_RedStateStartUp);
				}
				if (!changeAllowed)
				{
					// a switch from secondary not in sync to sync is allowed
					changeAllowed = (m_currentState == c_RedStateSecondaryNotSync) && (newState == c_RedStateSecondaryInSync);
				}
				if (!changeAllowed)
				{
					// a switch to secondary is allowed only if primary is in sync
					changeAllowed = isSecondary(newState) && (m_currentState == c_RedStatePrimInSync);
				}
			}

			if (!changeAllowed)
			{
				result = EnumStatusCode_BadInvalidState;
			}
			else
			{
				if (isSecondary(newState) && !isSecondary())
				{
					// switch to secondary - never received a sync
					m_syncReceivedCount = 0;
				}
				if (isPrimary() && !isPrimary(newState))
				{
					// switch from primary - do not switch back for some time
					// otherwise the new primary has not yet detected that it is primary now
					m_waitTillBecomePrimaryAgain = c_waitTillBecomePrimaryAgain;

					// search a new primary
					size_t bestPrimary = m_redPartnerInfos.size(); // an invalid value
					for (size_t i=0; i<m_redPartnerInfos.size(); i++)
					{
						if (m_redPartnerInfos[i].session->isConnected())
						{
							if (bestPrimary == m_redPartnerInfos.size())
							{
								// no other yet found
								bestPrimary = i;
							}
							else
							{
								// compare against the already found
								if (m_redPartnerInfos[i].compare(m_redPartnerInfos[bestPrimary]) > 0)
								{
									// found a better
									bestPrimary = i;
								}
							}
						}
					}
				}
				if (isPrimary(newState))
				{
					// Since I am primary, force the others to become secondary
					for (size_t i=0; i<m_redPartnerInfos.size(); i++)
					{
						if (isPrimary(m_redPartnerInfos[i].redState))
						{
							if (!inCallback)
							{
								m_redPartnerInfos[i].session->forceSecondary();
							}
						}
					}
				}

				_tprintf(_T("Redundancy state: \"%s\" -> \"%s\"\n"), getStateName(m_currentState), getStateName(newState));
				m_currentState = newState;
				m_pRedState->setState(m_currentState);
				OTUInt8 newServiceLevel = 255;
				if (!isPrimary())
				{
					newServiceLevel--;
				}
				AddressSpaceRoot::instance()->setServiceLevel(newServiceLevel);
			}
		}

		// has the changing of state an effect to local endpoint?
		// the change to endpoint may result in a call where a direct influence on the endpoint was not allowed
		if (!inCallback)
		{
			endpointTargetOpen = isPrimary() || (m_currentState == c_RedStateSecondaryInSync);
		}
	}

	if (!inCallback)
	{
		for(size_t i = 0; i < m_localEndpoints.size(); i++)
		{
			if(!endpointTargetOpen && m_localEndpoints[i]->getIsOpen())
			{
				_tprintf(_T("closing local endpoint %s\n"), m_localEndpoints[i]->getUrl().c_str());
				m_localEndpoints[i]->close();
			}
			if(endpointTargetOpen && !m_localEndpoints[i]->getIsOpen())
			{
				_tprintf(_T("opening local endpoint %s\n"), m_localEndpoints[i]->getUrl().c_str());
				if(StatusCode::isBad(m_localEndpoints[i]->open()))
				{
					printf("ERROR: failed to open endpoint\r\n");
				}
			}
		}
	}

	// reset all client information until newly read?
	return result;
}

OTUInt32 RedModule::calcNewState()
{
	UTIL_SINGLE_LOCK(Util_Sync, RedModuleLock, m_sync);
	size_t i;
	OTUInt8 ownServiceLevel;
	OTUInt32 newOwnState = m_currentState;
	m_serviceLevelVar->getDataValue()->getValue()->getUInt8(&ownServiceLevel);

	// is there a reason to switch over?
	bool primaryFound = false;
	bool secondaryInSyncFound = false;
	bool secondaryFound = false;

	for (i=0; i<m_redPartnerInfos.size(); i++)
	{
		if (isPrimary(m_redPartnerInfos[i].redState))
		{
			primaryFound = true;
		}
		if (isSecondary(m_redPartnerInfos[i].redState))
		{
			secondaryFound = true;
			if (m_redPartnerInfos[i].redState == c_RedStateSecondaryInSync)
			{
				secondaryInSyncFound = true;
			}
		}

		if (isPrimary() && (isPrimary(m_redPartnerInfos[i].redState)))
		{
			// found another primary beside myself - decide who is the better one
			RedPartnerInfo ownInfo;
			ownInfo.redState = m_currentState;
			ownInfo.serviceLevel = ownServiceLevel;
			ownInfo.url = m_localEndpoints[0]->getUrl();	// compare against the first local url
			if (ownInfo.compare(m_redPartnerInfos[i]) < 0)
			{
				newOwnState = c_RedStateSecondaryNotSync;
				break; // since I am no longer primary I can stop here
			}
		}
	}

	// now calculate new state
	if (!isPrimary(newOwnState) && !primaryFound)
	{
		newOwnState = c_RedStatePrimNoSync;
	}
	if (isPrimary(newOwnState))
	{
		// start with minimum - can become better
		newOwnState = c_RedStatePrimNoSync;
		if (secondaryFound)
		{
			newOwnState = c_RedStatePrimSyncronizing;
		}
		if (secondaryInSyncFound)
		{
			newOwnState = c_RedStatePrimInSync;
		}
		// may be I have to give up primary status?
	}

	return newOwnState;
}

void RedModule::handleRedundancyCheckAction()
{
	size_t i;
	std::vector<RedSyncSessionPtr> redSessions;
	std::vector<bool> needsSync;
	std::vector<DataValue> dataValuesToSync;
	{
		UTIL_SINGLE_LOCK(Util_Sync, RedModuleLock, m_sync);

		if (m_waitTillBecomePrimaryAgain > 0)
		{
			m_waitTillBecomePrimaryAgain--;
		}
		changeState(calcNewState(), true, false);

		// synchronize all data
		needsSync.reserve(m_redPartnerInfos.size());
		redSessions.reserve(m_redPartnerInfos.size());
		for (i=0; i<m_redPartnerInfos.size(); i++)
		{
			needsSync.push_back((isPrimary() && isSecondary(m_redPartnerInfos[i].redState)));
			redSessions.push_back(m_redPartnerInfos[i].session);
		}
		if (isPrimary())
		{
			// collect nodes and data values
			dataValuesToSync.reserve(m_nodesToSync.size());
			for (i=0; i<m_nodesToSync.size(); i++)
			{
				dataValuesToSync.push_back(*m_variablesToSync[i]->getDataValue());
			}
		}
	}

	for (i=0; i<redSessions.size(); i++)
	{
		if (needsSync[i])
		{
			redSessions[i]->syncData(m_nodesToSync, dataValuesToSync);
		}
		// initiate another asynchronous read
		redSessions[i]->readRedStateAsync();
	}

}

void RedModule::addChildVariables(const NodeId &parent, std::vector<NodeId> &nodes, std::vector<Server::VariablePtr> &variables)
{
	size_t i;
	for (i=0; i<nodes.size(); i++)
	{
		if (nodes[i] == parent)
		{
			// node already found
			return;
		}
	}
	Server::BaseNodePtr parentNode = Server::AddressSpaceRoot::instance()->getNode(&parent);
	Server::VariablePtr parentVar = Server::AddressSpaceRoot::instance()->getVariable(&parent);
	if (parentVar.isNotNull())
	{
		nodes.push_back(parent);
		variables.push_back(parentVar);
	}

	// Use NULL session (all nodes) to find children
	Server::ReferenceIteratorPtr it = parentNode->getReferenceIterator(EnumBrowseDirection_Forward, Statics::ReferenceTypeId_HasChild, true, NULL);
	while (!it->isEnd())
	{
		addChildVariables(it->getReferenceDescription()->getNodeId()->getNodeId(), nodes, variables);
		it->moveNext();
	}

}

const OTChar * RedModule::getStateName(OTUInt32 stateNumber)
{
	const OTChar * stateName = _T("invalid");
	if (stateNumber == c_RedStateStartUp)
	{
		stateName = _T("Startup");
	}
	else if (stateNumber == c_RedStateShutDown)
	{
		stateName =_T("Shutdown");
	}
	else if (stateNumber == c_RedStatePrimNoSync)
	{
		stateName =_T("PrimaryNoSecondary");
	}
	else if (stateNumber == c_RedStatePrimSyncronizing)
	{
		stateName =_T("PrimarySynchronizing");
	}
	else if (stateNumber == c_RedStatePrimInSync)
	{
		stateName =_T("PrimaryInSync");
	}
	else if (stateNumber == c_RedStateSecondaryNotSync)
	{
		stateName =_T("SecondaryNotSync");
	}
	else if (stateNumber == c_RedStateSecondarySynchronizing)
	{
		stateName =_T("SecondarySynchronizing");
	}
	else if (stateNumber == c_RedStateSecondaryInSync)
	{
		stateName =_T("SecondaryInSync");
	}

	return stateName;
}


///////////////////////////////////////
// class RedLinkEndpoint
///////////////////////////////////////
RedModule::RedLinkEndpoint::RedLinkEndpoint()
{
}
RedModule::RedLinkEndpoint::~RedLinkEndpoint()
{
}


SoftingOPCToolbox5::ObjectPointer<RedModule::RedLinkEndpoint> RedModule::RedLinkEndpoint::create()
{
	return new RedLinkEndpoint();
}

EnumStatusCode RedModule::RedLinkEndpoint::onAuthenticateUser(const IUserIdentityToken* userIdentityToken, const ISignatureData* signature, Server::Session* session, const IApplicationDescription* clientDescription, unsigned int* userGroupIndex)
{
	_UNUSED(signature);
	_UNUSED(session);
	_UNUSED(clientDescription);

	EnumStatusCode authenticationResult = EnumStatusCode_BadIdentityTokenRejected;

	switch(userIdentityToken->getType())
	{
	case EnumUserTokenType_UserName:
		{
			UserNameIdentityToken usernameToken;
			userIdentityToken->getUserNameIdentityToken(usernameToken);

			tstring userName = usernameToken.getUserName();
			ByteString pwd = usernameToken.getPassword();
			if ((userName == c_redLinkUser)  && (pwd == ByteString(c_redLinkPwd)))
			{
				authenticationResult = EnumStatusCode_Good;
				*userGroupIndex = 4; // a separate group only for redundancy sync
			}
		}
		break;
	default:
		break;
	}
	return authenticationResult;
}



///////////////////////////////////////
// class RedSyncSession
///////////////////////////////////////
RedModule::RedSyncSession::RedSyncSession() : m_index((size_t)-1), m_redModule(NULL)
{

}
RedModule::RedSyncSession::RedSyncSession(RedModule* redModule, size_t index) : m_index(index), m_redModule(redModule)
{

}
RedModule::RedSyncSession::~RedSyncSession()
{

}
SoftingOPCToolbox5::ObjectPointer<RedModule::RedSyncSession> RedModule::RedSyncSession::create(RedModule* redModule, size_t index)
{
	RedSyncSession* session = new RedSyncSession(redModule, index);
	if (session != NULL)
	{
		session->setAutoReconnectInterval(5000);
		session->setConnectionMonitoringInterval(2000);
	}
	return session;
}

void RedModule::RedSyncSession::onStatusChange(bool status)
{
	if (m_redModule && !status)
	{
		// if connection is broken service level is 0
		m_redModule->onRedInfoChanged(m_index, c_RedStateShutDown, 0);
	}
}

EnumStatusCode RedModule::RedSyncSession::readRedState()
{
	EnumStatusCode serviceResult = EnumStatusCode_BadInvalidState;

	if (isConnected())
	{
		std::vector<ReadValueId> readIds;
		std::vector<DataValue> values;
		ReadValueId id(NodeId(1, _T("RedState")), EnumAttributeId_Value);
		readIds.push_back(id);
		id.setNodeId(NodeId(EnumNumericNodeId_Server_ServiceLevel));
		readIds.push_back(id);

		serviceResult = read(EnumTimestampsToReturn_Both, readIds, 0.0, values);

		if (StatusCode::isGood(serviceResult))
		{
			updateState(values);
		}
	}

	return serviceResult;
}

EnumStatusCode RedModule::RedSyncSession::readRedStateAsync()
{
	EnumStatusCode serviceResult = EnumStatusCode_BadInvalidState;
	if (isConnected())
	{
		std::vector<ReadValueId> readIds;
		std::vector<DataValue> values;
		ReadValueId id(NodeId(1, _T("RedState")), EnumAttributeId_Value);
		readIds.push_back(id);
		id.setNodeId(NodeId(EnumNumericNodeId_Server_ServiceLevel));
		readIds.push_back(id);

		serviceResult = readAsync(NULL, EnumTimestampsToReturn_Both, readIds, 0.0);
	}

	return serviceResult;
}

void RedModule::RedSyncSession::onReadComplete(void* transactionHandle, EnumStatusCode serviceResult, std::vector<DataValue>& values)
{
	_UNUSED(transactionHandle);
	if (StatusCode::isGood(serviceResult) && isConnected())
	{
		updateState(values);
	}
}


void RedModule::RedSyncSession::updateState(const std::vector<SoftingOPCToolbox5::DataValue>& values)
{
	if (values.size() != 2)
	{
		_tprintf(_T("Invalid count of variables in onReadComplete\n"));
		return;
	}

	OTUInt32 redState = c_RedStateShutDown;
	OTUInt8 serviceLevel = 0;

	if (StatusCode::isGood(values[0].getStatusCode()) &&
		StatusCode::isGood(values[1].getStatusCode()))
	{
		values[0].getValue()->getUInt32(&redState);
		values[1].getValue()->getUInt8(&serviceLevel);
	}

	if (m_redModule)
	{
		m_redModule->onRedInfoChanged(m_index, redState, serviceLevel);
	}
}

EnumStatusCode RedModule::RedSyncSession::forceSecondary()
{
	EnumStatusCode result = EnumStatusCode_BadNoCommunication;
	if (isConnected())
	{
		std::vector<Value> inputArgs;
		std::vector<Value> outputArgs;
		std::vector<EnumStatusCode> inputArgStatusCodes;
		EnumStatusCode callResult;
		result = callMethod(c_RedundancyInfoNodeId, c_ForceSecondaryNodeId, inputArgs, outputArgs, inputArgStatusCodes, &callResult);
		if (StatusCode::isGood(result))
		{
			// service call ok, check call result
			result = callResult;
		}
	}
	return result;
}

EnumStatusCode RedModule::RedSyncSession::syncData(const std::vector<NodeId> &nodesToSync, const std::vector<DataValue> &dataValuesToSync)
{
	EnumStatusCode result = EnumStatusCode_BadNoCommunication;
	if (isConnected())
	{
		std::vector<Value> inputArgs;
		inputArgs.reserve(2);
		inputArgs.push_back(Value(nodesToSync));
		inputArgs.push_back(Value(dataValuesToSync));
		std::vector<Value> outputArgs;
		std::vector<EnumStatusCode> inputArgStatusCodes;
		EnumStatusCode callResult;
		result = callMethod(c_RedundancyInfoNodeId, c_RedundancySyncNodeId, inputArgs, outputArgs, inputArgStatusCodes, &callResult);
		if (StatusCode::isGood(result))
		{
			// service call ok, check call result
			result = callResult;
		}
	}
	return result;
}



///////////////////////////////////////
// class RedundancyAction
///////////////////////////////////////
RedModule::RedundancyAction::RedundancyAction(RedModule* redModule, unsigned long timeout, bool isCyclic) : Util_ScheduledAction(timeout, isCyclic)
{
	m_redModule = redModule;
}

RedModule::RedundancyAction::~RedundancyAction()
{

}

void RedModule::RedundancyAction::execute()
{
	m_redModule->handleRedundancyCheckAction();
}




///////////////////////////////////////
// class RedForceSecondaryMethod
///////////////////////////////////////
RedModule::RedForceSecondaryMethod::RedForceSecondaryMethod(RedModule* redModule) : m_redModule(redModule)
{

}

RedModule::RedForceSecondaryMethod::~RedForceSecondaryMethod()
{

}

ObjectPointer<RedModule::RedForceSecondaryMethod> RedModule::RedForceSecondaryMethod::create(RedModule* redModule, Server::BaseNodePtr parent)
{
	ObjectPointer<RedForceSecondaryMethod> pMethod = new RedForceSecondaryMethod(redModule);
	if (!pMethod)
	{
		return NULL;
	}

	pMethod->setNodeId(c_ForceSecondaryNodeId);
	pMethod->setBrowseName(QualifiedName(_T("ForceSecondary"), 1));
	pMethod->setDisplayName(LocalizedText(_T("ForceSecondary"), _T("en")));
	pMethod->init(NULL);
	pMethod->setDescription(LocalizedText(_T("Internal method to force a server to become secondary"), _T("en")));

	parent->addReference(Statics::ReferenceTypeId_HasComponent, pMethod);

	pMethod->setExecutable(true);
	ApplicationModule::assignAccessPatternAndRolePermissions(pMethod, ApplicationModule::s_AccessKey_Read_Read_Read_Read_ReadCall);

	vector<Argument> arguments;
	// no input argument
	NodeId argNodeId(1, _T("ForceSecondaryInputArg"));
	pMethod->setInputArguments(&argNodeId, arguments);

	// no output arguments
	arguments.clear();
	argNodeId = NodeId(1, _T("ForceSecondaryOutputArg"));
	pMethod->setOutputArguments(&argNodeId, arguments);

	return pMethod;
}

EnumStatusCode RedModule::RedForceSecondaryMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	size_t i;
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<Value> inputValues = request->getInputArguments();
	
	vector<EnumStatusCode> inputStatusCodes;
	inputStatusCodes.reserve(inputValues.size());
	// initialize status codes
	for (i=0; i<inputValues.size(); i++)
	{
		inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
	}

	if (inputValues.size() != 0)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}
	if (StatusCode::isGood(result) && m_redModule)
	{
		// do not change endpoint state while in method call!
		result = m_redModule->changeState(c_RedStateSecondaryNotSync, true, true);
	}

	request->setStatusCode(result);
	request->complete();
	return result;
}

///////////////////////////////////////
// class RedSwitchMethod
///////////////////////////////////////
RedModule::RedSwitchMethod::RedSwitchMethod(RedModule* redModule) : m_redModule(redModule)
{

}

RedModule::RedSwitchMethod::~RedSwitchMethod()
{

}

ObjectPointer<RedModule::RedSwitchMethod> RedModule::RedSwitchMethod::create(RedModule* redModule, Server::BaseNodePtr parent)
{
	ObjectPointer<RedSwitchMethod> pMethod = new RedSwitchMethod(redModule);
	if (!pMethod)
	{
		return NULL;
	}

	pMethod->setNodeId(c_RedundancySwitchNodeId);
	pMethod->setBrowseName(QualifiedName(_T("RedundancySwitch"), 1));
	pMethod->setDisplayName(LocalizedText(_T("RedundancySwitch"), _T("en")));
	pMethod->init(NULL);
	pMethod->setDescription(LocalizedText(_T("Method to force a redundancy switch. Can be performed only on primary in sync."), _T("en")));

	parent->addReference(Statics::ReferenceTypeId_HasComponent, pMethod);

	pMethod->setExecutable(true);
	// allowed only for special redundancy connection
	ApplicationModule::assignAccessPatternAndRolePermissions(pMethod, ApplicationModule::s_AccessKey_Read_Read_Read_Read_ReadCall);

	vector<Argument> arguments;
	// no input arguments to force as secondary
	NodeId argNodeId(1, _T("RedundancySwitchInputArg"));
	pMethod->setInputArguments(&argNodeId, arguments);
	// no output arguments
	argNodeId = NodeId(1, _T("RedundancySwitchOutputArg"));
	pMethod->setOutputArguments(&argNodeId, arguments);

	return pMethod;
}

EnumStatusCode RedModule::RedSwitchMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	EnumStatusCode result = EnumStatusCode_BadInvalidState;
	std::vector<Value> inputValues = request->getInputArguments();
	if (0 != inputValues.size())
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if (m_redModule)
	{
		// do not change endpoint state while in method call!
		result = m_redModule->changeState(c_RedStateSecondaryNotSync, false, true);
	}

	request->setStatusCode(result);
	request->complete();
	return result;
}

///////////////////////////////////////
// class RedSyncMethod
///////////////////////////////////////
RedModule::RedSyncMethod::RedSyncMethod(RedModule* redModule) : m_redModule(redModule)
{

}

RedModule::RedSyncMethod::~RedSyncMethod()
{

}

ObjectPointer<RedModule::RedSyncMethod> RedModule::RedSyncMethod::create(RedModule* redModule, Server::BaseNodePtr parent)
{
	ObjectPointer<RedSyncMethod> pMethod = new RedSyncMethod(redModule);
	if (!pMethod)
	{
		return NULL;
	}

	pMethod->setNodeId(c_RedundancySyncNodeId);
	pMethod->setBrowseName(QualifiedName(_T("RedundancySync"), 1));
	pMethod->setDisplayName(LocalizedText(_T("RedundancySync"), _T("en")));
	pMethod->init(NULL);
	pMethod->setDescription(LocalizedText(_T("Internal method to synchronize values"), _T("en")));

	parent->addReference(Statics::ReferenceTypeId_HasComponent, pMethod);

	pMethod->setExecutable(true);
	std::vector<RolePermissionType> rolePermissions;
	// allowed only for special redundancy connection
	ApplicationModule::assignAccessPatternAndRolePermissions(pMethod, ApplicationModule::s_AccessKey_Read_Read_Read_Read_ReadCall);

	vector<Argument> arguments;

	// define input arguments for the method
	// first argument is an array of node IDs to be synchronized
	Argument param;
	std::vector<OTUInt32> dimensions;
	param.setDataType(Statics::DataTypeId_NodeId);
	param.setName(_T("Nodes"));
	param.setValueRank(1);
	dimensions.push_back(0);
	param.setArrayDimensions(dimensions);
	arguments.push_back(param);

	// second argument is an array of data values for the node IDs
	param.setDataType(Statics::DataTypeId_DataValue);
	param.setName(_T("Values"));
	param.setValueRank(1);
	dimensions.push_back(0);
	param.setArrayDimensions(dimensions);

	arguments.push_back(param);

	NodeId argNodeId(1, _T("RedundancySyncInputArg"));
	pMethod->setInputArguments(&argNodeId, arguments);

	// no output arguments
	arguments.clear();
	argNodeId = NodeId(1, _T("RedundancySyncOutputArg"));
	pMethod->setOutputArguments(&argNodeId, arguments);

	return pMethod;
}

EnumStatusCode RedModule::RedSyncMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	EnumStatusCode result = EnumStatusCode_Good;
	size_t i;

	std::vector<Value> inputValues = request->getInputArguments();

	vector<EnumStatusCode> inputStatusCodes;
	inputStatusCodes.reserve(inputValues.size());
	// initialize status codes
	for (i=0; i<inputValues.size(); i++)
	{
		inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
	}

	if (inputValues.size() < 2)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	else if (inputValues.size() > 2)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	std::vector<NodeId> nodesToSync;
	if (StatusCode::isGood(result))
	{
		result = inputValues[0].getNodeIdArray(nodesToSync);
		inputStatusCodes[0] = result;
	}

	std::vector<DataValue> dataValuesToSync;
	if (StatusCode::isGood(result))
	{
		result = inputValues[1].getDataValueArray(dataValuesToSync);
		inputStatusCodes[1] = result;
	}
	if (StatusCode::isGood(result))
	{
		if (nodesToSync.size() != dataValuesToSync.size())
		{
			result = EnumStatusCode_BadInvalidArgument;
		}
	}

	request->setInputArgumentResults(inputStatusCodes);

	// no do the sync operation...

	std::vector<const INodeId*> nodes;
	std::vector<const IDataValue*> dataValues;
	if (StatusCode::isGood(result))
	{
		nodes.reserve(nodesToSync.size());
		dataValues.reserve(dataValuesToSync.size());

		for (i=0; i<nodesToSync.size(); i++)
		{
			nodes.push_back(&nodesToSync[i]);
			dataValues.push_back(&dataValuesToSync[i]);
		}
	}


	if (StatusCode::isGood(result) && m_redModule)
	{
		m_redModule->syncReceived(nodes, dataValues);
	}

	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

///////////////////////////////////////
// class RedSyncMethod
///////////////////////////////////////
int RedModule::RedPartnerInfo::compare(const RedPartnerInfo &other)
{
	// first compare states
	if (redState != other.redState)
	{
		// c_RedStatePrimInSync best state
		if (redState == c_RedStatePrimInSync) 
			return 1;
		if (other.redState == c_RedStatePrimInSync) 
			return -1;
		// c_RedStatePrimSyncronizing is next best state
		if (redState == c_RedStatePrimSyncronizing) 
			return 1;
		if (other.redState == c_RedStatePrimSyncronizing) 
			return -1;
		// c_RedStatePrimNoSync is next best state
		if (redState == c_RedStatePrimNoSync)
			return 1;
		if (other.redState == c_RedStatePrimNoSync) 
			return -1;
		// c_RedStateSecondaryInSync is next best state
		if (redState == c_RedStateSecondaryInSync)
			return 1;
		if (other.redState == c_RedStateSecondaryInSync) 
			return -1;
		// c_RedStateSecondarySynchronizing is next best state
		if (redState == c_RedStateSecondarySynchronizing)
			return 1;
		if (other.redState == c_RedStateSecondarySynchronizing) 
			return -1;
		// c_RedStateSecondaryNotSync is next best state
		if (redState == c_RedStateSecondaryNotSync)
			return 1;
		if (other.redState == c_RedStateSecondaryNotSync) 
			return -1;

		// other states are out of interest
	}

	// compare service level next
	if (serviceLevel > other.serviceLevel)
	{
		return 1;
	}
	if (serviceLevel < other.serviceLevel)
	{
		return -1;
	}


	// finally compare URL to get a save result
	return url.compare(other.url);
}
#endif
