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
#include <limits.h>
#include "TestServerAddressSpaceRoot.h"

#include "StringTable.h"
#include "ServerBaseNode.h"
#include "ServerVariable.h"
#include "ServerBaseEventNotification.h"
#include "ApplicationModule.h"
#include "ModelCompiler/ErrorMacros.h"
#include "DynamicNodeCache.h"
#include "ToolkitTrace.h"

using namespace SoftingOPCToolbox5;

extern bool g_resetAddressSpace;

TestServerAddressSpaceRoot::TestServerAddressSpaceRoot(void* otbHandle)
	: Server::AddressSpaceRoot(otbHandle)
{
	m_importDuplicateNodeHandling = EnumImportDuplicateNodeHandling_Default;
}

void TestServerAddressSpaceRoot::setImportDuplicateNodeHandling(EnumImportDuplicateNodeHandling handling)
{
	m_importDuplicateNodeHandling = handling;
}

void TestServerAddressSpaceRoot::onIncrementNodeVersion(const std::vector<Server::BaseNode*>& changedNodes)
{
	for(size_t i = 0; i < changedNodes.size(); i++)
	{
		if(changedNodes[i] != NULL)
		{
			tstring nodeVersion;
			if(StatusCode::isSUCCEEDED(changedNodes[i]->getNodeVersion(nodeVersion)))
			{
				OTUInt32 nodeVersionNumber = 0;
				_stscanf(nodeVersion.c_str(), _T("%u"), &nodeVersionNumber);

				OTChar nextNodeVersion[12];
				_stprintf(nextNodeVersion, _T("%u"), ++nodeVersionNumber);

				changedNodes[i]->setNodeVersion(nextNodeVersion);
			}
		}
	}
}

EnumStatusCode TestServerAddressSpaceRoot::defineTranslations()
{
	EnumStatusCode result = EnumStatusCode_Good;

	AddressSpaceRoot::defineTranslations();
#if TB5_ALARMS
	/* AcknowledgeableCondition */
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AcknowledgeableConditionType_AckedState_FalseState), EnumAttributeId_Value, _T("Unacknowledged"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AcknowledgeableConditionType_AckedState_FalseState), EnumAttributeId_Value, _T("Unquittiert"), _T("de"));
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AcknowledgeableConditionType_AckedState_TrueState), EnumAttributeId_Value, _T("Acknowledged"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AcknowledgeableConditionType_AckedState_TrueState), EnumAttributeId_Value, _T("Quittiert"), _T("de"));
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AcknowledgeableConditionType_ConfirmedState_FalseState), EnumAttributeId_Value, _T("Unconfirmed"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
#ifdef UNICODE
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AcknowledgeableConditionType_ConfirmedState_FalseState), EnumAttributeId_Value, _T("Unbestätigt"), _T("de"));
#else
		// use UTF8 encoded strings
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AcknowledgeableConditionType_ConfirmedState_FalseState), EnumAttributeId_Value, _T("UnbestÃ¤tigt"), _T("de"));
#endif
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AcknowledgeableConditionType_ConfirmedState_TrueState), EnumAttributeId_Value, _T("Confirmed"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
#ifdef UNICODE
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AcknowledgeableConditionType_ConfirmedState_TrueState), EnumAttributeId_Value, _T("Bestätigt"), _T("de"));
#else
		// use UTF8 encoded strings
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AcknowledgeableConditionType_ConfirmedState_TrueState), EnumAttributeId_Value, _T("BestÃ¤tigt"), _T("de"));
#endif
	}

	/* AlarmCondition */
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AlarmConditionType_ActiveState_FalseState), EnumAttributeId_Value, _T("Inactive"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AlarmConditionType_ActiveState_FalseState), EnumAttributeId_Value, _T("Inaktiv"), _T("de"));
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AlarmConditionType_ActiveState_TrueState), EnumAttributeId_Value, _T("Active"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AlarmConditionType_ActiveState_TrueState), EnumAttributeId_Value, _T("Aktiv"), _T("de"));
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AlarmConditionType_SuppressedState_FalseState), EnumAttributeId_Value, _T("Unsuppressed"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
#ifdef UNICODE
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AlarmConditionType_SuppressedState_FalseState), EnumAttributeId_Value, _T("Nicht unterdrückt"), _T("de"));
#else
		// use UTF8 encoded strings
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AlarmConditionType_SuppressedState_FalseState), EnumAttributeId_Value, _T("Nicht unterdrÃ¼ckt"), _T("de"));
#endif
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AlarmConditionType_SuppressedState_TrueState), EnumAttributeId_Value, _T("Suppressed"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
#ifdef UNICODE
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AlarmConditionType_SuppressedState_TrueState), EnumAttributeId_Value, _T("Unterdrückt"), _T("de"));
#else
		// use UTF8 encoded strings
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_AlarmConditionType_SuppressedState_TrueState), EnumAttributeId_Value, _T("UnterdrÃ¼ckt"), _T("de"));
#endif
	}

	/* Condition */
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_ConditionType_EnabledState_FalseState), EnumAttributeId_Value, _T("Disabled"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_ConditionType_EnabledState_FalseState), EnumAttributeId_Value, _T("Ausgeschaltet"), _T("de"));
	}

	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_ConditionType_EnabledState_TrueState), EnumAttributeId_Value, _T("Enabled"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_ConditionType_EnabledState_TrueState), EnumAttributeId_Value, _T("Eingeschaltet"), _T("de"));
	}
