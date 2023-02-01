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
#include "PubSubModule.h"
#include "ApplicationModule.h"

#include "Statics.h"
#include "ServerAddressSpaceRoot.h"
#include "ServerVariable.h"
#include "Application.h"
#include "Argument.h"
#include "ServerMethodCallRequest.h"

#include "NetworkAddressUrlDataType.h"
#include "UadpWriterGroupMessageDataType.h"
#include "UadpDataSetWriterMessageDataType.h"

#include "PubSubConnection.h"
#include "PubSubWriterGroup.h"
#include "PubSubDataSetWriter.h"
#include "PubSubPublishedDataItems.h"
#include "PublishedDataItemsDataType.h"

using namespace SoftingOPCToolbox5;

static const EnumDataSetFieldContentMask c_valueEncodingFlags = 0;
static const EnumDataSetFieldContentMask c_noTimestampDataValueEncodingFlags = EnumDataSetFieldContentMask_StatusCode;
static const EnumDataSetFieldContentMask c_fullDataValueEncodingFlags = EnumDataSetFieldContentMask_StatusCode |
	EnumDataSetFieldContentMask_SourceTimestamp |
	EnumDataSetFieldContentMask_SourcePicoSeconds |
	EnumDataSetFieldContentMask_ServerTimestamp |
	EnumDataSetFieldContentMask_ServerPicoSeconds;	// all valid flags, thus without Raw
static const EnumDataSetFieldContentMask c_rawEncodingFlags = EnumDataSetFieldContentMask_RawData;


EnumStatusCode PubSubModule::initModule(const tstring& name, OTUInt16 namespaceIndex, Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);


	{
		tstring variableName = _T("PubSubConfigurations");
		m_configurationVariable = Server::Variable::create();
		result = m_configurationVariable->setNodeId(NodeId(namespaceIndex, variableName));
		if(StatusCode::isGood(result))
		{
			m_configurationVariable->setBrowseName(QualifiedName(variableName, namespaceIndex));
			m_configurationVariable->setDisplayName(LocalizedText(variableName, _T("")));
			m_configurationVariable->setDescription(LocalizedText(_T("Exposes the amount of configurations and whether they are enabled or disabled"), _T("")));
			m_configurationVariable->setDataType(Statics::DataTypeId_Boolean);
			m_configurationVariable->setValueRank(1);	// this also initializes the array dimensions to {0}
			this->addReference(Statics::ReferenceTypeId_HasComponent, m_configurationVariable);
			result = m_configurationVariable->init(NULL);
		}
	}

	if(StatusCode::isGood(result))
	{
		result = initPublisherConfigs(namespaceIndex);
	}

	if(StatusCode::isGood(result))
	{
		m_enableDisableMethod = new EnableDisablePublisherConfigMethod();
		result = m_enableDisableMethod->initialize(_T("EnableDisablePublisher"), namespaceIndex, this);
	}

	if(StatusCode::isGood(result))
	{
		// the showPubSubNodes method has no special dependencies for cleanup, so we don't need a member for this
		SoftingOPCToolbox5::ObjectPointer<ShowHidePubSubNodesMethod> showHideMethod = new ShowHidePubSubNodesMethod();
		result = showHideMethod->initialize(_T("ShowHidePubSubNodes"), namespaceIndex, this);
	}

	if(StatusCode::isGood(result))
	{
		Application::instance()->setShowPubSubNodes(true);
	}
	return result;
}

void PubSubModule::endModule()
{
	if(m_enableDisableMethod)
	{
		m_enableDisableMethod->removeTree();
	}
	ModuleFolder::endModule();
}

