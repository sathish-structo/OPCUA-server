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
#ifndef _MY_REFRIGERATOR_STATE_VARIABLE_
#define _MY_REFRIGERATOR_STATE_VARIABLE_

#include <list>
#include "Util_Sync.h"
#include "LocalizedText.h"
#include "QualifiedName.h"
#include "ServerVariable.h"
#include "TestVariable.h"

class MyRefrigeratorStateVariable: public TestVariableForAuditEvents
{
public:
	MyRefrigeratorStateVariable();
	~MyRefrigeratorStateVariable();

	static SoftingOPCToolbox5::ObjectPointer<MyRefrigeratorStateVariable> create();

	virtual EnumStatusCode init(const SoftingOPCToolbox5::INodeId* typeId);
	virtual void onDataValueChanged(const SoftingOPCToolbox5::IDataValue* pNewDataValue);

	/*! The NodeId of the event type, that shall be reported */
	void setEventTypeToReport(const SoftingOPCToolbox5::INodeId& eventTypeIdToReport);
	/*! Set the NodeId of the node, where the event shall be reported, whenever the DataValue has changed */
	void setReportingNode(const SoftingOPCToolbox5::INodeId& nodeIdToReport);
protected:
	SoftingOPCToolbox5::NodeId m_reportingNodeId;
	SoftingOPCToolbox5::NodeId m_eventTypeIdToReport;
	SoftingOPCToolbox5::DataValue m_lastValue;
};

typedef SoftingOPCToolbox5::ObjectPointer<MyRefrigeratorStateVariable> MyRefrigeratorStateVariablePtr;
typedef SoftingOPCToolbox5::ObjectPointer<const MyRefrigeratorStateVariable> MyRefrigeratorStateVariableConstPtr;

#endif //_MY_REFRIGERATOR_STATE_VARIABLE_
