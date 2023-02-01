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
#ifndef PUBSUBMODULE_H
#define PUBSUBMODULE_H

#include "ModuleFolder.h"
#include "ServerMethod.h"
#include "PubSubConnection.h"

namespace SoftingOPCToolbox5
{
	class FieldMetaData;
	class PublishedVariableDataType;
	class ConfigurationVersionDataType;
	class WriterGroupMessageDataType;
	class DataSetWriterMessageDataType;

	namespace PubSub
	{
		class DataSetWriter;
		typedef ObjectPointer<DataSetWriter> DataSetWriterPtr;
		class PublishedDataSet;
		typedef ObjectPointer<PublishedDataSet> PublishedDataSetPtr;
		class PublishedDataItems;
		typedef ObjectPointer<PublishedDataItems> PublishedDataItemsPtr;
	}
}

class PubSubModule;

class EnableDisablePublisherConfigMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	EnableDisablePublisherConfigMethod();
	virtual ~EnableDisablePublisherConfigMethod();

	EnumStatusCode initialize(const tstring& name, OTUInt16 namespaceIndex, PubSubModule* pPubSubModule);
	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
private:
	PubSubModule* m_pPubSubModule;
};

class ShowHidePubSubNodesMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	ShowHidePubSubNodesMethod();
	virtual ~ShowHidePubSubNodesMethod();

	EnumStatusCode initialize(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);
private:
};

class PubSubModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);

	virtual void endModule();

	EnumStatusCode enableDisablePublisherConfig(OTUInt32 configIndex,
		bool enable,
		const tstring& publisherUrl,
		const tstring& networkInterface,
		const SoftingOPCToolbox5::ConfigurationVersionDataType& configurationVersion,
		OTUInt32 keyFrameCount);