EnumStatusCode PubSubModule::initPublisherConfigs(OTUInt16 namespaceIndex)
{
	// global settings for reuse
	Value defaultPublisherId((OTUInt32)256);
	Value alternativePublisherId(_T("TestPublisherId")); // use a string for publisher ID - ensure cleanup does work and do not produce leaks
	UadpDataSetWriterMessageDataType defaultUadpWriterMessage;
	defaultUadpWriterMessage.setDataSetMessageContentMask(EnumUadpDataSetMessageContentMask_SequenceNumber |
		EnumUadpDataSetMessageContentMask_MajorVersion |
		EnumUadpDataSetMessageContentMask_MinorVersion);
	UadpDataSetWriterMessageDataType noVersionUadpWriterMessage;
	noVersionUadpWriterMessage.setDataSetMessageContentMask(EnumUadpDataSetMessageContentMask_SequenceNumber);

	EnumStatusCode result = EnumStatusCode_Good;

	OTUInt16 refrigeratorNamespaceIndex = 0;
	{
		ApplicationModule* pAppModule = getApplicationModule();
		refrigeratorNamespaceIndex = pAppModule->getTypesNameSpaceIndex();
	}

	// all configurations use the same hard-coded version time
	ConfigurationVersionDataType defaultConfigurationVersion;
	{
		DateTime configurationTime, periodStart;
		// hard coded 
		periodStart.parse(_T("2000-01-01T00:00:00Z"));
		configurationTime.parse(_T("2019-06-13T10:02:35Z"));
		OTUInt64 versionTime = DateTime::getDurationMilliseconds(periodStart, configurationTime) / 1000; // time since Jan, 01.01. 00:00:00 in seconds
		defaultConfigurationVersion.setMajorVersion((OTUInt32)versionTime);
		defaultConfigurationVersion.setMinorVersion((OTUInt32)versionTime);
	}

	// GroupMessage
	UadpWriterGroupMessageDataType defaultUadpGroupMessage;
	{
		// Default
		if(StatusCode::isGood(result))
		{
			defaultUadpGroupMessage.setNetworkMessageContentMask(EnumUadpNetworkMessageContentMask_PublisherId |
				EnumUadpNetworkMessageContentMask_GroupHeader |
				EnumUadpNetworkMessageContentMask_WriterGroupId |
				EnumUadpNetworkMessageContentMask_GroupVersion |
				// no NetworkMessageNumber, this option is not supported
				EnumUadpNetworkMessageContentMask_SequenceNumber |
				EnumUadpNetworkMessageContentMask_PayloadHeader |
				EnumUadpNetworkMessageContentMask_Timestamp |
				EnumUadpNetworkMessageContentMask_PicoSeconds |
				EnumUadpNetworkMessageContentMask_DataSetClassId);
			defaultUadpGroupMessage.setDataSetOrdering(EnumDataSetOrderingType_Undefined);
			// no group version
			// no sampling offset
			// no publishing offsets
		}
	}

	// Address
	// Note: The address is assigned or overwritten by the enableDisablePublisher method

	// Publisher Configs
	std::vector<bool> configurationSettings;
	{
		std::vector<PubSub::ConnectionPtr> publisherConfig;
		// Config 0
		if(StatusCode::isGood(result))
		{
			result = createPublisherConfig0(defaultPublisherId,
				namespaceIndex,
				defaultConfigurationVersion,
				defaultUadpWriterMessage,
				defaultUadpGroupMessage,
				publisherConfig);
			if(StatusCode::isGood(result))
			{
				m_publisherConfigs.push_back(publisherConfig);
				configurationSettings.push_back(false);	// all configs are initially disabled
			}
		}

		// create some DataSets on the outside of the configurations, as it is used by several configurations
		PubSub::PublishedDataItemsPtr staticWellKnownTypesDataSet = PubSub::PublishedDataItems::create();
		{
			Guid dataSetFieldId;
			FieldMetaData fieldMetaData;
			PublishedVariableDataType publishedVariableType;
			std::vector<FieldMetaData> fields;
			std::vector<PublishedVariableDataType> publishedVariables;
			// Static Well Known Types
			{
				// static QualifiedName
				if(StatusCode::isGood(result))
				{
					dataSetFieldId.parse(_T("4305042A-4668-46BD-8C77-CC0D92ABCA35"));
					result = createFieldAndVariableConfigFromNode(
						NodeId(namespaceIndex, _T("QualifiedName")),
						EnumNumericNodeId_QualifiedName,
						EnumDeadbandType_None, 0.0,
						NumericRange(),
						Value(),
						std::vector<QualifiedName>(),
						false,
						0,
						dataSetFieldId,
						fieldMetaData,
						publishedVariableType);
					if(StatusCode::isGood(result))
					{
						fields.push_back(fieldMetaData);
						publishedVariables.push_back(publishedVariableType);
					}
					dataSetFieldId.clear();
				}
				// static Range Array
				if(StatusCode::isGood(result))
				{
					dataSetFieldId.parse(_T("16CAFAA8-04D0-4BE5-A2FC-9850BB02B6C3"));
					result = createFieldAndVariableConfigFromNode(
						NodeId(namespaceIndex, _T("RangeArray")),
						EnumNumericNodeId_Structure,
						EnumDeadbandType_None, 0.0,
						NumericRange(),
						Value(),
						std::vector<QualifiedName>(),
						false,
						0,
						dataSetFieldId,
						fieldMetaData,
						publishedVariableType);
					if(StatusCode::isGood(result))
					{
						fields.push_back(fieldMetaData);
						publishedVariables.push_back(publishedVariableType);
					}
					dataSetFieldId.clear();
				}

				if(StatusCode::isGood(result))
				{
					result = createDataItems(_T("StaticWellKnownTypesMetaData"),
						defaultConfigurationVersion,
						LocalizedText(_T("Contains static values of well known data types"), _T("")),
						fields,
						publishedVariables,
						staticWellKnownTypesDataSet);
					fields.clear();
					publishedVariables.clear();
				}
			}
		}

		// Config 1
		if(StatusCode::isGood(result))
		{
			result = createPublisherConfig1(defaultPublisherId,
				staticWellKnownTypesDataSet,
				namespaceIndex,
				refrigeratorNamespaceIndex,
				defaultConfigurationVersion,
				defaultUadpWriterMessage,
				defaultUadpGroupMessage,
				publisherConfig);
			if(StatusCode::isGood(result))
			{
				m_publisherConfigs.push_back(publisherConfig);
				configurationSettings.push_back(false);	// all configs are initially disabled
			}
		}

		// Config 2
		if(StatusCode::isGood(result))
		{
			result = createPublisherConfig2(defaultPublisherId,
				alternativePublisherId,
				staticWellKnownTypesDataSet,
				namespaceIndex,
				defaultConfigurationVersion,
				defaultUadpWriterMessage,
				defaultUadpGroupMessage,
				publisherConfig);
			if(StatusCode::isGood(result))
			{
				m_publisherConfigs.push_back(publisherConfig);
				configurationSettings.push_back(false);	// all configs are initially disabled
			}
		}

		// Config 3
		if(StatusCode::isGood(result))
		{
			result = createPublisherConfig3(defaultPublisherId,
				refrigeratorNamespaceIndex,
				defaultConfigurationVersion,
				defaultUadpWriterMessage,
				defaultUadpGroupMessage,
				publisherConfig);
			if(StatusCode::isGood(result))
			{
				m_publisherConfigs.push_back(publisherConfig);
				configurationSettings.push_back(false);	// all configs are initially disabled
			}
		}

		// Config 4
		if(StatusCode::isGood(result))
		{
			result = createPublisherConfig4(defaultPublisherId,
				refrigeratorNamespaceIndex,
				defaultConfigurationVersion,
				noVersionUadpWriterMessage,	// use different message settings without sending the version
				defaultUadpGroupMessage,
				publisherConfig);
			if(StatusCode::isGood(result))
			{
				m_publisherConfigs.push_back(publisherConfig);
				configurationSettings.push_back(false);	// all configs are initially disabled
			}
		}

		// Config 5
		if(StatusCode::isGood(result))
		{
			result = createPublisherConfig5(defaultPublisherId,
				namespaceIndex,
				defaultConfigurationVersion,
				defaultUadpWriterMessage,
				defaultUadpGroupMessage,
				publisherConfig);
			if(StatusCode::isGood(result))
			{
				m_publisherConfigs.push_back(publisherConfig);
				configurationSettings.push_back(false);	// all configs are initially disabled
			}
		}
	}

	if(StatusCode::isGood(result))
	{
		DataValue dval;
		dval.setValue(Value(configurationSettings));
		m_configurationVariable->setDataValue(dval);
	}

	return result;
}

EnumStatusCode PubSubModule::createPublisherConfig0(const Value& defaultPublisherId,
	OTUInt16 testModuleNamespaceIndex,
	const ConfigurationVersionDataType& defaultConfigurationVersion,
	const DataSetWriterMessageDataType& defaultWriterMessage,
	const WriterGroupMessageDataType& defaultGroupMessage,
	std::vector<PubSub::ConnectionPtr>& publisherConfig) const
{
	EnumStatusCode result = EnumStatusCode_Good;
	publisherConfig.clear();

	// DataSets with MetaData and FieldMetaData
	PubSub::PublishedDataItemsPtr simpleDataSet = PubSub::PublishedDataItems::create();
	{
		Guid dataSetFieldId;
		FieldMetaData fieldMetaData;
		PublishedVariableDataType publishedVariableType;
		std::vector<FieldMetaData> fields;
		std::vector<PublishedVariableDataType> publishedVariables;

		// Simple
		if(StatusCode::isGood(result))
		{
			dataSetFieldId.parse(_T("B11D5AC9-8276-409E-AFB5-EA2D25664E58"));
			result = createFieldAndVariableConfigFromNode(
				NodeId(testModuleNamespaceIndex, _T("Dynamic.UInt32")),
				EnumNumericNodeId_UInt32,
				EnumDeadbandType_None, 0.0,
				NumericRange(),
				Value(),
				std::vector<QualifiedName>(),
				false,
				0,
				dataSetFieldId,
				fieldMetaData,
				publishedVariableType);
			if(StatusCode::isGood(result))
			{
				fields.push_back(fieldMetaData);
				publishedVariables.push_back(publishedVariableType);
			}
			dataSetFieldId.clear();

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("SimpleMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains 1 simple data type field"), _T("")),
					fields,
					publishedVariables,
					simpleDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
	}

	std::vector<PubSub::ConnectionPtr> connections;
	// Connection 0
	{
		PubSub::ConnectionPtr connection;
		std::vector<PubSub::WriterGroupPtr> writerGroups;
		// Group_0
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_0
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_0"),
					1,	// must be unique for the PublisherId
					c_rawEncodingFlags,
					1,
					defaultWriterMessage,
					simpleDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_0"),
					1,	// use Id 1, as 0 is invalid
					1000.0,
					10000.0,
					0,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					0, // keep the default size
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
				}
			}
		}

		if(StatusCode::isGood(result))
		{
			result = createConnection(_T("Connection_0_0"),
				defaultPublisherId,
				writerGroups,
				connection);

			if(StatusCode::isGood(result))
			{
				result = Application::instance()->addPubSubConnection(connection);
				connections.push_back(connection);
				connection.reset();
			}
		}
	}

	if(StatusCode::isGood(result))
	{
		publisherConfig = connections;
	}
	return result;
}

