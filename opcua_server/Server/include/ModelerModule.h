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
#ifndef _MODELERMODULE_H_
#define _MODELERMODULE_H_

#include "TestVariable.h"
#include "DynamicTestVariable.h"
#include "ServerObject.h"
#include "ServerVariable.h"
#include "ServerMethod.h"
#include "Statics.h"
#include "ModuleFolder.h"

using namespace std;
using namespace SoftingOPCToolbox5;
// forward references
class ApplicationModule;

class ModelerModule;
typedef SoftingOPCToolbox5::ObjectPointer<ModelerModule> ModelerModulePtr;

/**
 * @brief ModellerModule will expose Nodes and methods for the modeler
 */
class ModelerModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
private:

	/*! @brief Creates all the test variables into TestModule folder */
	void createCreateEnableDisable(OTUInt16 namespaceIndex);
};

#endif
