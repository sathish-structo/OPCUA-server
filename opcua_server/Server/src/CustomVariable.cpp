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
#include "CustomVariable.h"
#include "Statics.h"

/**
 * @brief Constructor
 */
CustomVariable::CustomVariable()
{
}

/**
 * @brief Destructor
 */
CustomVariable::~CustomVariable()
{
}

void CustomVariable::init(Server::Object* parent,
									const INodeId& nodeId,
									const ILocalizedText& displayName,
									const IQualifiedName& browseName)
{
	m_parent = parent;

	setBrowseName(browseName);
	setDisplayName(displayName);
	setNodeId(nodeId);
	setVariableType(Statics::VariableTypeId_BaseDataVariableType);

	m_parent->addReference(Statics::ReferenceTypeId_HasComponent, this);
}

EnumStatusCode CustomVariable::init(const INodeId* typeId)
{
	return Server::Variable::init(typeId);
}
