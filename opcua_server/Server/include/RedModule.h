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
#ifndef _REDMODULE_H_
#define _REDMODULE_H_

#include "Enums.h"
#include "ServerVariable.h"
#include "ServerObject.h"
#include "ClientSession.h"
#include "Util_Sync.h"
#include "ApplicationDescription.h"
#include "ServerEndpoint.h"
#include "ServerMultiStateItem.h"
#include "ServerMethod.h"
#include "Util_TimerThread.h"

#if TB5_CLIENT
// forward references
class ApplicationModule;

using namespace SoftingOPCToolbox5;

/*! class handling redundancy support of test server
*
* Possible extension: Add method for decision whether a specific variable in test server 
*  - shall be writable by client
*  - shall be changed by simulation 
*/
class RedModule
{
private:

	/*! Internal class to expose a redundancy internal endpoint
	*
	* Our test server uses OPC UA as redundancy data exchange protocol.
	* This is neither typical nor the best way - it is just convenience to avoid another protocol.
	* The endpoint shall not be used for other purposes.
	*
	*/
	class RedLinkEndpoint : public Server::Endpoint
	{
	private:
		RedLinkEndpoint();
	public:
		~RedLinkEndpoint();
		static ObjectPointer<RedLinkEndpoint> create();

		/*! This endpoint allows only a validated user access.
		*/
		virtual EnumStatusCode onAuthenticateUser(const IUserIdentityToken* userIdentityToken,
			const ISignatureData* signature,
			Server::Session* session,
			const IApplicationDescription* clientDescription,
			unsigned int* userGroupIndex);

		// static configuration needed both for server and client side
		static tstring c_redLinkPolicyId;
		static tstring c_redLinkUser;
		static tstring c_redLinkPwd;
	};
	typedef ObjectPointer<RedLinkEndpoint> RedLinkEndpointPtr;



	/*! Internal class to handle redundancy internal client sessions
	*/
	class RedSyncSession : public Client::Session
	{
	private:
		RedSyncSession();
		RedSyncSession(RedModule* redModule, size_t index);
	public:
		static ObjectPointer<RedSyncSession> create(RedModule* redModule, size_t index);
		~RedSyncSession();

		/*! Reads both service level and redundancy state from the redundant partner.
		* 
		* With this information decision which of their partners (including the local server) is the best choice for redundancy.
		*/
		EnumStatusCode readRedState();
		/*! Overloaded method to invoke an asynchronous read call*/
		EnumStatusCode readRedStateAsync();

		/*! Forces the redundancy partner to secondary by invoking a method on the partner server.
		*
		* The partner is forced to switch to the state SecondaryNotSync.
		* In case the partner was the previous primary it will refuse to become primary for a while.
		*/
		EnumStatusCode forceSecondary();

		/*! Synchronizes a number of nodes by invoking a method on the partner server
		*
		* Our test server simply synchronizes always all selected variables.
		*/
		EnumStatusCode syncData(const std::vector<NodeId> &nodesToSync, const std::vector<DataValue> &dataValuesToSync);

		/*!  Overwritten handler for asynchronous reads*/
		virtual void onReadComplete(void* transactionHandle, EnumStatusCode serviceResult, std::vector<DataValue>& values);
		/*!  Overwritten handler for connection status changes*/
		virtual void onStatusChange(bool status);

	private:
		/*! Handles both synchronous and asynchronous read responses*/
		void updateState(const std::vector<DataValue>& values);

		size_t m_index;			// internal index in the RedModule redundancy partner table
		RedModule* m_redModule;	// pointer to the RedModule
	};
	typedef ObjectPointer<RedSyncSession> RedSyncSessionPtr;



	/*! Internal class to handle cyclic redundancy actions 
	*/
	class RedundancyAction: public Util_ScheduledAction
	{
	public:
		RedundancyAction(RedModule* redModule, unsigned long timeout = 1000, bool isCyclic = true);
		virtual ~RedundancyAction();
		/*! Invokes the method of the containing RedModule */
		void execute();

	protected:
		RedModule* m_redModule; // containing RedModule
	};



	/*! Internal class to create and handle the force to secondary method. 
	*/
	class RedForceSecondaryMethod : public Server::Method
	{
	private:
		RedForceSecondaryMethod(RedModule* redModule);
	public:
		~RedForceSecondaryMethod();

