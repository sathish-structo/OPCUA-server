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
#include "EndpointManipulationMethods.h"

#include "Constants.h"
#include "Statics.h"
#include "Application.h"
#include "ServerMethodCallRequest.h"
#include "Argument.h"
#include "PkiStoreConfiguration.h"

#include "TestEndpoint.h"
#include "ApplicationModule.h"
#include "ServerMethodCallTransaction.h"

extern bool g_cttCompatible;

using namespace SoftingOPCToolbox5;

// use a separate thread for all methods which can invoke Endpoint::close
// the close may invoke other callbacks which can add some actions to the workitem processor
class EndpointManipulationThread  : public Util_Thread
{
public:
	EndpointManipulationThread(OTUInt32 delay, Util_WorkItem *workItem, SoftingOPCToolbox5::Server::MethodCallRequestPtr requestToComplete);
	~EndpointManipulationThread();
	OTUInt32 m_delay;
	Util_WorkItem *m_workItem;
	SoftingOPCToolbox5::Server::MethodCallRequestPtr m_requestToComplete;
	virtual long run();
};

EndpointManipulationThread::EndpointManipulationThread(OTUInt32 delay, Util_WorkItem *workItem, SoftingOPCToolbox5::Server::MethodCallRequestPtr requestToComplete)
{
	m_delay = delay;
	m_workItem = workItem;
	m_requestToComplete = requestToComplete;
}

EndpointManipulationThread::~EndpointManipulationThread()
{
	if(m_workItem)
	{
		delete m_workItem;
	}
}

long EndpointManipulationThread::run()
{
	if(m_delay > 0)
	{
		// complete request immediately - client may have called using session on this endpoint and needs time to close session
		m_requestToComplete->setStatusCode(EnumStatusCode_GoodCompletesAsynchronously);
		m_requestToComplete->complete();
	}
	SLEEP(m_delay);
	m_workItem->execute();
	if(m_delay == 0)
	{
		// request is completely handled
		// actually here a normal "Good" would be more accurate, but this would mean many changes in test XMLs
		m_requestToComplete->setStatusCode(EnumStatusCode_GoodCompletesAsynchronously);
		m_requestToComplete->complete();
	}
	return 0;
}

