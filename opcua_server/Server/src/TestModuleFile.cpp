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
#include "TestModuleFile.h"
#include "Statics.h"
#include "Argument.h"
#include "ServerMethodCallRequest.h"
#include "Application.h"
#include "ServerObject.h"
#include "ServerFile.h"
#include "ApplicationModule.h"

using namespace SoftingOPCToolbox5;

EnumStatusCode FileModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = EnumStatusCode_Good;
	NodeId addedNodeId;
	tstring stringId;
	Server::FilePtr addedFile;
	Server::ObjectPtr addedObject;
	tstring sourceFolder = getApplicationModule()->getSourceFolder();

	//Relative path to server TestFiles folder
	tstring pathToServerFilesFolder = m_pathToServerFiles;
	if (pathToServerFilesFolder == _T(""))
	{
		pathToServerFilesFolder=sourceFolder + _T("/Apps/Test/Cpp/StructoAutomoldServer/TestFiles");
	}

	// Create the root FileDirectory object, initialize path on file system
	m_root = Server::FileDirectory::create();
	result = m_root->setDirectoryPath(pathToServerFilesFolder);

	if(StatusCode::isGood(result))
	{
		result = m_root->setNodeId(NodeId(namespaceIndex, name));
	}
	if(StatusCode::isGood(result))
	{
		result = m_root->setBrowseName(QualifiedName(name, namespaceIndex));
	}
	if(StatusCode::isGood(result))
	{
		result = m_root->setDisplayName(LocalizedText(name, _T("")));
	}
	if(StatusCode::isGood(result))
	{
		result = parentFolder->addReference(Statics::ReferenceTypeId_Organizes, m_root);
	}
	if(StatusCode::isGood(result))
	{
		result = m_root->init(NULL);
	}
	if(StatusCode::isGood(result))
	{
		stringId = _T("ReadOnlyFile.file");
		Server::FilePtr readOnlyFile = Server::File::create();
		result = readOnlyFile->setNodeId(NodeId(namespaceIndex, stringId));
		if(StatusCode::isGood(result))
		{
			readOnlyFile->setBrowseName(QualifiedName(stringId, namespaceIndex));
			readOnlyFile->setDisplayName(LocalizedText(stringId, _T("")));
			readOnlyFile->setWritable(false);
			readOnlyFile->setFilePath(pathToServerFilesFolder + _T("/ReadOnlyFile.file"));
			// let the SDK produce NodeIds
			result = readOnlyFile->init(NULL);
		}
		if(StatusCode::isGood(result))
		{
			result = m_root->addReference(Statics::ReferenceTypeId_Organizes, readOnlyFile);
		}
	}

	if(StatusCode::isGood(result))
	{
		stringId = _T("ReadWriteFile.file");
		Server::FilePtr readWriteFile = Server::File::create();
		result = readWriteFile->setNodeId(NodeId(namespaceIndex, stringId));
		if(StatusCode::isGood(result))
		{
			readWriteFile->setBrowseName(QualifiedName(stringId, namespaceIndex));
			readWriteFile->setDisplayName(LocalizedText(stringId, _T("")));
			readWriteFile->setWritable(true);
			readWriteFile->setFilePath(pathToServerFilesFolder + _T("/ReadWriteFile.file"));
		}
		if(StatusCode::isGood(result))
		{
			// specify explicitly NodeIds
			result = readWriteFile->initEx(Statics::ObjectTypeId_FileType,
				NodeId(namespaceIndex, _T("ReadWriteFile.file.Size")),
				NodeId(namespaceIndex, _T("ReadWriteFile.file.Writable")),
				NodeId(namespaceIndex, _T("ReadWriteFile.file.UserWritable")),
				NodeId(namespaceIndex, _T("ReadWriteFile.file.OpenCount")));
		}
		if(StatusCode::isGood(result))
		{
			m_root->addReference(Statics::ReferenceTypeId_Organizes, readWriteFile);
		}
	}

	if(StatusCode::isGood(result))
	{
		stringId = _T("UserAccessFile.file");
		Server::FilePtr userAccessFile = Server::File::create();
		result = userAccessFile->setNodeId(NodeId(namespaceIndex, stringId));
		if(StatusCode::isGood(result))
		{
			userAccessFile->setBrowseName(QualifiedName(stringId, namespaceIndex));
			userAccessFile->setDisplayName(LocalizedText(stringId, _T("")));
			userAccessFile->setWritable(true);
			userAccessFile->setFilePath(pathToServerFilesFolder + _T("/UserAccessFile.file"));
			ApplicationModule::assignAccessPatternAndRolePermissions(userAccessFile, ApplicationModule::s_AccessKey_None_ReadCall_ReadWriteCall_ReadCall_ReadCall);
			result = userAccessFile->initEx(Statics::ObjectTypeId_FileType,
				NodeId(namespaceIndex, _T("UserAccessFile.file.Size")),
				NodeId(namespaceIndex, _T("UserAccessFile.file.Writable")),
				NodeId(namespaceIndex, _T("UserAccessFile.file.UserWritable")),
				NodeId(namespaceIndex, _T("UserAccessFile.file.OpenCount")));
		}
		if(StatusCode::isGood(result))
		{
			result = m_root->addReference(Statics::ReferenceTypeId_Organizes, userAccessFile);
		}
	}

	if(StatusCode::isGood(result))
	{
		// Add a sub directory
		stringId = _T("UserAccessDir");
		addedNodeId.setString(namespaceIndex, stringId);
		result = m_root->addSubDirectory(stringId, addedNodeId);
		_ASSERTION(StatusCode::isGood(result), _T("Sub-directory not created!"));
		if(StatusCode::isGood(result))
		{
			addedObject = Server::AddressSpaceRoot::instance()->getObject(&addedNodeId);
			if(addedNodeId.isNull() || addedObject->getApiClass() != EnumApiClassType_ServerFileDirectory)
			{
				_REPORT_ASSERTION(_T("Failed to find the valid Directory object"));
				result = EnumStatusCode_BadUnexpectedError;
			}
		}
		if(StatusCode::isGood(result))
		{
			Server::FileDirectoryPtr userAccessDir = static_pointer_cast<FileDirectory>(addedObject);
			ApplicationModule::assignAccessPatternAndRolePermissions(userAccessDir, ApplicationModule::s_AccessKey_None_ReadCall_ReadWriteCall_ReadCall_ReadCall);

			if(StatusCode::isGood(result))
			{
				// Add a file
				stringId = _T("ProtectedFile.file");
				addedNodeId.setString(namespaceIndex, stringId);
				result = userAccessDir->addFile(stringId, addedNodeId);
				_ASSERTION(StatusCode::isGood(result), _T("File not created!"));
			}
			if(StatusCode::isGood(result))
			{
				addedObject = Server::AddressSpaceRoot::instance()->getObject(&addedNodeId);
				if(addedNodeId.isNull() || addedObject->getApiClass() != EnumApiClassType_ServerFile)
				{
					_REPORT_ASSERTION(_T("Failed to find the valid File object"));
					result = EnumStatusCode_BadUnexpectedError;
				}
			}
			if(StatusCode::isGood(result))
			{
				ApplicationModule::assignAccessPatternAndRolePermissions(addedObject, ApplicationModule::s_AccessKey_None_ReadCall_ReadWriteCall_ReadCall_ReadCall);
			}
		}
	}
	else
	{
		(void)m_root->removeTree();
		m_root = NULL;
	}

	return result;
}

void FileModule::endModule()
{
	if(m_root.isNotNull())
	{
		(void)m_root->removeTree();
	}
}
