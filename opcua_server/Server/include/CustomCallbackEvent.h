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
#ifndef CUSTOM_CALLBACK_EVENT_H
#define CUSTOM_CALLBACK_EVENT_H
#if TB5_ALARMS

#include "ServerBaseEventNotification.h"

class CustomCallbackEvent : public SoftingOPCToolbox5::Server::BaseEventNotification
{
public:
	/*! Constructs a new BaseEvent instance */
	static SoftingOPCToolbox5::ObjectPointer<CustomCallbackEvent> create(tstring callbackName);

	/*! Destructor */
	virtual ~CustomCallbackEvent();

	tstring getCallbackName();
	static void initialize(OTUInt16 namespaceIndex);
	static void uninitialize();
protected:
	CustomCallbackEvent(tstring callbackName);

	EnumStatusCode setCallbackName(tstring callbackName);
private:
	static OTUInt16 c_namespaceIndex;
	static SoftingOPCToolbox5::NodeId* c_typeId;
};

typedef SoftingOPCToolbox5::ObjectPointer<CustomCallbackEvent> CustomCallbackEventPtr;
typedef SoftingOPCToolbox5::ObjectPointer<const CustomCallbackEvent> CustomCallbackEventConstPtr;

#endif
#endif //_CUSTOM_CALLBACK_EVENT_H_
