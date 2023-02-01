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
#include "CustomCallbackEvent.h"
#include "QualifiedName.h"
#include "LocalizedText.h"
#include "Application.h"
#include "Constants.h"
#include "ServerAddressSpaceRoot.h"
#include "ServerObjectType.h"
#include "Statics.h"
using namespace SoftingOPCToolbox5;

NodeId* CustomCallbackEvent::c_typeId;
OTUInt16 CustomCallbackEvent::c_namespaceIndex;

	CustomCallbackEvent::CustomCallbackEvent(tstring callbackName) : Server::BaseEventNotification(c_typeId)
	{
		setCallbackName(callbackName);
		setMessage(LocalizedText(_T("callback invocked"), _T("en")));
		setSeverity(1);
		setSourceName(_T("Server"));
		setSourceNode(NodeId(EnumNumericNodeId_Server));
		setEventType(c_typeId);
	}

	CustomCallbackEvent::~CustomCallbackEvent()
	{

	}

	EnumStatusCode CustomCallbackEvent::setCallbackName(tstring callbackName)
	{
		return setProperty(QualifiedName(_T("callbackName"), c_namespaceIndex), Value(callbackName));
	}

	void CustomCallbackEvent::initialize(OTUInt16 namespaceIndex)
	{
		_ASSERTION(c_typeId == NULL, _T("c_typeId not NULL"));
		c_namespaceIndex = namespaceIndex;
		c_typeId = new NodeId(namespaceIndex, _T("callbackEvent"));
		
		Server::ObjectTypePtr customCallbackEventObjectTypePtr = Server::ObjectType::create();
		customCallbackEventObjectTypePtr->setBrowseName(QualifiedName(_T("CustomCallbackEvent"), namespaceIndex));
		customCallbackEventObjectTypePtr->setNodeId(c_typeId);

		customCallbackEventObjectTypePtr->setDisplayName(LocalizedText(_T("CustomCallbackEvent"), _T("en")));
		customCallbackEventObjectTypePtr->setDescription(LocalizedText(_T("CustomCallbackEvent"), _T("en")));

		Server::VariablePtr callbackNamePropertyVariablePtr = Server::Variable::create();
		callbackNamePropertyVariablePtr->setBrowseName(QualifiedName(_T("callbackName"), namespaceIndex));
		callbackNamePropertyVariablePtr->setNodeId(NodeId(namespaceIndex, _T("callbackName")));

		callbackNamePropertyVariablePtr->setDisplayName(LocalizedText(_T("Callbackname"), _T("en")));
		callbackNamePropertyVariablePtr->setDescription(LocalizedText(_T("Callbackname"), _T("en")));
		callbackNamePropertyVariablePtr->setAccessLevel(EnumAccessLevel_CurrentRead);
		callbackNamePropertyVariablePtr->addReference(Statics::ReferenceTypeId_HasTypeDefinition, Server::AddressSpaceRoot::instance()->getNode(Statics::VariableTypeId_PropertyType));

		customCallbackEventObjectTypePtr->addReference(Statics::ReferenceTypeId_HasProperty, callbackNamePropertyVariablePtr);
		customCallbackEventObjectTypePtr->insertTree();
		Server::ObjectTypePtr baseEventType  = Server::AddressSpaceRoot::instance()->getObjectType(Statics::ObjectTypeId_BaseEventType);
		if(baseEventType.isNotNull())
		{
			baseEventType->addReference(Statics::ReferenceTypeId_HasSubtype, customCallbackEventObjectTypePtr);
		}
	}

	tstring CustomCallbackEvent::getCallbackName()
	{
		tstring callbackName;
		getProperty(QualifiedName(_T("callbackName"), c_namespaceIndex)).getString(callbackName);
		return callbackName;
	}

	SoftingOPCToolbox5::ObjectPointer<CustomCallbackEvent> CustomCallbackEvent::create(tstring callbackName)
	{
		return new CustomCallbackEvent(callbackName);
	}

	void CustomCallbackEvent::uninitialize()
	{
		Server::ObjectTypePtr baseEventType = Server::AddressSpaceRoot::instance()->getObjectType(c_typeId);
		if(baseEventType.isNotNull())
		{
			baseEventType->removeTree();
		}
		
		delete c_typeId;
		c_typeId = NULL;
	}
#endif