bool CreateEndpointMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("CreateEndpoint");
	setNodeId(NodeId(namespaceIndex, methodName));
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Creates and opens an additional Endpoint on the server"), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	NodeId inputArgsNodeId(namespaceIndex, _T("CreateEndpoint_InputArguments"));
	std::vector<Argument> inputArguments;
	Argument currentArgument;
	currentArgument.setName(_T("EndpointUrl"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("URL of the new Endpoint."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("RestrictEndpoint"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_Boolean));
	currentArgument.setDescription(LocalizedText(_T("Bind Endpoint to one network interface or use all network interfaces."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("EndpointsMatchRequest"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_Boolean));
	currentArgument.setDescription(LocalizedText(_T("Return only the EndpointDescriptions corresponding to the endpoint on which the request was performed."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("PKIStorePath"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("The root path of the PKI store."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("CertificateFile"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("The path of the endpoint certificate."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("PrivateKeyFile"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("The path of the private key file."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("PrivateKeyPassword"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("The passphrase for the private key."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("CRLCheckOption"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_UInt16));
	currentArgument.setDescription(LocalizedText(_T("The CRL check option value."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	if(StatusCode::isBad(this->setInputArguments(&inputArgsNodeId, inputArguments)))
	{
		_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
		return false;
	}
	if(StatusCode::isBad(init(NULL)))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return false;
	}
	return true;
}

EnumStatusCode CreateEndpointMethod::handleExecuteRequest(Server::MethodCallTransaction* /* transaction */, Server::MethodCallRequest* request)
{
	std::vector<Value> inputArguments = request->getInputArguments();
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<EnumStatusCode> inputArgumentResults;
	tstring url;
	bool restrictEndpoint = false;
	bool endpointsMatchRequest = false;
	tstring pkiStorePath;
	tstring certificateFile;
	tstring privateKeyFile;
	tstring privateKeyPassword;
	OTUInt16 crlCheckOption = 0;
	size_t inputArgumentSize = inputArguments.size();

	Server::EndpointPtr defaultEndpoint;

	if(inputArgumentSize < 3)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	else if(inputArgumentSize > 8)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}
	// optional input arguments: PKIStorePath and certificate-related group (certificate file, private key, passphrase)
	// method must have either 3, 4 or 8 input arguments
	else if (inputArgumentSize > 4 && inputArgumentSize < 8)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isGood(result))
	{
		if(inputArguments[0].getDataType() == EnumDataTypeId_String &&
			inputArguments[0].getArrayType() == EnumValueArrayType_Scalar)
		{
			url = inputArguments[0].getString();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}
		if(inputArguments[1].getDataType() == EnumDataTypeId_Boolean &&
			inputArguments[1].getArrayType() == EnumValueArrayType_Scalar)
		{
			restrictEndpoint = inputArguments[1].getBoolean();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}
		if(inputArguments[2].getDataType() == EnumDataTypeId_Boolean &&
			inputArguments[2].getArrayType() == EnumValueArrayType_Scalar)
		{
			endpointsMatchRequest = inputArguments[2].getBoolean();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}
		request->setInputArgumentResults(inputArgumentResults);
		if(inputArgumentSize > 3)
		{
			if(inputArguments[3].getDataType() == EnumDataTypeId_String &&
				inputArguments[3].getArrayType() == EnumValueArrayType_Scalar)
			{
				pkiStorePath = inputArguments[3].getString();
				inputArgumentResults.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
				result = EnumStatusCode_BadInvalidArgument;
			}
		}
		if(inputArgumentSize > 4)
		{
			if(inputArguments[4].getDataType() == EnumDataTypeId_String &&
				inputArguments[4].getArrayType() == EnumValueArrayType_Scalar)
			{
				certificateFile = inputArguments[4].getString();
				inputArgumentResults.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
				result = EnumStatusCode_BadInvalidArgument;
			}
			if(inputArguments[5].getDataType() == EnumDataTypeId_String &&
				inputArguments[5].getArrayType() == EnumValueArrayType_Scalar)
			{
				privateKeyFile = inputArguments[5].getString();
				inputArgumentResults.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
				result = EnumStatusCode_BadInvalidArgument;
			}
			if(inputArguments[6].getDataType() == EnumDataTypeId_String &&
				inputArguments[6].getArrayType() == EnumValueArrayType_Scalar)
			{
				privateKeyPassword = inputArguments[6].getString();
				inputArgumentResults.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
				result = EnumStatusCode_BadInvalidArgument;
			}
			if(inputArguments[7].getDataType() == EnumDataTypeId_UInt16 &&
				inputArguments[7].getArrayType() == EnumValueArrayType_Scalar)
			{
				crlCheckOption = inputArguments[7].getUInt16();
				inputArgumentResults.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
				result = EnumStatusCode_BadInvalidArgument;
			}
		}
		request->setInputArgumentResults(inputArgumentResults);
	}

	if(StatusCode::isGood(result))
	{
		if(StatusCode::isGood(result))
		{
			AddWorkItemAction* pAction = new AddWorkItemAction(new CreateEndpointWorkItem(url, restrictEndpoint, pkiStorePath, certificateFile,
				privateKeyFile, privateKeyPassword, crlCheckOption, endpointsMatchRequest, request));
			// give the ownership to the TimerThread
			pAction->setAutoDelete(true);
			if(!getApplicationModule()->addAction(pAction))
			{
				delete pAction;
				// complete the request if the action does not execute
				request->setStatusCode(EnumStatusCode_BadInternalError);
				request->complete();
			}
			pAction = NULL;
		}
	}
	else
	{
		request->setStatusCode(result);
		request->complete();
	}

	return result;
}

EnumStatusCode CreateEndpointMethod::createEndpoint(tstring url,
	tstring pkiStorePath /* = _T("") */,
	tstring certificateFile /* = _T("") */, tstring privateKeyFile /* = _T("") */, tstring privateKeyPassword /* = _T("") */,
	OTUInt16 crlCheckOption /* = 0xFFFF */,
	bool restrictEndpoint /* = false */,
	bool endpointsMatchRequest /* = false */)
{
	EnumStatusCode result = EnumStatusCode_Good;
	bool endpointAdded = false;
	bool endpointOpened = false;
	ObjectPointer<TestEndpoint> newEndpoint = new TestEndpoint();
	newEndpoint->setIsDefaultEndpoint(false);

	Server::EndpointPtr defaultEndpoint;

	if(StatusCode::isGood(result))
	{
		std::vector<Server::EndpointPtr> endpoints = Application::instance()->getEndpoints();
		for(size_t i = 0; i < endpoints.size(); i++)
		{
			// all endpoints in the StructoAutomoldServer are TestEndpoint (even when created via config file)
			TestEndpoint* pTestEndpoint = static_cast<TestEndpoint*>(endpoints[i].p());
			if(pTestEndpoint->isDefaultEndpoint())
			{
				defaultEndpoint = pTestEndpoint;
				break;
			}
		}
		if(defaultEndpoint.isNull())
		{
			result = EnumStatusCode_BadInternalError;
		}
	}

	if(StatusCode::isGood(result))
	{
		newEndpoint->setUrl(url);

		// copy the user PKI store from the default endpoint
		PkiStoreConfiguration userPkiStore = defaultEndpoint->getUserPkiConfiguration();
		newEndpoint->setUserPkiConfiguration(&userPkiStore);

		// set endpoint PKI store
		if(!pkiStorePath.empty())
		{
			result = setPkiStoreConfiguration(newEndpoint, pkiStorePath, crlCheckOption);
		}
	}
	if(StatusCode::isGood(result))
	{
		// set endpoint certificate
		if(!certificateFile.empty() && !privateKeyFile.empty() && !privateKeyPassword.empty())
		{
			result = newEndpoint->setInstanceCertificate(certificateFile, privateKeyFile, privateKeyPassword);
		}
	}
	if(StatusCode::isGood(result))
	{
		result = setDefaultSecurityConfigurations(newEndpoint);
	}
	if(StatusCode::isGood(result))
	{
		result = setDefaultUserTokenPolicies(newEndpoint);
	}

	if(StatusCode::isGood(result))
	{
		result = newEndpoint->setRestrictToSingleInterface(restrictEndpoint);
	}

	if(StatusCode::isGood(result))
	{
		// EndpointsMatchRequest is a hidden (internal) feature, enable/disable on the endpoint
		result = SoftingOPCToolbox5Internal::useReqMatchEndpointFiltering(newEndpoint, endpointsMatchRequest);
	}

	if(StatusCode::isGood(result))
	{
		result = Application::instance()->addEndpoint(newEndpoint);
		if(StatusCode::isGood(result))
		{
			endpointAdded = true;
		}
		else
		{
			_tprintf(_T("Failed to add new endpoint\n"));
		}
	}
	if(StatusCode::isGood(result))
	{
		result = newEndpoint->open();
		if(StatusCode::isGood(result))
		{
			endpointOpened = true;
			_tprintf(_T("Create Endpoint on %s%s\n"), url.c_str(), restrictEndpoint ? _T(" (restricted)") : _T(""));
		}
		else
		{
			_tprintf(_T("Failed to open new endpoint on %s\n"), url.c_str());
		}
	}

	if(StatusCode::isBad(result))
	{
		if(endpointOpened)
		{
			newEndpoint->close();
		}
		if(endpointAdded)
		{
			Application::instance()->removeEndpoint(newEndpoint);
		}
	}
	return result;
}

EnumStatusCode CreateEndpointMethod::setDefaultSecurityConfigurations(Server::EndpointPtr endpoint)
{
	std::vector<EnumMessageSecurityMode> securityModes;
	std::vector<tstring> securityPolicies;

	if(!g_cttCompatible)
	{
		// None should be disabled by default when other configurations are enabled
		securityModes.push_back(EnumMessageSecurityMode_None);
		securityPolicies.push_back(SecurityPolicyUri_None);

		// The deprecated policies should be disabled by default
		securityModes.push_back(EnumMessageSecurityMode_Sign);
		securityPolicies.push_back(SecurityPolicyUri_Basic128Rsa15);
		securityModes.push_back(EnumMessageSecurityMode_SignAndEncrypt);
		securityPolicies.push_back(SecurityPolicyUri_Basic128Rsa15);

		securityModes.push_back(EnumMessageSecurityMode_Sign);
		securityPolicies.push_back(SecurityPolicyUri_Basic256);
		securityModes.push_back(EnumMessageSecurityMode_SignAndEncrypt);
		securityPolicies.push_back(SecurityPolicyUri_Basic256);
	}

	securityModes.push_back(EnumMessageSecurityMode_Sign);
	securityPolicies.push_back(SecurityPolicyUri_Basic256Sha256);
	securityModes.push_back(EnumMessageSecurityMode_SignAndEncrypt);
	securityPolicies.push_back(SecurityPolicyUri_Basic256Sha256);

	securityModes.push_back(EnumMessageSecurityMode_Sign);
	securityPolicies.push_back(SecurityPolicyUri_Aes128Sha256RsaOaep);
	securityModes.push_back(EnumMessageSecurityMode_SignAndEncrypt);
	securityPolicies.push_back(SecurityPolicyUri_Aes128Sha256RsaOaep);

	securityModes.push_back(EnumMessageSecurityMode_Sign);
	securityPolicies.push_back(SecurityPolicyUri_Aes256Sha256RsaPss);
	securityModes.push_back(EnumMessageSecurityMode_SignAndEncrypt);
	securityPolicies.push_back(SecurityPolicyUri_Aes256Sha256RsaPss);

	EnumStatusCode result = endpoint->setSecurityConfigurations(securityModes, securityPolicies);
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to set the default security configurations\n"));
	}
	return result;
}

EnumStatusCode CreateEndpointMethod::setDefaultUserTokenPolicies(EndpointPtr endpoint)
{
	std::vector<UserTokenPolicy> allPolicies;
	tstring securityPolicy;
	tstring policyIdAddon;

	/////////////////////////////////////////////
	// Policies using security of secure channel
	/////////////////////////////////////////////
	securityPolicy = _T("");
	policyIdAddon = _T("Channel");
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_Anonymous, policyIdAddon, securityPolicy));
	if(!g_cttCompatible)
	{
		// using the security of the channel is insecure for insecure channels
		// user tokens with secrets should only be used with encryption
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_UserName, policyIdAddon, securityPolicy));
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_Certificate, policyIdAddon, securityPolicy));
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_IssuedToken, policyIdAddon, securityPolicy));
	}

	/////////////////////////////////////////////
	// Policies using None security
	/////////////////////////////////////////////
	securityPolicy = SecurityPolicyUri_None;
	policyIdAddon = _T("None");
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_Anonymous, policyIdAddon, securityPolicy));
	if(!g_cttCompatible)
	{
		// user tokens with secrets should only be used with encryption
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_UserName, policyIdAddon, securityPolicy));
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_Certificate, policyIdAddon, securityPolicy));
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_IssuedToken, policyIdAddon, securityPolicy));
	}

	/////////////////////////////////////////////
	// Policies using Basic128Rsa15 security
	/////////////////////////////////////////////
	securityPolicy = SecurityPolicyUri_Basic128Rsa15;
	policyIdAddon = _T("Basic128Rsa15");
	if(!g_cttCompatible)
	{
		// deprecated securities should not be enabled by default
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_UserName, policyIdAddon, securityPolicy));
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_Certificate, policyIdAddon, securityPolicy));
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_IssuedToken, policyIdAddon, securityPolicy));
	}

	/////////////////////////////////////////////
	// Policies using Basic256 security
	/////////////////////////////////////////////
	securityPolicy = SecurityPolicyUri_Basic256;
	policyIdAddon = _T("Basic256");
	if(!g_cttCompatible)
	{
		// deprecated securities should not be enabled by default
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_UserName, policyIdAddon, securityPolicy));
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_Certificate, policyIdAddon, securityPolicy));
		allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_IssuedToken, policyIdAddon, securityPolicy));
	}

	/////////////////////////////////////////////
	// Policies using Basic256Sha256 security
	/////////////////////////////////////////////
	securityPolicy = SecurityPolicyUri_Basic256Sha256;
	policyIdAddon = _T("Basic256Sha256");
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_UserName, policyIdAddon, securityPolicy));
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_Certificate, policyIdAddon, securityPolicy));
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_IssuedToken, policyIdAddon, securityPolicy));

	/////////////////////////////////////////////
	// Policies using Aes128Sha256RsaOaep security
	/////////////////////////////////////////////
	securityPolicy = SecurityPolicyUri_Aes128Sha256RsaOaep;
	policyIdAddon = _T("RsaOaep");
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_UserName, policyIdAddon, securityPolicy));
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_Certificate, policyIdAddon, securityPolicy));
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_IssuedToken, policyIdAddon, securityPolicy));

	/////////////////////////////////////////////
	// Policies using Aes256Sha256RsaPss security
	/////////////////////////////////////////////
	securityPolicy = SecurityPolicyUri_Aes256Sha256RsaPss;
	policyIdAddon = _T("RsaPss");
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_UserName, policyIdAddon, securityPolicy));
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_Certificate, policyIdAddon, securityPolicy));
	allPolicies.push_back(createUserTokenPolicy(EnumUserTokenType_IssuedToken, policyIdAddon, securityPolicy));


	EnumStatusCode result = endpoint->setUserTokenPolicies(allPolicies);
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to set the default user token policies\n"));
	}
	return result;
}

