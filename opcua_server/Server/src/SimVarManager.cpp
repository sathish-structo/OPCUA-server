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
#include "Util_SingleLock.h"
#include "Base.h"
#include "Enums.h"
#include "NodeId.h"
#include "Guid.h"
#include "ByteString.h"
#include "ExpandedNodeId.h"
#include "SimulationVariable.h"
#include "Statics.h"
#include "ToolkitTrace.h"
#include "ApplicationModule.h"
#include "SimVarManager.h"
#include "ServerDummyValueGenerator.h"
#include "ServerMethodCallRequest.h"

//#define _TRACE(__Message) Trace::instance()->writeSourceLine(EnumTraceLevel_Error, EnumTraceGroup_Api, __Message, _T(__FILE__), __LINE__);
#define _TRACE(__Message)

SimVarRecord::SimVarRecord()
{
	simVarSetId = 0;
	simulationStarted = false;
}

InputSimulationRecord::InputSimulationRecord()
{
	simVarSetId      = 0;
	changeInterval   = 0;
	repeatCount      = 0;
	increment        = 0;
	changeCount      = 0;
	bulkHandling     = 0;
	bulkSize         = 100;
	lastIndex        = 0;
#ifdef DEBUG_SIMCHANGES
	startTime        = 0;
	lastTime         = 0;
	performedChanges = 0;
#endif
	isCyclic         = false;
}

bool SimVarRecord::areEqualByNodeIds(const SimVarRecord& rec1, const SimVarRecord& rec2)
{
	if(rec1.nodeIds.size() != rec2.nodeIds.size())
		return false;
	for (vector<NodeId>::size_type i = 0; i < rec1.nodeIds.size( ); ++i)
	{
		NodeId nodeId1 = rec1.nodeIds[i];
		NodeId nodeId2 = rec2.nodeIds[i];
		if(nodeId1.getIdentifierType() != nodeId2.getIdentifierType())
			return false;
		Value value1 = nodeId1.getIdentifier();
		Value value2 = nodeId2.getIdentifier();
		if(!Value::areEqual(value1, value2))
			return false;
	}
	return true;
}

void SimVarChangeAction::execute()
{
	SimVarManager::getInstance().doSimulation(&simRecord);
}

SimVarManager *SimVarManager::m_Instance;

SimVarManager::SimVarManager(OTUInt16 namespaceIndex)
{
	// this can cause problems with numeric identifier 0
	_ASSERTION(namespaceIndex != 0, _T("namespace 0 should not be used for SimVarManager"));

	m_namespaceIndex = namespaceIndex;
	m_nextSimVarId = 0;
	m_nextFreeNumericIdentifier = 0;
	UTIL_INIT_SYNC(&m_Sync,"SimVarManager");
}

SimVarManager::SimVarManager()
{
}

SimVarManager::~SimVarManager()
{
	clear();
}

SimVarManager& SimVarManager::getInstance()
{
	return *m_Instance;
}

void SimVarManager::init(OTUInt16 namespaceIndex)
{
	if(!m_Instance)
	{
		m_Instance = new SimVarManager(namespaceIndex);
	}
}
void SimVarManager::exit()
{
	if (m_Instance)
	{
		delete m_Instance;
		m_Instance = NULL;
	}
}

void SimVarManager::clear()
{
	UTIL_SINGLE_LOCK(Util_Sync,lock,&m_Sync);
	map<OTUInt32, SimVarRecord*>::iterator it;
	for (it = m_simVarInfo.begin( ); it != m_simVarInfo.end( ); ++it)
	{
		SimVarRecord* pRec = it->second;
		delete pRec;
	}
	m_simVarInfo.clear();
	m_simulationVariables.clear();
	m_simulationVariableData.clear();
	m_namespaceIndex = 0;
	m_nextSimVarId = 0;
	m_nextFreeNumericIdentifier = 0;
}

void SimVarManager::setTestVariablesFolder(Object* testVariablesFolder)
{
	m_TestVariablesFolder = testVariablesFolder;
}