private:
	EnumStatusCode initPublisherConfigs(OTUInt16 namespaceIndex);

	EnumStatusCode createPublisherConfig0(const SoftingOPCToolbox5::Value& defaultPublisherId,
		OTUInt16 testModuleNamespaceIndex,
		const SoftingOPCToolbox5::ConfigurationVersionDataType& defaultConfigurationVersion,
		const SoftingOPCToolbox5::DataSetWriterMessageDataType& defaultWriterMessage,
		const SoftingOPCToolbox5::WriterGroupMessageDataType& defaultGroupMessage,
		std::vector<SoftingOPCToolbox5::PubSub::ConnectionPtr>& publisherConfig) const;

	EnumStatusCode createPublisherConfig1(const SoftingOPCToolbox5::Value& defaultPublisherId,
		SoftingOPCToolbox5::PubSub::PublishedDataItemsPtr staticWellKnownTypesDataSet,
		OTUInt16 testModuleNamespaceIndex,
		OTUInt16 refrigeratorNamespaceIndex,
		const SoftingOPCToolbox5::ConfigurationVersionDataType& defaultConfigurationVersion,
		const SoftingOPCToolbox5::DataSetWriterMessageDataType& defaultWriterMessage,
		const SoftingOPCToolbox5::WriterGroupMessageDataType& defaultGroupMessage,
		std::vector<SoftingOPCToolbox5::PubSub::ConnectionPtr>& publisherConfig) const;

	EnumStatusCode createPublisherConfig2(const SoftingOPCToolbox5::Value& defaultPublisherId,
		const SoftingOPCToolbox5::Value& alternativePublisherId,
		SoftingOPCToolbox5::PubSub::PublishedDataItemsPtr staticWellKnownTypesDataSet,
		OTUInt16 testModuleNamespaceIndex,
		const SoftingOPCToolbox5::ConfigurationVersionDataType& defaultConfigurationVersion,
		const SoftingOPCToolbox5::DataSetWriterMessageDataType& defaultWriterMessage,
		const SoftingOPCToolbox5::WriterGroupMessageDataType& defaultGroupMessage,
		std::vector<SoftingOPCToolbox5::PubSub::ConnectionPtr>& publisherConfig) const;

	EnumStatusCode createPublisherConfig3(const SoftingOPCToolbox5::Value& defaultPublisherId,
		OTUInt16 refrigeratorNamespaceIndex,
		const SoftingOPCToolbox5::ConfigurationVersionDataType& defaultConfigurationVersion,
		const SoftingOPCToolbox5::DataSetWriterMessageDataType& defaultWriterMessage,
		const SoftingOPCToolbox5::WriterGroupMessageDataType& defaultGroupMessage,
		std::vector<SoftingOPCToolbox5::PubSub::ConnectionPtr>& publisherConfig) const;

	EnumStatusCode createPublisherConfig4(const SoftingOPCToolbox5::Value& defaultPublisherId,
		OTUInt16 refrigeratorNamespaceIndex,
		const SoftingOPCToolbox5::ConfigurationVersionDataType& defaultConfigurationVersion,
		const SoftingOPCToolbox5::DataSetWriterMessageDataType& noVersionUadpWriterMessage,
		const SoftingOPCToolbox5::WriterGroupMessageDataType& defaultGroupMessage,
		std::vector<SoftingOPCToolbox5::PubSub::ConnectionPtr>& publisherConfig) const;

	EnumStatusCode createPublisherConfig5(const SoftingOPCToolbox5::Value& defaultPublisherId,
		OTUInt16 testModuleNamespaceIndex,
		const SoftingOPCToolbox5::ConfigurationVersionDataType& defaultConfigurationVersion,
		const SoftingOPCToolbox5::DataSetWriterMessageDataType& defaultWriterMessage,
		const SoftingOPCToolbox5::WriterGroupMessageDataType& defaultGroupMessage,
		std::vector<SoftingOPCToolbox5::PubSub::ConnectionPtr>& publisherConfig) const;

	EnumStatusCode createFieldAndVariableConfigFromNode(const SoftingOPCToolbox5::NodeId& nodeId,
		EnumNumericNodeId builtInType,
		EnumDeadbandType deadbandType,
		double deadbandValue,
		const SoftingOPCToolbox5::NumericRange& indexRange,
		const SoftingOPCToolbox5::Value& substituteValue,
		const std::vector<SoftingOPCToolbox5::QualifiedName>& metaDataProperties,
		bool promoteFields,
		OTUInt32 maxStringLength,
		const SoftingOPCToolbox5::Guid& dataSetFieldId,
		SoftingOPCToolbox5::FieldMetaData& fieldConfig,
		SoftingOPCToolbox5::PublishedVariableDataType& variableConfig) const;

	/*! Helper to create and configure PublishedDataItems and MetaData, including error handling */
	EnumStatusCode createDataItems(const tstring& name,
		const SoftingOPCToolbox5::ConfigurationVersionDataType& configurationVersion,
		const SoftingOPCToolbox5::LocalizedText& description,
		const std::vector<SoftingOPCToolbox5::FieldMetaData>& fields,
		const std::vector<SoftingOPCToolbox5::PublishedVariableDataType>& publishedVariables,
		SoftingOPCToolbox5::PubSub::PublishedDataItemsPtr& dataItems) const;

	/*! Helper to create and configure a DataSetWriter, including error handling */
	EnumStatusCode createDataSetWriter(const tstring& name,
		OTUInt16 writerId,
		EnumDataSetFieldContentMask fieldContentMask,
		OTUInt32 keyFrameCount,
		const SoftingOPCToolbox5::DataSetWriterMessageDataType& messageSettings,
		SoftingOPCToolbox5::PubSub::PublishedDataSetPtr dataSet,
		SoftingOPCToolbox5::PubSub::DataSetWriterPtr& writer) const;

	/*! Helper to create and configure a WriterGroup, including error handling */
	EnumStatusCode createWriterGroup(const tstring& name,
		OTUInt16 writerGroupId,
		double publishingInterval,
		double keepAliveTime,
		OTUInt8 priority,
		const SoftingOPCToolbox5::WriterGroupMessageDataType& messageSettings,
		EnumMessageSecurityMode securityMode,
		OTUInt32 maxNetworkMessageSize,
		const std::vector<SoftingOPCToolbox5::PubSub::DataSetWriterPtr>& writers,
		SoftingOPCToolbox5::PubSub::WriterGroupPtr& writerGroup) const;

	/*! Helper to create and configure a Connection, including error handling */
	EnumStatusCode createConnection(const tstring& name,
		const SoftingOPCToolbox5::Value& publisherId,
		const std::vector<SoftingOPCToolbox5::PubSub::WriterGroupPtr>& writerGroups,
		SoftingOPCToolbox5::PubSub::ConnectionPtr& connection) const;

	void updateConfigurationVersion(SoftingOPCToolbox5::PubSub::ConnectionPtr connection,
		const SoftingOPCToolbox5::ConfigurationVersionDataType& configurationVersion) const;

	void updateKeyFrameCount(SoftingOPCToolbox5::PubSub::ConnectionPtr connection, OTUInt32 keyFrameCount) const;

	// each index stores a list of PubSubConnections to enable/disable for one configuration
	std::vector<std::vector<SoftingOPCToolbox5::PubSub::ConnectionPtr> > m_publisherConfigs;

	// exposes the amount of available configurations and which ones are enabled
	SoftingOPCToolbox5::Server::VariablePtr m_configurationVariable;
	SoftingOPCToolbox5::ObjectPointer<EnableDisablePublisherConfigMethod> m_enableDisableMethod;
};

#endif // PUBSUBMODULE_H
