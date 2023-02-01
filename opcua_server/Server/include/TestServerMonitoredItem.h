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
#ifndef _TEST_SERVER_MONITORED_ITEM_H_
#define _TEST_SERVER_MONITORED_ITEM_H_

#include "ServerMonitoredItem.h"

class TestServerMonitoredItem : public SoftingOPCToolbox5::Server::MonitoredItem
{
public:
	TestServerMonitoredItem(TBHandle otbHandle);
	virtual ~TestServerMonitoredItem();

	virtual void onSetTriggering(const std::vector<SoftingOPCToolbox5::Server::MonitoredItem*>& addedLinks,
		const std::vector<SoftingOPCToolbox5::Server::MonitoredItem*>& removedLinks);

	OTUInt32 m_testId; // an ID just to validate that the same API MI is kept alive
};

#endif // _TEST_SERVER_MONITORED_ITEM_H_
