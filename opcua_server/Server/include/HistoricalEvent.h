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
#ifndef _HISTORICAL_EVENT_RECORD_H_
#define _HISTORICAL_EVENT_RECORD_H_

#include "NodeId.h"
#include "QualifiedName.h"
#include "IEventFilterTarget.h"

using namespace SoftingOPCToolbox5;

class HistoricalEvent : public Server::IEventFilterTarget
{
public:
#ifdef _MSC_VER
#if _MSC_VER==1400
	// default constructor only needed with VS2005
	HistoricalEvent();
#endif
#endif
	/*! Constructs a new record of an historical event
	*
	* @param	eventTypeId		The NodeId of the event's TypeDefinitionNode
	* @param	time			The Time field of the historized event
	*/
	HistoricalEvent(const NodeId& eventTypeId, const DateTime& time);

	HistoricalEvent(const HistoricalEvent& other);
	virtual ~HistoricalEvent();

	/*! Adds an event field to the history */
	void addEventField(const std::vector<QualifiedName>& browsePath, EnumAttributeId attributeId, const Value& value);
	/*! @overload */
	void addEventField(const QualifiedName& browsePath, EnumAttributeId attributeId, const Value& value);

	/*! Returns the time specified in the event's Time field */
	DateTime getTime() const;

	/*! Historical events are ordered by their Time field */
	bool operator<(const HistoricalEvent& other) const;

	// IContentFilterTarget
	virtual EnumStatusCode getTypeDefinitionId(NodeId& typeDefinitionId);
	virtual EnumStatusCode getAttributeValue(const std::vector<QualifiedName>& browsePath, EnumAttributeId attributeId,
		Value& attributeValue);

	// Const version of getAttributeValue
	virtual EnumStatusCode getAttributeValue(const std::vector<QualifiedName>& browsePath, EnumAttributeId attributeId,
		Value& attributeValue) const;

private:
	class HistoricalEventKey;
	struct HistoricalEventRecord;
	class HistoricalEventPrivate;
	HistoricalEventPrivate* d;
};

#endif // _HISTORICAL_EVENT_RECORD_H_
