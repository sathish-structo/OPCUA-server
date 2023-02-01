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
#ifndef ENDPOINT_MANIPULATION_METHODS_H
#define ENDPOINT_MANIPULATION_METHODS_H

#include "ServerMethod.h"
#include "ServerMethodCallRequest.h"
#include "Util_TimerThread.h"
#include "TestEndpoint.h"
#include "Util_WorkItem.h"

class CreateEndpointMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);

	static EnumStatusCode createEndpoint(tstring url,
		tstring pkiStorePath = _T(""),
		tstring certificateFile = _T(""), tstring privateKeyFile = _T(""), tstring privateKeyPassword = _T(""),
		OTUInt16 crlCheckOption = 0xFFFF,
		bool restrictEndpoint = false,
		bool endpointsMatchRequest = false);

	static EnumStatusCode setDefaultSecurityConfigurations(SoftingOPCToolbox5::Server::EndpointPtr endpoint);
	static EnumStatusCode setDefaultUserTokenPolicies(SoftingOPCToolbox5::Server::EndpointPtr endpoint);
	static EnumStatusCode setPkiStoreConfiguration(SoftingOPCToolbox5::Server::EndpointPtr endpoint,
		const tstring& pkiStoreLocation = _T(""),
		OTUInt16 crlCheckOption = 0xFFFF);

	/*! Generates PolicyIds like <tokenType>_<policyIdAddon>, configures the custom issued token type for issued tokens */
	static SoftingOPCToolbox5::UserTokenPolicy createUserTokenPolicy(EnumUserTokenType tokenType, const tstring& policyIdAddon, const tstring& securityPolicyUri);
};

typedef SoftingOPCToolbox5::ObjectPointer<CreateEndpointMethod> CreateEndpointMethodPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const CreateEndpointMethod> CreateEndpointMethodConstPtr;

class CreateEndpointWorkItem : public Util_WorkItem
{
public:
	CreateEndpointWorkItem(tstring url, bool restrictEndpoint, tstring pkiStorePath, tstring certificateFile, tstring privateKeyFile, 
		tstring privateKeyPassword, OTUInt16 crlCheckOption, bool endpointsMatchRequest, SoftingOPCToolbox5::Server::MethodCallRequestPtr requestToComplete);
	~CreateEndpointWorkItem();
	virtual void execute();
private:
	CreateEndpointWorkItem();

	tstring m_url;
	tstring m_pkiStorePath;
	tstring m_certificateFile;
	tstring m_privateKeyFile;
	tstring m_privateKeyPassword;
	bool m_restrictEndpoint;
	OTUInt16 m_crlCheckOption;
	bool m_endpointsMatchRequest;
	SoftingOPCToolbox5::Server::MethodCallRequestPtr m_requestToComplete;
};

class RemoveEndpointMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
	static EnumStatusCode removeEndpoint(const tstring& url);
	static EnumStatusCode removeEndpoint(SoftingOPCToolbox5::Server::EndpointPtr endpoint);
};

typedef SoftingOPCToolbox5::ObjectPointer<RemoveEndpointMethod> RemoveEndpointMethodPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const RemoveEndpointMethod> RemoveEndpointMethodConstPtr;

class RemoveEndpointWorkItem : public Util_WorkItem
{
public:
	RemoveEndpointWorkItem(tstring url);
	~RemoveEndpointWorkItem();
	virtual void execute();
private:
	tstring m_url;
};

/*! Temporarily closes an endpoint */
class DisableEndpointMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);

	// the UA method call has an unsigned duration, so negative values for no duration are reserved for the server internal usage
	static EnumStatusCode closeEndpoint(const tstring& url, OTInt32 closeDuration = -1);
	static EnumStatusCode closeEndpoint(SoftingOPCToolbox5::Server::EndpointPtr endpoint, OTInt32 closeDuration = -1);

	static EnumStatusCode reopenEndpoint(const tstring& url);
	static EnumStatusCode reopenEndpoint(SoftingOPCToolbox5::Server::EndpointPtr endpoint);
};

typedef SoftingOPCToolbox5::ObjectPointer<DisableEndpointMethod> DisableEndpointMethodPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const DisableEndpointMethod> DisableEndpointMethodConstPtr;

class DisableEndpointWorkItem : public Util_WorkItem
{
public:
	DisableEndpointWorkItem(tstring url, OTUInt32 closeDuration);
	~DisableEndpointWorkItem();
	virtual void execute();
private:
	DisableEndpointWorkItem();
	tstring m_url;
	OTUInt32 m_closeDuration;
};

class OpenEndpointWorkItem : public Util_WorkItem
{
public:
	OpenEndpointWorkItem(tstring url);
	~OpenEndpointWorkItem();
	virtual void execute();
private:
	OpenEndpointWorkItem();
	tstring m_url;
};

class AddRemoveReverseConnectorMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
};

typedef SoftingOPCToolbox5::ObjectPointer<AddRemoveReverseConnectorMethod> AddRemoveReverseConnectorMethodPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const AddRemoveReverseConnectorMethod> AddRemoveReverseConnectorMethodConstPtr;

class DenySecureChannelOpenMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
};

typedef SoftingOPCToolbox5::ObjectPointer<DenySecureChannelOpenMethod> DenySecureChannelOpenMethodPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const DenySecureChannelOpenMethod> DenySecureChannelOpenMethodConstPtr;

class AddWorkItemAction
	: public Util_ScheduledAction
{
public:
	AddWorkItemAction(Util_WorkItem* pWorkItem);
	virtual ~AddWorkItemAction();
	// ensure the work item is invoked in any case
	virtual void cleanUp();
	virtual void execute();
private:
	Util_WorkItem* m_pWorkItem;
};


/*! Action to delay the destruction of the API object when removing the endpoint.
* This might be required for delayed deleteSession, deleteSubscriptions and deleteMonitoredItem handling,
* which are invoked by closing the endpoint */
class DelayReleaseEndpointAction
	: public Util_ScheduledAction
{
public:
	DelayReleaseEndpointAction(Server::EndpointPtr endpoint);
	virtual ~DelayReleaseEndpointAction();
	virtual void cleanUp() { execute(); };
	virtual void execute();
private:
	Server::EndpointPtr m_endpoint;
};

class ChangeWatcherMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	bool initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode, OTUInt16 namespaceIndex);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
};

typedef SoftingOPCToolbox5::ObjectPointer<ChangeWatcherMethod> ChangeWatcherMethodPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const ChangeWatcherMethod> ChangeWatcherMethodConstPtr;

#endif	// ENDPOINT_MANIPULATION_METHODS_H