EnumStatusCode CreateEndpointMethod::setPkiStoreConfiguration(SoftingOPCToolbox5::Server::EndpointPtr endpoint,
	const tstring& pkiStoreLocation /* = _T("") */,
	OTUInt16 crlCheckOption /* = 0xFFFF */)
{
	if(pkiStoreLocation.empty())
	{
		// keep the default application PKI store (and ignore the crlCheckOption)
		return EnumStatusCode_Good;
	}
	PkiStoreConfiguration appPkiStore;
	PkiStoreConfiguration endpointPkiStore;

	ApplicationPtr pApp = Application::instance();

	appPkiStore = pApp->getPkiStoreConfiguration();

	// take all configuration data from application's PKI store and modify only the provided settings
	endpointPkiStore = appPkiStore;

	if(crlCheckOption != 0xFFFF)	// the default value is a not usable dummy value
	{
		EnumCrlCheckOption enumCrl = (EnumCrlCheckOption)crlCheckOption;
		endpointPkiStore.setRevocationCheckOptions(enumCrl);
	}
	EnumStatusCode result = endpointPkiStore.setBasePath(pkiStoreLocation);
	if(StatusCode::isGood(result))
	{
		result = endpoint->setPkiStoreConfiguration(&endpointPkiStore);
	}
	return result;
}