#endif

	/* TwoStateVariable */
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_TwoStateVariableType_FalseState), EnumAttributeId_Value, _T("False"), _T("en"));
	}
	if(StatusCode::isSUCCEEDED(result))
	{
		result = Server::BaseNode::defineTranslation(NodeId(EnumNumericNodeId_TwoStateVariableType_FalseState), EnumAttributeId_Value, _T("False"), _T("de"));
	}

	return result;
}

void TestServerAddressSpaceRoot::onImportNodesError(const tstring& fileName, int lineNumber, EnumTraceLevel severity, EnumModelDesignerStatus /* errorCode */, const tstring& errorMessage)
{
	//http://softing.com/OPCToolbox/Console
	OTUInt16 testModuleNamspaceIndex;
	StringTableConstPtr namespaces;
	EnumStatusCode result;
	namespaces = getNamespaces();
	result = namespaces->getIndex(_T("http://softing.com/OPCToolbox/Console"), testModuleNamspaceIndex);
	if(StatusCode::isBad(result))
	{
		// will happen during import before address space creation

		// defined incorrect index
		testModuleNamspaceIndex = 0;
	}

	if(severity == EnumTraceLevel_Warning)
	{
		Server::VariablePtr warningVariable;
		NodeId warningsNodeId(testModuleNamspaceIndex, _T("ExportImportWarnings"));
		_tprintf(_T("ModelDesigner WARNING at %s:%d: %s\n"), fileName.c_str(), lineNumber, errorMessage.c_str());
		OTChar buf[256];
		_stprintf(buf, _T("ModelDesigner WARNING at %s:%d: %s"), fileName.c_str(), lineNumber, errorMessage.c_str());
		tstring warningText(buf);

		if(testModuleNamspaceIndex > 0)
		{
			warningVariable = getVariable(&warningsNodeId);
		}
		// can happen that variable is not yet available...
		if(warningVariable)
		{
			std::vector<tstring> warnings;
			warnings = warningVariable->getDataValue()->getValue()->getStringArray();
			warnings.push_back(warningText);
			DataValue warningValue;
			warningValue.setValue(Value(warnings));
			warningVariable->setDataValue(&warningValue);
		}
	}
	else
	{
		NodeId errorsNodeId(testModuleNamspaceIndex, _T("ExportImportErrors"));
		_tprintf(_T("ModelDesigner ERROR at %s:%d: %s\n"), fileName.c_str(), lineNumber, errorMessage.c_str());
		OTChar buf[256];
		_stprintf(buf, _T("ModelDesigner ERROR at %s:%d: %s"), fileName.c_str(), lineNumber, errorMessage.c_str());
		
		tstring errorText(buf);
		Server::VariablePtr errorVariable;
		if(testModuleNamspaceIndex > 0)
		{
			errorVariable = getVariable(&errorsNodeId);
		}
		// can happen that variable is not yet available...
		if(errorVariable)
		{
			std::vector<tstring> errors;
			errors = errorVariable->getDataValue()->getValue()->getStringArray();
			errors.push_back(errorText);
			DataValue errorValue;
			errorValue.setValue(Value(errors));
			errorVariable->setDataValue(&errorValue);
		}
	}
}

