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
#ifndef DYNAMICNODECACHE_H
#define DYNAMICNODECACHE_H

// includes
//#include "ServerVariable.h"
#include "ReferenceDescription.h"
#include "Util_Mutex.h"
#include "ServerCreateMonitoredItemRequest.h"
#include "ServerBaseNode.h"
#include "ServerReadRequest.h"
#include "ServerWriteRequest.h"
#include "ServerSession.h"
#include "ServerExportCustomNodesTransaction.h"
#include "Util_WorkItem.h"
#include "ServerReadTransaction.h"
#include <vector>
#include <set>
#include "ServerSubscription.h"
#include "DataTypeDescription.h"

class VirtualNodeInfo
{
public:
	VirtualNodeInfo(EnumNodeClass nodeClass);
	virtual ~VirtualNodeInfo();

	// access methods to attributes
	EnumNodeClass getNodeClass() const;

	const SoftingOPCToolbox5::INodeId& getNodeId() const;
	void setNodeId(const SoftingOPCToolbox5::INodeId& nodeId);

	void setMonitoredItemsWithoutNode(bool flag);
	bool getMonitoredItemsWithoutNode() const;
	void updateMonitoredItems(EnumAttributeId attributeId = EnumAttributeId_Invalid);

	void getAttribute(EnumAttributeId attributeId, SoftingOPCToolbox5::DataValue & dataValue);
	EnumStatusCode setAttribute(EnumAttributeId attributeId, const SoftingOPCToolbox5::IDataValue* dataValue);

	const SoftingOPCToolbox5::IQualifiedName& getBrowseName() const;
	void setBrowseName(const SoftingOPCToolbox5::IQualifiedName& browseName);

	const SoftingOPCToolbox5::ILocalizedText& getDisplayName() const;
	void setDisplayName(const SoftingOPCToolbox5::ILocalizedText& displayName);

	const SoftingOPCToolbox5::ILocalizedText& getDescription() const;
	void setDescription(const SoftingOPCToolbox5::ILocalizedText& description);

	OTUInt32 getWriteMask() const;
	void setWriteMask(OTUInt32 writeMask);

	const std::vector<SoftingOPCToolbox5::ReferenceDescription>& getReferences() const;
	void setReferences(const std::vector<SoftingOPCToolbox5::ReferenceDescription>& references);

	const SoftingOPCToolbox5::IDataValue& getDataValue() const;
	void setDataValue(const SoftingOPCToolbox5::IDataValue& value);

	OTUInt8 getAccessLevel() const;
	void setAccessLevel(OTUInt8 accessLevel);

	const SoftingOPCToolbox5::INodeId& getDataTypeId() const;
	void setDataTypeId(const SoftingOPCToolbox5::INodeId& dataTypeId);

	OTInt32 getValueRank() const;
	void setValueRank(OTInt32 valueRank);

	const std::vector<OTUInt32>& getArrayDimensions() const;
	void setArrayDimensions(const std::vector<OTUInt32>& arrayDimensions);

	bool getExecutable() const;
	void setExecutable(bool executable);

	bool getHistorizing() const;
	void setHistorizing(bool historizing);

	double getMinimumSamplingInterval() const;
	void setMinimumSamplingInterval(double minimumSamplingInterval);

	EnumEventNotifier getEventNotifier() const;
	void setEventNotifier(EnumEventNotifier eventNotifier);

	bool getContainsNoLoops() const;
	void setContainsNoLoops(bool containsNoLoops);

	bool getIsAbstract() const;
	void setIsAbstract(bool isAbstract);

	const SoftingOPCToolbox5::ILocalizedText& getInverseName() const;
	void setInverseName(const SoftingOPCToolbox5::ILocalizedText& inverseName);

	SoftingOPCToolbox5::DataTypeDescriptionConstPtr getDataTypeDescription() const;
	void setDataTypeDescription(SoftingOPCToolbox5::DataTypeDescriptionConstPtr dataTypeDescription);

