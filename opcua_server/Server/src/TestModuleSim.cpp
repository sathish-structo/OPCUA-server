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
#include "Constants.h"

using namespace SoftingOPCToolbox5;

/*! Create the methods for simulation variables. */
void TestModule::createSimVarMethods(void)
{
	static const unsigned short namespaceIdx = 3;
	{
		// Simulation variables - CreateVariables method by Range
		MethodPtr method = new CreateSimVarsByRangeMethod();
		tstring strMethodName(_T("CreateVariablesByRange"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("createVariablesByRange"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);

		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_SByte));
		param.setName(_T("nodeIdType"));
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("Numeric: 0, String: 1, Guid: 2, Opaque: 3"), _T("en")));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("count"));
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("Number of variables to create"), _T("en")));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("dataType"));
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("The numeric identifier of the data type for the created variables' Value. Some examples: Boolean: 1, Byte: 3, Int16: 4, UInt32: 7, Int64: 8, Double: 11, String: 12."), _T("en")));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("arrayDimensions"));
		param.setValueRank(1);
		param.setDescription(LocalizedText(_T("The number of dimensions and their sizes. An empty array means scalar."), _T("en")));
		inputArgs.push_back(param);

		NodeId inputArgumentId(m_nameSpaceIndex, _T("createVariablesByRange_InputArguments"));
		method->setInputArguments(&inputArgumentId, inputArgs);

		VariablePtr pOutputArgs = Variable::create();
		vector<Argument> outputArgs;

		// define output args for the method ......
		Argument outputParam;

		outputParam.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		outputParam.setName(_T("simVarSetId"));
		outputParam.setValueRank(ValueRank_Scalar);
		outputParam.setDescription(LocalizedText(_T("The ID assigned to the created set of variable"), _T("en")));
		outputArgs.push_back(outputParam);

		outputParam.setDataType(NodeId(0, EnumDataTypeId_NodeId));
		outputParam.setName(_T("firstNodeId"));
		outputParam.setValueRank(ValueRank_Scalar);
		outputParam.setDescription(LocalizedText(_T("The nodeId of the first created node. All other nodes of the range will have an ascending NodeId identifier."), _T("en")));
		outputArgs.push_back(outputParam);

		NodeId outputArgumentId(m_nameSpaceIndex, _T("createVariablesByRange_OutputArguments"));
		method->setOutputArguments(&outputArgumentId, outputArgs);
	}

	{
		// Simulation variables - DeleteVariables method by Range
		MethodPtr method = new DeleteVarsMethod();
		tstring strMethodName(_T("DeleteVariables"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("deleteVariables"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);

		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("simVarSetId"));
		param.setDescription(LocalizedText(_T("Id of the variable set to be deleted."),_T("en")));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("deleteVariables_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		pInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);
	}

	{
		// Simulation variables - StartSimulation method
		MethodPtr method = new StartSimulationSimVarsMethod();
		tstring strMethodName(_T("StartSimulation"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("startSimulation"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);

		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("simVarSetId"));
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("The ID of the created set of simulation variables"), _T("en")));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("changeInterval"));
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("The interval in milliseconds in which variable values are updated"), _T("en")));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("repeatCount"));
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("The number of times each variable value is updated. 0 means infinite."), _T("en")));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_Double));
		param.setName(_T("increment"));
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("The number by which each variable varlue is incremented every changeInterval"), _T("en")));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("changeCount"));
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("The number of variables to update each changeInterval"), _T("en")));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("bulkHandling"));
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("0: update one variable after the other, 1: use bulk writing"), _T("en")));
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("bulkSize"));
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("Only used if bulkHandling is 1. The number of values to update at once."), _T("en")));
		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("startSimulation_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		pInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);

	}


	{
		// Simulation variables - StopAllSimulations method
		MethodPtr method = new ResetSimulationsMethod();
		tstring strMethodName(_T("StopAllSimulations"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		method->setNodeId(NodeId(namespaceIdx, strMethodName));
		method->setExecutable(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);
	}

	{
		// Simulation variables - StopSimulation method
		MethodPtr method = new StopSimVarsMethod();
		tstring strMethodName(_T("StopSimulation"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("stopSimulation"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);

		VariablePtr pInputArgs = Variable::create();
		vector<Argument> inputArgs;

		// define input args for the method ......
		Argument param;
		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("simVarSetId"));
		// 1 stands for arrays
		param.setValueRank(ValueRank_Scalar);
		param.setDescription(LocalizedText(_T("The ID of the variable set for which to stop the simulation"), _T("en")));

		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(m_nameSpaceIndex, _T("stopSimulation_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments, 0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));
		pInputArgs->setVariableType(Statics::VariableTypeId_PropertyType);
		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		method->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);

	}

	{
		// Simulation variables - DumpSimVars method
		MethodPtr method = new DumpSimVars();
		tstring strMethodName(_T("DumpSimulation"));
		method->setBrowseName(QualifiedName(strMethodName, m_nameSpaceIndex));
		LocalizedText ltStr;
		ltStr.setText(strMethodName);
		ltStr.setLocale(_T("en"));
		method->setDisplayName(&ltStr);
		tstring strMethodId(_T("dumpSimulation"));

		method->setNodeId(NodeId(namespaceIdx, strMethodId));
		method->setExecutable(true);
		method->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, method);
	}
}