EnumModelDesignerStatus TestServerAddressSpaceRoot::onImportDuplicateNode(Server::BaseNodePtr existingNode, const Server::NodeSet2Import::ImportUANodeDescription* pImportedNodeDescription, const XmlElement& xmlTag)
{
	_UNUSED(pImportedNodeDescription);

#if TB5_ALARMS
	{
		// create an event notification
		Server::BaseEventNotificationPtr importDuplicateNodeEvent;
		StringTableConstPtr namespaceTable = Server::AddressSpaceRoot::getNamespaces();
		OTUInt16 exportImportNamespaceIndex = 0;
		if(StatusCode::isGood(namespaceTable->getIndex(_T("http://softing.com/OPCToolbox/Console"), exportImportNamespaceIndex)))
		{

			NodeId importDuplicateNodeEventTypeId(exportImportNamespaceIndex, _T("ImportDuplicateNodeEventType"));
			importDuplicateNodeEvent = Server::BaseEventNotification::create(&importDuplicateNodeEventTypeId);
			importDuplicateNodeEvent->setMessage(LocalizedText(_T("NodeSet2Import tried to import an already existing node."), _T("en")));
			importDuplicateNodeEvent->setSeverity(100);
			importDuplicateNodeEvent->setTime(DateTime::getUtcNow());
			importDuplicateNodeEvent->setReceiveTime(DateTime::getUtcNow());
			importDuplicateNodeEvent->setSourceName(_T("ImportNameSpaces"));
			importDuplicateNodeEvent->setSourceNode(NodeId(exportImportNamespaceIndex, _T("ImportNameSpaces")));
			// set the custom data
			importDuplicateNodeEvent->setProperty(QualifiedName(_T("NodeId"), exportImportNamespaceIndex), Value(existingNode->getNodeId()));
			importDuplicateNodeEvent->setProperty(QualifiedName(_T("XmlTag"), exportImportNamespaceIndex), Value(xmlTag.getStringFromUtf8()));

			// get the ExportImport folder
			NodeId exportImportFolderId(NodeId(exportImportNamespaceIndex, _T("ExportImport")));
			Server::ObjectPtr exportImportFolder = Server::AddressSpaceRoot::getObject(&exportImportFolderId);
			// ignore the error, this can happen when the nodeset is imported via command line
			if(exportImportFolder.isNotNull())
			{
				// report the event
				importDuplicateNodeEvent->report(&exportImportFolderId);
			}
		}
	}
#endif

	EnumModelDesignerStatus result = EnumModelDesignerStatus_Good;
	switch(m_importDuplicateNodeHandling)
	{
	case EnumImportDuplicateNodeHandling_Default:
		result = AddressSpaceRoot::onImportDuplicateNode(existingNode, pImportedNodeDescription, xmlTag);
		break;
	case EnumImportDuplicateNodeHandling_IgnoreImport:
		result = EnumModelDesignerStatus_Good;
		break;
	case EnumImportDuplicateNodeHandling_RemoveExisting:
		{
			// remove can be rejected for data types, that are still in use
			if(StatusCode::isBad(existingNode->remove()))
			{
				// the node will remain and the new node won't be imported
				_tprintf(_T("Unable to remove existing duplicate node '%s', ignore importing node.\n"), existingNode->getNodeId()->toString().c_str());
			}
			result = EnumModelDesignerStatus_Good;
		}
		break;
	default:
		_REPORT_ASSERTION(_T("Unhandled enumeration"));
		result = AddressSpaceRoot::onImportDuplicateNode(existingNode, pImportedNodeDescription, xmlTag);
		break;
	}
	return result;
}

EnumModelDesignerStatus TestServerAddressSpaceRoot::onModelDesignerMethod(const INodeId* methodId, const std::vector<Value>& inputArguments)
{
	OTUInt16 toolkitNamespaceIdx;
	tstring methodName;
MC_InitializeStatus;
	getToolkitNamespaceIndex(&toolkitNamespaceIdx);
	if(methodId->getIdentifierType() != EnumIdentifierType_String || methodId->getNamespaceIndex() != toolkitNamespaceIdx)
	{
		MC_GotoErrorWithStatus(EnumModelDesignerStatus_BadInvalidArgument);
	}

	methodId->getStringIdentifier(&methodName);
	if(methodName == _T("Export"))
	{
		// Export file is allowed below source folder only
		tstring exportFile;
		MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[1].getString(exportFile)), EnumModelDesignerStatus_BadInvalidArgument);