EnumStatusCode PubSubModule::createPublisherConfig1(const Value& defaultPublisherId,
	PubSub::PublishedDataItemsPtr staticWellKnownTypesDataSet,
	OTUInt16 testModuleNamespaceIndex,
	OTUInt16 refrigeratorNamespaceIndex,
	const ConfigurationVersionDataType& defaultConfigurationVersion,
	const DataSetWriterMessageDataType& defaultWriterMessage,
	const WriterGroupMessageDataType& defaultGroupMessage,
	std::vector<PubSub::ConnectionPtr>& publisherConfig) const
{
	EnumStatusCode result = EnumStatusCode_Good;
	publisherConfig.clear();

	// DataSets with MetaData and FieldMetaData
	PubSub::PublishedDataItemsPtr dynamicWellKnownTypesDataSet = PubSub::PublishedDataItems::create();
	PubSub::PublishedDataItemsPtr wellKnownStructureDataSet = PubSub::PublishedDataItems::create();
	{
		Guid dataSetFieldId;
		FieldMetaData fieldMetaData;
		PublishedVariableDataType publishedVariableType;
		std::vector<FieldMetaData> fields;
		std::vector<PublishedVariableDataType> publishedVariables;

		// Dynamic Well Known Types
		{
			// dynamic String
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("71741A7B-6A1B-4633-996B-7F65B57DB937"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Dynamic.String")),
					EnumNumericNodeId_String,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// dynamic Value Array
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("5A1A3F1D-3DA9-45ED-B7FB-5DAD3E15A1CD"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Dynamic.ValueArray")),
					EnumNumericNodeId_BaseDataType,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("DynamicWellKnownTypesMetaData_1_0"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains dynamic changing values of well knonw data types"), _T("")),
					fields,
					publishedVariables,
					dynamicWellKnownTypesDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}

		// Well Known Structure
		{
			// static XVType
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("E14874A1-F4E9-4FB4-BF80-2F1EFD8ACAF9"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(refrigeratorNamespaceIndex, _T("XVType")),
					EnumNumericNodeId_Structure,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("WellKnownStructureMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains well known structured data types"), _T("")),
					fields,
					publishedVariables,
					wellKnownStructureDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
	}

	std::vector<PubSub::ConnectionPtr> connections;
	// Connection_1_0
	{
		std::vector<PubSub::WriterGroupPtr> writerGroups;
		// WriterGroup_1_0
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_DynamicWellKnown_NoTimestamps
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_DynamicWellKnown_NoTimestamps"),
					2,	// must be unique for the PublisherId
					c_noTimestampDataValueEncodingFlags,
					1,
					defaultWriterMessage,
					dynamicWellKnownTypesDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_1_0"),
					10,
					1000.0,
					10000.0,
					0,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					0, // keep the default size
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
				}
			}
		}

		// WriterGroup_1_1
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_StaticWellKnown_AllTimestamps
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_StaticWellKnown_AllTimestamps"),
					3,	// must be unique for the PublisherId
					c_fullDataValueEncodingFlags,
					1,
					defaultWriterMessage,
					staticWellKnownTypesDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}
			// Writer_WellKnownStructure
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_WellKnownStructure"),
					4,	// must be unique for the PublisherId
					c_valueEncodingFlags,
					1,
					defaultWriterMessage,
					wellKnownStructureDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_1_1"),
					11,
					1000.0,
					10000.0,
					0,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					0, // keep the default size
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
					writerGroup.reset();
				}
			}
		}
		PubSub::ConnectionPtr connection;
		if(StatusCode::isGood(result))
		{
			result = createConnection(_T("Connection_1_0"),
				defaultPublisherId,
				writerGroups,
				connection);
			if(StatusCode::isGood(result))
			{
				result = Application::instance()->addPubSubConnection(connection);
				connections.push_back(connection);
				connection.reset();
			}
		}
	}

	// Config
	if(StatusCode::isGood(result))
	{
		publisherConfig = connections;
	}
	return result;
}

EnumStatusCode PubSubModule::createPublisherConfig2(const Value& defaultPublisherId,
	const Value& alternativePublisherId,
	PubSub::PublishedDataItemsPtr staticWellKnownTypesDataSet,
	OTUInt16 testModuleNamespaceIndex,
	const ConfigurationVersionDataType& defaultConfigurationVersion,
	const DataSetWriterMessageDataType& defaultWriterMessage,
	const WriterGroupMessageDataType& defaultGroupMessage,
	std::vector<PubSub::ConnectionPtr>& publisherConfig) const
{
	EnumStatusCode result = EnumStatusCode_Good;
	publisherConfig.clear();


	// DataSets with MetaData and FieldMetaData
	PubSub::PublishedDataItemsPtr wellKnownMatrixDataSet = PubSub::PublishedDataItems::create();
	PubSub::PublishedDataItemsPtr wellKnownEnumerationDataSet = PubSub::PublishedDataItems::create();
	PubSub::PublishedDataItemsPtr maxStringLengthDataSet = PubSub::PublishedDataItems::create();
	PubSub::PublishedDataItemsPtr abstractTypeDataSet = PubSub::PublishedDataItems::create();
	{
		Guid dataSetFieldId;
		FieldMetaData fieldMetaData;
		PublishedVariableDataType publishedVariableType;
		std::vector<FieldMetaData> fields;
		std::vector<PublishedVariableDataType> publishedVariables;

		// Well Known Matrix
		{
			// static Guid
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("65DECCA0-D0E7-4BD5-9CB5-38009B68D8CF"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Guid")),
					EnumNumericNodeId_Guid,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// static DateTime
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("EA69B28A-D33C-4DEE-898D-21A155ADBBA0"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("DateTime")),
					EnumNumericNodeId_DateTime,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("WellKnownMatrixMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains matrixes of well known data types"), _T("")),
					fields,
					publishedVariables,
					wellKnownMatrixDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
		// Well Known Enumeration
		{
			// static well known enumeration (ServerState)
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("9296E2CE-92AF-4920-86A1-5B546429D5A1"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Enumeration")),
					EnumNumericNodeId_Int32,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("WellKnownEnumerationMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains well known enumeration data types"), _T("")),
					fields,
					publishedVariables,
					wellKnownEnumerationDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
		// MaxStringLength
		{
			// static String with MaxStringLength
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("9055F0E6-DE16-4DA0-95D5-BAE157B2849C"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("String")),
					EnumNumericNodeId_String,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					10,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// static ByteString with MaxStringLength
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("6134D97D-E97D-49FC-B1BD-C734CDED8B60"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("ByteString")),
					EnumNumericNodeId_ByteString,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					10,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("MaxStringLengthMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains data types that apply to the MaxStringLenght parameter"), _T("")),
					fields,
					publishedVariables,
					maxStringLengthDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
		// Abstract Types
		{
			// UnsignedInteger
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("62A079BE-62CD-4185-BF70-37F47D97F704"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("UnsignedInteger")),
					EnumNumericNodeId_BaseDataType,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// static Variant
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("2CCE8DA9-E526-4BC6-BBF4-D190B58CD973"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Variant")),
					EnumNumericNodeId_BaseDataType,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("AbstractTypesMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains abstract data types"), _T("")),
					fields,
					publishedVariables,
					abstractTypeDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
	}

	std::vector<PubSub::ConnectionPtr> connections;
	// Connection_2_0
	{
		std::vector<PubSub::WriterGroupPtr> writerGroups;
		// WriterGroup_2_0_0
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_WellKnownMatrixRaw
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_WellKnownMatrixRaw"),
					5,	// must be unique for the PublisherId
					c_noTimestampDataValueEncodingFlags,
					1,
					defaultWriterMessage,
					wellKnownMatrixDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_2_0_0"),
					200,
					1000.0,
					10000.0,
					0,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					0, // keep the default size
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
				}
			}
		}
		
		// WriterGroup_2_0_1
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_WellKnownScalarAndArray
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_WellKnownScalarAndArray"),
					6,	// must be unique for the PublisherId
					c_noTimestampDataValueEncodingFlags,
					1,
					defaultWriterMessage,
					staticWellKnownTypesDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			// Writer_WellKnownEnumerationWithTimestamps
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_WellKnownEnumerationWithTimestamps"),
					7,	// must be unique for the PublisherId
					c_fullDataValueEncodingFlags,
					1,
					defaultWriterMessage,
					wellKnownEnumerationDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_2_0_1"),
					201,
					1000.0,
					10000.0,
					0,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					0, // keep the default size
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
				}
			}
		}

		// WriterGroup_2_0_2
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_MaxStringLengthValue
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_MaxStringLengthValue"),
					8,	// must be unique for the PublisherId
					c_valueEncodingFlags,
					1,
					defaultWriterMessage,
					maxStringLengthDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			// Writer_MaxStringLengthRaw
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_MaxStringLengthRaw"),
					9,	// must be unique for the PublisherId
					c_valueEncodingFlags,
					1,
					defaultWriterMessage,
					maxStringLengthDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_2_0_2"),
					202,
					1000.0,
					10000.0,
					0,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					0, // keep the default size
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
				}
			}
		}

		PubSub::ConnectionPtr connection;
		if(StatusCode::isGood(result))
		{
			result = createConnection(_T("Connection_2_0"),
				defaultPublisherId,
				writerGroups,
				connection);
			if(StatusCode::isGood(result))
			{
				result = Application::instance()->addPubSubConnection(connection);
				connections.push_back(connection);
				connection.reset();
			}
		}
	}

	// Connection_2_1
	{
		std::vector<PubSub::WriterGroupPtr> writerGroups;
		// WriterGroup_2_1_0
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_AbstractType
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_AbstractType"),
					10,	// must be unique for the PublisherId
					c_noTimestampDataValueEncodingFlags,
					1,
					defaultWriterMessage,
					abstractTypeDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_2_1_0"),
					210,
					2000.0,
					10000.0,
					0,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					0, // keep the default size
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
				}
			}
		}

		PubSub::ConnectionPtr connection;
		if(StatusCode::isGood(result))
		{
			result = createConnection(_T("Connection_2_1"),
				alternativePublisherId,
				writerGroups,
				connection);
			if(StatusCode::isGood(result))
			{
				result = Application::instance()->addPubSubConnection(connection);
				connections.push_back(connection);
				connection.reset();
			}
		}
	}

	// Config
	if(StatusCode::isGood(result))
	{
		publisherConfig = connections;
	}
	return result;
}

