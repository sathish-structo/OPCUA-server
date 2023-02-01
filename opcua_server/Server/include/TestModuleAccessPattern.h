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
#ifndef TEST_MODULE_ACCESS_PATTERN_H
#define TEST_MODULE_ACCESS_PATTERN_H

#include "ServerMethod.h"
#include "ModuleFolder.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		class Object;
		typedef ObjectPointer<Object> ObjectPtr;
		class Variable;
		typedef ObjectPointer<Variable> VariablePtr;
	}
}

class ModifyAccessPatternMethod;
typedef SoftingOPCToolbox5::ObjectPointer<ModifyAccessPatternMethod> ModifyAccessPatternMethodPtr;

/*! Allows to modify one specific access pattern of the server */
class ModifyAccessPatternMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	virtual ~ModifyAccessPatternMethod();

	static ModifyAccessPatternMethodPtr initialize(OTUInt16 namespaceIndex,
		const tstring& name,
		OTUInt32 accessPatternKey,
		SoftingOPCToolbox5::Server::VariablePtr currentValueVar);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);

	EnumStatusCode setAccessPattern(const SoftingOPCToolbox5::ByteString& accessField);

private:
	ModifyAccessPatternMethod(OTUInt32 accessPatternKey, tstring accessPatternName, SoftingOPCToolbox5::Server::VariablePtr currentValueVar);
	ModifyAccessPatternMethod(const ModifyAccessPatternMethod& other);

	OTUInt32 m_accessPatternKey;
	tstring m_accessPatternName;
	SoftingOPCToolbox5::Server::VariablePtr m_currentValueVar;
};

class TestAccessPatternMethod;
typedef SoftingOPCToolbox5::ObjectPointer<TestAccessPatternMethod> TestAccessPatternMethodPtr;

/*! A simple test method to test the user executable attribute via the access pattern. */
class TestAccessPatternMethod
	: public SoftingOPCToolbox5::Server::Method
{
public:
	virtual ~TestAccessPatternMethod();

	static TestAccessPatternMethodPtr initialize(OTUInt16 namespaceIndex,
		const tstring& name,
		OTUInt32 accessPatternKey);

	virtual EnumStatusCode handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request);

	OTInt32 add(OTInt32 a, OTInt32 b);

private:
	TestAccessPatternMethod();
	TestAccessPatternMethod(const TestAccessPatternMethod& other);

	OTUInt32 m_accessPatternKey;
	tstring m_accessPatternName;
};

class AccessPatternModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

#endif	// TEST_MODULE_ACCESS_PATTERN_H