	bool getIsSymmetric() const;
	void setIsSymmetric(bool isSymmetric);

	OTAtomic incrementMiCount();
	OTAtomic decrementMiCount();

	std::vector<VirtualNodeInfo*> getLinkedNodeInfos() const;
	/*! Add a VirtualNodeInfo, whose node shall remain in address space, as long as this VirtualNodeInfo's node is in the address space.
	* @note Ensure to create no loops */
	void addLinkedNodeInfo(size_t indexInStore);

		/* Sets the pointer to the store that contains the VirtualNodeInfos
	* This is used to access linked VirtualNodeInfos */
	void setPointerToStore(std::vector<VirtualNodeInfo>* pNodeInfoStore);

	const SoftingOPCToolbox5::INodeId& getMonitoredItemCountNodeId() const;
	void setMonitoredItemCountNodeId(const SoftingOPCToolbox5::INodeId& nodeId);

	EnumStatusCode handleMonitoredItemCreation(SoftingOPCToolbox5::Server::MonitoredItemPtr monitoredItem);
	EnumStatusCode handleMonitoredItemDeletion(SoftingOPCToolbox5::Server::MonitoredItemPtr monitoredItem);

	std::vector<SoftingOPCToolbox5::ReferenceDescription> searchReferences(const SoftingOPCToolbox5::NodeId& nodeIdToBrowse,
		const SoftingOPCToolbox5::NodeId& referenceTypeId,
		bool includeSubtypes,
		EnumBrowseDirection browseDirection,
		EnumNodeClass nodeClassMask);

private:
	EnumNodeClass m_nodeClass;
	bool m_monitoredItemsWithoutNode;
	std::set<SoftingOPCToolbox5::Server::MonitoredItemPtr> m_monitoredItems;
	SoftingOPCToolbox5::NodeId m_nodeId;
	SoftingOPCToolbox5::QualifiedName m_browseName;
	SoftingOPCToolbox5::LocalizedText m_displayName;
	SoftingOPCToolbox5::LocalizedText m_description;
	OTUInt32 m_writeMask;
	std::vector<SoftingOPCToolbox5::ReferenceDescription> m_references;

	// attributes for variables / variable types
	OTUInt8 m_accessLevel;
	SoftingOPCToolbox5::NodeId m_dataTypeId;
	OTInt32 m_valueRank;
	std::vector<OTUInt32> m_arrayDimensions;
	SoftingOPCToolbox5::DataValue m_dataValue;
	bool m_historizing;
	double m_minimumSamplingInterval;

	// attributes for methods
	bool m_executable;

	// attributes for objects / views
	EnumEventNotifier m_eventNotifier;

	// attributes for views
	bool m_containsNoLoops;

	// attributes for types
	bool m_isAbstract;

	// attributes for data types
	SoftingOPCToolbox5::DataTypeDescriptionConstPtr m_dataTypeDescription;

	// attributes for reference types
	SoftingOPCToolbox5::LocalizedText m_inverseName;
	bool m_isSymmetric;

	OTAtomic m_monitoredItemCount;
	SoftingOPCToolbox5::NodeId m_monitoredItemCountNodeId;
	/* the MI count of the linked node infos has to be increased and decreased as well,
	* whenever it is done for this node info. This is required for virtual instances of virtual types.
	* Pointer access to Elements in a vector with possibly increasing content is unsafe, use a pointer to the store and an index instead. */
	std::vector<VirtualNodeInfo>* m_pNodeInfoStore;
	std::vector<size_t> m_linkedNodeInfoIndexes;
};

/*! Contains all necessary information about a dynamic node.
* This class is designed for nodes of the type Variable. */
class DynamicNodeCache
{
private:
	DynamicNodeCache();

public:
	virtual ~DynamicNodeCache();

	static EnumStatusCode initialize(SoftingOPCToolbox5::Server::BaseNodePtr parentNode);