EnumStatusCode PubSubModule::createPublisherConfig3(const Value& defaultPublisherId,
	OTUInt16 refrigeratorNamespaceIndex,
	const ConfigurationVersionDataType& defaultConfigurationVersion,
	const DataSetWriterMessageDataType& defaultWriterMessage,
	const WriterGroupMessageDataType& defaultGroupMessage,
	std::vector<PubSub::ConnectionPtr>& publisherConfig) const
{
	EnumStatusCode result = EnumStatusCode_Good;
	publisherConfig.clear();

	// DataSets with MetaData and FieldMetaData
	PubSub::PublishedDataItemsPtr customStructureDataSet = PubSub::PublishedDataItems::create();
	PubSub::PublishedDataItemsPtr customStructureArrayAndMatrixDataSet = PubSub::PublishedDataItems::create();
	PubSub::PublishedDataItemsPtr customEnumerationDataSet = PubSub::PublishedDataItems::create();
	{
		Guid dataSetFieldId;
		FieldMetaData fieldMetaData;
		PublishedVariableDataType publishedVariableType;
		std::vector<FieldMetaData> fields;
		std::vector<PublishedVariableDataType> publishedVariables;

		// Custom Structure
		{
			// static custom data type (RefrigeratorStatus)
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("B6C43DDB-A934-4031-80B1-76C8D5571AFF"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(refrigeratorNamespaceIndex, 13),
					EnumNumericNodeId_Structure,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// static custom data type with nesting fields (OuterStructureType)
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("86591140-D9BC-4C98-993D-BC13662B8394"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(refrigeratorNamespaceIndex, _T("OuterStructVar")),
					EnumNumericNodeId_Structure,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("CustomStructureMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains custom structure data types"), _T("")),
					fields,
					publishedVariables,
					customStructureDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
		// Custom Structure Array and Matrix
		{
			// static custom data type array (RefrigeratorStatusArray)
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("C17ACDF3-76EC-48A9-AA99-8921467CB820"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(refrigeratorNamespaceIndex, _T("RefrigeratorStatusArray")),
					EnumNumericNodeId_Structure,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// static custom data type matrix (OuterStructureTypeMatrix)
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("5A4BAF07-FCF0-47B3-8980-978529427875"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(refrigeratorNamespaceIndex, _T("OuterStructMatrixVar")),
					EnumNumericNodeId_Structure,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("CustomStructureArrayAndMatrixMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains arrays and matrixes of custom structure data types"), _T("")),
					fields,
					publishedVariables,
					customStructureArrayAndMatrixDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
		// Custom Enumeration
		{
			// static custom enumeration (RefrigeratorState)
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("A854E8A4-F905-4CF3-ADC1-EAC1F3D91409"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(refrigeratorNamespaceIndex, 16),
					EnumNumericNodeId_Int32,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("CustomEnumerationMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains custom enumeration data types"), _T("")),
					fields,
					publishedVariables,
					customEnumerationDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
	}

	std::vector<PubSub::ConnectionPtr> connections;
	// Connection_3_0
	{
		std::vector<PubSub::WriterGroupPtr> writerGroups;
		// WriterGroup_3_0
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_CustomStructureDataValue
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_CustomStructureDataValue"),
					11,	// must be unique for the PublisherId
					c_noTimestampDataValueEncodingFlags,
					1,
					defaultWriterMessage,
					customStructureDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}
			// Writer_CustomStructureRaw
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_CustomStructureRaw"),
					12,	// must be unique for the PublisherId
					c_valueEncodingFlags,
					1,
					defaultWriterMessage,
					customStructureDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}
			// Writer_CustomStructureArrayAndMatrix
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_CustomStructureArrayAndMatrix"),
					13,	// must be unique for the PublisherId
					c_noTimestampDataValueEncodingFlags,
					1,
					defaultWriterMessage,
					customStructureArrayAndMatrixDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_3_0"),
					30,
					1000.0,
					10000.0,
					30,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					0, // keep the default size
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
				}
			}
		}

		// WriterGroup_3_1
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_CustomEnumeration
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_CustomEnumeration"),
					14,	// must be unique for the PublisherId
					c_fullDataValueEncodingFlags,
					1,
					defaultWriterMessage,
					customEnumerationDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_3_1"),
					31,
					1000.0,
					10000.0,
					0,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					0, // keep the default size
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
				}
			}
		}

		PubSub::ConnectionPtr connection;
		if(StatusCode::isGood(result))
		{
			result = createConnection(_T("Connection_3_0"),
				defaultPublisherId,
				writerGroups,
				connection);
			if(StatusCode::isGood(result))
			{
				result = Application::instance()->addPubSubConnection(connection);
				connections.push_back(connection);
				connection.reset();
			}
		}
	}

	// Config
	if(StatusCode::isGood(result))
	{
		publisherConfig = connections;
	}
	return result;
}