#ifdef SOOS_WINDOWS
		// WINDOWS_CE/EC does not support fullpath
#ifndef SOOS_WINDOWS_CE
		OTChar fullPath[MAX_PATH];
		OTChar fullPathSourceFolder[MAX_PATH];
		OTChar* absExportPath;
		OTChar* absSourcePath;
		size_t sourcePathLen;
		absExportPath = _tfullpath(fullPath, exportFile.c_str(), MAX_PATH);
		absSourcePath = _tfullpath(fullPathSourceFolder, getApplicationModule()->getSourceFolder().c_str(), MAX_PATH);
		if(!absExportPath || !absSourcePath
		   || _tcsnicmp(absSourcePath, absExportPath, sourcePathLen = _tcslen(absSourcePath)) != 0
		   || (absExportPath[sourcePathLen] != '\\' && absExportPath[sourcePathLen] != '/'))
		{
			MC_GotoErrorWithStatus(EnumModelDesignerStatus_BadDenied);
		}
#endif
#endif

#ifdef SOOS_POSIX
		// VxWorks does not support realpath
#ifndef SODEVENV_VXWORKS
		struct stat st;
		char exportPath[PATH_MAX];
		char sourcePath[PATH_MAX];
		char* absExportPath;
		char* absSourcePath;
		size_t sourcePathLen;
		char* filename = strdup(exportFile.c_str());
		if(!filename)
		{
			MC_GotoErrorWithStatus(EnumModelDesignerStatus_BadDenied);
		}
		while(filename[0] && lstat(filename, &st) < 0 && errno == ENOENT)
		{
			char* pDirSep = strrchr(filename, '/');
			if(!pDirSep)
			{
				pDirSep = filename;
			}
			else if(pDirSep == filename)
			{
				pDirSep = filename+1;
			}
			*pDirSep = '\0';
		}
		absSourcePath = realpath(getApplicationModule()->getSourceFolder().c_str(), sourcePath);
		absExportPath = realpath(filename[0] ? filename : ".", exportPath);
		free(filename);
		if(!absExportPath || !absSourcePath
		   || _tcsncmp(absSourcePath, absExportPath, sourcePathLen = _tcslen(absSourcePath)) != 0
		   || (absExportPath[sourcePathLen] && absExportPath[sourcePathLen] != '/'))
		{
			// any path could not be converted - or the path is not below the source folder
			MC_GotoErrorWithStatus(EnumModelDesignerStatus_BadDenied);
		}
