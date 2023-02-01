/*****************************************************************************
*                                                                            *
*                     SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                          *
*                                D-85540 Haar                                *
*                        Phone: ++49-89-4 56 56-0                            *
*                          Fax: ++49-89-4 56 56-3 99                         *
*                                                                            *
*                            SOFTING CONFIDENTIAL                            *
*                                                                            *
*                     Copyright (C) SOFTING IA GmbH 2022                     *
*                             All Rights Reserved                            *
*                                                                            *
* NOTICE: All information contained herein is, and remains the property of   *
*   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- *
*   lectual and technical concepts contained herein are proprietary to       *
*   SOFTING Industrial Automation GmbH and its suppliers and may be covered  *
*   by German and Foreign Patents, patents in process, and are protected by  *
*   trade secret or copyright law. Dissemination of this information or      *
*   reproduction of this material is strictly forbidden unless prior         *
*   written permission is obtained from SOFTING Industrial Automation GmbH.  *
******************************************************************************
******************************************************************************
*                                                                            *
* PROJECT_NAME             OPC UA C++ Server & Client SDK                    *
*                                                                            *
* VERSION                  6.00.0                                            *
*                                                                            *
* DATE                     13.6.2022                                            *
*                                                                            *
*****************************************************************************/

#ifndef SERVERHISTORYREADEVENTCONTINUATIONPOINT_H
#define SERVERHISTORYREADEVENTCONTINUATIONPOINT_H
#if TB5_HISTORY

#include "Base.h"
#include "ServerContinuationPoint.h"
#include "DateTime.h"
#include "InnerDateTime.h"
#include "EventFilter.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		/*! A continuation point for HistoryReadEvent requests.
		*
		* @see ContinuationPoint HistoryReadEventTransaction HistoryReadEventRequest */
		class TBC_EXPORT HistoryReadEventContinuationPoint
			: public ContinuationPoint
		{
		public:
			/*! Constructs a new HistoryReadEventContinuationPoint instance */
			static ObjectPointer<HistoryReadEventContinuationPoint> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			HistoryReadEventContinuationPoint(TBHandle otbHandle);

		protected:
			/*! Default constructor */
			HistoryReadEventContinuationPoint();

		public:
			/*! Destructor */
			virtual ~HistoryReadEventContinuationPoint();

			/*! Returns the type of this continuation point (HistoryReadEvent).
			*
			* @return The continuation point type */
			virtual EnumContinuationPointType getType() const;

			/*! A proposed interface to store context information to be able to continue an incomplete request in a follow up request.
			*
			* It is proposed to store the timestamp of the next HistoricalEvent where the request shall be continued.
			*
			* @note This method isn't used by the SDK and is just a proposal and provides a usable default implementation.
			*		To store additional or different context information for this ContinuationPoint, create
			*		and use a derived HistoryReadEventContinuationPoint which holds the additional data as additional members. */
			EnumStatusCode setContextFromTimestamp(const IDateTime* timestamp);
			/*! @overload */
			EnumStatusCode setContextFromTimestamp(const DateTime& timestamp);
			/*! Returns the context information which was previously stored via setContextFromTimestamp(). */
			DateTime getContextAsTimestamp() const;

			/*! @deprecated Please store custom data to a custom derived HistoryReadEventContinuationPoint class and
			* ensure to free the custom data via the destructor.
			*
			* Interprets and returns the context information as ByteString.
			*
			* @return The context information interpreted as ByteString */
			DEPRECATED(ByteString getContextAsByteString() const);
			/*! @deprecated Please store custom data to a custom derived HistoryReadEventContinuationPoint class and
			* ensure to free the custom data via the destructor.
			*
			* Stores the provided @p byteString as context information for this
			* continuation point.
			*
			* Any previously stored context information is deleted. */
			DEPRECATED(EnumStatusCode setContextFromByteString(const ByteString& byteString));

			/*! Returns the beginning of the period to read. If the start time equals the minimum value of DateTime,
			* no specific start time is specified.
			*
			* @note	The client must have specified at least two of the three parameters StartTime, EndTime and MaxNumberOfValuesPerNode.
			*		This precondition is already verified by the SDK.
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadEventRequest
			* @see Server::HistoryReadEventRequest::setContinuationPoint() */
			const IDateTime* getStartTime() const;

			/*! Returns the end of the period to read. If the start time equals the minimum value of DateTime,
			* no specific end time is specified.
			*
			* @note	The client must have specified at least two of the three parameters StartTime, EndTime and MaxNumberOfValuesPerNode.
			*		This precondition is already verified by the SDK.
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadEventRequest
			* @see Server::HistoryReadEventRequest::setContinuationPoint() */
			const IDateTime* getEndTime() const;

			/*! Returns the maximum number of historical values to return for every
			* node in a request.
			*
			* If the available number of values exceeds the provided maximum, the
			* server application may create a continuation point to allow the
			* client to fetch more values.
			*
			* @note	The client must have specified at least two of the three parameters StartTime, EndTime and MaxNumberOfValuesPerNode.
			*		This precondition is already verified by the SDK.
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadEventRequest
			* @see Server::HistoryReadEventRequest::setContinuationPoint() */
			OTUInt32 getMaxNumberOfValuesPerNode() const;

			/*! Returns the filter that shall be used by the server application to determine which historical events to include in
			* the response and which event fields to return. This parameter must be specified and at least one EventField must be selected.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadEventRequest
			* @see EventFilterEvaluator, Server::HistoryReadEventRequest::setContinuationPoint() */
			EnumStatusCode getFilter(EventFilter& filter) const;

			/*! Returns which timestamps to return for DataValues in event fields.
			* This parameter can be ignored for event fields that are not of type DataValue.
			*
			* @note This parameter applies only to event fields that are of type DataValue.
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadEventRequest
			* @see Server::HistoryReadEventRequest::setContinuationPoint() */
			EnumTimestampsToReturn getTimestampsToReturn() const;

		private:
			/*! For internal use only */
			void initMembers();

			InnerDateTime m_startTime, m_endTime;

			/*! Forbid use assignment operator */
			HistoryReadEventContinuationPoint& operator=(const HistoryReadEventContinuationPoint&);
			/*! Forbid use of copy constructor */
			HistoryReadEventContinuationPoint(const HistoryReadEventContinuationPoint&);
		};
		typedef ObjectPointer<Server::HistoryReadEventContinuationPoint> HistoryReadEventContinuationPointPtr;
		typedef ObjectPointer<const Server::HistoryReadEventContinuationPoint> HistoryReadEventContinuationPointConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif
#endif	// SERVERHISTORYREADEVENTCONTINUATIONPOINT_H