UserTokenPolicy CreateEndpointMethod::createUserTokenPolicy(EnumUserTokenType tokenType, const tstring& policyIdAddon, const tstring& securityPolicyUri)
{
	tstring issuedTokenType = _T("urn:Softing:CustomIssuedTokeType");
	tstring policyId;
	switch(tokenType)
	{
	case EnumUserTokenType_Anonymous:
		policyId = _T("Anonymous");
		break;
	case EnumUserTokenType_UserName:
		policyId = _T("UserName");
		break;
	case EnumUserTokenType_Certificate:
		policyId = _T("Certificate");
		break;
	case EnumUserTokenType_IssuedToken:
		policyId = _T("IssuedToken");
		break;
	default:
		policyId = _T("Unknown");
		break;
	}


	policyId.append(_T("_"));
	policyId.append(policyIdAddon);

	UserTokenPolicy userTokenPolicy;
	userTokenPolicy.setPolicyId(policyId);
	userTokenPolicy.setTokenType(tokenType);
	userTokenPolicy.setSecurityPolicyUri(securityPolicyUri);
	if(tokenType == EnumUserTokenType_IssuedToken)
	{
		userTokenPolicy.setIssuedTokenType(issuedTokenType);
	}
	return userTokenPolicy;
}

CreateEndpointWorkItem::CreateEndpointWorkItem(tstring url, bool restrictEndpoint, tstring pkiStorePath, tstring certificateFile, tstring privateKeyFile, 
	tstring privateKeyPassword, OTUInt16 crlCheckOption, bool endpointsMatchRequest, SoftingOPCToolbox5::Server::MethodCallRequestPtr requestToComplete)
{
	m_url = url;
	m_restrictEndpoint = restrictEndpoint;
	m_requestToComplete = requestToComplete;
	m_pkiStorePath = pkiStorePath;
	m_certificateFile = certificateFile;
	m_privateKeyFile = privateKeyFile;
	m_privateKeyPassword = privateKeyPassword;
	m_crlCheckOption = crlCheckOption;
	m_endpointsMatchRequest = endpointsMatchRequest;
}

CreateEndpointWorkItem::~CreateEndpointWorkItem()
{
}

void CreateEndpointWorkItem::execute()
{
	EnumStatusCode result = CreateEndpointMethod::createEndpoint(m_url,
		m_pkiStorePath,
		m_certificateFile, m_privateKeyFile, m_privateKeyPassword,
		m_crlCheckOption,
		m_restrictEndpoint,
		m_endpointsMatchRequest);

	m_requestToComplete->setStatusCode(result);
	m_requestToComplete->complete();
}