#endif
#endif
		_tprintf(_T("Export tries to write to: %s\n"), exportFile.c_str());
	}
	else if(methodName == _T("Import"))
	{
		// no restriction
	}
	else if(methodName == _T("RemoveNode"))
	{
		NodeId nodeIdToRemove;

		MC_GotoErrorIfTrue(inputArguments.size() != 1, EnumModelDesignerStatus_BadInvalidArgument);
		MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[0].getNodeId(nodeIdToRemove)), EnumModelDesignerStatus_BadInvalidArgument);

		MC_GotoErrorIfTrue(nodeIdToRemove.getNamespaceIndex() <= toolkitNamespaceIdx, EnumModelDesignerStatus_BadDenied);
	}
	else if(methodName == _T("ResetAddressSpace"))
	{
		// no restriction
	}
	else if(methodName == _T("CreateInstance"))
	{
		NodeId nodeIdToTest;
		MC_GotoErrorIfTrue(inputArguments.size() < 3, EnumModelDesignerStatus_BadInvalidArgument);

		// NULL value shall produce generated node ID - no check necessary
		if(inputArguments[3].getDataType() != EnumDataTypeId_Null)
		{
			MC_GotoErrorIfTrue(inputArguments[3].getNodeId(nodeIdToTest), EnumModelDesignerStatus_BadInvalidArgument);

			// NULL nodeId shall produce generated node ID - no check necessary
			if(nodeIdToTest.compare(NodeId()) != 0)
			{
				MC_GotoErrorIfTrue(nodeIdToTest.getNamespaceIndex() <= toolkitNamespaceIdx, EnumModelDesignerStatus_BadDenied);
			}
		}
	}
	else if(methodName == _T("AddNamespace"))
	{
		// no restriction
	}
	else if(methodName == _T("ModifyNamespace"))
	{
		OTUInt16 idxToModify;
		MC_GotoErrorIfTrue(inputArguments.size() != 2, EnumModelDesignerStatus_BadInvalidArgument);
		MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[0].getUInt16(&idxToModify)), EnumModelDesignerStatus_BadInvalidArgument);

		MC_GotoErrorIfTrue(idxToModify <= toolkitNamespaceIdx, EnumModelDesignerStatus_BadDenied);
	}
	// the types have similar input arguments
	else if(methodName == _T("CreateReferenceType") ||
			methodName == _T("CreateObjectType") ||
			methodName == _T("CreateVariableType"))
	{
		NodeId nodeIdToTest;
		MC_GotoErrorIfTrue(inputArguments.size() < 2, EnumModelDesignerStatus_BadInvalidArgument);

		// NULL value shall produce generated node ID - no check necessary
		if(inputArguments[1].getDataType() != EnumDataTypeId_Null)
		{
			MC_GotoErrorIfTrue(inputArguments[1].getNodeId(nodeIdToTest), EnumModelDesignerStatus_BadInvalidArgument);

			// NULL nodeId shall produce generated node ID - no check necessary
			if(nodeIdToTest.compare(NodeId()) != 0)
			{
				MC_GotoErrorIfTrue(nodeIdToTest.getNamespaceIndex() <= toolkitNamespaceIdx, EnumModelDesignerStatus_BadDenied);
			}
		}
	}
	else if(methodName == _T("CreateMethod"))
	{
		NodeId nodeIdToTest;
		MC_GotoErrorIfTrue(inputArguments.size() < 2, EnumModelDesignerStatus_BadInvalidArgument);

		// NULL value shall produce generated node ID - no check necessary
		if(inputArguments[1].getDataType() != EnumDataTypeId_Null)
		{
			MC_GotoErrorIfTrue(inputArguments[1].getNodeId(nodeIdToTest), EnumModelDesignerStatus_BadInvalidArgument);

			// NULL nodeId shall produce generated node ID - no check necessary
			if(nodeIdToTest.compare(NodeId()) != 0)
			{
				MC_GotoErrorIfTrue(nodeIdToTest.getNamespaceIndex() <= toolkitNamespaceIdx, EnumModelDesignerStatus_BadDenied);
			}
		}
	}
	else if(methodName == _T("AddReference"))
	{
		NodeId sourceNodeId;
		NodeId targetNodeId;
		MC_GotoErrorIfTrue(inputArguments.size() != 3, EnumModelDesignerStatus_BadInvalidArgument);
		MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[1].getNodeId(sourceNodeId)), EnumModelDesignerStatus_BadInvalidArgument);
		MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[2].getNodeId(targetNodeId)), EnumModelDesignerStatus_BadInvalidArgument);

		MC_GotoErrorIfTrue((sourceNodeId.getNamespaceIndex() <= toolkitNamespaceIdx) && (targetNodeId.getNamespaceIndex() <= toolkitNamespaceIdx), EnumModelDesignerStatus_BadDenied);
	}
	else if(methodName == _T("RemoveReference"))
	{
		NodeId sourceNodeId;
		NodeId targetNodeId;
		MC_GotoErrorIfTrue(inputArguments.size() != 3, EnumModelDesignerStatus_BadInvalidArgument);
		MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[1].getNodeId(sourceNodeId)), EnumModelDesignerStatus_BadInvalidArgument);
		MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[2].getNodeId(targetNodeId)), EnumModelDesignerStatus_BadInvalidArgument);

		MC_GotoErrorIfTrue((sourceNodeId.getNamespaceIndex() <= toolkitNamespaceIdx) && (targetNodeId.getNamespaceIndex() <= toolkitNamespaceIdx), EnumModelDesignerStatus_BadDenied);
	}
	else if(methodName == _T("MoveNode"))
	{
		NodeId nodeToMoveId;
		MC_GotoErrorIfTrue(inputArguments.size() != 3, EnumModelDesignerStatus_BadInvalidArgument);
		MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[0].getNodeId(nodeToMoveId)), EnumModelDesignerStatus_BadInvalidArgument);
		// reject to move nodes of predefined namespaces
		MC_GotoErrorIfTrue((nodeToMoveId.getNamespaceIndex() <= toolkitNamespaceIdx), EnumModelDesignerStatus_BadDenied);
	}
	else if(methodName == _T("GetExtensions"))
	{
		// no restriction
	}
	else if(methodName == _T("SetExtensions"))
	{
		NodeId nodeToMoveId;
		MC_GotoErrorIfTrue(inputArguments.size() != 2, EnumModelDesignerStatus_BadInvalidArgument);
		MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[0].getNodeId(nodeToMoveId)), EnumModelDesignerStatus_BadInvalidArgument);
		// reject to modify nodes of predefined namespaces
		MC_GotoErrorIfTrue((nodeToMoveId.getNamespaceIndex() <= toolkitNamespaceIdx), EnumModelDesignerStatus_BadDenied);
	}
	else if(methodName == _T("ModifyAttributes"))
	{
		NodeId nodeToModify;
		MC_GotoErrorIfTrue(inputArguments.size() < 2, EnumModelDesignerStatus_BadInvalidArgument);
		MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[0].getNodeId(nodeToModify)), EnumModelDesignerStatus_BadInvalidArgument);
		// reject to modify nodes of predefined namespaces
		MC_GotoErrorIfTrue((nodeToModify.getNamespaceIndex() <= toolkitNamespaceIdx), EnumModelDesignerStatus_BadDenied);

		// NULL value for new nodeId shall be ignored
		if(inputArguments[1].getDataType() != EnumDataTypeId_Null)
		{
			NodeId newNodeId;
			MC_GotoErrorIfTrue(StatusCode::isBad(inputArguments[1].getNodeId(newNodeId)), EnumModelDesignerStatus_BadInvalidArgument);

			// NULL nodeId shall be ignored as well
			if(newNodeId.compare(NodeId()) != 0)
			{
				// reject to modify nodeId to predefined namespaces
				MC_GotoErrorIfTrue((newNodeId.getNamespaceIndex() <= toolkitNamespaceIdx), EnumModelDesignerStatus_BadDenied);
			}
		}
	}
	else
	{
		_tprintf(_T("Unhandled ModelDesigner Method '%s'\n"), methodName.c_str());
		_REPORT_ASSERTION(_T("Unhandled ModelDesigner Method"));
		MC_GotoErrorWithStatus(EnumModelDesignerStatus_BadNotImplemented);
	}

