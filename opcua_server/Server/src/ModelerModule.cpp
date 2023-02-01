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
#include "ModelerModule.h"

#include "ModelerEnableDisable.h"
#include "ServerObject.h"
#include "ApplicationModule.h"


using namespace SoftingOPCToolbox5;

EnumStatusCode ModelerModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isGood(result))
	{
		createCreateEnableDisable(namespaceIndex);
	}
	return result;
}

void ModelerModule::createCreateEnableDisable(OTUInt16 namespaceIndex)
{

	// xml Export functionality
	EnableModelerMethodPtr enableModelerMethod = new EnableModelerMethod();
	enableModelerMethod->initialize(this, namespaceIndex);
	DisableModelerMethodPtr disableModelerMethod = new DisableModelerMethod();
	disableModelerMethod->initialize(this, namespaceIndex);
}
