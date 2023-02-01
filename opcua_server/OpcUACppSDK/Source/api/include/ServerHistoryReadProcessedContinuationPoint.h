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

#ifndef SERVERHISTORYREADPROCESSEDCONTINUATIONPOINT_H
#define SERVERHISTORYREADPROCESSEDCONTINUATIONPOINT_H
#if TB5_HISTORY

#include "Base.h"
#include "ServerContinuationPoint.h"
#include "InnerDateTime.h"
#include "InnerAggregateConfiguration.h"
#include "InnerNodeId.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		/*! A continuation point for HistoryReadProcessed requests.
		*
		* @see ContinuationPoint HistoryReadProcessedTransaction HistoryReadProcessedRequest */
		class TBC_EXPORT HistoryReadProcessedContinuationPoint
			: public ContinuationPoint
		{
		public:
			/*! Constructs a new HistoryReadProcessedContinuationPoint instance */
			static ObjectPointer<HistoryReadProcessedContinuationPoint> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			HistoryReadProcessedContinuationPoint(TBHandle otbHandle);

		protected:
			/*! Default constructor */
			HistoryReadProcessedContinuationPoint();

		public:
			/*! Destructor */
			virtual ~HistoryReadProcessedContinuationPoint();

			/*! Returns the type of this continuation point (HistoryReadProcessed).
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

			/*! @deprecated Please store custom data to a custom derived HistoryReadProcessedContinuationPoint class and
			* ensure to free the custom data via the destructor.
			*
			* Interprets and returns the context information as ByteString.
			*
			* @return The context information interpreted as ByteString */
			DEPRECATED(ByteString getContextAsByteString() const);
			/*! @deprecated Please store custom data to a custom derived HistoryReadProcessed ContinuationPoint class and
			* ensure to free the custom data via the destructor.
			*
			* Stores the provided @p byteString as context information for this
			* continuation point.
			*
			* Any previously stored context information is deleted. */
			DEPRECATED(EnumStatusCode setContextFromByteString(const ByteString& byteString));

			/*! Returns the beginning of the period for which to calculate aggregated historical values
			*
			* If the client does not provide a start time, this method returns the NULL time (@see IDateTime::isNull)
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadProcessedRequest
			* @see Server::HistoryReadProcessedRequest::setContinuationPoint() */
			const IDateTime* getStartTime() const;

			/*! Returns the end of the period for which to calculate aggregated historical values
			*
			* If the client does not provide an end time, this method returns the NULL time (@see IDateTime::isNull)
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadProcessedRequest
			* @see Server::HistoryReadProcessedRequest::setContinuationPoint() */
			const IDateTime* getEndTime() const;

			/*! Returns the processing interval (in ms) that divides the total range |StartTime - EndTime|
			* into sub-ranges of the given length. The server shall return aggregate values for each
			* sub-range. If the processing interval is zero, only one aggregate value shall be calculated for
			* the total time range |StartTime - EndTime|.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadProcessedRequest
			* @see Server::HistoryReadProcessedRequest::setContinuationPoint() */
			double getProcessingInterval() const;

			/*! Returns the configuration that is to be used for calculating aggregate values.
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadProcessedRequest
			* @see IAggregateConfiguration, Server::HistoryReadProcessedRequest::setContinuationPoint() */
			const IAggregateConfiguration* getAggregateConfiguration() const;

			/*! Returns the NodeId of the aggregate that is to be calculated.
			*
			* The OPC UA specification defines the following standard aggregates:
			* @li EnumNumericNodeId_AggregateFunction_Average
			* @li EnumNumericNodeId_AggregateFunction_TimeAverage
			* @li EnumNumericNodeId_AggregateFunction_Total
			* @li EnumNumericNodeId_AggregateFunction_Minimum
			* @li EnumNumericNodeId_AggregateFunction_Maximum
			* @li EnumNumericNodeId_AggregateFunction_MinimumActualTime
			* @li EnumNumericNodeId_AggregateFunction_MaximumActualTime
			* @li EnumNumericNodeId_AggregateFunction_Range
			* @li EnumNumericNodeId_AggregateFunction_AnnotationCount
			* @li EnumNumericNodeId_AggregateFunction_Count
			* @li EnumNumericNodeId_AggregateFunction_NumberOfTransitions
			* @li EnumNumericNodeId_AggregateFunction_Start
			* @li EnumNumericNodeId_AggregateFunction_End
			* @li EnumNumericNodeId_AggregateFunction_Delta
			* @li EnumNumericNodeId_AggregateFunction_DurationGood
			* @li EnumNumericNodeId_AggregateFunction_DurationBad
			* @li EnumNumericNodeId_AggregateFunction_PercentGood
			* @li EnumNumericNodeId_AggregateFunction_PercentBad
			* @li EnumNumericNodeId_AggregateFunction_WorstQuality
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadProcessedRequest
			* @see Server::HistoryReadProcessedRequest::setContinuationPoint() */
			const INodeId* getAggregateType() const;

			/*! Returns the timestamp requested by the client: 'ServerTimestamp', 'SourceTimestamp' or 'Both'
			* ('Neither' is not allowed for history read processed requests).
			*
			* @note	Calling this getter is only valid after this ContinuationPoint was assigned to a HistoryReadProcessedRequest
			* @see Server::HistoryReadProcessedRequest::setContinuationPoint() */
			EnumTimestampsToReturn getTimestampsToReturn() const;

		private:
			/*! For internal use only */
			void initMembers();

			InnerDateTime m_startTime, m_endTime;
			InnerAggregateConfiguration m_aggregateConfiguration;
			InnerNodeId m_aggregateType;

			/*! Forbid use assignment operator */
			HistoryReadProcessedContinuationPoint& operator=(const HistoryReadProcessedContinuationPoint&);
			/*! Forbid use of copy constructor */
			HistoryReadProcessedContinuationPoint(const HistoryReadProcessedContinuationPoint&);
		};
		typedef ObjectPointer<Server::HistoryReadProcessedContinuationPoint> HistoryReadProcessedContinuationPointPtr;
		typedef ObjectPointer<const Server::HistoryReadProcessedContinuationPoint> HistoryReadProcessedContinuationPointConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif

#endif	// SERVERHISTORYREADPROCESSEDCONTINUATIONPOINT_H