//BAL is not synch, but is called just from synch methods.
void SimVarManager::getNextSimVarId(OTUInt32* pSimVarId)
{
	m_nextSimVarId++;
	*pSimVarId = m_nextSimVarId;
}

CreateSimVarWorkItem::CreateSimVarWorkItem(Server::MethodCallRequestPtr request)
{
	m_request = request;
}

CreateSimVarWorkItem::~CreateSimVarWorkItem()
{
	m_request->complete();
}

void CreateSimVarWorkItem::execute()
{
	std::vector<Value> inputArgs = m_request->getInputArguments();
	vector<EnumStatusCode> inputStatusCodes;
	EnumStatusCode result = EnumStatusCode_Good;
	std::vector<Value> outputArgs;
	Value outputValue;
	// The ID associated with the current set of sim vars created.
	OTUInt32 simVarSetId = 0;

	SimVarRecord* pRec = new SimVarRecord();

	// The [IN] params for Range
	EnumIdentifierType		nodeIdType = EnumIdentifierType_Numeric;
	OTUInt32				count = 0;
	EnumDataTypeId			dataType = EnumDataTypeId_UInt32;
	std::vector<OTUInt32>	arrayDimensions;

	// Validate the input params
	if(inputArgs.size() != 4)
	{
		result = EnumStatusCode_BadInvalidArgument;
	}
	for(size_t i = 0; i < inputArgs.size(); i++)
	{
		switch(i)
		{
		case 0:
			// identifier type
			OTInt8 ch;
			if(inputArgs[i].getInt8(&ch) == EnumStatusCode_Good)
			{
				nodeIdType = static_cast<EnumIdentifierType>(ch);
				inputStatusCodes.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
			}
			break;
		case 1:
			// node count
			if(inputArgs[i].getUInt32(&count) == EnumStatusCode_Good)
			{
				inputStatusCodes.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
			}
			break;
		case 2:
			// data type
			{
				OTUInt32 dataTypeVal;
				if(inputArgs[i].getUInt32(&dataTypeVal) == EnumStatusCode_Good)
				{
					dataType = (EnumDataTypeId)dataTypeVal;
					// test if the data type is valid
					if((OTInt32)getEnumDataTypeIdFromString(getEnumDataTypeIdString(dataType)) != -1)
					{
						inputStatusCodes.push_back(EnumStatusCode_Good);
					}
					else
					{
						inputStatusCodes.push_back(EnumStatusCode_BadDataTypeIdUnknown);
					}
				}
				else
				{
					inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
				}
			}
			break;
		case 3:
			// array dimensions
			if(inputArgs[i].getUInt32Array(arrayDimensions) == EnumStatusCode_Good)
			{
				inputStatusCodes.push_back(EnumStatusCode_Good);
			}
			else
			{
				inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
			}
			break;
		default:
			inputStatusCodes.push_back(EnumStatusCode_BadInvalidArgument);
			break;
		}
	}

	if(StatusCode::isGood(result))
	{
		SimVarManager& simVarManager = SimVarManager::getInstance();
		// Transform the IN params to SimVarRecord
		result = simVarManager.rangeToSimVarRec(nodeIdType, count, dataType, arrayDimensions, pRec);
		if(StatusCode::isBad(result))
		{
			_tprintf(_T("Invalid arguments for CreatevariablesByRange\n"));
		}
		if(result == EnumStatusCode_Good)
		{
			// If all arguments were read successful call the creation of the variables method
			result = simVarManager.createVariables(pRec);
			if(result == EnumStatusCode_Good)
			{
				// We will send back the UID associated with the set of variable just created.
				simVarSetId = pRec->simVarSetId;
			}
		}
	}
	else
	{
		result = EnumStatusCode_BadInvalidArgument;
	}

	if(StatusCode::isBad(result))
	{
		delete pRec;
		pRec = NULL;
	}

	// SimVarSetId
	outputValue.setUInt32(simVarSetId);
	outputArgs.push_back(outputValue);
	// FirstNodeId

	{
		NodeId nullId;
		outputValue.setNodeId((pRec == NULL || pRec->nodeIds.empty()) ? &nullId : &pRec->nodeIds[0]);
		outputArgs.push_back(outputValue);
	}

	m_request->setStatusCode(result);
	m_request->setInputArgumentResults(inputStatusCodes);
	m_request->setOutputArguments(outputArgs);
	m_request->complete();

	return;
}

