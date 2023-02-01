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

#include "TestMethod.h"
#include "Argument.h"
#include <limits>
#include "TestModule.h"
#include "ApplicationModule.h"
#include "Guid.h"
#include "EUInformation.h"
#include "Range.h"
#include "Statics.h"
#include "ServerMethod.h"
#include "SimVarMethods.h"
#include "ChangeAddressSpaceMethod.h"
#include "ServerAnalogItem.h"
#include "ServerTwoStateItem.h"
#include "ServerMultiStateItem.h"
#include "HistorizingVariable.h"

using namespace SoftingOPCToolbox5;


EnumStatusCode UserAccessModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	{ // add a user access method in the test server
		MethodPtr method = new TestMethod(3,3);
		tstring strMethodName(_T("AuthenticatedUserMethod"));
		method->setBrowseName(QualifiedName(strMethodName, namespaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);

		method->setNodeId(NodeId(namespaceIndex, strMethodName));
		method->setExecutable(true);
		// Executable only for authenticated users
		ApplicationModule::assignAccessPatternAndRolePermissions(method, ApplicationModule::s_AccessKey_Read_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);


		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(Statics::DataTypeId_UInt32);
		param.setName(_T("Input_1"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		param.setDataType(Statics::DataTypeId_UInt32);
		param.setName(_T("Input_2"));
		inputArgs.push_back(param);

		param.setDataType(Statics::DataTypeId_Double);
		param.setName(_T("Input_3"));
		inputArgs.push_back(param);

		NodeId inputArgNodeId(namespaceIndex, _T("AuthenticatedUserMethodInputArgs"));
		method->setInputArguments(&inputArgNodeId, inputArgs);

		// define output args for the method ......
		VariablePtr pOutputArgs = Variable::create();
		vector<Argument> outputArgs;
		param.setDataType(Statics::DataTypeId_UInt32);
		param.setName(_T("Output_1"));
		param.setValueRank(ValueRank_Scalar);
		outputArgs.push_back(param);

		param.setDataType(Statics::DataTypeId_UInt32);
		param.setName(_T("Output_2"));
		outputArgs.push_back(param);

		param.setDataType(Statics::DataTypeId_Double);
		param.setName(_T("Output_3"));
		outputArgs.push_back(param);

		NodeId outputArgNodeId(namespaceIndex, _T("AuthenticatedUserMethodOutputArgs"));
		method->setOutputArguments(&outputArgNodeId, outputArgs);
	}

	{ // add a user access method in the test server
		MethodPtr method = new TestMethod(3,3);
		tstring strMethodName(_T("MethodForUser1"));
		method->setBrowseName(QualifiedName(strMethodName, namespaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		ltStr.defineTranslation(_T("Method can be invoked only by group1 (usr)"), _T("en"));

		method->setNodeId(NodeId(namespaceIndex, strMethodName));
		method->setExecutable(true);
		// Executable only for user group 1
		ApplicationModule::assignAccessPatternAndRolePermissions(method, ApplicationModule::s_AccessKey_Read_ReadWriteCall_Read_Read_Read);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);


		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(Statics::DataTypeId_UInt32);
		param.setName(_T("Input_1"));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		param.setDataType(Statics::DataTypeId_UInt32);
		param.setName(_T("Input_2"));
		inputArgs.push_back(param);

		param.setDataType(Statics::DataTypeId_Double);
		param.setName(_T("Input_3"));
		inputArgs.push_back(param);

		NodeId inputArgNodeId(namespaceIndex, _T("InputArgs_User1"));
		method->setInputArguments(&inputArgNodeId, inputArgs);

		// define output args for the method ......
		VariablePtr pOutputArgs = Variable::create();
		vector<Argument> outputArgs;
		param.setDataType(Statics::DataTypeId_UInt32);
		param.setName(_T("Output_1"));
		param.setValueRank(ValueRank_Scalar);
		outputArgs.push_back(param);

		param.setDataType(Statics::DataTypeId_UInt32);
		param.setName(_T("Output_2"));
		outputArgs.push_back(param);

		param.setDataType(Statics::DataTypeId_Double);
		param.setName(_T("Output_3"));
		outputArgs.push_back(param);

		NodeId outputArgNodeId(namespaceIndex, _T("OutputArgs_User1"));
		method->setOutputArguments(&outputArgNodeId, outputArgs);
	}

	(void)createUserAccessVariable(_T("ReadOnly"),                 namespaceIndex, Statics::DataTypeId_UInt32, Value((OTUInt32)1234), true,  false,
		ApplicationModule::s_AccessKey_Read_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None);
	(void)createUserAccessVariable(_T("NoAccess"),                 namespaceIndex, Statics::DataTypeId_UInt32, Value((OTUInt32)1234), false, false,
		ApplicationModule::s_AccessKey_Read_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None);
	(void)createUserAccessVariable(_T("ReadOnlyForAnonymousUser"), namespaceIndex, Statics::DataTypeId_UInt32, Value((OTUInt32)1234), true,  true,
		ApplicationModule::s_AccessKey_Read_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None);
	(void)createUserAccessVariable(_T("NoAccessForAnonymousUser"), namespaceIndex, Statics::DataTypeId_UInt32, Value((OTUInt32)1234), true,  true,
		ApplicationModule::s_AccessKey_None_ReadWriteHistoryCall_ReadWriteHistoryCall_ReadWriteHistoryCall_None);
	(void)createUserAccessVariable(_T("WritableUser1"),            namespaceIndex, Statics::DataTypeId_UInt32, Value((OTUInt32)1234), true,  true,
		ApplicationModule::s_AccessKey_Read_ReadWriteCall_Read_Read_Read);

	return result;
}

/* @brief Creates one test variable with user access rights into TestModule folder
* @param varName          Name of the OPC variable
* @param dataType          Datatype of the OPC variable
* @param readable         Indicates if it is possible to read the current value
* @param writeable        Indicates if it is possible to write the current value
* @param accessPatternKey The AccessPattern for with user access rights
* @return TestVariable as OPC variable */
TestVariablePtr UserAccessModule::createUserAccessVariable(const tstring& varName, OTUInt16 namespaceIndex, NodeId* pDataTypeId, const Value& initialValue, bool readable, bool writeable, OTUInt32 accessPatternKey)
{
	TestVariablePtr testVariable = new TestVariable();

	testVariable->setBrowseName(QualifiedName(varName, namespaceIndex));
	LocalizedText lt;
	lt.setText(varName);
	lt.setLocale(_T("en"));
	testVariable->setDisplayName(&lt);
	testVariable->setNodeId(NodeId(namespaceIndex, varName));
	testVariable->setDataType(pDataTypeId);
	DataValue dv;
	dv.setValue(initialValue);
	dv.setServerTimestamp(DateTime::getUtcNow());
	dv.setSourceTimestamp(DateTime::getUtcNow());
	testVariable->setDataValue(dv);
	testVariable->setVariableType(Statics::VariableTypeId_DataItemType);
	EnumAccessLevel accessLevel = EnumAccessLevel_None;
	if (readable)
	{
		accessLevel |= EnumAccessLevel_CurrentRead;
	}
	if (writeable)
	{
		accessLevel |= EnumAccessLevel_CurrentWrite;
	}
	testVariable->setAccessLevel(accessLevel);

	// apply user access settings
	ApplicationModule::assignAccessPatternAndRolePermissions(testVariable, accessPatternKey);

	testVariable->setWriteMask(EnumWriteMask_Description);
	testVariable->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
	testVariable->insert();
	this->addReference(Statics::ReferenceTypeId_HasComponent, testVariable);
	return testVariable;
}
