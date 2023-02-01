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
#ifndef TEST_MODULE_FILE_H
#define TEST_MODULE_FILE_H

#include "ModuleFolder.h"
#include "ServerFileDirectory.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		class Object;
		typedef ObjectPointer<Object> ObjectPtr;
	}
}

class FileModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	virtual void endModule();

private:
	SoftingOPCToolbox5::Server::FileDirectoryPtr m_root;
};

#endif	// TEST_MODULE_FILE_H