EnumStatusCode SimVarManager::createVariables(SimVarRecord* pSimVarRecord)
{
	UTIL_SINGLE_LOCK(Util_Sync,lock,&m_Sync);

#if TB5_ALARMS
	// Collect all changes to the address space so that only a single
	// ModelChange event is emitted for all changes.
	Server::AddressSpaceRoot::instance()->beginModelChange();
#endif

	// Create the sim vars
	SimVarSet simVarSet;
	SimVarSetData simVarSetData;
	DateTime startTime = DateTime::getUtcNow();
	if(insertVariables(pSimVarRecord, simVarSet, simVarSetData) == EnumStatusCode_Good)
	{
		// If insert into the address space succeeded, add it to the map.
		m_simVarInfo.insert(std::make_pair(pSimVarRecord->simVarSetId, pSimVarRecord));
		// Store each set of sim var associated with the UID in order to be retrieved at input simulation.
		m_simulationVariables.insert(std::make_pair(pSimVarRecord->simVarSetId, simVarSet));
		m_simulationVariableData.insert(std::make_pair(pSimVarRecord->simVarSetId, simVarSetData));
	}
	DateTime endTime = DateTime::getUtcNow();
	OTUInt64 diff = DateTime::getDurationMilliseconds(startTime, endTime);
	OTDouble varPreSec = (OTDouble)(pSimVarRecord->nodeIds.size() / ((OTDouble)diff / 1000.0));
	_tprintf(_T("Creating %u variables took %u ms (%1.2f / sec)\n"), (OTUInt32)pSimVarRecord->nodeIds.size(), (OTUInt32)diff, varPreSec);

	LocalizedText modelChangeEventMessage(_T("Model change detected"), _T("en"));
#if TB5_ALARMS
	Server::AddressSpaceRoot::instance()->endModelChange(&modelChangeEventMessage, 100);
#endif

	return EnumStatusCode_Good;
}