bool RemoveEndpointMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("RemoveEndpoint");
	setNodeId(NodeId(namespaceIndex, methodName));
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Removes an Endpoint, which was additionally created"), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	NodeId inputArgsNodeId(namespaceIndex, _T("RemoveEndpoint_InputArguments"));
	std::vector<Argument> inputArguments;
	Argument currentArgument;
	currentArgument.setName(_T("EndpointUrl"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("Identify the Endpoint by its URL."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("Delay"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_UInt32));
	currentArgument.setDescription(LocalizedText(_T("Delay in milliseconds, until the Endpoint is removed."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	if(StatusCode::isBad(this->setInputArguments(&inputArgsNodeId, inputArguments)))
	{
		_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
		return false;
	}
	if(StatusCode::isBad(init(NULL)))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return false;
	}
	return true;
}

EnumStatusCode RemoveEndpointMethod::handleExecuteRequest(Server::MethodCallTransaction* /* transaction */, Server::MethodCallRequest* request)
{
	std::vector<Value> inputArguments = request->getInputArguments();
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<EnumStatusCode> inputArgumentResults;
	tstring url;
	OTUInt32 delay = 0;

	if(inputArguments.size() < 2)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	else if(inputArguments.size() > 2)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isGood(result))
	{
		if(inputArguments[0].getDataType() == EnumDataTypeId_String &&
			inputArguments[0].getArrayType() == EnumValueArrayType_Scalar)
		{
			url = inputArguments[0].getString();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}

		if(inputArguments[1].getDataType() == EnumDataTypeId_UInt32 &&
			inputArguments[1].getArrayType() == EnumValueArrayType_Scalar)
		{
			delay = inputArguments[1].getUInt32();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}
		request->setInputArgumentResults(inputArgumentResults);
	}

	if(StatusCode::isGood(result))
	{
		// do not use the normal action / workItem processing here
		// SDK will perform other callbacks like handleDeleteMonitoredItems before endpoint close returns
		EndpointManipulationThread *pthread = new EndpointManipulationThread(delay, new RemoveEndpointWorkItem(url), request);

		if(!getApplicationModule()->getThreadReaper()->addThread(pthread) || !pthread->start())
		{
			delete pthread;
			// complete the request if the action does not execute
			request->setStatusCode(EnumStatusCode_BadInternalError);
			request->complete();
		}
	}
	else
	{
		request->setStatusCode(result);
		request->complete();
	}
	return EnumStatusCode_Good;
}

EnumStatusCode RemoveEndpointMethod::removeEndpoint(const tstring& url)
{
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<Server::EndpointPtr> endpoints = Application::instance()->getEndpoints();
	Server::EndpointPtr foundEndpoint;
	for(size_t i = 0; i < endpoints.size(); i++)
	{
		if(endpoints[i]->getUrl() == url)
		{
			foundEndpoint = endpoints[i];
			break;
		}
	}
	if(foundEndpoint.isNotNull())
	{
		removeEndpoint(foundEndpoint);
	}
	else
	{
		result = EnumStatusCode_BadNoMatch;
		// normally the other overload of removeEndpoint would produce this message
		_tprintf(_T("Failed to Remove the endpoint %s\nReason %s\n"), url.c_str(), getEnumStatusCodeString(result));
	}
	return result;
}

EnumStatusCode RemoveEndpointMethod::removeEndpoint(SoftingOPCToolbox5::Server::EndpointPtr endpoint)
{
	EnumStatusCode result = EnumStatusCode_Good;
	// assume that all endpoints are TestEndpoints
	if(((TestEndpoint*)endpoint.p())->isDefaultEndpoint())
	{
		_tprintf(_T("Rejected to remove the default endpoint, only previously added Endpoints can be removed.\n"));
		result = EnumStatusCode_BadInvalidState;
	}
	else
	{
		_tprintf(_T("Remove Endpoint on %s\n"), endpoint->getUrl().c_str());
		result = Application::instance()->removeEndpoint(endpoint);
	}

	if(StatusCode::isGood(result))
	{
		// keep the pointer to the instance alive until all deletion callbacks are complete
		// not sure if this is still necessary
		OTUInt32 closeSessionDelay = getApplicationModule()->getServiceDelay(EnumNumericNodeId_CloseSessionRequest);
		OTUInt32 deleteSubscriptionsDelay = getApplicationModule()->getServiceDelay(EnumNumericNodeId_DeleteSubscriptionsRequest);
		OTUInt32 deleteMonitoredItemsDelay = getApplicationModule()->getServiceDelay(EnumNumericNodeId_DeleteMonitoredItemsRequest);

		// 1,5 * all delays
		OTUInt32 totalDelay = closeSessionDelay + deleteSubscriptionsDelay + deleteMonitoredItemsDelay;
		totalDelay += totalDelay / 2;

		if(totalDelay != 0)
		{
			DelayReleaseEndpointAction* pAction = new DelayReleaseEndpointAction(endpoint);
			pAction->setAutoDelete(true);
			pAction->setTimeout(totalDelay);
			if(!getApplicationModule()->addAction(pAction))
			{
				//addAction failed, don't delay the release
				delete pAction;
			}
		}
	}

	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to Remove the endpoint %s\nReason %s\n"), endpoint->getUrl().c_str(), getEnumStatusCodeString(result));
	}
	return result;
}

RemoveEndpointWorkItem::RemoveEndpointWorkItem(tstring url)
{
	m_url = url;
}

RemoveEndpointWorkItem::~RemoveEndpointWorkItem()
{
}

void RemoveEndpointWorkItem::execute()
{
	RemoveEndpointMethod::removeEndpoint(m_url);
}

DelayReleaseEndpointAction::DelayReleaseEndpointAction(Server::EndpointPtr endpoint)
{
	m_endpoint = endpoint;
}

DelayReleaseEndpointAction::~DelayReleaseEndpointAction()
{
	m_endpoint.reset();
}

void DelayReleaseEndpointAction::execute()
{
	// nothing to do, then handling (deleayed release) happens in the destructor
}

bool DisableEndpointMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("DisableEndpoint");
	setNodeId(NodeId(namespaceIndex, methodName));
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Temporarily closes and reopens an endpoint"), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	NodeId inputArgsNodeId(namespaceIndex, _T("DisableEndpoint_InputArguments"));
	std::vector<Argument> inputArguments;
	Argument currentArgument;
	currentArgument.setName(_T("EndpointUrl"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("URL of the new Endpoint."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("CloseDelay"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_UInt32));
	currentArgument.setDescription(LocalizedText(_T("The time in milliseconds, how long to wait until the endpoint shall be closed"), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("ClosedDuration"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_UInt32));
	currentArgument.setDescription(LocalizedText(_T("The time in milliseconds, how long the endpoint shall be closed"), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	if(StatusCode::isBad(this->setInputArguments(&inputArgsNodeId, inputArguments)))
	{
		_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
		return false;
	}
	if(StatusCode::isBad(init(NULL)))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return false;
	}
	return true;
}

EnumStatusCode DisableEndpointMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* /* transaction */, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	std::vector<Value> inputArguments = request->getInputArguments();
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<EnumStatusCode> inputArgumentResults;
	tstring url;
	OTUInt32 delay = 0;
	OTUInt32 closedDuration = 0;

	if(inputArguments.size() < 3)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	else if(inputArguments.size() > 3)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isGood(result))
	{
		if(inputArguments[0].getDataType() == EnumDataTypeId_String &&
			inputArguments[0].getArrayType() == EnumValueArrayType_Scalar)
		{
			url = inputArguments[0].getString();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}

		if(inputArguments[1].getDataType() == EnumDataTypeId_UInt32 &&
			inputArguments[1].getArrayType() == EnumValueArrayType_Scalar)
		{
			delay = inputArguments[1].getUInt32();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}

		request->setInputArgumentResults(inputArgumentResults);
		if(inputArguments[2].getDataType() == EnumDataTypeId_UInt32 &&
			inputArguments[2].getArrayType() == EnumValueArrayType_Scalar)
		{
			closedDuration = inputArguments[2].getUInt32();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}

		request->setInputArgumentResults(inputArgumentResults);
	}

	if(StatusCode::isGood(result))
	{
		// do not use the normal action / workItem processing here
		// SDK will perform other callbacks like handleDeleteMonitoredItems before endpoint close returns
		EndpointManipulationThread *pthread = new EndpointManipulationThread(delay, new DisableEndpointWorkItem(url, closedDuration), request);

		if(!getApplicationModule()->getThreadReaper()->addThread(pthread) || !pthread->start())
		{
			delete pthread;
			request->setStatusCode(EnumStatusCode_BadInternalError);
			request->complete();
		}
	}
	else
	{
		request->setStatusCode(result);
		request->complete();
	}
	return EnumStatusCode_Good;
}

EnumStatusCode DisableEndpointMethod::closeEndpoint(const tstring& url, OTInt32 closeDuration /* = -1 */)
{
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<Server::EndpointPtr> endpoints = Application::instance()->getEndpoints();

	Server::EndpointPtr targetEndpoint;
	for(size_t i = 0; i < endpoints.size(); i++)
	{
		if(endpoints[i]->getUrl() == url)
		{
			targetEndpoint = endpoints[i];
			break;
		}
	}
	if(targetEndpoint.isNotNull())
	{
		result = closeEndpoint(targetEndpoint, closeDuration);
	}
	else
	{
		result = EnumStatusCode_BadNoMatch;
		_tprintf(_T("Failed to Close the endpoint %s\nReason %s\n"), url.c_str(), getEnumStatusCodeString(result));
	}
	return result;
}

EnumStatusCode DisableEndpointMethod::closeEndpoint(SoftingOPCToolbox5::Server::EndpointPtr endpoint, OTInt32 closeDuration /* = -1 */)
{
	EnumStatusCode result = EnumStatusCode_Good;
	_tprintf(_T("Close Endpoint on %s\n"), endpoint->getUrl().c_str());
	result = endpoint->close();

	// also start the reopen WorkItem for duration == 0 to keep the existing behavior
	if(StatusCode::isGood(result) && closeDuration >= 0)
	{
		AddWorkItemAction* pAction = new AddWorkItemAction(new OpenEndpointWorkItem(endpoint->getUrl()));
		// give the ownership to the TimerThread
		pAction->setAutoDelete(true);
		pAction->setTimeout((OTUInt32)closeDuration);
		if(!getApplicationModule()->addAction(pAction))
		{
			delete pAction;
			result = EnumStatusCode_BadInternalError;
		}
		pAction = NULL;
	}
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to Close the endpoint %s\nReason %s\n"), endpoint->getUrl().c_str(), getEnumStatusCodeString(result));
	}
	return result;
}

EnumStatusCode DisableEndpointMethod::reopenEndpoint(const tstring& url)
{
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<Server::EndpointPtr> endpoints = Application::instance()->getEndpoints();
	Server::EndpointPtr endpoint;
	for(size_t i = 0; i < endpoints.size(); i++)
	{
		if(endpoints[i]->getUrl() == url)
		{
			endpoint = endpoints[i];
			break;
		}
	}
	if(endpoint.isNotNull())
	{
		result = reopenEndpoint(endpoint);
	}
	else
	{
		result = EnumStatusCode_BadInternalError;
		_tprintf(_T("Failed to reopen the endpoint %s\nReason %s\n"), url.c_str(), getEnumStatusCodeString(result));
	}
	return result;
}

EnumStatusCode DisableEndpointMethod::reopenEndpoint(SoftingOPCToolbox5::Server::EndpointPtr endpoint)
{
	EnumStatusCode result = EnumStatusCode_Good;
	_tprintf(_T("Reopen Endpoint on %s\n"), endpoint->getUrl().c_str());
	result = endpoint->open();
	if(StatusCode::isBad(result))
	{
		_tprintf(_T("Failed to reopen the endpoint %s\nReason %s\n"), endpoint->getUrl().c_str(), getEnumStatusCodeString(result));
	}
	return result;
}

DisableEndpointWorkItem::DisableEndpointWorkItem(tstring url, OTUInt32 closeDuration)
{
	m_url = url;
	m_closeDuration = closeDuration;
}

DisableEndpointWorkItem::DisableEndpointWorkItem()
{
}

DisableEndpointWorkItem::~DisableEndpointWorkItem()
{
}

void DisableEndpointWorkItem::execute()
{
	// we mainly need to check against std::numeric_limits<OTInt32>::max() for the unsigned to signed conversion,
	// but we better use a reasonable max time
	// negative values are only allowed for the server internal usage
	OTInt32 closeDuration = 10 * 60 * 1000;
	if(m_closeDuration < (OTUInt32)closeDuration)
	{
		closeDuration = (OTInt32)m_closeDuration;
	}
	DisableEndpointMethod::closeEndpoint(m_url, closeDuration);
}

OpenEndpointWorkItem::OpenEndpointWorkItem(tstring url)
{
	m_url = url;
}

OpenEndpointWorkItem::OpenEndpointWorkItem()
{
}

OpenEndpointWorkItem::~OpenEndpointWorkItem()
{
}

void OpenEndpointWorkItem::execute()
{
	DisableEndpointMethod::reopenEndpoint(m_url);
}

bool AddRemoveReverseConnectorMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
{
	tstring methodName = _T("AddRemoveReverseConnector");
	setNodeId(NodeId(namespaceIndex, methodName));
	setBrowseName(QualifiedName(methodName, namespaceIndex));
	setDisplayName(LocalizedText(methodName, _T("")));
	setDescription(LocalizedText(_T("Add or remove a reverse connector to an Endpoint on the server"), _T("")));

	parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

	NodeId inputArgsNodeId(namespaceIndex, _T("AddRemoveReverseConnector_InputArguments"));
	std::vector<Argument> inputArguments;
	Argument currentArgument;
	currentArgument.setName(_T("EndpointUrl"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("URL of an existing Endpoint."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("ClientUrl"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
	currentArgument.setDescription(LocalizedText(_T("URL of the client."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("MaxConnections"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_UInt32));
	currentArgument.setDescription(LocalizedText(_T("The maximum number of connections allowed for this client. (Ignored for remove)"), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	currentArgument.setName(_T("Remove"));
	currentArgument.setDataType(NodeId(EnumNumericNodeId_Boolean));
	currentArgument.setDescription(LocalizedText(_T("Requested operation. Set 'true' to remove connector."), _T("")));
	currentArgument.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(currentArgument);

	if(StatusCode::isBad(this->setInputArguments(&inputArgsNodeId, inputArguments)))
	{
		_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
		return false;
	}
	if(StatusCode::isBad(init(NULL)))
	{
		_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
		return false;
	}
	return true;
}

EnumStatusCode AddRemoveReverseConnectorMethod::handleExecuteRequest(
	SoftingOPCToolbox5::Server::MethodCallTransaction* transaction,
	SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArguments = request->getInputArguments();
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<EnumStatusCode> inputArgumentResults;
	tstring endpointUrlToFind;
	tstring clientUrl;
	OTUInt32 maxConnections = 0;
	OTBoolean remove = false;

	if(inputArguments.size() < 4)
	{
		result = EnumStatusCode_BadArgumentsMissing;
	}
	else if(inputArguments.size() > 4)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isGood(result))
	{
		if(inputArguments[0].getDataType() == EnumDataTypeId_String &&
			inputArguments[0].getArrayType() == EnumValueArrayType_Scalar)
		{
			endpointUrlToFind = inputArguments[0].getString();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}

		if(inputArguments[1].getDataType() == EnumDataTypeId_String &&
			inputArguments[1].getArrayType() == EnumValueArrayType_Scalar)
		{
			clientUrl = inputArguments[1].getString();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}

		if(inputArguments[2].getDataType() == EnumDataTypeId_UInt32 &&
			inputArguments[2].getArrayType() == EnumValueArrayType_Scalar)
		{
			maxConnections = inputArguments[2].getUInt32();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}

		if(inputArguments[3].getDataType() == EnumDataTypeId_Boolean &&
			inputArguments[3].getArrayType() == EnumValueArrayType_Scalar)
		{
			remove = inputArguments[3].getBoolean();
			inputArgumentResults.push_back(EnumStatusCode_Good);
		}
		else
		{
			inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
			result = EnumStatusCode_BadInvalidArgument;
		}

		request->setInputArgumentResults(inputArgumentResults);
	}

	if(StatusCode::isGood(result))
	{
		result = EnumStatusCode_BadNoMatch;
		std::vector<Server::EndpointPtr> endpoints = Application::instance()->getEndpoints();
		bool found = false;
		for(size_t i = 0; i < endpoints.size(); i++)
		{
			tstring currentEndpointUrl = endpoints[i]->getUrl();
			if(currentEndpointUrl == endpointUrlToFind)
			{
				// We found the target endpoint, perform add/remove as requested
				found = true;
				Server::EndpointPtr foundEndpoint = endpoints[i];
				if(remove)
				{
					result = foundEndpoint->removeReverseConnect(clientUrl);
				}
				else
				{
					result = foundEndpoint->addReverseConnect(clientUrl, maxConnections);
				}
				break;
			}
		}
		if(!found)
		{
			result = EnumStatusCode_BadNoMatch;
		}
	}

	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

	bool DenySecureChannelOpenMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
	{
		tstring methodName = _T("DenySecureChannelOpen");
		setNodeId(NodeId(namespaceIndex, methodName));
		setBrowseName(QualifiedName(methodName, namespaceIndex));
		setDisplayName(LocalizedText(methodName, _T("")));
		setDescription(LocalizedText(_T("Instruct the endpoint to deny secure channel opening"), _T("")));

		parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

		NodeId inputArgsNodeId(namespaceIndex, _T("DenySecureChannelOpenMethod_InputArguments"));
		std::vector<Argument> inputArguments;
		Argument currentArgument;
		currentArgument.setName(_T("EndpointUrl"));
		currentArgument.setDataType(NodeId(EnumNumericNodeId_String));
		currentArgument.setDescription(LocalizedText(_T("URL of an existing Endpoint."), _T("")));
		currentArgument.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(currentArgument);

		currentArgument.setName(_T("Deny"));
		currentArgument.setDataType(NodeId(EnumNumericNodeId_Boolean));
		currentArgument.setDescription(LocalizedText(_T("Set 'true' to deny secure channel opening."), _T("")));
		currentArgument.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(currentArgument);

		if(StatusCode::isBad(this->setInputArguments(&inputArgsNodeId, inputArguments)))
		{
			_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
			return false;
		}
		if(StatusCode::isBad(init(NULL)))
		{
			_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
			return false;
		}
		return true;
	}

	EnumStatusCode DenySecureChannelOpenMethod::handleExecuteRequest(
		SoftingOPCToolbox5::Server::MethodCallTransaction* transaction,
		SoftingOPCToolbox5::Server::MethodCallRequest* request)
	{
		_UNUSED(transaction);
		std::vector<Value> inputArguments = request->getInputArguments();
		EnumStatusCode result = EnumStatusCode_Good;
		std::vector<EnumStatusCode> inputArgumentResults;
		tstring endpointUrl;
		OTBoolean deny = false;

		if(inputArguments.size() < 2)
		{
			result = EnumStatusCode_BadArgumentsMissing;
		}
		else if(inputArguments.size() > 2)
		{
			result = EnumStatusCode_BadInvalidArgument;
		}

		if(StatusCode::isGood(result))
		{
			if(inputArguments[0].getDataType() == EnumDataTypeId_String &&
				inputArguments[0].getArrayType() == EnumValueArrayType_Scalar)
			{
				endpointUrl = inputArguments[0].getString();
				inputArgumentResults.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
				result = EnumStatusCode_BadInvalidArgument;
			}

			if(inputArguments[1].getDataType() == EnumDataTypeId_Boolean &&
				inputArguments[1].getArrayType() == EnumValueArrayType_Scalar)
			{
				deny = inputArguments[1].getBoolean();
				inputArgumentResults.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputArgumentResults.push_back(EnumStatusCode_BadTypeMismatch);
				result = EnumStatusCode_BadInvalidArgument;
			}

			request->setInputArgumentResults(inputArgumentResults);
		}

		if(StatusCode::isGood(result))
		{
			result = EnumStatusCode_BadNoMatch;
			std::vector<Server::EndpointPtr> endpoints = Application::instance()->getEndpoints();
			for(size_t i = 0; i < endpoints.size(); i++)
			{
				if(endpoints[i]->getUrl() == endpointUrl)
				{
					// We found the target endpoint, set deny flag
					Server::EndpointPtr foundEndpoint = endpoints[i];
					((TestEndpoint*)foundEndpoint.p())->setDenySecureChannelOpen(deny);
					result = EnumStatusCode_Good;
					break;
				}
			}
		}

		request->setStatusCode(result);
		request->complete();
		return EnumStatusCode_Good;
	}

	AddWorkItemAction::AddWorkItemAction(Util_WorkItem* pWorkItem)
	{
		m_pWorkItem = pWorkItem;
	}

	AddWorkItemAction::~AddWorkItemAction()
	{
		if(m_pWorkItem)
		{
			delete m_pWorkItem;
		}
	}

	void AddWorkItemAction::cleanUp()
	{
		// ensure the workitem is executed in any case
		if(m_pWorkItem)
		{
			m_pWorkItem->execute();
			delete m_pWorkItem;
			m_pWorkItem = NULL;
		}
	}

	void AddWorkItemAction::execute()
	{
		if(m_pWorkItem)
		{
			if(!getApplicationModule()->addWorkItem(m_pWorkItem))
			{
				m_pWorkItem->execute();
				delete m_pWorkItem;
			}
			// in any case, execute the workitem only once
			m_pWorkItem = NULL;
		}
	}

	bool ChangeWatcherMethod::initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex)
	{
		tstring methodName = _T("ChangeWatcher");
		setNodeId(NodeId(namespaceIndex, methodName));
		setBrowseName(QualifiedName(methodName, namespaceIndex));
		setDisplayName(LocalizedText(methodName, _T("")));
		setDescription(LocalizedText(_T("Change this endpoint watcher to reject repeated denied user authentication."), _T("")));

		parentNode->addReference(Statics::ReferenceTypeId_HasComponent, this);

		NodeId inputArgsNodeId(namespaceIndex, _T("ChangeWatcherMethod_InputArguments"));
		std::vector<Argument> inputArguments;
		Argument currentArgument;
		currentArgument.setName(_T("MaxFails"));
		currentArgument.setDataType(NodeId(EnumNumericNodeId_UInt32));
		currentArgument.setDescription(LocalizedText(_T("Lock endpoint after this number of failed attempts."), _T("")));
		currentArgument.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(currentArgument);

		currentArgument.setName(_T("CheckInterval"));
		currentArgument.setDataType(NodeId(EnumNumericNodeId_UInt32));
		currentArgument.setDescription(LocalizedText(_T("The failed attempts shall be within this interval."), _T("")));
		currentArgument.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(currentArgument);

		currentArgument.setName(_T("LockInterval"));
		currentArgument.setDataType(NodeId(EnumNumericNodeId_UInt32));
		currentArgument.setDescription(LocalizedText(_T("The endpoint is locked for this interval after failed attempts exceeded."), _T("")));
		currentArgument.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(currentArgument);
		if(StatusCode::isBad(this->setInputArguments(&inputArgsNodeId, inputArguments)))
		{
			_tprintf(_T("Failed to set the InputArguments of %s\n"), methodName.c_str());
			return false;
		}
		if(StatusCode::isBad(init(NULL)))
		{
			_tprintf(_T("Failed to initialize the %s\n"), methodName.c_str());
			return false;
		}
		return true;
	}

	EnumStatusCode ChangeWatcherMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
	{
		_UNUSED(transaction);
		std::vector<Value> inputArguments = request->getInputArguments();
		EnumStatusCode result = EnumStatusCode_Good;
		OTUInt32 maxFailedAttempts = 0;
		OTUInt32 checkInterval = 0;
		OTUInt32 lockInterval = 0;

		std::vector<EnumStatusCode> inputArgumentResults;
		result = validateInputArguments(inputArguments, inputArgumentResults);
		request->setInputArgumentResults(inputArgumentResults);

		EndpointPtr endpoint = transaction->getSession()->getEndpoint();
		if(endpoint.isNull())
		{
			result = EnumStatusCode_BadInternalError;
		}
		if(StatusCode::isGood(result))
		{
			result = inputArguments[0].getUInt32(&maxFailedAttempts);
		}
		if(StatusCode::isGood(result))
		{
			result = inputArguments[1].getUInt32(&checkInterval);
		}
		if(StatusCode::isGood(result))
		{
			result = inputArguments[2].getUInt32(&lockInterval);
		}

		if(StatusCode::isGood(result))
		{
			_UNUSED(maxFailedAttempts);
			_UNUSED(checkInterval);
			_UNUSED(lockInterval);
			result = endpoint->configureRepeatedConnectAttempts(maxFailedAttempts, checkInterval, lockInterval);
		}

		request->setStatusCode(result);
		request->complete();
		return EnumStatusCode_Good;
	}
