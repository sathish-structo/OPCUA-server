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
#ifndef STRUCTUREDDATATYPEMODULE_H
#define STRUCTUREDDATATYPEMODULE_H

#include "NodeId.h"
#include "ModuleFolder.h"
#include "StructureField.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		class Object;
		typedef ObjectPointer<Object> ObjectPtr;
	}

	class FieldDescription;
	typedef ObjectPointer<FieldDescription> FieldDescriptionPtr;
}

class StructuredDataTypeModule
	: public ModuleFolder
{
public:
	/*! Constructor */
	StructuredDataTypeModule();

	/*! Destructor */
	~StructuredDataTypeModule();

	/*! Initialize the structured data type module
	* It creates a data type with a dictionary and data type description for a
	* structured data type, similarly to the type in the .Net Demo Server.
	*
	* @note The namespaceIndex should be the index of the namespace "http://opcfoundation.org/UA/Refrigerator"*/
	virtual EnumStatusCode initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);

	/*! Cleans up the structured data type module
	*
	* In general all actions of the init() method are cleaned up (in reverse order) */
	virtual void endModule();

	SoftingOPCToolbox5::NodeId getDataTypeNodeId();
private:
	void initBigBarrel(OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	void initRefrigeratorStateEnumeration(OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	EnumStatusCode initStructureWithOptional(OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	EnumStatusCode initChildStructure(OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);
	void createScalarFieldDescription(std::vector<SoftingOPCToolbox5::StructureField>& fields, EnumNumericNodeId type);
	void createArrayFieldDescription(std::vector<SoftingOPCToolbox5::StructureField>& fields, EnumNumericNodeId type);
	EnumStatusCode initUnion(OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder);

	template<typename T>
	void insertArrayValues(std::vector<SoftingOPCToolbox5::Value>& values, const T& val1)
	{
		std::vector<T> arrValues;
		arrValues.push_back(val1);
		values.push_back(SoftingOPCToolbox5::Value(arrValues));
	}
	template<typename T>
	void insertArrayValues(std::vector<SoftingOPCToolbox5::Value>& values, const T& val1, const T& val2)
	{
		std::vector<T> arrValues;
		arrValues.push_back(val1);
		arrValues.push_back(val2);
		values.push_back(SoftingOPCToolbox5::Value(arrValues));
	}
	template<typename T>
	void insertArrayValues(std::vector<SoftingOPCToolbox5::Value>& values, const T& val1, const T& val2, const T& val3)
	{
		std::vector<T> arrValues;
		arrValues.push_back(val1);
		arrValues.push_back(val2);
		arrValues.push_back(val3);
		values.push_back(SoftingOPCToolbox5::Value(arrValues));
	}
	template<typename T>
	void insertArrayValues(std::vector<SoftingOPCToolbox5::Value>& values, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5)
	{
		std::vector<T> arrValues;
		arrValues.push_back(val1);
		arrValues.push_back(val2);
		arrValues.push_back(val3);
		arrValues.push_back(val4);
		arrValues.push_back(val5);
		values.push_back(SoftingOPCToolbox5::Value(arrValues));
	}

	SoftingOPCToolbox5::NodeId m_refrigeratorStatusDataTypeId;
	SoftingOPCToolbox5::NodeId m_bigBarrelTypeId;
	SoftingOPCToolbox5::NodeId m_dictionaryId;
	SoftingOPCToolbox5::NodeId m_enumRefrigeratorStateTypeId;
	SoftingOPCToolbox5::NodeId m_enumRefrigeratorStateEventTypeId;
	SoftingOPCToolbox5::NodeId m_outerStructureDataTypeId;

	SoftingOPCToolbox5::NodeId m_structureDataWithOptionalsTypeId;
	SoftingOPCToolbox5::NodeId m_structureDataWithOptionalsNestedTypeId;

	SoftingOPCToolbox5::NodeId m_childStructureDataTypeId;
	SoftingOPCToolbox5::NodeId m_childStructureNestedDataTypeId;

	SoftingOPCToolbox5::NodeId m_optionSetDataTypeId;

	SoftingOPCToolbox5::NodeId m_unionTypeId;
	SoftingOPCToolbox5::NodeId m_unionNestedTypeId;
};

#endif	// STRUCTUREDDATATYPEMODULE_H
