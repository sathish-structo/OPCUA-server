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
#if TB5_ALARMS
#include "TestEvent.h"
#include "ServerObjectType.h"
#include "ServerAddressSpaceRoot.h"
#include "Statics.h"
#include "TestEventType.h"
#include "ServerVariable.h"

using namespace Server;


TestEvent::TestEvent()
{

}

SoftingOPCToolbox5::ObjectPointer<TestEvent> TestEvent::create()
{
	return new TestEvent();
}

TestEvent::~TestEvent()
{

}

EnumStatusCode TestEvent::init(const INodeId* typeId)
{
	if(TestEventType::s_typeId == NULL)
	{
		return EnumStatusCode_BadInvalidState;
	}
	EnumStatusCode result = BaseEvent::init((typeId == NULL) ? TestEventType::s_typeId : typeId);

	DataValue dv;
	VariablePtr propertyVar;
	propertyVar = getVariable(*TestEventType::s_propertyBrowseName);
	dv.setValue(Value(_T("TestPropVal")));
	propertyVar->setDataValue(dv);

	propertyVar = getVariable(*TestEventType::s_userSpecificPropertyBrowseName);
	dv.setValue(Value(_T("TestUserPropVal")));
	propertyVar->setDataValue(dv);
	return result;
}

#endif