		/*! Creates a new method object in the address space. 
		*
		* The parent object is given, the own node ID are provided by the containing RedModule in class variables.
		* The other information (strings, ...) are hard coded
		*/
		static ObjectPointer<RedForceSecondaryMethod> create(RedModule* redModule, Server::BaseNodePtr parent);

		/*! Overwritten handler. 
		*
		* The handler forces the containing RedModule to change its state to SecondaryNotSync.
		*/
		virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);

	private:
		RedModule* m_redModule;	// containing RedModule
	};
	typedef ObjectPointer<RedForceSecondaryMethod> RedForceSecondaryMethodPtr;



	/*! Internal class to create and handle the switch over method. 
	*/
	class RedSwitchMethod : public Server::Method
	{
	private:
		RedSwitchMethod(RedModule* redModule);
	public:
		~RedSwitchMethod();

		/*! Creates a new method object in the address space. 
		*
		* The parent object is given, the own node ID are provided by the containing RedModule in class variables.
		* The other information (strings, ...) are hard coded
		*/
		static ObjectPointer<RedSwitchMethod> create(RedModule* redModule, Server::BaseNodePtr parent);

		/*! Overwritten handler. 
		*
		* The handler requests a state change in the containing RedModule to SecondaryNotSync.
		* The containing module can deny the request (e.g. if there is no secondary, ...)
		*/
		virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);

	private:
		RedModule* m_redModule;	// containing RedModule
	};
	typedef ObjectPointer<RedSwitchMethod> RedSwitchMethodPtr;



	/*! Internal class to create and handle the switch over method. 
	*/
	class RedSyncMethod : public Server::Method
	{
	private:
		RedSyncMethod(RedModule* redModule);
	public:
		~RedSyncMethod();

		/*! Creates a new method object in the address space. 
		*
		* The parent object is given, the own node ID are provided by the containing RedModule in class variables.
		* The other information (strings, ...) are hard coded
		*/
		static ObjectPointer<RedSyncMethod> create(RedModule* redModule, Server::BaseNodePtr parent);

		/*! Overwritten handler. 
		*
		* The handler passes the nodes and data values to the containing RedModule.
		* The containing module can deny the request (e.g. if its own server state is not secondary)
		*/
		virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);

	private:
		RedModule* m_redModule;	// containing RedModule
	};
	typedef ObjectPointer<RedSyncMethod> RedSyncMethodPtr;



	/*! Class storing information of a redundancy partner */
	class RedPartnerInfo
	{
	public:
		tstring url;				// URL of the partner
		RedSyncSessionPtr session;	// client session to handle redundancy internal communication
		OTUInt32 redState;			// the (last received) redundancy state of the partner
		OTUInt8 serviceLevel;		// the (last received) service level of the partner

		int compare(const RedPartnerInfo &other); // compares two information for decisions which is the better choice for primary
	};

public:
	/*! @brief Constructor */
	RedModule();

	/*! @brief Destructor */
	~RedModule();

	/*! Initialize the redundancy module (during startup) 
	*
	* - class variables (e.g. global known node IDs) are initialized
	* - If redundancy is enabled the redundancy information nodes (both internal and normally accessible) are created .
	* - The redundancy partner list is build
	* - The sessions to the partners is established
	*    If successful a synchronous read is performed for calculation of initial status
	* - the new redundancy state is calculated and exposed
	* - the cyclic redundancy action is started
	*
	* In case of normal execution (no redundancy), the endpoint is opened here.
	* In case of Redundancy, the given local endpoint is not yet opened here - it is opened within the RedModule::changeState() method
	*
	* @return Returns whether opening the non redundant local endpoint succeeded (always true for enabled redundancy) */
	bool init(ApplicationModule* app, const ApplicationDescription& applicationDescription, const std::vector<Server::EndpointPtr>& localEndpoints, EnumRedundancySupport redMode, const std::vector<tstring> &redUrls);

	/*! Cleans up the redundancy module (during shutdown) 
	*
	* In general all actions of the init() method are cleaned up (in reverse order)
	*/
	void end();

	/*! Returns whether the actual server is in a primary state */
	bool isPrimary();
	/*! Returns whether the given state is a primary state */
	static bool isPrimary(OTUInt32 redState);
	/*! Returns whether the actual server is in a secondary state */
	bool isSecondary();
	/*! Returns whether the given state is a secondary state */
	static bool isSecondary(OTUInt32 redState);

	/*! Callback initiated from redundancy partner sessions when new read values are available. */ 
	void onRedInfoChanged(size_t index, OTUInt32 redState, OTUInt8 serviceLevel);

	/*! Calculate the state of the local server with the information of the redundancy partners */
	OTUInt32 calcNewState();
	/*! Change the state to the desired state
	*
	* The change can be rejected if an invalid transition is given (e.g. switch to secondary if no other partner is in sync)
	*
	* @param newState	The new desired state
	* @param force		The validation of the transition is ignored
	* @param inCallback	If the state change is invoked out of a callback synchronous actions (e.g. closing an endpoint, ...) shall not be performed
	*
	*/
	EnumStatusCode changeState(OTUInt32 newState, bool force, bool inCallback);

	/*! Callback initiated from synchronization method handler. */ 
	void syncReceived(const std::vector<const INodeId*> &nodes, const std::vector<const IDataValue*> &dataValues);

	/*! Callback for cyclic redundancy action
	*
	* - executes possible state changes 
	* - executes the data synchronization (if primary)
	* - invokes another asynchronous read from redundancy partners
	*/ 
	void handleRedundancyCheckAction();

