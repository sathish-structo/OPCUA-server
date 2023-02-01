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
#ifndef _TESTMODULE_H_
#define _TESTMODULE_H_

#include "TestVariable.h"
#include "DynamicTestVariable.h"
#include "ServerObject.h"
#include "ServerMethod.h"
#include "Statics.h"
#include "ModuleFolder.h"

using namespace std;
using namespace SoftingOPCToolbox5;

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		class DataType;
		typedef ObjectPointer<DataType> DataTypePtr;
	}
}

// Modules

// handles the content of the Statics folder
class StaticsModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);

private:
	void createAllScalarVariables(OTUInt16 namespaceIndex);
	void createAllArrayVariables(OTUInt16 namespaceIndex);

	/* @brief Creates one test variable into TestModule folder */
	TestVariablePtr createVariable(const tstring& varName, OTUInt16 namespaceIndex, const NodeId* pDataTypeId, bool writeable = true);
	TestVariablePtr createArrayVariable(const tstring& varName, OTUInt16 namespaceIndex, const NodeId* pDataTypeId);

	TestVariablePtr createAndInitVariable(const tstring& varName, OTUInt16 namespaceIndex, const NodeId* pDataTypeId, bool writeable = true);
	TestVariablePtr createAndInitArrayVariable(const tstring& varName, OTUInt16 namespaceIndex, const NodeId* pDataTypeId);

	void initVariableValue(TestVariablePtr variable, const NodeId* pDataTypeId);
	void initArrayVariableValue(TestVariablePtr variable, const NodeId* pDataTypeId);
};

// handles the content of the Dynamics folder
class DynamicsModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	virtual void endModule();

private:
	void createAllDynamicVariables();
	void createDynamicArrayTestVariablesPart1();
	void createDynamicArrayTestVariablesPart2();
	void createDynamicArrayTestVariablesPart3();

	/**
	* @brief Creates one test variable into TestModule folder
	* @param varName Name of the OPC variable
	* @param dataValues A set of valid data values
	* @param varType Datatype of the OPC variable
	* @param isArray If true, the variable is declared as array
	* @return TestVariable as OPC variable
	*/
	template<typename T>
	DynamicTestVariablePtr createDynamicVariable(const tstring& varName, T* values, unsigned int count, NodeId* varType, bool isArray)
	{
		std::vector<DataValue> dataValues;
		dataValues.reserve(count);
		for(unsigned int i = 0; i < count; i++)
		{
			DataValue dv;
			dv.setValue(values[i]);
			dataValues.push_back(dv);
		}

		DynamicTestVariablePtr testVariable = new DynamicTestVariable(dataValues);
		testVariable->setBrowseName(QualifiedName(varName, m_namespaceIndex));
		LocalizedText lt;
		lt.setText(varName);
		lt.setLocale(_T("en"));
		testVariable->setDisplayName(&lt);
		testVariable->setNodeId(NodeId(m_namespaceIndex, varName));
		testVariable->setDataType(varType);
		testVariable->setAccessLevel(testVariable->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		testVariable->setWriteMask(EnumWriteMask_Description);
		if(isArray)
		{
			testVariable->setValueRank(1);
		}
		testVariable->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		testVariable->insert();
		// this set for the initial value is not necessary, but tests to set values from different threads
		if(!dataValues.empty())
		{
			testVariable->setDataValue(dataValues[0]);
		}
		this->addReference(Statics::ReferenceTypeId_HasComponent, testVariable);
		m_dynamicVariableChangeAction.add(testVariable);
		return testVariable;
	}

	/* @brief Same as above, but uses constructor for complex types
	* (that constructors takes pointers as parameter) */
	template<typename T>
	DynamicTestVariablePtr createDynamicVariableComplex(const tstring& varName, T* values, unsigned int count, NodeId* varType)
	{
		std::vector<DataValue> dataValues;
		dataValues.reserve(count);
		for(unsigned int i = 0; i < count; i++)
		{
			DataValue dv;
			dv.setValue(&values[i]);
			dataValues.push_back(dv);
		}

		DynamicTestVariablePtr testVariable = new DynamicTestVariable(dataValues);
		testVariable->setBrowseName(QualifiedName(varName, m_namespaceIndex));
		LocalizedText lt;
		lt.setText(varName);
		lt.setLocale(_T("en"));
		testVariable->setDisplayName(&lt);
		testVariable->setNodeId(NodeId(m_namespaceIndex, varName));
		testVariable->setDataType(varType);
		//NodeId dataType = testVariable->getDataType();
		testVariable->setVariableType(Statics::VariableTypeId_DataItemType);
		testVariable->setAccessLevel(testVariable->getAccessLevel() | EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);
		testVariable->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
		testVariable->insert();
		this->addReference(Statics::ReferenceTypeId_HasComponent, testVariable);
		m_dynamicVariableChangeAction.add(testVariable);
		return testVariable;
	}

	OTUInt16 m_namespaceIndex;
	/*! @brief The scheduled action which manages the changes for the dynamic variables */
	DynamicVariableChange m_dynamicVariableChangeAction;
};

class MatrixModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);

private:
	TestVariablePtr createMatrixVariable(const tstring& varName, OTUInt16 namespaceIndex, const NodeId* pDataTypeId, const std::vector<OTUInt32> &arrayDimensions, bool writeable);

};

class ChangingContentModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

class BrowseTestModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

class CustomDataTypeTestModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	virtual void endModule();
private:
	Server::DataTypePtr m_myInt32;
	Server::VariablePtr m_myInt32Variable;
};

class ExportImportNodesModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

class ServiceDelayModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	virtual void endModule();

	OTUInt32 getServiceDelay(EnumNumericNodeId serviceId);
	EnumStatusCode setServiceDelay(EnumNumericNodeId serviceId, OTUInt32 delay);
private:
	void createDelayVariable(const tstring& name, OTUInt16 namespaceIndex, EnumNumericNodeId serviceId, OTUInt32 initialValue);

	std::map<EnumNumericNodeId, SoftingOPCToolbox5::Server::VariablePtr> m_delayVariables;
};
typedef SoftingOPCToolbox5::ObjectPointer<ServiceDelayModule> ServiceDelayModulePtr;

class UserAccessModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);

private:
	TestVariablePtr createUserAccessVariable(const tstring& varName, OTUInt16 namespaceIndex, NodeId* pDataTypeId, const Value& initialValue, bool readable, bool writeable, OTUInt32 accessPatternKey);
};

class AnalogItemsModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

class DiscreteItemsModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

class AttributeItemsModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

class DynamicHistoricalItemsModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

class StaticHistoricalItemsModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

class HistoricalEventsModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

class OperationLimitsModule
	: public ModuleFolder
{
public:
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
};

/**
 * @brief TestModule will expose some variables with the different types.
 * One variable for each type with a name containing the variables type-name.
 */
class TestModule
	: public ModuleFolder
{
public:
	/*! @brief Constructor */
	TestModule();

	/*! @brief Destructor */
	virtual ~TestModule();

	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	virtual void endModule();

	/*! Internal class to handle the set service level method. */
	class SetServiceLevelMethod : public Server::Method
	{
	private:
		SetServiceLevelMethod();
	public:
		~SetServiceLevelMethod();

		/*! Creates a new method object in the address space. 
		*
		* The parent object is given, the own node ID are provided by the containing RedModule in class variables.
		* The other information (strings, ...) are hard coded
		*/
		static ObjectPointer<SetServiceLevelMethod> create(const NodeId& methodNodeId, Server::BaseNodePtr parent);

		/*! Overwritten handler. 
		*
		* The handler passes the nodes and data values to the containing RedModule.
		* The containing module can deny the request (e.g. if its own server state is not secondary)
		*/
		virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);
	};
	typedef ObjectPointer<SetServiceLevelMethod> RedSetServiceLevelMethodPtr;

	OTUInt32 getServiceDelay(EnumNumericNodeId serviceId);
	EnumStatusCode setServiceDelay(EnumNumericNodeId serviceId, OTUInt32 delay);

private:
	/*! Contains the sub modules folders below the TestModule */
	std::vector<ModuleFolderPtr> m_modules;

	/*! Specific modules for direct access */
	ServiceDelayModulePtr m_serviceDelayModule;

	/*! @brief Name space index */
	OTUInt16 m_nameSpaceIndex;

private:
	/*! @brief Creates all the test variables into TestModule folder */
	void createNodeIdTestVariables(void);
	/*! Create the methods for simulation variables.*/
	void createSimVarMethods(void);

    /*! Create variables/methods for internal parallel access testing.*/
    EnumStatusCode createParrallelAccessNodes();

	Server::ObjectPtr createQueryTestArea();

	/* @brief Tests all the constructors from the Value class*/
	void testValueConstructors();

	void defineSupportedAggregateFunctions();
};
typedef SoftingOPCToolbox5::ObjectPointer<TestModule> TestModulePtr;

#endif