	static DynamicNodeCache* instance();

	static void deleteInstance();

	/*! Locks the synchronization lock for this cache.
	* @note This lock has to be set whenever a value is accessed. */
	void lock();
	/*! Unlocks the synchronization lock for this cache.
	* @note This lock has to be set whenever a value is accessed. */
	void unlock();

	/*! Handles creation of unknown monitored items. */
	EnumStatusCode handleMonitoredItemCreation(SoftingOPCToolbox5::Server::MonitoredItemPtr monitoredItem, EnumStatusCode origStatus);
	/*! Handles deletion of unknown monitored items. */
	EnumStatusCode handleMonitoredItemDeletion(SoftingOPCToolbox5::Server::MonitoredItemPtr monitoredItem, EnumStatusCode origStatus);

	bool handleReadRequest(SoftingOPCToolbox5::Server::ReadTransactionPtr transaction, SoftingOPCToolbox5::Server::ReadRequestPtr request);
	bool handleWriteRequest(SoftingOPCToolbox5::Server::WriteRequestPtr request);

	/* Returns the index within the internal virtual node info store */
	size_t addVirtualNode(const VirtualNodeInfo &nodeInfo);
	bool resolveDynamicNodePath(const SoftingOPCToolbox5::INodeId& startingNodeId, EnumBrowseDirection browseDir, const SoftingOPCToolbox5::INodeId* pReferenceId, bool includeSubtypes, 
		const SoftingOPCToolbox5::IQualifiedName* pBrowseName, SoftingOPCToolbox5::INodeId* pResultNodeId);

	bool isNodeIdRelatedToVirtualNode(const SoftingOPCToolbox5::NodeId& nodeId);
	bool addVirtualReferences(const SoftingOPCToolbox5::NodeId& nodeIdToBrowse, const SoftingOPCToolbox5::NodeId& referenceTypeId, bool includeSubtypes, EnumBrowseDirection browseDirection, EnumNodeClass nodeClassMask,
		EnumResultMask resultMask, std::vector<SoftingOPCToolbox5::ReferenceDescription> &allReferences);

	/*! Checks, whether a NodeId is a type and subtype of another type */
	bool isOfUaType(const SoftingOPCToolbox5::INodeId* sourceNodeId, const SoftingOPCToolbox5::INodeId* parentTypeId, bool includeSubtypes);

	EnumModelDesignerStatus exportVirtualNodes(SoftingOPCToolbox5::Server::NodeSet2Export::ExportCustomNodesTransactionPtr transaction);

	/*! Convenience initialize methods to fill a ReferenceDescription for a VirtualNodeInfo */
	static void initializeReferenceDescription(SoftingOPCToolbox5::ReferenceDescription &referenceDescription,
		const VirtualNodeInfo& virtualNode,
		const SoftingOPCToolbox5::NodeId& referenceTypeId,
		bool isForward);

	static void initializeReferenceDescription(SoftingOPCToolbox5::ReferenceDescription &referenceDescription,
		SoftingOPCToolbox5::Server::BaseNodePtr existingNode,
		const SoftingOPCToolbox5::NodeId& referenceTypeId,
		bool isForward);


private:
	mutable Util_Mutex m_sync;

	std::vector<VirtualNodeInfo> m_virtualNodes;

	static DynamicNodeCache* m_instance;
#ifdef _DEBUG
	static bool m_wasUninitialized;
#endif
};


class DynamicExportWorkItem : public Util_WorkItem
{
public:
	DynamicExportWorkItem(const SoftingOPCToolbox5::Server::NodeSet2Export::ExportCustomNodesTransactionPtr transaction);
	~DynamicExportWorkItem();
	virtual void execute();
private:
	DynamicExportWorkItem();
	SoftingOPCToolbox5::Server::NodeSet2Export::ExportCustomNodesTransactionPtr m_tranaction;

};
#endif	// DYNAMICNODECACHE_H
