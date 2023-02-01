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
#ifndef _CUSTOMVARIABLE_H_
#define _CUSTOMVARIABLE_H_

#include "ServerObject.h"
#include "TestVariable.h"
#include "ClientSession.h"
#include "Application.h"

using namespace SoftingOPCToolbox5;

/**
 * A custom variable class
 */
class CustomVariable
	: public TestVariableForAuditEvents
{
public:

	/*! @brief Constructor */
	CustomVariable();

	/*! @brief Destructor */
	~CustomVariable();

	virtual EnumStatusCode init(const INodeId* typeId);
	void init(Server::Object* parent,
		const INodeId& nodeId,
		const ILocalizedText& displayName,
		const IQualifiedName& browseName);

private:
	Server::ObjectPtr m_parent;
};

typedef ObjectPointer<CustomVariable> CustomVariablePtr;
typedef ObjectPointer<const CustomVariable> CustomVariableConstPtr;

#endif
