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

#ifndef SERVERHISTORYREADRAWCONTINUATIONPOINT_H
#define SERVERHISTORYREADRAWCONTINUATIONPOINT_H
#if TB5_HISTORY

#include "Base.h"
#include "ServerContinuationPoint.h"
#include "DateTime.h"
#include "InnerDateTime.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		/*! A continuation point for HistoryReadRaw requests.
		*
		* @see ContinuationPoint HistoryReadRawTransaction HistoryReadRawRequest */
		class TBC_EXPORT HistoryReadRawContinuationPoint
			: public ContinuationPoint
		{
		public:
			/*! Constructs a new HistoryReadRawContinuationPoint instance */
			static ObjectPointer<HistoryReadRawContinuationPoint> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			HistoryReadRawContinuationPoint(TBHandle otbHandle);

		protected:
			/*! Default constructor */
			HistoryReadRawContinuationPoint();

		public:
			/*! Destructor */
			virtual ~HistoryReadRawContinuationPoint();

			/*! Returns the type of this continuation point (HistoryReadRaw).
			*
			* @return The continuation point type */
			virtual EnumContinuationPointType getType() const;

			/*! A proposed interface to store context information to be able to continue an incomplete request in a follow up request.
			*
			* It is proposed to store the timestamp of the next HistoricalEvent where the request shall be continued.
			*
			* @note This method isn't used by the SDK and is just a proposal and provides a usable default implementation.
			*		To store additional or different context information for this ContinuationPoint, create
			*		and use a derived HistoryReadRawContinuationPoint which holds the additional data as additional members. */
			EnumStatusCode setContextFromTimestamp(const IDateTime* timestamp);
			/*! @overload */
			EnumStatusCode setContextFromTimestamp(const DateTime& timestamp);
			/*! Returns the context information which was previously stored via setContextFromTimestamp(). */
			DateTime getContextAsTimestamp() const;

			/*! @deprecated Please store custom data to a custom derived HistoryReadRawContinuationPoint class and
			* ensure to free the custom data via the destructor.
			*
			* Interprets and returns the context information as ByteString.
			*
			* @return The context information interpreted as ByteString */
			DEPRECATED(ByteString getContextAsByteString() const);
			/*! @deprecated Please store custom data to a custom derived HistoryReadRawContinuationPoint class and
			* ensure to free the custom data via the destructor.
			*
			* Stores the provided @p byteString as context information for this
			* continuation point.
			*
			* Any previously stored context information is deleted. */
			DEPRECATED(EnumStatusCode setContextFromByteString(const ByteString& byteString));

			/*! Returns the beginning of the period for which to read historical values.
			*
			* If the client does not provide a start time, this method returns the NULL time (@see IDateTime::isNull)
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadRawRequest
			* @see Server::HistoryReadRawRequest::setContinuationPoint() */
			const IDateTime* getStartTime() const;

			/*! Returns the end of the period for which to read historical values.
			*
			* If the client does not provide an end time, this method returns the NULL time (@see IDateTime::isNull)
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadRawRequest
			* @see Server::HistoryReadRawRequest::setContinuationPoint() */
			const IDateTime* getEndTime() const;

			/*! Returns the maximum number of historical values to return for every
			* node in a request.
			*
			* If the available number of values exceeds the provided maximum, the
			* server application may create a continuation point to allow the
			* client to fetch more values.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadRawRequest
			* @see Server::HistoryReadRawRequest::setContinuationPoint() */
			OTUInt32 getMaxNumberOfValuesPerNode() const;

			/*! Returns whether bounding values (the first historical value stored before the provided
			* start time and the first historical value stored after the provided end time) are
			* to be included.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadRawRequest
			* @see Server::HistoryReadRawRequest::setContinuationPoint() */
			bool returnBoundingValues() const;

			/*! Returns the timestamps requested by the client: 'ServerTimestamp', 'SourceTimestamp' or 'Both'
			* ('Neither' is not allowed for history read raw requests).
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadRawRequest
			* @see Server::HistoryReadRawRequest::setContinuationPoint() */
			EnumTimestampsToReturn getTimestampsToReturn() const;

		private:
			/*! For internal use only */
			void initMembers();

			/*! Forbid use assignment operator */
			HistoryReadRawContinuationPoint& operator=(const HistoryReadRawContinuationPoint&);
			/*! Forbid use of copy constructor */
			HistoryReadRawContinuationPoint(const HistoryReadRawContinuationPoint&);

			InnerDateTime m_startTime, m_endTime;
		};
		typedef ObjectPointer<Server::HistoryReadRawContinuationPoint> HistoryReadRawContinuationPointPtr;
		typedef ObjectPointer<const Server::HistoryReadRawContinuationPoint> HistoryReadRawContinuationPointConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif

#endif	// SERVERHISTORYREADRAWCONTINUATIONPOINT_H
