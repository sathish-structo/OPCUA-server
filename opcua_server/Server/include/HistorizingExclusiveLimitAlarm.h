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
#ifndef _HISTORIZINGEXCLUSIVELIMITALARM_H_
#define _HISTORIZINGEXCLUSIVELIMITALARM_H_
#if TB5_HISTORY

#include "ServerExclusiveLimitAlarm.h"
#include "ServerHistoryReadEventTransaction.h"
#include "ServerHistoryReadEventRequest.h"
#include "HistoricalEvent.h"

#include <list>
#include "Util_Sync.h"

using namespace SoftingOPCToolbox5;

/*! An exclusive limit alarm that provides historical events */
class HistorizingExclusiveLimitAlarm : public Server::ExclusiveLimitAlarm
{
public:
	HistorizingExclusiveLimitAlarm();

	/*! Adds a historical event which can be retrieved using HistoryRead */
	void addHistoricalEvent(const HistoricalEvent& historicalEvent);

	/*! @see Object::handleHistoryReadEventRequest() */
	virtual EnumStatusCode handleHistoryReadEventRequest(Server::HistoryReadEventTransaction* transaction, Server::HistoryReadEventRequest* request);
private:
	std::list<HistoricalEvent> m_historicalEvents;
	mutable Util_Sync m_sync;

	EnumStatusCode processRequest(DateTime startTime, DateTime endTime,
		const EventFilter& filter, OTUInt32 maxNumberOfValuesPerNode, EnumTimestampsToReturn timestampsToReturn,
		std::vector<HistoryEventFieldList>& historicalEvents, DateTime& nextStartTime);
};

#endif
#endif
