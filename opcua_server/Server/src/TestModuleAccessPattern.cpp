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
#include "TestModuleAccessPattern.h"
#include "Statics.h"
#include "Argument.h"
#include "AccessPattern.h"
#include "ServerMethodCallRequest.h"
#include "Application.h"
#include "ServerObject.h"
#include "ApplicationModule.h"

using namespace SoftingOPCToolbox5;


BEGIN_IGNORE_DEPRECATED()
EnumStatusCode AccessPatternModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}
	
	if(!ApplicationModule::s_accessPatternEnabled)
	{
		// initialize this module only if configuration of access patterns is enabled
		return result;
	}
	
	// server startup creates the access pattern 1 to 5
	OTUInt32 accessPatternKey = ApplicationModule::s_AccessKey_ForAccessModifications;

	// Exposes the AccessField of the modifyable access pattern
	tstring stringId = _T("AccessPatternCurrentValue");
	Server::VariablePtr currentValueVar = Server::Variable::create();
	currentValueVar->setNodeId(NodeId(namespaceIndex, stringId));
	currentValueVar->setBrowseName(QualifiedName(stringId, namespaceIndex));
	currentValueVar->setDisplayName(LocalizedText(stringId, _T("")));
	currentValueVar->setDescription(LocalizedText(_T("Shows the current value of the modifyable access pattern"), _T("")));
	currentValueVar->setDataType(Statics::DataTypeId_ByteString);
	// the DataValue will be initialized later
	// this variable can be read by everyone
	currentValueVar->init(NULL);
	this->addReference(Statics::ReferenceTypeId_HasComponent, currentValueVar);

	ModifyAccessPatternMethodPtr modifyMethod = ModifyAccessPatternMethod::initialize(namespaceIndex, _T("ModifyAccessPattern"), accessPatternKey, currentValueVar);
	this->addReference(Statics::ReferenceTypeId_HasComponent, modifyMethod);

	DataValue initialValue;
	initialValue.setValue((OTInt32)10);

	stringId = _T("AccessPatternReadWriteVar");
	Server::VariablePtr readWriteVar = TestVariableForAuditEvents::create();
	readWriteVar->setNodeId(NodeId(namespaceIndex, stringId));
	readWriteVar->setBrowseName(QualifiedName(stringId, namespaceIndex));
	readWriteVar->setDisplayName(LocalizedText(stringId, _T("")));
	readWriteVar->setDataType(Statics::DataTypeId_Int32);
	readWriteVar->setDataValue(initialValue);
	readWriteVar->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
	readWriteVar->setAccessPatternKey(accessPatternKey);
	readWriteVar->init(NULL);
	this->addReference(Statics::ReferenceTypeId_HasComponent, readWriteVar);

	stringId = _T("AccessPatternReadOnlyVar");
	Server::VariablePtr readOnlyVar = TestVariableForAuditEvents::create();
	readOnlyVar->setNodeId(NodeId(namespaceIndex, stringId));
	readOnlyVar->setBrowseName(QualifiedName(stringId, namespaceIndex));
	readOnlyVar->setDisplayName(LocalizedText(stringId, _T("")));
	readOnlyVar->setDataType(Statics::DataTypeId_Int32);
	readOnlyVar->setDataValue(initialValue);
	readOnlyVar->setAccessLevel(EnumAccessLevel_CurrentRead);
	readOnlyVar->setAccessPatternKey(accessPatternKey);
	readOnlyVar->init(NULL);
	this->addReference(Statics::ReferenceTypeId_HasComponent, readOnlyVar);

	TestAccessPatternMethodPtr testMethod = TestAccessPatternMethod::initialize(namespaceIndex, _T("AccessPatternTestMethod"), accessPatternKey);
	this->addReference(Statics::ReferenceTypeId_HasComponent, testMethod);

	return result;
}
END_IGNORE_DEPRECATED()

ModifyAccessPatternMethod::ModifyAccessPatternMethod(OTUInt32 accessPatternKey, tstring accessPatternName, Server::VariablePtr currentValueVar)
{
	m_accessPatternKey = accessPatternKey;
	m_accessPatternName = accessPatternName;
	m_currentValueVar = currentValueVar;
}

ModifyAccessPatternMethod::~ModifyAccessPatternMethod()
{
}

ModifyAccessPatternMethodPtr ModifyAccessPatternMethod::initialize(OTUInt16 namespaceIndex,
	const tstring& name,
	OTUInt32 accessPatternKey,
	Server::VariablePtr currentValueVar)
{
	tstringstream ss;
	ModifyAccessPatternMethodPtr modifyMethod = new ModifyAccessPatternMethod(accessPatternKey, _T("ModifyableTestPattern"), currentValueVar);
	modifyMethod->setNodeId(NodeId(namespaceIndex, name));
	modifyMethod->setBrowseName(QualifiedName(name, namespaceIndex));
	modifyMethod->setDisplayName(LocalizedText(name, _T("")));
	ss << _T("Allows to modify the access pattern for the key ") << accessPatternKey;
	modifyMethod->setDescription(LocalizedText(ss.str(), _T("")));
	ss.clear();

	std::vector<Argument> inputArguments;
	Argument accessFieldArg;
	accessFieldArg.setName(_T("AccessField"));
	accessFieldArg.setDescription(LocalizedText(_T("Defines the user access rights for the 4 available user groups:\n")
		_T("  0: anonymous    + user \"eve\" + issued token\n")
		_T("  1: user \"usr\" + X509\n")
		_T("  2: user \"usr2\"\n")
		_T("  3: user \"query\"\n")
		_T("AccessLevels:\n")
		_T("  0x01 Read\n")
		_T("  0x02 Write\n")
		_T("  0x80 Execute"), _T("")));
	accessFieldArg.setDataType(Statics::DataTypeId_ByteString);
	accessFieldArg.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(accessFieldArg);

	ss << name << _T("_InputArguments");
	NodeId inputArgId(namespaceIndex, ss.str());
	ss.clear();
	modifyMethod->setInputArguments(&inputArgId, inputArguments);
	modifyMethod->init(NULL);

	// initialize the access pattern with default values
	unsigned char initialField[] = {
		EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable,
		EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable,
		EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable,
		EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite | EnumAccessLevel_Executable,
	};
	modifyMethod->setAccessPattern(ByteString(4, initialField));
	return modifyMethod;
}

