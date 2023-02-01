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
#include "TestServerMonitoredItem.h"

// #define TRACE_TRIGGERING_LINKS

using namespace SoftingOPCToolbox5;

static OTUInt32 TEST_MI_ID = 0;

TestServerMonitoredItem::TestServerMonitoredItem(TBHandle otbHandle)
	: Server::MonitoredItem(otbHandle)
{
	TEST_MI_ID++;
	if(TEST_MI_ID == 0)
	{
		TEST_MI_ID++;
	}
	m_testId = TEST_MI_ID;
}

TestServerMonitoredItem::~TestServerMonitoredItem()
{
}

void TestServerMonitoredItem::onSetTriggering(const std::vector<Server::MonitoredItem*>& addedLinks, const std::vector<Server::MonitoredItem*>& removedLinks)
{
	_UNUSED(addedLinks); _UNUSED(removedLinks);

#ifdef TRACE_TRIGGERING_LINKS
	size_t addedLinksSize = addedLinks.size(), removedLinksSize = removedLinks.size();
	_tprintf(_T("Changed triggering links for item %u.\n"), getId());
	_tprintf(_T("Added %u links\n"), addedLinksSize);
	for(size_t i = 0; i < addedLinksSize; i++)
	{
		_tprintf(_T("\t%u -> %u\n"), getId(), addedLinks[i]->getId());
	}
	_tprintf(_T("Removed %u links\n"), removedLinksSize);
	for(size_t i = 0; i < removedLinksSize; i++)
	{
		_tprintf(_T("\t%u -> %u\n"), getId(), removedLinks[i]->getId());
	}

	std::vector<Server::MonitoredItemPtr> currentLinks = getTriggeredMonitoredItems();
	size_t currentLinksSize = currentLinks.size();
	_tprintf(_T("Currently the item has established %u links.\n"), currentLinksSize);
	for(size_t i = 0; i < currentLinksSize; i++)
	{
		_tprintf(_T("\t%u -> %u\n"), getId(), currentLinks[i]->getId());
	}
#endif
}