EnumStatusCode SimVarManager::insertVariables(SimVarRecord* pSimVarRecord, SimVarSet& simVarSet, SimVarSetData& simVarSetData)
{
	_ASSERTION(pSimVarRecord->nodeIds.size() > 0, _T("NodeIDs not set for create variables!"));
	_ASSERTION(pSimVarRecord->nodeIds.size() == pSimVarRecord->initialValues.size(), _T("Invalid SimVar params"));

	size_t nrOfNodesToInsert      = pSimVarRecord->nodeIds.size();

	simVarSet.reserve(nrOfNodesToInsert);
	simVarSetData.reserve(nrOfNodesToInsert);

	for(size_t i = 0; i < nrOfNodesToInsert; i++)
	{
		SimulationVariablePtr simVariable = new SimulationVariable();


		OTChar buff[255];
		_stprintf(buff,_T("TV_%03d_%08d"), (int)pSimVarRecord->simVarSetId, (int)i);
		tstring browseName(buff);
		simVariable->setBrowseName(QualifiedName(browseName, m_namespaceIndex));

		tstring displayName(buff);
		simVariable->setDisplayName(LocalizedText(displayName, _T("en")));

		ExpandedNodeId expandedNodeId;
		expandedNodeId.set(pSimVarRecord->nodeIds[i]);
		simVariable->setNodeId(expandedNodeId);

		// Set the dataType
		NodeId dataType;
		EnumDataTypeId dataTypeEnum = pSimVarRecord->initialValues[i].getDataType();
		if(DummyValueGenerator::supportsDataType(dataTypeEnum))
		{
			dataType = NodeId((EnumNumericNodeId)dataTypeEnum);
		}
		else
		{
			_TRACE(_T("Not supported data type for create nodes"));
			_tprintf(_T("Not supported data type for create nodes\n"));
			return EnumStatusCode_BadNotSupported;
		}

		simVariable->setDataType(dataType);
		simVariable->init(Statics::VariableTypeId_BaseDataVariableType);
		simVariable->setWriteMask(EnumWriteMask_DataType); // allow data type changes for ModelChangeEvent tests
		simVariable->setAccessLevel(simVariable->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		simVariable->setValueRank(ValueRank_Any);
		m_TestVariablesFolder->addReference(Statics::ReferenceTypeId_HasComponent, simVariable);

		DataValue initialValue;
		initialValue.setValue(pSimVarRecord->initialValues[i]);
		initialValue.setStatusCode(EnumStatusCode_Good);
		initialValue.setServerTimestamp(DateTime::getUtcNow());
		initialValue.setSourceTimestamp(DateTime::getUtcNow());
		simVariable->setDataValue(initialValue);

		// Emit ModelChange events for this node
		simVariable->setVersioned(_T("0"));

		// Store a reference to each sim var in a vector
		simVarSet.push_back(simVariable);
		simVarSetData.push_back(initialValue);
	}

	return EnumStatusCode_Good;
}

EnumStatusCode SimVarManager::deleteVariables(OTUInt32 simVarSetId)
{
	UTIL_SINGLE_LOCK(Util_Sync,lock,&m_Sync);

#if TB5_ALARMS
	// Collect all changes to the address space so that only a single
	// ModelChange event is emitted for all changes.
	Server::AddressSpaceRoot::instance()->beginModelChange();
#endif

	EnumStatusCode result = EnumStatusCode_Good;
	std::map<OTUInt32, SimVarRecord*>::iterator it = m_simVarInfo.find(simVarSetId);
	if(it != m_simVarInfo.end())
	{
		SimVarRecord* pRec = it->second;
		DateTime startTime = DateTime::getUtcNow();
		OTUInt32 variablesDeleted = (OTUInt32)pRec->nodeIds.size();
		{
			// Remove the variables from the address space
			std::map<OTUInt32, SimVarSet>::const_iterator itSet = m_simulationVariables.find(simVarSetId);
			if(itSet != m_simulationVariables.end())
			{
				SimVarSet simVarSet = itSet->second;
				//remove the variables from the address space
				SimulationVariablePtr simVarPtr;
				for(unsigned int j = 0; j < simVarSet.size(); j++)
				{
					simVarSet[j]->removeTree();
				}
			}
			// and remove all info about this record
			// Delete pRec
			delete pRec;
			// We can delete the variables info, no more references.
			m_simVarInfo.erase(it);
			// Remove the references to the Variable set
			OTUInt32 nrOfEntriesDeleted = (OTUInt32)m_simulationVariables.erase(simVarSetId);
			if(nrOfEntriesDeleted != 1)
			{
				// That's an error, we should have exactly one set stored for a given ID
				tstring message(_T("Set of simulation variables stored is inconsistent,"));
				OTChar buf[255];
				_stprintf(buf, _T(" nr. of sets for ID %d is %d"), simVarSetId, nrOfEntriesDeleted);
				message.append(buf);
				_TRACE(message.c_str());
				_REPORT_ASSERTION(message.c_str());
			}
			m_simulationVariableData.erase(simVarSetId);
		}
		DateTime endTime = DateTime::getUtcNow();
		OTUInt64 diff = DateTime::getDurationMilliseconds(startTime, endTime);
		OTDouble varPreSec = (OTDouble)(variablesDeleted / ((OTDouble)diff / 1000.0));
		_tprintf(_T("Deleting %u variables took %u ms (%1.2f / sec)\n"), variablesDeleted, (OTUInt32)diff, varPreSec);
	}
	else
	{
		tstring message(_T("Try to delete a set of simulation variables which doesn't exists: "));
		OTChar buf[255];
		_stprintf(buf, _T("%d"), simVarSetId);
		message.append(buf);
		Trace::instance()->writeSourceLine(EnumTraceLevel_Warning, EnumTraceGroup_User1, message, _T(__FILE__), __LINE__);
		result = EnumStatusCode_BadInvalidArgument;
	}

	LocalizedText modelChangeEventMessage(_T("Model change detected"), _T("en"));
#if TB5_ALARMS
	Server::AddressSpaceRoot::instance()->endModelChange(&modelChangeEventMessage, 100);
#endif

	return result;
}

EnumStatusCode SimVarManager::startSimulation(InputSimulationRecord* pInputSimRec)
{
	bool doStartSimulation = false;
	SimVarRecord* pRec = NULL;
	{	// Synchronized block for the SimManager
		UTIL_SINGLE_LOCK(Util_Sync,lock,&m_Sync);
		// Retrieve the info about the current set of sim vars
		std::map<OTUInt32, SimVarRecord*>::const_iterator it = m_simVarInfo.find(pInputSimRec->simVarSetId);
		if(it != m_simVarInfo.end())
		{
			if(pInputSimRec->repeatCount == 0)
				pInputSimRec->isCyclic = true;
			// Update the info about this set with the input simulation data.
			pRec = it->second;
			if(!pRec->simulationStarted)
			{
				pRec->inputSimulationRecord = *pInputSimRec;
				// Build the action necessary for the scheduler
				if(pInputSimRec->repeatCount == 0)
				{
					pRec->m_simAction.setCyclic(true);
				}
				else
				{
					// When we have a repeat count
					pRec->m_simAction.setCyclic(false);
				}
				pRec->m_simAction.setTimeout(pInputSimRec->changeInterval);
				pRec->m_simAction.simRecord = *pInputSimRec;
				pRec->simulationStarted = true;
				doStartSimulation = true;
			}
			else
			{
				_tprintf(_T("Simulation already started for UID: %d\n"), pInputSimRec->simVarSetId);
			}
		}
		else
		{
			_tprintf(_T("Start simulation for an inexistent set of nodes: %d\n"), pInputSimRec->simVarSetId);
			return EnumStatusCode_BadNotFound;
		}
	}

	if(doStartSimulation)
	{
		// This must be done outside the  Synchronized block for the SimManager, to avoid
		// deadlock with the timer thread.
		getApplicationModule()->addAction(&pRec->m_simAction);
	}

	return EnumStatusCode_Good;
}

void SimVarManager::doSimulation(InputSimulationRecord* pInputSimRecord)
{
#ifdef DEBUG_SIMCHANGES
	if(pInputSimRecord->startTime == 0)
	{
		pInputSimRecord->startTime = Util_Timer::currentTickCount();
		pInputSimRecord->lastTime = pInputSimRecord->startTime + 1;
		pInputSimRecord->callCount = 0;
		pInputSimRecord->performedChanges = 0;
	}

	static long lastSecs = -1;
	long thisSecs = Util_Timer::currentTickCount() / 100;

	if(thisSecs != lastSecs && pInputSimRecord->callCount != 0)
	{
		lastSecs = thisSecs;
		printf("ChangeCount...%f (dt = %f)\n", (double) (pInputSimRecord->performedChanges * 1000.0 / (double) (pInputSimRecord->lastTime - pInputSimRecord->startTime))
			, (double) (pInputSimRecord->lastTime - pInputSimRecord->startTime) / (double)pInputSimRecord->callCount);
	}

	pInputSimRecord->callCount++;
#endif

	UTIL_SINGLE_LOCK(Util_Sync,lock,&m_Sync);
	std::map<OTUInt32, SimVarSet>::const_iterator it = m_simulationVariables.find(pInputSimRecord->simVarSetId);
	std::map<OTUInt32, SimVarSetData>::const_iterator itData = m_simulationVariableData.find(pInputSimRecord->simVarSetId);
	if(it != m_simulationVariables.end() && itData != m_simulationVariableData.end())
	{
		SimVarSet& simVarSet = (SimVarSet&) it->second;
		SimVarSetData& simVarSetData = (SimVarSetData&) itData->second;
		unsigned int size = (unsigned int)simVarSet.size();

		// collect all variables and data in case of bulk handling
		std::vector<VariablePtr> variablesToChange;
		std::vector<const INodeId*> nodeIdsToChange;
		std::vector<const IDataValue*> dataToChange;

		// use the same time for all changes
		DateTime now = DateTime::getUtcNow();

		if(pInputSimRecord->changeCount) // we perform the given number of changes and make a wrap around
		{
			OTUInt32 changes = pInputSimRecord->changeCount;
			OTUInt32 lastIndex = pInputSimRecord->lastIndex;

			switch (pInputSimRecord->bulkHandling)
			{
			case 0:
				// no bulk handling
				break;
			case 1:
				// bulk handling
				nodeIdsToChange.reserve(pInputSimRecord->bulkSize);
				dataToChange.reserve(pInputSimRecord->bulkSize);
				break;
			}

			while(changes > 0)
			{
				if(lastIndex >= size)
				{
					lastIndex = 0;
				}

				switch (pInputSimRecord->bulkHandling)
				{
				case 0:
					// no bulk handling
					simVarSet[lastIndex]->incrementValue(pInputSimRecord->increment);
					break;
				case 1:
					// bulk handling
					{
						nodeIdsToChange.push_back(simVarSet[lastIndex]->getNodeId());
						simVarSet[lastIndex]->incrementValue(pInputSimRecord->increment, &now, &simVarSetData[lastIndex]);
						dataToChange.push_back(&simVarSetData[lastIndex]);
					}
					break;
				}
				lastIndex++;
				changes--;

				if(dataToChange.size() >= pInputSimRecord->bulkSize)
				{
					// in case of bulk handling change a bulk of 100 variables in one API call
					switch (pInputSimRecord->bulkHandling)
					{
					case 0:
						// no bulk handling, value was modified by SimulationVariable::incrementValue()
						break;
					case 1:
						// bulk handling
						AddressSpaceRoot::instance()->setVariableDataValues(nodeIdsToChange, dataToChange, NULL);
						dataToChange.clear();
						nodeIdsToChange.clear();
						break;
					}

				}
			}

#ifdef DEBUG_SIMCHANGES
			pInputSimRecord->performedChanges += pInputSimRecord->changeCount;
			pInputSimRecord->lastTime = Util_Timer::currentTickCount();
#endif
			pInputSimRecord->lastIndex = lastIndex;
		}
		else // make change on complete set of variables in each cycle
		{
			switch (pInputSimRecord->bulkHandling)
			{
			case 0:
				// no bulk handling
				break;
			case 1:
				// bulk handling
				nodeIdsToChange.reserve(size);
				dataToChange.reserve(size);
				break;
			}
			for(unsigned i = 0; i < size; i++)
			{
				switch (pInputSimRecord->bulkHandling)
				{
				case 0:
					// no bulk handling
					simVarSet[i]->incrementValue(pInputSimRecord->increment);
					break;
				case 1:
					// bulk handling
					{
						nodeIdsToChange.push_back(simVarSet[i]->getNodeId());
						simVarSet[i]->incrementValue(pInputSimRecord->increment, &now, &simVarSetData[i]);
						dataToChange.push_back(&simVarSetData[i]);
					}
					break;
				}
			}
		}

		// in case of bulk handling change write rest of the variables in one API call
		switch (pInputSimRecord->bulkHandling)
		{
		case 0:
			// no bulk handling
			break;
		case 1:
			// bulk handling
			AddressSpaceRoot::instance()->setVariableDataValues(nodeIdsToChange, dataToChange, NULL);
			break;
		}

		// Let's see if it is cyclic or with repeat count
		if(!pInputSimRecord->isCyclic)
		{
			pInputSimRecord->repeatCount--;

			if(pInputSimRecord->repeatCount == 0)
			{
				// We reached the end of repeat count, remove the action from the scheduler
				std::map<OTUInt32, SimVarRecord*>::const_iterator svrIt = m_simVarInfo.find(pInputSimRecord->simVarSetId);
				if(svrIt != m_simVarInfo.end())
				{
					SimVarRecord* pRec = svrIt->second;
					getApplicationModule()->removeAction(&pRec->m_simAction);
					// BAL we do not delete the record from SimVarManager, should be deleted from stopSimulation.
				}
			}
		}
	}
}

EnumStatusCode SimVarManager::stopAllSimulations()
{
	std::map<OTUInt32, SimVarRecord*> simVarInfoToStop;
	// the timer thread must be locked before this action!!!!
	if(getApplicationModule())
	{
		UTIL_SINGLE_LOCK(Util_Sync, applock, getApplicationModule()->getSync());
	}

	{
		UTIL_SINGLE_LOCK(Util_Sync, lock, &m_Sync);
		simVarInfoToStop = m_simVarInfo;
	}

	std::map<OTUInt32, SimVarRecord*>::iterator it;
	for(it = simVarInfoToStop.begin(); it != simVarInfoToStop.end(); it++)
	{
		stopSimulation(it->first);
	}

	{
		UTIL_SINGLE_LOCK(Util_Sync, lock, &m_Sync);
		while(m_simulationVariables.begin() != m_simulationVariables.end())
		{
			deleteVariables(m_simulationVariables.begin()->first);
		}
		m_simulationVariableData.clear();
	}

	return EnumStatusCode_Good;
}

EnumStatusCode SimVarManager::stopSimulation(OTUInt32 simVarSetId)
{
	SimVarRecord* pRec = NULL;
	{ // Synchronized block for the SimManager
		UTIL_SINGLE_LOCK(Util_Sync,lock,&m_Sync);
		if(simVarSetId == 0)
		{
			return EnumStatusCode_BadInvalidArgument;
		}
		std::map<OTUInt32, SimVarRecord*>::const_iterator it = m_simVarInfo.find(simVarSetId);
		if(it != m_simVarInfo.end())
		{
			pRec = it->second;
		}
		else
		{
			return EnumStatusCode_BadInvalidArgument;
		}
	}

	if(pRec)
	{
		// This must be done outside the  Synchronized block for the SimManager, to avoid
		// deadlock sith the timer thread.
		getApplicationModule()->removeAction(&pRec->m_simAction);
		pRec->simulationStarted = false;
	}
	return EnumStatusCode_Good;
}

/*! @brief Print all info about the current data from the SimVarManager.
*   Can be called from SubscriptionUtils asynch.
*   Can be deleted if not necessary.
*/
void SimVarManager::dump()
{
	UTIL_SINGLE_LOCK(Util_Sync,lock,&m_Sync);
	_tprintf(_T("----------- Simulation Variables Info --------------\n"));
	_tprintf(_T("Map simVarInfo:\n"));
	for (map<OTUInt32, SimVarRecord*>::iterator p = m_simVarInfo.begin( );
		p != m_simVarInfo.end( ); ++p )
	{
		_tprintf(_T("simVarSetID=%u\n"), p->first);
		unsigned int size = (unsigned int)p->second->nodeIds.size();
		for(unsigned i = 0; i < size; i++)
		{
			NodeId nodeId = p->second->nodeIds[i];
			_tprintf(_T("NodeId=%s "),nodeId.toString().c_str());
		}
	}
	_tprintf(_T("\nMap simVarInfo end\n"));
	_tprintf(_T("Map Simulation Variables:\n"));
	for (map<OTUInt32, SimVarSet>::iterator p = m_simulationVariables.begin( );
		p != m_simulationVariables.end( ); ++p )
	{
		_tprintf(_T("simVarSetID=%u \n"),p->first );
	}
	_tprintf(_T("\nMap Simulation Variables end\n"));
	_tprintf(_T("Allocated Namespace Indexes:\n"));
	std::list<unsigned short>::iterator listIt = m_allocatedNamespaceIndexes.begin();
	for(; listIt != m_allocatedNamespaceIndexes.end(); listIt ++)
	{
		_tprintf(_T("namespaceIndex:%u"), *listIt);
	}
	_tprintf(_T("----------- END Simulation Variables Info --------------\n"));
}

/*! Build input data for SimVarManager from range information.*/
EnumStatusCode SimVarManager::rangeToSimVarRec(EnumIdentifierType nodeIdType,
											OTUInt32 count, EnumDataTypeId dataType, const std::vector<OTUInt32>& arrayDimensions, SimVarRecord* pRec)
{
	if(nodeIdType != EnumIdentifierType_Numeric &&
		nodeIdType != EnumIdentifierType_String &&
		nodeIdType != EnumIdentifierType_Guid   &&
		nodeIdType != EnumIdentifierType_Opaque)
	{
		return EnumStatusCode_BadInvalidArgument;
	}
	if(count == 0)
	{
		return EnumStatusCode_BadNothingToDo;
	}
	std::vector<NodeId> nodeIds;
	nodeIds.reserve(count);

	// If insert succeeded associate a UID for this set of variables
	getNextSimVarId(&pRec->simVarSetId);

	OTUInt32 startIndex = 0;	// used to skip ranges of already existing nodes
	for(;;)
	{
		bool nodeIdsAvailable = true;
		for(OTUInt32 i = startIndex; i < startIndex + count; i++)
		{
			NodeId nodeId;
			switch(nodeIdType)
			{
			case EnumIdentifierType_Numeric:
				{
					OTUInt32 nextNumericIdentifier;
					getNextNumericIdentifier(&nextNumericIdentifier);
					nodeId.setNumeric(m_namespaceIndex, nextNumericIdentifier);
				}
				break;
			case EnumIdentifierType_String:
				{
					OTChar buff[16];
					_stprintf(buff, _T("TV_%03d_%08d"), (int)pRec->simVarSetId, (int)i);
					nodeId.setString(m_namespaceIndex, buff);
				}
				break;
			case EnumIdentifierType_Guid:
				{
					Guid guid = Guid::generateGuid();
					nodeId.setGuid(m_namespaceIndex, &guid);
				}
				break;
			case EnumIdentifierType_Opaque:
				{
					OTUInt32 nextNumericIdentifier;
					getNextNumericIdentifier(&nextNumericIdentifier);
					ByteString byteString;
					//BAL ad-hoc solution... you can try a better one
					byteString.init(sizeof(unsigned int),(unsigned char*)&nextNumericIdentifier);
					nodeId.setOpaque(m_namespaceIndex, byteString);
				}
				break;
			case EnumIdentifierType_Invalid:
				return EnumStatusCode_Bad;
			}
			if(AddressSpaceRoot::instance()->getNode(&nodeId).isNotNull())
			{
				// retry all NodeIds at a higher start index
				nodeIdsAvailable = false;
				nodeIds.clear();
				startIndex += i + 1;
				break;
			}
			nodeIds.push_back(nodeId);
		}
		if(nodeIdsAvailable)
		{
			break;
		}
	}

	if(DummyValueGenerator::supportsDataType(dataType))
	{
		pRec->initialValues.reserve(count);
		for(unsigned int i = 0; i < count; i++)
		{
			Value val = DummyValueGenerator::generateDummyValue(dataType, i, &arrayDimensions);
			pRec->initialValues.push_back(val);
		}
	}
	else
	{
		_TRACE(_T("Not supported data type for simulation variables"));
		_tprintf(_T("Data type %s is not supported for simulation variables\n"), getEnumDataTypeIdString(dataType));
		return EnumStatusCode_BadNotSupported;
	}
	pRec->nodeIds = nodeIds;
	return EnumStatusCode_Good;
}

void SimVarManager::getNextNumericIdentifier(OTUInt32* pNumericIdentifier)
{
	*pNumericIdentifier = m_nextFreeNumericIdentifier;
	m_nextFreeNumericIdentifier++;
}