EnumStatusCode PubSubModule::createPublisherConfig4(const Value& defaultPublisherId,
	OTUInt16 refrigeratorNamespaceIndex,
	const ConfigurationVersionDataType& defaultConfigurationVersion,
	const DataSetWriterMessageDataType& noVersionUadpWriterMessage,
	const WriterGroupMessageDataType& defaultGroupMessage,
	std::vector<PubSub::ConnectionPtr>& publisherConfig) const
{
	EnumStatusCode result = EnumStatusCode_Good;
	publisherConfig.clear();

	// DataSets with MetaData and FieldMetaData
	PubSub::PublishedDataItemsPtr bigBarrelDataSet = PubSub::PublishedDataItems::create();
	{
		Guid dataSetFieldId;
		FieldMetaData fieldMetaData;
		PublishedVariableDataType publishedVariableType;
		std::vector<FieldMetaData> fields;
		std::vector<PublishedVariableDataType> publishedVariables;

		// Big Barrel
		{
			// static custom data type (BigBarrel)
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("BF2F5E62-51BC-45BF-B34D-9302C95B56FF"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(refrigeratorNamespaceIndex, 2),
					EnumNumericNodeId_Structure,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("BigBarrelMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains a BigBarrel data type"), _T("")),
					fields,
					publishedVariables,
					bigBarrelDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
	}

	std::vector<PubSub::ConnectionPtr> connections;
	// Connection_4_0
	{
		std::vector<PubSub::WriterGroupPtr> writerGroups;
		// WriterGroup_4_0
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_BigBarrel
			if(StatusCode::isGood(result))
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_BigBarrel"),
					15,	// must be unique for the PublisherId
					c_noTimestampDataValueEncodingFlags,
					1,
					noVersionUadpWriterMessage,
					bigBarrelDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}

			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_4_0"),
					40,
					1000.0,
					10000.0,
					0,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					500,	// requires chunking to send the data
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
				}
			}
		}

		PubSub::ConnectionPtr connection;
		if(StatusCode::isGood(result))
		{
			result = createConnection(_T("Connection_4_0"),
				defaultPublisherId,
				writerGroups,
				connection);
			if(StatusCode::isGood(result))
			{
				result = Application::instance()->addPubSubConnection(connection);
				connections.push_back(connection);
				connection.reset();
			}
		}
	}

	// Config
	if(StatusCode::isGood(result))
	{
		publisherConfig = connections;
	}
	return result;
}

