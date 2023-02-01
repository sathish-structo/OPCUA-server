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
#ifndef MODULEFOLDER_H
#define MODULEFOLDER_H

#include "ServerObject.h"

class ModuleFolder;
typedef SoftingOPCToolbox5::ObjectPointer<ModuleFolder> ModuleFolderPtr;

/*! A special folder object to contain nodes and settings testing */
class ModuleFolder
	: public SoftingOPCToolbox5::Server::Object
{
public:
	ModuleFolder();
	virtual ~ModuleFolder();

	/*! Initializes the module folder, its sub nodes and settings.
	*
	* The base implementation initializes and inserts the folder to the address space */
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	/*! Cleans up of the module folder and removes it from the address space
	*
	* The base implementation removes the folder from the address space, including the child nodes. */
	virtual void endModule();
	// PROSANTA
    virtual void Update(void *pData);

	static void setPathToServerFiles(tstring path);

protected:
	static tstring m_pathToServerFiles;
};

#endif	// MODULEFOLDER_H
