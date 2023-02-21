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
#include "StructoPrinter.h"
#include "ServerBaseEvent.h"
#include "ServerStateMachine.h"
#include "ApplicationModule.h"
#include "ClientSession.h"
#include "Application.h"
#include "ServerTwoStateVariable.h"
#include "Argument.h"
#include "Statics.h"
//#include "MyLimitAlarm.h"
//#include "MyExclusiveLimitAlarm.h"
//#include "MyNonExclusiveLimitAlarm.h"
//#include "MeasureConditionBranchPerformanceMethod.h"
#include "TestMethod.h"
#include "ServerAnalogItem.h"
#include "Util_SingleLock.h"
#include "ServerExclusiveLimitStateMachine.h"
#include "ServerState.h"
#include "ServerTransition.h"
#include "Constants.h"
using namespace SoftingOPCToolbox5;
PrinterModule::PrinterModule()
{
}

EnumStatusCode PrinterModule::initModule(const tstring& name, OTUInt16 namespaceIndex, SoftingOPCToolbox5::Server::ObjectPtr parentFolder)
{
	EnumStatusCode result = ModuleFolder::initModule(name, namespaceIndex, parentFolder);
	if(StatusCode::isBad(result))
	{
		return result;
	}

	DateTime datetime;
	DataValue value;

	p_monitoredNotificationArea = new EventSourceObject();
	p_monitoredNotificationArea->initialize(this,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea")),
		LocalizedText(_T("MonitoredNotificationArea"),_T("en")),
		QualifiedName(_T("MonitoredNotificationArea"), 0));
	result = this->addReference(Statics::ReferenceTypeId_HasNotifier, p_monitoredNotificationArea);

	p_motorArea = new EventSourceObject();
	p_motorArea->initialize(this,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.MotorArea")),
		LocalizedText(_T("MotorArea"),_T("en")),
		QualifiedName(_T("MotorArea"), 0));
	result = this->addReference(Statics::ReferenceTypeId_HasNotifier, p_motorArea);

	datetime.utcNow();
	value.setSourceTimestamp(datetime);
	value.setServerTimestamp(datetime);
	value.setStatusCode(EnumStatusCode_Good);

	// variable for printer
	p_status = new CustomVariable();
	p_status->init(p_monitoredNotificationArea,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.Status")),
		LocalizedText(_T("Status"),_T("en")),
		QualifiedName(_T("Status"), 0));
	p_status->insertTree();
	p_status->addReference(Statics::ReferenceTypeId_HasCondition, p_monitoredNotificationArea);
	p_status->setDataType(Statics::DataTypeId_Double);
	p_status->setAccessLevel(EnumAccessLevel_CurrentWrite|EnumAccessLevel_CurrentRead);
	value.setValue(Value((double)91.25)); // initially active in high limit
	p_status->setDataValue(value);
	
		// variable for printer
	p_tankStatus = new CustomVariable();
	p_tankStatus->init(p_monitoredNotificationArea,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.TankStatus")),
		LocalizedText(_T("Tank Status"),_T("en")),
		QualifiedName(_T("Tank Status"), 0));
	p_tankStatus->insertTree();
	p_tankStatus->addReference(Statics::ReferenceTypeId_HasCondition, p_monitoredNotificationArea);
	p_tankStatus->setDataType(Statics::DataTypeId_Boolean);
	p_tankStatus->setAccessLevel(EnumAccessLevel_CurrentWrite|EnumAccessLevel_CurrentRead);
	value.setValue(Value(1)); // initially active in high limit
	p_tankStatus->setDataValue(value);

	// variable for printer
	p_pallatStatus = new CustomVariable();
	p_pallatStatus->init(p_monitoredNotificationArea,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.PalletStatus")),
		LocalizedText(_T("Pallet Status"),_T("en")),
		QualifiedName(_T("Pallet Status"), 0));
	p_pallatStatus->insertTree();
	p_pallatStatus->addReference(Statics::ReferenceTypeId_HasCondition, p_monitoredNotificationArea);
	p_pallatStatus->setDataType(Statics::DataTypeId_Int32);
	p_pallatStatus->setAccessLevel(EnumAccessLevel_CurrentWrite|EnumAccessLevel_CurrentRead);
	value.setValue(Value(12)); // initially active in high limit
	p_pallatStatus->setDataValue(value);

	// variable for printer
	p_pallatStatus = new CustomVariable();
	p_pallatStatus->init(p_monitoredNotificationArea,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.MonitoredNotificationArea.FilePath")),
		LocalizedText(_T("File Location"),_T("en")),
		QualifiedName(_T("File Location"), 0));
	p_pallatStatus->insertTree();
	p_pallatStatus->addReference(Statics::ReferenceTypeId_HasCondition, p_monitoredNotificationArea);
	p_pallatStatus->setDataType(Statics::DataTypeId_Int32);
	p_pallatStatus->setAccessLevel(EnumAccessLevel_CurrentWrite|EnumAccessLevel_CurrentRead);
	value.setValue(Value(12)); // initially active in high limit
	p_pallatStatus->setDataValue(value);


	// variable for printer
	p_distance = new CustomVariable();
	p_distance->init(p_motorArea,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.MotorArea.Distance")),
		LocalizedText(_T("Distance"),_T("en")),
		QualifiedName(_T("Distance"), 0));
	p_distance->insertTree();
	p_distance->addReference(Statics::ReferenceTypeId_HasCondition, p_motorArea);
	p_distance->setDataType(Statics::DataTypeId_Double);
	p_distance->setAccessLevel(EnumAccessLevel_CurrentWrite|EnumAccessLevel_CurrentRead);
	value.setValue(Value((double)91.25)); // initially active in high limit
	p_distance->setDataValue(value);

	// variable for printer
	p_accleration = new CustomVariable();
	p_accleration->init(p_motorArea,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.MotorArea.Accleration")),
		LocalizedText(_T("Accleration"),_T("en")),
		QualifiedName(_T("Accleration"), 0));
	p_accleration->insertTree();
	p_accleration->addReference(Statics::ReferenceTypeId_HasCondition, p_motorArea);
	p_accleration->setDataType(Statics::DataTypeId_Int32);
	p_accleration->setAccessLevel(EnumAccessLevel_CurrentWrite|EnumAccessLevel_CurrentRead);
	value.setValue(Value(10)); // initially active in high limit
	p_accleration->setDataValue(value);

	// variable for printer
	p_velocity = new CustomVariable();
	p_velocity->init(p_motorArea,
		NodeId(namespaceIndex, _T("StructoAutomoldServer.MotorArea.Velocity")),
		LocalizedText(_T("Velocity"),_T("en")),
		QualifiedName(_T("Velocity"), 0));
	p_velocity->insertTree();
	p_velocity->addReference(Statics::ReferenceTypeId_HasCondition, p_motorArea);
	p_velocity->setDataType(Statics::DataTypeId_Int32);
	p_velocity->setAccessLevel(EnumAccessLevel_CurrentWrite|EnumAccessLevel_CurrentRead);
	value.setValue(Value(14)); // initially active in high limit
	p_velocity->setDataValue(value);

	pthread_create(&serverAutoUpdate,NULL,&PrinterModule::serverUpdateVariables,this);
	_tprintf(_T("Server Data Thread Join\n"));

	return EnumStatusCode_Good;
}

/*! @brief Printer module is ended */
void PrinterModule::endModule()
{
	p_monitoredNotificationArea->removeTree();
	ModuleFolder::endModule();
}

void PrinterModule::Update()
{
    DateTime datetime;
    datetime.utcNow();
    DataValue value;
}

void* PrinterModule::UpdateData()
{
	double data_val = 0;
	while(1)
	{	
		if (data_val < 90)
		{
			data_val = 95;
		}
		if (data_val >= 115)
		{
			data_val = 95;
		}
		else {
			data_val = data_val+0.5;
		}

#if 1
		DataValue value;
		value.setValue(Value((double)data_val)); // initially active in high limit
		p_distance->setDataValue(value);
#endif
		sleep(1);
	}
}
#endif