private:
	/*! builds the URL of internal redundancy link out of the given "normal" URL */
	tstring buildRedLinkUrl(const tstring &url);

	/*! Adds all variables below the given parent node (recursively) */
	static void addChildVariables(const NodeId &parent, std::vector<NodeId> &nodes, std::vector<Server::VariablePtr> &variables);

	// global states added to multi state variable
	static OTUInt32	c_RedStateStartUp;
	static OTUInt32	c_RedStateShutDown;
	static OTUInt32	c_RedStatePrimNoSync;
	static OTUInt32	c_RedStatePrimSyncronizing;
	static OTUInt32	c_RedStatePrimInSync;
	static OTUInt32	c_RedStateSecondaryNotSync;
	static OTUInt32	c_RedStateSecondarySynchronizing;
	static OTUInt32	c_RedStateSecondaryInSync;
	// get the name of the state
	static const OTChar * getStateName(OTUInt32 stateNumber);

	// node IDs of new nodes for redundancy information
	static NodeId* c_RedundancyInfoNodeId;
	static NodeId* c_ForceSecondaryNodeId;
	static NodeId* c_RedundancySwitchNodeId;
	static NodeId* c_RedundancySyncNodeId;

	static OTUInt32 c_waitTillBecomePrimaryAgain;	// max cycles until the server tries to gain primary (after switching from primary to secondary)
	static OTUInt32 c_syncCountsToWait;				// number of synchronization calls to be received before switching to SecondaryInSync

	Util_Sync m_sync;
	
	// objects, variables and methods
	Server::ObjectPtr m_redInfo;
	Server::MultiStateItemPtr m_pRedState;
	Server::VariablePtr m_serviceLevelVar;
	RedForceSecondaryMethodPtr m_redForceSecondaryMethod;
	RedSwitchMethodPtr m_redundancySwitchMethod;
	RedSyncMethodPtr m_redundancySyncMethod;


	OTUInt32 m_currentState;							// current redundancy state
	OTUInt32 m_syncReceivedCount;						// number of remaining sync method calls until state change secondary synchronizing -> InSync is performed
	OTUInt32 m_waitTillBecomePrimaryAgain;				// number of remaining redundancy actions to wait until the server tries to gain primary (after switching from primary to secondary)

	EnumRedundancySupport m_redMode;					// configured redundancy mode
	ApplicationModule* m_pAppModule;					// application module (parent module)
	RedLinkEndpointPtr m_redLinkEndpoint;				// endpoint for redundancy internal sessions, method calls, reads, ...
	std::vector<Server::EndpointPtr> m_localEndpoints;	// normal endpoints of the application: closed / opened depending on redundancy state

	std::vector<RedPartnerInfo> m_redPartnerInfos;		// information of all other redundancy partners
	RedundancyAction* m_pRedAction;						// the cyclic redundancy action. new read requests are initiated and nodes are synchronized to secondaries

	std::vector<NodeId> m_nodesToSync;					// nodes to be synchronized - are filled in init()
	std::vector<Server::VariablePtr> m_variablesToSync;	// variables to allow faster access to variable values
};
#endif

#endif