MC_ReturnStatusCode;

MC_BeginErrorHandling;

MC_FinishErrorHandling;
}

EnumModelDesignerStatus TestServerAddressSpaceRoot::onResetAddressSpace()
{
	g_resetAddressSpace = true;
	getApplicationModule()->shutDown(1000);
	return EnumModelDesignerStatus_Good;
}


void TestServerAddressSpaceRoot::handleExportCustomNodes(Server::NodeSet2Export::ExportCustomNodesTransactionPtr transaction)
{
	Trace::instance()->writeLine(EnumTraceLevel_Info , EnumTraceGroup_ModelDesigner, _T("handleExportCustomNodes"), _T(""));
	DynamicExportWorkItem* workItem = new DynamicExportWorkItem(transaction);
	if(!getApplicationModule()->addWorkItem(workItem))
	{
		workItem->execute();
		delete workItem;
	}
	workItem = NULL;
}

void TestServerAddressSpaceRoot::onValidateError( const INodeId* nodeId, OTUInt16 severity, const tstring& errorMessage )
{
	tstring entry;
	getNamespaces()->getEntry(nodeId->getNamespaceIndex(), entry);
	tstringstream ss;
	ss << _T("Validation ERROR at ") << nodeId->toString() << _T("\n");
	ss << _T("    ns URI:  ") << entry << _T("\n");
	// severity is currently always 1
	//ss << _T("    severity: ") << severity << _T("\n");
	_UNUSED(severity);
	ss << _T("    message: ") << errorMessage << _T("\n");
	_tprintf(_T("%s"), ss.str().c_str());
}
