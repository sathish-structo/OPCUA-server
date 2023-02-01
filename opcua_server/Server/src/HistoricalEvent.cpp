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
#include "HistoricalEvent.h"
#include "Constants.h"
#include "DateTime.h"

//////////////////////////////////////////////////////////////////////////
// HistoricalEvent::HistoricalEventRecord implementation
//////////////////////////////////////////////////////////////////////////

class HistoricalEvent::HistoricalEventKey
{
public:
	HistoricalEventKey(const std::vector<QualifiedName>& browsePath, EnumAttributeId attributeId);

	bool operator<(const HistoricalEventKey& other) const;
	bool operator==(const HistoricalEventKey& other) const;
	bool operator!=(const HistoricalEventKey& other) const;

private:
	std::vector<QualifiedName> m_browsePath;
	EnumAttributeId m_attributeId;

	int compare(const HistoricalEventKey& other) const;
};

HistoricalEvent::HistoricalEventKey::HistoricalEventKey(const std::vector<QualifiedName>& browsePath, EnumAttributeId attributeId)
{
	m_attributeId = attributeId;
	m_browsePath.assign(browsePath.begin(), browsePath.end());
}

bool HistoricalEvent::HistoricalEventKey::operator<(const HistoricalEventKey& other) const
{
	return compare(other) > 0;
}

bool HistoricalEvent::HistoricalEventKey::operator==(const HistoricalEventKey& other) const
{
	return compare(other) == 0;
}

bool HistoricalEvent::HistoricalEventKey::operator!=(const HistoricalEventKey& other) const
{
	return compare(other) != 0;
}

int HistoricalEvent::HistoricalEventKey::compare(const HistoricalEventKey& other) const
{
	int ret = 0;

	if(m_attributeId < other.m_attributeId)
	{
		ret = -1;
	}
	else if(m_attributeId > other.m_attributeId)
	{
		ret = 1;
	}

	if(ret == 0)
	{
		size_t browsePathSize = m_browsePath.size(), otherBrowsePathSize = other.m_browsePath.size();
		if(browsePathSize < otherBrowsePathSize)
		{
			ret = -1;
		}
		else if(browsePathSize > otherBrowsePathSize)
		{
			ret = 1;
		}
		else
		{
			for(size_t i = 0; i < browsePathSize && ret == 0; i++)
			{
				ret = m_browsePath[i].compare(other.m_browsePath[i]);
			}
		}
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////
// HistoricalEvent::HistoricalEventRecord implementation
//////////////////////////////////////////////////////////////////////////

struct HistoricalEvent::HistoricalEventRecord
{
	EnumAttributeId attributeId;
	Value value;
};

//////////////////////////////////////////////////////////////////////////
// HistoricalEvent::HistoricalEventPrivate implementation
//////////////////////////////////////////////////////////////////////////

class HistoricalEvent::HistoricalEventPrivate
{
public:
	std::map<HistoricalEventKey, Value> eventFields;
};

//////////////////////////////////////////////////////////////////////////
// HistoricalEvent::HistoricalEvent implementation
//////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#if _MSC_VER==1400
// default constructor only needed with VS2005
HistoricalEvent::HistoricalEvent() : d(new HistoricalEventPrivate())
{
}
#endif
#endif

HistoricalEvent::HistoricalEvent(const NodeId& eventTypeId, const DateTime& time)
	: d(new HistoricalEventPrivate())
{
	// EventType
	Value eventTypeValue;
	eventTypeValue.setNodeId(&eventTypeId);
	addEventField(QualifiedName(BrowseName_EventType, 0), EnumAttributeId_Value, eventTypeValue);

	// Time
	Value timeValue;
	timeValue.setDateTime(&time);
	addEventField(QualifiedName(BrowseName_Time, 0), EnumAttributeId_Value, timeValue);
}

HistoricalEvent::HistoricalEvent(const HistoricalEvent& other)
	: d(new HistoricalEventPrivate())
{
	*d = *(other.d);
}

HistoricalEvent::~HistoricalEvent()
{
	delete d;
}


void HistoricalEvent::addEventField(const std::vector<QualifiedName>& browsePath, EnumAttributeId attributeId, const Value& value)
{
	d->eventFields[HistoricalEventKey(browsePath, attributeId)] = value;
}

void HistoricalEvent::addEventField(const QualifiedName& browsePath, EnumAttributeId attributeId, const Value& value)
{
	std::vector<QualifiedName> browsePathVector;
	browsePathVector.push_back(browsePath);

	addEventField(browsePathVector, attributeId, value);
}

EnumStatusCode HistoricalEvent::getTypeDefinitionId(NodeId& typeDefinitionId)
{
	std::vector<QualifiedName> browsePath;
	browsePath.push_back(QualifiedName(BrowseName_EventType, 0));

	Value value;
	EnumStatusCode result = getAttributeValue(browsePath, EnumAttributeId_Value, value);
	_ASSERTION(StatusCode::isGood(result), _T("getAttributeValue() failed"));
	typeDefinitionId = value.getNodeId();

	return result;
}

EnumStatusCode HistoricalEvent::getAttributeValue(const std::vector<QualifiedName>& browsePath, EnumAttributeId attributeId, Value& attributeValue)
{
	const HistoricalEvent* pConstThis = const_cast<const HistoricalEvent*>(this);
	return pConstThis->getAttributeValue(browsePath, attributeId, attributeValue);
}

EnumStatusCode HistoricalEvent::getAttributeValue(const std::vector<QualifiedName>& browsePath, EnumAttributeId attributeId, Value& attributeValue) const
{
	// return the attribute value if available, else return null
	HistoricalEventKey key(browsePath, attributeId);
	std::map<HistoricalEventKey, Value>::const_iterator it = d->eventFields.find(key);
	if(it != d->eventFields.end())
	{
		attributeValue = it->second;
	}
	else
	{
		attributeValue.setStatusCode(EnumStatusCode_BadNoData);
	}

	return EnumStatusCode_Good;
}

bool HistoricalEvent::operator<(const HistoricalEvent& other) const
{
	return (getTime().compare(other.getTime()) < 0);
}

SoftingOPCToolbox5::DateTime HistoricalEvent::getTime() const
{
	std::vector<QualifiedName> browsePath;
	browsePath.push_back(QualifiedName(BrowseName_Time, 0));

	Value value;
	EnumStatusCode result = getAttributeValue(browsePath, EnumAttributeId_Value, value);
	_UNUSED(result);
	_ASSERTION(StatusCode::isGood(result), _T("getAttributeValue() failed"));
	return value.getDateTime();
}
