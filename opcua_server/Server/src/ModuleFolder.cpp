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
#include "ModuleFolder.h"
#include "Statics.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

tstring ModuleFolder::m_pathToServerFiles;

ModuleFolder::ModuleFolder()
{
}

ModuleFolder::~ModuleFolder()
{
}

EnumStatusCode ModuleFolder::initModule(const tstring& name, OTUInt16 namespaceIndex, ObjectPtr parentFolder)
{
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
		result = parentFolder->addReference(Statics::ReferenceTypeId_Organizes, this);
	}
	if(StatusCode::isGood(result))
	{
		result = init(Statics::ObjectTypeId_FolderType);
	}
	if(StatusCode::isBad(result))
	{
		(void)this->removeTree();
	}
	return result;
}

void ModuleFolder::endModule()
{
	(void)removeTree();
}

void ModuleFolder::setPathToServerFiles(tstring path)
{
	m_pathToServerFiles = path;
}

void ModuleFolder::Update(void *pData)
{
   (void)(pData);
}