EnumStatusCode ModifyAccessPatternMethod::handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArguments = request->getInputArguments();
	std::vector<EnumStatusCode> inputArgumentResults;

	EnumStatusCode result = validateInputArguments(inputArguments, inputArgumentResults);
	if(StatusCode::isGood(result))
	{
		ByteString accessField = inputArguments[0].getByteString();
		result = setAccessPattern(accessField);
	}

	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}

BEGIN_IGNORE_DEPRECATED()
EnumStatusCode ModifyAccessPatternMethod::setAccessPattern(const ByteString& accessField)
{
	AccessPattern accessPattern;
	accessPattern.setKey(m_accessPatternKey);
	accessPattern.setAccessField(accessField);
	accessPattern.setName(m_accessPatternName);
	EnumStatusCode result = Application::instance()->setAccessPattern(&accessPattern);
	if(StatusCode::isGood(result))
	{
		DataValue dval;
		dval.setValue(&accessField);
		m_currentValueVar->setDataValue(dval);
	}
	return result;
}
END_IGNORE_DEPRECATED()

TestAccessPatternMethod::TestAccessPatternMethod()
{
}

TestAccessPatternMethod::~TestAccessPatternMethod()
{
}

BEGIN_IGNORE_DEPRECATED()
TestAccessPatternMethodPtr TestAccessPatternMethod::initialize(OTUInt16 namespaceIndex, const tstring& name, OTUInt32 accessPatternKey)
{
	tstringstream ss;
	TestAccessPatternMethodPtr testMethod = new TestAccessPatternMethod();
	testMethod->setNodeId(NodeId(namespaceIndex, name));
	testMethod->setBrowseName(QualifiedName(name, namespaceIndex));
	testMethod->setDisplayName(LocalizedText(name, _T("")));
	testMethod->setDescription(LocalizedText(_T("Tests the UserExecutable attribute for different AccessPattern, it adds two values"), _T("")));
	testMethod->setAccessPatternKey(accessPatternKey);

	std::vector<Argument> inputArguments;
	std::vector<Argument> outputArguments;
	Argument a;
	a.setName(_T("A"));
	a.setDescription(LocalizedText(_T("The first summand"), _T("")));
	a.setDataType(Statics::DataTypeId_Int32);
	a.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(a);

	Argument b;
	b.setName(_T("B"));
	b.setDescription(LocalizedText(_T("The second summand"), _T("")));
	b.setDataType(Statics::DataTypeId_Int32);
	b.setValueRank(ValueRank_Scalar);
	inputArguments.push_back(b);

	Argument sum;
	sum.setName(_T("Sum"));
	sum.setDescription(LocalizedText(_T("The sum of the addition"), _T("")));
	sum.setDataType(Statics::DataTypeId_Int32);
	sum.setValueRank(ValueRank_Scalar);
	outputArguments.push_back(sum);

	ss << name << _T("_InputArguments");
	NodeId inputArgId(namespaceIndex, ss.str());
	ss.clear();
	testMethod->setInputArguments(&inputArgId, inputArguments);
	ss << name << _T("_OutputArguments");
	NodeId outputArgId(namespaceIndex, ss.str());
	ss.clear();
	testMethod->setOutputArguments(&outputArgId, outputArguments);
	testMethod->init(NULL);

	return testMethod;
}
END_IGNORE_DEPRECATED()

EnumStatusCode TestAccessPatternMethod::handleExecuteRequest(SoftingOPCToolbox5::Server::MethodCallTransaction* transaction, SoftingOPCToolbox5::Server::MethodCallRequest* request)
{
	_UNUSED(transaction);
	std::vector<Value> inputArguments = request->getInputArguments();
	std::vector<Value> outputArguments;
	std::vector<EnumStatusCode> inputArgumentResults;

	EnumStatusCode result = validateInputArguments(inputArguments, inputArgumentResults);
	if(StatusCode::isGood(result))
	{
		OTInt32 a = inputArguments[0].getInt32();
		OTInt32 b = inputArguments[1].getInt32();

		OTInt32 sum = add(a, b);

		outputArguments.push_back(Value(sum));
		request->setOutputArguments(outputArguments);
	}

	request->setInputArgumentResults(inputArgumentResults);
	request->setStatusCode(result);
	request->complete();
	return EnumStatusCode_Good;
}
OTInt32 TestAccessPatternMethod::add(OTInt32 a, OTInt32 b)
{
	return a + b;
}