EnumStatusCode PubSubModule::createPublisherConfig5(const Value& defaultPublisherId,
	OTUInt16 testModuleNamespaceIndex,
	const ConfigurationVersionDataType& defaultConfigurationVersion,
	const DataSetWriterMessageDataType& defaultWriterMessage,
	const WriterGroupMessageDataType& defaultGroupMessage,
	std::vector<PubSub::ConnectionPtr>& publisherConfig) const
{
	EnumStatusCode result = EnumStatusCode_Good;
	publisherConfig.clear();

	// DataSets with MetaData and FieldMetaData
	PubSub::PublishedDataItemsPtr deltaFrameContentDataSet = PubSub::PublishedDataItems::create();
	{
		Guid dataSetFieldId;
		FieldMetaData fieldMetaData;
		PublishedVariableDataType publishedVariableType;
		std::vector<FieldMetaData> fields;
		std::vector<PublishedVariableDataType> publishedVariables;
	
		// DeltaFrameContent DataSet
		{
			// 0 UInt32
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("C627929F-9809-45E4-B9F4-B6A47F662DE1"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("UInt32")),
					EnumNumericNodeId_UInt32,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// 1 String
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("75489FA6-CB2F-4754-A055-FDF266D322D8"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("String")),
					EnumNumericNodeId_String,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// 2 Double
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("8E2AFCD5-1F86-4826-8E5F-1EBE20B41993"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Double")),
					EnumNumericNodeId_Double,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// 3 ByteString
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("F0051573-C7A6-4A15-9025-CA6EBDC5339C"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("ByteString")),
					EnumNumericNodeId_ByteString,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// 4 Boolean
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("1B0BFC11-D887-49BE-8C00-1EFB14B6D478"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Boolean")),
					EnumNumericNodeId_Boolean,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// 5 Variant
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("36C4D71F-6863-4859-B2F2-E1836D8E6688"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Variant")),
					EnumNumericNodeId_BaseDataType,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// 6 Int64
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("12C48D07-7DDD-4914-8614-D01E866F37A9"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Int64")),
					EnumNumericNodeId_Int64,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// 7 Int16Array
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("8AB470BD-2DAD-4B65-8688-B6F11B4DCCCB"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Int16Array")),
					EnumNumericNodeId_Int16,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// 8 Range
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("F1FFC81A-C7FF-438F-AC15-ABDC91E2385C"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("Range")),
					EnumNumericNodeId_Structure,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}
			// 9 QualifiedName
			if(StatusCode::isGood(result))
			{
				dataSetFieldId.parse(_T("6F1A0E54-5C62-4E69-B940-BA84F30B5697"));
				result = createFieldAndVariableConfigFromNode(
					NodeId(testModuleNamespaceIndex, _T("QualifiedName")),
					EnumNumericNodeId_QualifiedName,
					EnumDeadbandType_None, 0.0,
					NumericRange(),
					Value(),
					std::vector<QualifiedName>(),
					false,
					0,
					dataSetFieldId,
					fieldMetaData,
					publishedVariableType);
				if(StatusCode::isGood(result))
				{
					fields.push_back(fieldMetaData);
					publishedVariables.push_back(publishedVariableType);
				}
				dataSetFieldId.clear();
			}

			if(StatusCode::isGood(result))
			{
				result = createDataItems(_T("DeltaFrameContentMetaData"),
					defaultConfigurationVersion,
					LocalizedText(_T("Contains static content of different types to test delta frames"), _T("")),
					fields,
					publishedVariables,
					deltaFrameContentDataSet);
				fields.clear();
				publishedVariables.clear();
			}
		}
	}
	
	std::vector<PubSub::ConnectionPtr> connections;
	// Connection_5_0
	{
		std::vector<PubSub::WriterGroupPtr> writerGroups;
		// WriterGroup_5_0
		{
			std::vector<PubSub::DataSetWriterPtr> writers;
			// Writer_DeltaFrameContent
			{
				PubSub::DataSetWriterPtr writer;
				result = createDataSetWriter(_T("Writer_DeltaFrameContent"),
					50,	// must be unique for the PublisherId
					c_valueEncodingFlags,
					10,
					defaultWriterMessage,
					deltaFrameContentDataSet,
					writer);
				if(StatusCode::isGood(result))
				{
					writers.push_back(writer);
				}
			}
	
			if(StatusCode::isGood(result))
			{
				PubSub::WriterGroupPtr writerGroup;
				result = createWriterGroup(_T("WriterGroup_5_0"),
					50,
					1000.0,
					3000.0,	// KeyFrames will be sent every 5 * 1000 milliseconds, so send a KeepAlive when no values are changed
					0,
					defaultGroupMessage,
					EnumMessageSecurityMode_None,
					0, // keep the default size
					writers,
					writerGroup);
				if(StatusCode::isGood(result))
				{
					writerGroups.push_back(writerGroup);
				}
			}
		}
	
		PubSub::ConnectionPtr connection;
		if(StatusCode::isGood(result))
		{
			result = createConnection(_T("Connection_5_0"),
				defaultPublisherId,
				writerGroups,
				connection);
			if(StatusCode::isGood(result))
			{
				result = Application::instance()->addPubSubConnection(connection);
				connections.push_back(connection);
				connection.reset();
			}
		}
	}
	
	// Config
	if(StatusCode::isGood(result))
	{
		publisherConfig = connections;
	}
	return result;
}

EnumStatusCode PubSubModule::createFieldAndVariableConfigFromNode(const NodeId& nodeId,
	EnumNumericNodeId builtInType,
	EnumDeadbandType deadbandType,
	double deadbandValue,
	const NumericRange& indexRange,
	const Value& substituteValue,
	const std::vector<QualifiedName>& metaDataProperties,
	bool promoteFields,
	OTUInt32 maxStringLength,
	const Guid& dataSetFieldId,
	FieldMetaData& fieldConfig,
	PublishedVariableDataType& variableConfig) const
{
	EnumStatusCode result = EnumStatusCode_Good;
	Server::VariablePtr variable = Server::AddressSpaceRoot::instance()->getVariable(&nodeId);
	if(variable.isNull())
	{
		result = EnumStatusCode_BadNodeIdUnknown;
	}

	if(StatusCode::isGood(result))
	{
		result = variableConfig.setPublishedVariable(nodeId);
	}
	if(StatusCode::isGood(result))
	{
		// this helper method supports only the ValueAttribute, otherwise it would require a special handling
		// for the DataType, ValueRank and ArrayDimensions
		variableConfig.setAttributeId(EnumAttributeId_Value);
		// no sampling interval hint, the toolkit ignores this hint anyway
		variableConfig.setDeadbandType(deadbandType);
		variableConfig.setDeadbandValue(deadbandValue);
		result = variableConfig.setIndexRange(indexRange);
	}
	if(StatusCode::isGood(result))
	{
		result = variableConfig.setSubstituteValue(substituteValue);
	}
	if(StatusCode::isGood(result))
	{
		for(size_t i = 0; i < metaDataProperties.size(); i++)
		{
			result = variableConfig.addMetaDataProperty(metaDataProperties[i]);
			if(StatusCode::isBad(result))
			{
				break;
			}
		}
	}

	if(StatusCode::isGood(result))
	{
		fieldConfig.setName(variable->getDisplayName()->getText());
		result = fieldConfig.setDescription(variable->getDescription());
	}
	if(StatusCode::isGood(result))
	{
		EnumDataSetFieldFlags fieldFlags = 0;
		if(promoteFields)
		{
			fieldFlags &= EnumDataSetFieldFlags_PromotedField;
		}
		fieldConfig.setFieldFlags(fieldFlags);
		_ASSERTION(builtInType < 256, _T("Invalid BuiltInType"));
		fieldConfig.setBuiltInType((OTUInt8)builtInType);
		result = fieldConfig.setDataType(variable->getDataType());
	}
	if(StatusCode::isGood(result))
	{
		std::vector<OTUInt32> arrayDimensions;
		fieldConfig.setValueRank(variable->getValueRank());
		fieldConfig.setArrayDimensions(variable->getArrayDimensions());
		fieldConfig.setMaxStringLength(maxStringLength);
		result = fieldConfig.setDataSetFieldId(dataSetFieldId);
	}
	if(StatusCode::isGood(result))
	{
		for(size_t i = 0; i < metaDataProperties.size(); i++)
		{
			Server::VariablePtr propertyVar = variable->getVariable(metaDataProperties[i]);
			if(propertyVar.isNull())
			{
				result = EnumStatusCode_BadNoMatch;
				break;
			}
			result = fieldConfig.addProperty(KeyValuePair(metaDataProperties[i], *propertyVar->getDataValue()->getValue()));
			if(StatusCode::isBad(result))
			{
				break;
			}
		}
	}

	if(StatusCode::isBad(result))
	{
		fieldConfig.clear();
		variableConfig.clear();
	}
	return result;
}

EnumStatusCode PubSubModule::createDataItems(const tstring& name,
	const ConfigurationVersionDataType& configurationVersion,
	const LocalizedText& description,
	const std::vector<FieldMetaData>& fields,
	const std::vector<PublishedVariableDataType>& publishedVariables,
	PubSub::PublishedDataItemsPtr& dataItems) const
{
	EnumStatusCode result = EnumStatusCode_Good;
	DataSetMetaDataType metaData;
	PublishedDataItemsDataType dataItemsType;
	dataItems = PubSub::PublishedDataItems::create();

	// MetaData
	metaData.setName(name);
	metaData.setConfigurationVersion(configurationVersion);
	result = metaData.setDescription(description);
	if(StatusCode::isGood(result))
	{
		// no data set class
		// no configuration version
		// don't set any namespaces, all types belong to the default namespace
		// no structured data type descriptions for the meta data
		// no enumeration data type descriptions for the meta data
		// no custom simple data type descriptions for the meta data
		for(size_t i = 0; i < fields.size(); i++)
		{
			result = metaData.addField(fields[i]);
			if(StatusCode::isBad(result))
			{
				break;
			}
		}
	}

	// DataItemsDataType
	if(StatusCode::isGood(result))
	{
		for(size_t i = 0; i < publishedVariables.size(); i++)
		{
			result = dataItemsType.addPublishedData(publishedVariables[i]);
			if(StatusCode::isBad(result))
			{
				break;
			}
		}
	}

	// DataItems
	if(StatusCode::isGood(result))
	{
		result = dataItems->setName(name);
	}
	if(StatusCode::isGood(result))
	{
		result = dataItems->setDataSetMetaData(metaData);
	}
	if(StatusCode::isGood(result))
	{
		// no extension fields
		result = dataItems->setDataSetSource(dataItemsType);
	}

	if(StatusCode::isBad(result))
	{
		dataItems.reset();
	}
	return result;
}

EnumStatusCode PubSubModule::createDataSetWriter(const tstring& name,
	OTUInt16 writerId,
	EnumDataSetFieldContentMask fieldContentMask,
	OTUInt32 keyFrameCount,
	const DataSetWriterMessageDataType& messageSettings,
	PubSub::PublishedDataSetPtr dataSet,
	PubSub::DataSetWriterPtr& writer) const
{
	EnumStatusCode result = EnumStatusCode_Good;

	writer = PubSub::DataSetWriter::create();
	result = writer->setName(name);
	if(StatusCode::isGood(result))
	{
		writer->setEnabled(true);
		result = writer->setId(writerId);
	}
	if(StatusCode::isGood(result))
	{
		result = writer->setDataSetFieldContentMask(fieldContentMask);
	}
	if(StatusCode::isGood(result))
	{
		result = writer->setKeyFrameCount(keyFrameCount);
	}
	if(StatusCode::isGood(result))
	{
		// no support for dataSetWriterProperties yet
		// the DataSetWriterTransportDataType is only defined for Broker - not yet supported
		result = writer->setMessageSettings(messageSettings);
	}
	if(StatusCode::isGood(result))
	{
		result = writer->setDataSet(dataSet);
	}

	if(StatusCode::isBad(result))
	{
		writer.reset();
	}
	return result;
}

EnumStatusCode PubSubModule::createWriterGroup(const tstring& name,
	OTUInt16 writerGroupId,
	double publishingInterval,
	double keepAliveTime,
	OTUInt8 priority,
	const SoftingOPCToolbox5::WriterGroupMessageDataType& messageSettings,
	EnumMessageSecurityMode securityMode,
	OTUInt32 maxNetworkMessageSize,
	const std::vector<SoftingOPCToolbox5::PubSub::DataSetWriterPtr>& writers,
	SoftingOPCToolbox5::PubSub::WriterGroupPtr& writerGroup) const
{

	EnumStatusCode result = EnumStatusCode_Good;

	writerGroup = PubSub::WriterGroup::create();
	result = writerGroup->setName(name);
	if(StatusCode::isGood(result))
	{
		writerGroup->setEnabled(true);
		result = writerGroup->setWriterGroupId(writerGroupId);
	}
	if(StatusCode::isGood(result))
	{
		result = writerGroup->setPublishingInterval(publishingInterval);
	}
	if(StatusCode::isGood(result))
	{
		result = writerGroup->setKeepAliveTime(keepAliveTime);
	}
	if(StatusCode::isGood(result))
	{
		result = writerGroup->setPriority(priority);
	}
	if(StatusCode::isGood(result))
	{
		// the transport settings (like RepeatCount and RepeatDelay) are not yet supported
		result = writerGroup->setMessageSettings(messageSettings);
	}
	if(StatusCode::isGood(result))
	{
		result = writerGroup->setSecurityMode(securityMode);
	}
	if(StatusCode::isGood(result))
	{
		// no support for the SecurityGroupId, SecurityKeyService and GroupProperties yet

		if(maxNetworkMessageSize > 0)	// treat value 0 as "keep the default"
		{
			result = writerGroup->setMaxNetworkMessageSize(maxNetworkMessageSize);
		}
	}
	if(StatusCode::isGood(result))
	{
		for(size_t i = 0; i < writers.size(); i++)
		{
			result = writerGroup->addDataSetWriter(writers[i]);
			if(StatusCode::isBad(result))
			{
				break;
			}
		}
	}

	if(StatusCode::isBad(result))
	{
		writerGroup.reset();
	}
	return result;
}

	EnumStatusCode PubSubModule::createConnection(const tstring& name,
	const Value& publisherId,
	const std::vector<PubSub::WriterGroupPtr>& writerGroups,
	PubSub::ConnectionPtr& connection) const
{
	EnumStatusCode result = EnumStatusCode_Good;
	connection = PubSub::Connection::create();
	result = connection->setName(name);
	if(StatusCode::isGood(result))
	{
		connection->setEnabled(false);	// the initial state shall be disabled
		result = connection->setPublisherId(publisherId);
	}
	if(StatusCode::isGood(result))
	{
		// the address is set during the enableDisablePublisher method
		// currently we don't have the TransportProfileUris for PubSub available
		// no specific transportSettings - for datagram this defines only the discovery address, which we don't support yet
		for(size_t i = 0; i < writerGroups.size(); i++)
		{
			result = connection->addWriterGroup(writerGroups[i]);
			if(StatusCode::isBad(result))
			{
				break;
			}
		}
	}
	if(StatusCode::isBad(result))
	{
		connection.reset();
	}
	return result;
}

EnumStatusCode PubSubModule::enableDisablePublisherConfig(OTUInt32 configIndex,
	bool enable,
	const tstring& publisherUrl,
	const tstring& networkInterface,
	const ConfigurationVersionDataType& configurationVersion,
	OTUInt32 keyFrameCount)
{
	if(configIndex >= m_publisherConfigs.size())
	{
		return EnumStatusCode_BadOutOfRange;
	}
	if(m_configurationVariable.isNull())
	{
		return EnumStatusCode_BadInternalError;
	}

	EnumStatusCode result = EnumStatusCode_Good;
	DataValue configurationsDataValue;
	std::vector<bool> configurations;
	if(StatusCode::isGood(result))
	{
		configurationsDataValue = m_configurationVariable->getDataValue();
		if(StatusCode::isBad(configurationsDataValue.getValue()->getBooleanArray(configurations)) ||
			configurations.size() != m_publisherConfigs.size())
		{
			result = EnumStatusCode_BadInternalError;
		}
	}
	if(StatusCode::isGood(result))
	{
		tstring defaultUrl = _T("opc.udp://239.0.0.1:4840");	// 4840 is the default port and is recommended to use
		tstring defaultNetworkInterface;	// // publish on default interface

		NetworkAddressUrlDataType address;
		if(publisherUrl.empty())
		{
			address.setUrl(defaultUrl);
		}
		else
		{
			address.setUrl(publisherUrl);
		}
		if(networkInterface.empty())
		{
			address.setNetworkInterface(defaultNetworkInterface);
		}
		else
		{
			address.setNetworkInterface(networkInterface);
		}

		// use the names to identify the publishers, this might not be completely correct
		// to identify it uniquely, but it should be sufficient for the test server
		std::vector<PubSub::ConnectionPtr>& connections = m_publisherConfigs[configIndex];

		for(size_t i = 0; i < connections.size(); i++)
		{
			if(enable)
			{
				result = connections[i]->setNetworkAddress(address);
				if(StatusCode::isBad(result))
				{
					break;	// for connections
				}
				// Update ConfigurationVersion of MetaData if requested
				if(configurationVersion.getMajorVersion() != 0 || configurationVersion.getMinorVersion() != 0)
				{
					updateConfigurationVersion(connections[i], configurationVersion);
				}
				// Update KeyFrameCount if requested
				if(keyFrameCount > 0)
				{
					updateKeyFrameCount(connections[i], keyFrameCount);
				}
			}
			connections[i]->setEnabled(enable);
		}
		if(StatusCode::isGood(result))
		{
			configurations[configIndex] = enable;

			configurationsDataValue.setValue(Value(configurations));
			(void)m_configurationVariable->setDataValue(configurationsDataValue);
		}
	}

	return EnumStatusCode_Good;
}


void PubSubModule::updateConfigurationVersion(PubSub::ConnectionPtr connection,
	const ConfigurationVersionDataType& configurationVersion) const
{
	if(connection.isNull())
	{
		return;
	}
	// Groups
	std::vector<PubSub::WriterGroupPtr> writerGroups = connection->getWriterGroups();
	for(std::vector<PubSub::WriterGroupPtr>::iterator groupIt = writerGroups.begin(); groupIt != writerGroups.end(); ++groupIt)
	{
		// Writers
		std::vector<PubSub::DataSetWriterPtr> writers = (*groupIt)->getDataSetWriters();
		for(std::vector<PubSub::DataSetWriterPtr>::iterator writerIt = writers.begin(); writerIt != writers.end(); ++writerIt)
		{
			// PublishedDataSet
			PubSub::PublishedDataSetPtr dataSet = (*writerIt)->getDataSet();
			if(dataSet.isNotNull())
			{
				// Update ConfigurationVersion on the MetaData
				DataSetMetaDataType metaData;
				if(StatusCode::isSUCCEEDED(dataSet->getDataSetMetaData(metaData)))
				{
					metaData.setConfigurationVersion(configurationVersion);
					dataSet->setDataSetMetaData(metaData);
				}
			}
		}
	}
}

void PubSubModule::updateKeyFrameCount(SoftingOPCToolbox5::PubSub::ConnectionPtr connection, OTUInt32 keyFrameCount) const
{
	if(connection.isNull() || 0 == keyFrameCount)
	{
		return;
	}
	// Groups
	std::vector<PubSub::WriterGroupPtr> writerGroups = connection->getWriterGroups();
	for(std::vector<PubSub::WriterGroupPtr>::iterator groupIt = writerGroups.begin(); groupIt != writerGroups.end(); ++groupIt)
	{
		// Writers
		std::vector<PubSub::DataSetWriterPtr> writers = (*groupIt)->getDataSetWriters();
		for(std::vector<PubSub::DataSetWriterPtr>::iterator writerIt = writers.begin(); writerIt != writers.end(); ++writerIt)
		{
			// Update KeyFrameCount on the writers. Writers with KeyFrameCount=0 (non-cyclic) will be skipped.
			if((*writerIt)->getKeyFrameCount() > 0)
			{
				(*writerIt)->setKeyFrameCount(keyFrameCount);
			}
		}
	}
}

EnableDisablePublisherConfigMethod::EnableDisablePublisherConfigMethod()
{
	m_pPubSubModule = NULL;
}

EnableDisablePublisherConfigMethod::~EnableDisablePublisherConfigMethod()
{
	m_pPubSubModule = NULL;
}

EnumStatusCode EnableDisablePublisherConfigMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArgumentValues = request->getInputArguments();
	std::vector<EnumStatusCode> inputArgumentResults;
	EnumStatusCode result = validateInputArguments(inputArgumentValues, inputArgumentResults);
	request->setInputArgumentResults(inputArgumentResults);

	OTUInt32 publisherConfigId = 0;
	bool enable = false;
	tstring publisherUrl;
	tstring networkInterface;
	ConfigurationVersionDataType configurationVersion;
	OTUInt32 keyFrameCount = 0;

	if(StatusCode::isGood(result))
	{
		if(inputArgumentValues.size() == 6 && m_pPubSubModule != NULL)
		{
			publisherConfigId = inputArgumentValues[0].getUInt32();
			enable = inputArgumentValues[1].getBoolean();
			publisherUrl = inputArgumentValues[2].getString();
			networkInterface = inputArgumentValues[3].getString();
			configurationVersion = inputArgumentValues[4].getConfigurationVersionDataType();
			keyFrameCount = inputArgumentValues[5].getUInt32();
			result = m_pPubSubModule->enableDisablePublisherConfig(publisherConfigId,
				enable, publisherUrl, networkInterface, configurationVersion, keyFrameCount);
		}
		else
		{
			result = EnumStatusCode_BadInternalError;
		}
	}

	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

EnumStatusCode EnableDisablePublisherConfigMethod::initialize(const tstring& name, OTUInt16 namespaceIndex, PubSubModule* pPubSubModule)
{
	if(pPubSubModule == NULL)
	{
		return EnumStatusCode_BadInvalidArgument;
	}
	EnumStatusCode result = setNodeId(NodeId(namespaceIndex, name));
	if(StatusCode::isGood(result))
	{
		result = setBrowseName(QualifiedName(name, namespaceIndex));
	}
	if(StatusCode::isGood(result))
	{
		result = setDisplayName(LocalizedText(name, _T("")));
	}
	if(StatusCode::isGood(result))
	{
		m_pPubSubModule = pPubSubModule;
		result = pPubSubModule->addReference(Statics::ReferenceTypeId_HasComponent, this);
	}
	if(StatusCode::isGood(result))
	{
		std::vector<Argument> inputArguments;
		Argument arg;
		arg.setName(_T("PublisherConfigId"));
		arg.setDataType(Statics::DataTypeId_UInt32);
		arg.setDescription(LocalizedText(_T("Selects a hardcoded configuration to enable/disable"), _T("")));
		arg.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(arg);
		arg.clear();

		arg.setName(_T("Enabled"));
		arg.setDataType(Statics::DataTypeId_Boolean);
		arg.setDescription(LocalizedText(_T("Selects whether to enable or disable a the selected configuration"), _T("")));
		arg.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(arg);
		arg.clear();

		arg.setName(_T("PublisherUrl"));
		arg.setDataType(Statics::DataTypeId_String);
		arg.setDescription(LocalizedText(_T("The URL to publish the values to.\n")
			_T("An empty string selects the default URL 239.0.0.1:4840 for all NetworkInterfaces."), _T("")));
		arg.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(arg);
		arg.clear();

		arg.setName(_T("NetworkInterface"));
		arg.setDataType(Statics::DataTypeId_String);
		arg.setDescription(LocalizedText(_T("The IP of the network interface to use.\n")
			_T("An empty string selects all network interfaces."), _T("")));
		arg.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(arg);
		arg.clear();

		arg.setName(_T("ConfigurationVersion"));
		arg.setDataType(Statics::DataTypeId_ConfigurationVersionDataType);
		arg.setDescription(LocalizedText(_T("Change the MetaData ConfigurationVersion for all DataSets in the enabled configuration.\n")
			_T("Use ConfigurationVersion \"0;0\" to leave unchanged."), _T("")));
		arg.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(arg);
		arg.clear();

		arg.setName(_T("KeyFrameCount"));
		arg.setDataType(Statics::DataTypeId_UInt32);
		arg.setDescription(LocalizedText(_T("Change the KeyFrameCount for all Writers in the enabled configuration.\n")
			_T("A value of 0 will leave all Writers unchanged, Writers with KeyFrameCount=0 are never changed."), _T("")));
		arg.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(arg);
		arg.clear();

		NodeId inputArgId(namespaceIndex, name + _T("_InputArguments"));
		result = setInputArguments(&inputArgId, inputArguments);
	}
	if(StatusCode::isGood(result))
	{
		result = init(NULL);
	}
	return result;
}

ShowHidePubSubNodesMethod::ShowHidePubSubNodesMethod()
{
}

ShowHidePubSubNodesMethod::~ShowHidePubSubNodesMethod()
{
}

EnumStatusCode ShowHidePubSubNodesMethod::initialize(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	if(parentFolder == NULL)
	{
		return EnumStatusCode_BadInvalidArgument;
	}
	EnumStatusCode result = setNodeId(NodeId(namespaceIndex, name));
	if(StatusCode::isGood(result))
	{
		result = setBrowseName(QualifiedName(name, namespaceIndex));
	}
	if(StatusCode::isGood(result))
	{
		result = setDisplayName(LocalizedText(name, _T("")));
	}
	if(StatusCode::isGood(result))
	{
		result = parentFolder->addReference(Statics::ReferenceTypeId_HasComponent, this);
	}
	if(StatusCode::isGood(result))
	{
		std::vector<Argument> inputArguments;
		Argument arg;
		arg.setName(_T("ShowNodes"));
		arg.setDataType(Statics::DataTypeId_Boolean);
		arg.setDescription(LocalizedText(_T("Selects whether to show or hide the PubSub nodes"), _T("")));
		arg.setValueRank(ValueRank_Scalar);
		inputArguments.push_back(arg);
		arg.clear();

		NodeId inputArgId(namespaceIndex, name + _T("_InputArguments"));
		result = setInputArguments(&inputArgId, inputArguments);
	}
	if(StatusCode::isGood(result))
	{
		result = init(NULL);
	}
	return result;
}

EnumStatusCode ShowHidePubSubNodesMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArgumentValues = request->getInputArguments();
	std::vector<EnumStatusCode> inputArgumentResults;
	EnumStatusCode result = validateInputArguments(inputArgumentValues, inputArgumentResults);
	request->setInputArgumentResults(inputArgumentResults);

	bool showNodes = false;
	if(StatusCode::isGood(result))
	{
		if(inputArgumentValues.size() == 1)
		{
			showNodes = inputArgumentValues[0].getBoolean();

			Application::instance()->setShowPubSubNodes(showNodes);
		}
		else
		{
			result = EnumStatusCode_BadInternalError;
		}
	}

	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}
