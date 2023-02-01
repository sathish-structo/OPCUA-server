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
#ifndef _SIMVAR_MANAGER_H_
#define _SIMVAR_MANAGER_H_

#include <list>
#include "Util_Sync.h"
#include "LocalizedText.h"
#include "QualifiedName.h"
#include "SimulationVariable.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		class MethodCallRequest;
		typedef ObjectPointer<MethodCallRequest> MethodCallRequestPtr;
	}
}

/*! Contains data used by the input simulation for a set of sim vars.*/
struct InputSimulationRecord
{
	/*! The ID of the sim var set to be updated.*/
	OTUInt32 simVarSetId;

	/*! The simulation interval in milliseconds. */
	OTUInt32 changeInterval;

	/*! Number of input simulation cycles.*/
	OTUInt32 repeatCount;

	/*! Increment value for numeric types.*/
	double increment;

	/*! Number of changes to be performed in one change cylce.
	if 0 the number of variables in the simvar set will be used */
	OTUInt32 changeCount;

	/*! Denote if is forever or with repeat count.*/
	bool isCyclic;

	/*! Denote which bulk mode is used.*/
	OTUInt32 bulkHandling;
	OTUInt32 bulkSize;

	/*! The last index which was used in a simulation call to continue with the next variable.*/
	OTUInt32 lastIndex;

#ifdef DEBUG_SIMCHANGES
	OTUInt32 performedChanges;

	OTUInt32 startTime;

	OTUInt32 lastTime;

	OTUInt32 callCount;
#endif

	InputSimulationRecord();
};

/*! Input class for the scheduler. Used by the input simulation.*/
class SimVarChangeAction
	: public Util_ScheduledAction
{
public:
	SimVarChangeAction()
	{
	}

	virtual void execute();

	/** @brief Input simulation data.
	*/
	InputSimulationRecord simRecord;
};

/*! @brief utility structure to hold information about
*   a set of simulation variables allocation.
*/
struct SimVarRecord
{
	/*! allocated nodeIds. */
	std::vector<NodeId> nodeIds;

	/*! The UID allocated for this record by the server */
	OTUInt32 simVarSetId;

	/*! Initial values of the value attribute of the nodes to be created.*/
	std::vector<Value> initialValues;

	/*! All data related to the input simulation for this set of variables.*/
	InputSimulationRecord inputSimulationRecord;

	/*! @brief The change variable action. Will be executed by the server scheduler
	 *  at each changeInterval.
	 *  @par A set of sim vars has associated one action.
	 */
	SimVarChangeAction m_simAction;

	bool simulationStarted;

	SimVarRecord();

	/*! A 'special' equal test, verify just the identifiers of the nodeIds. */
	static bool areEqualByNodeIds(const SimVarRecord& rec1, const SimVarRecord& rec2);
};

typedef ObjectPointer<SimulationVariable> SimulationVariablePtr;
typedef ObjectPointer<const SimulationVariable> SimulationVariableConstPtr;
typedef	vector<SimulationVariablePtr> SimVarSet;
typedef	vector<DataValue> SimVarSetData;

class CreateSimVarWorkItem : public Util_WorkItem
{
public:
	CreateSimVarWorkItem(Server::MethodCallRequestPtr request);
	~CreateSimVarWorkItem();
	virtual void execute();
	Server::MethodCallRequestPtr m_request;
};

/*! @brief Handles the simulation variables on the test server
*/
class SimVarManager
{
public:
	static SimVarManager& getInstance();

	static void init(OTUInt16 namespaceIndex);
	static void exit();

	/*! Create a set of simulation variables on server-side.
	 *  @param simVarRecord The data of the simulation variables to be created,
	 *  stores also the created serverID for this record.
	 */
	EnumStatusCode createVariables(SimVarRecord* pSimVarRecord);

	/*! Start the input simulation for a given set of nodes,
	 *  with the parameters specified.
	 */
	EnumStatusCode startSimulation(InputSimulationRecord* pInputSimRec);

	/*! Method executed at each input simulation cycle.*/
	void doSimulation(InputSimulationRecord* pInputSimRecord);

	/*! Stop the simulation and deletes a set of sim vars.
	 *  @param simVarSetId The ID of the set of sim vars.
	 */
	EnumStatusCode stopSimulation(OTUInt32 simVarSetId);

	/*! Makes a reset on sim settings to allow the client to reset the server in a initial state */
	EnumStatusCode stopAllSimulations();

	/*! Set the folder where to link the methods.*/
	void setTestVariablesFolder(Object* testVariablesFolder);

	/*! Used just for debug.*/
	void dump();

	/*! @brief Transform the range input data to record used by the SimVarManager.
	* @param nodeIdType      [IN] The type of nodeIds to be created.
	* @param count           [IN] How many nodes to create
	* @param arrayDimensions [IN] The array dimensions of variables to create. Empty means scalar.
	* @param pRec            [OUT] The simulation record build upon the input data.
	*        It is the standard input data for SimVarManager::CreateVariables.
	*/
	EnumStatusCode rangeToSimVarRec(EnumIdentifierType nodeIdType,
			OTUInt32 count, EnumDataTypeId dataType, const std::vector<OTUInt32>& arrayDimensions,
			SimVarRecord* pRec);

private:
	/*! Get the next unique SimvarID. Each set of sim var has associated an ID.
	*   @param pSimVarId The simulation variable ID returned.
	*/
	void getNextSimVarId(OTUInt32* pSimVarId);

	/*! Get the next numeric identifier, which is unused of this manager.
	* The identifier might be used by other nodes */
	void getNextNumericIdentifier(OTUInt32* pNumericIdentifier);

	/*! Insert a set of variables to the address space. */
	EnumStatusCode insertVariables(SimVarRecord* pSimVarRecord, SimVarSet& simVarSet, SimVarSetData& simVarSetData);

public:
	/*! Delete a set of variables from the address space.
	*  @param serverID The UID of this set of nodes.
	*/
	EnumStatusCode deleteVariables(OTUInt32 serverID);

private:
	/*! List with all namespace indexes allocated for the simulation variables.*/
	std::list<unsigned short> m_allocatedNamespaceIndexes;

	/*! An index for the namespace indexes. */
	OTUInt16 m_namespaceIndex;

	/*! A constant for the upper value of the namespace indexes to be allocated.*/
	static const unsigned short MAX_NAMESPACE_INDEX   = 1000;

	/*! The UID assigned to each set of allocated nodeId.*/
	OTUInt32 m_nextSimVarId;

	OTUInt32 m_nextFreeNumericIdentifier;

	/*! All the info related to simulation variables.*/
	std::map<OTUInt32, SimVarRecord*> m_simVarInfo;

	/*! Stores reference to all sets of sim vars created.*/
	std::map<OTUInt32, SimVarSet > m_simulationVariables;
	std::map<OTUInt32, SimVarSetData > m_simulationVariableData;
	OTUInt32 m_lastUsedNumericIndex;

	SimVarManager(OTUInt16 namespaceIndex);
	SimVarManager();
	SimVarManager(const SimVarManager&);
	SimVarManager& operator=(const SimVarManager&);
	void clear();
	~SimVarManager();

	/*! @brief OPC variable - Test Variables folder */
	ObjectPtr m_TestVariablesFolder;

	/*! The only mutex used along SimVarManager.*/
	Util_Sync m_Sync;

	static SimVarManager *m_Instance;
};


#endif
