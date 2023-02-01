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

#ifndef READRAWMODIFIEDDETAILS_H
#define READRAWMODIFIEDDETAILS_H
#if TB5_HISTORY

#include "ReadRawModifiedDetailsStruct.h"
#include "InnerDateTime.h"

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

namespace SoftingOPCToolbox5
{
	/*! The ReadRawDetails data type is used by the HistoryRead service (see Client::Session::historyReadRaw())
	* to specify details for the service call. */
	class TBC_EXPORT ReadRawDetails
	{
	public:
		/*! Default constructor */
		ReadRawDetails();
		/*! Constructs a copy of the given instance */
		ReadRawDetails(const ReadRawDetails*);
		/*! @overload */
		ReadRawDetails(const ReadRawDetails&);

		/*! Destructor */
		virtual ~ReadRawDetails();

		/*! Assigns the contents of @p cp to this instance */
		ReadRawDetails& operator=(const ReadRawDetails& cp);

		/*! Copies the content of @p source to this instance */
		EnumStatusCode set(const ReadRawDetails* cp);
		/*! @overload */
		EnumStatusCode set(const ReadRawDetails& cp);

		/*! Compares this instance to the given @p other instance.
		*
		* @li Returns <0, if "this < other"
		* @li Returns 0, if "this == other"
		* @li Returns >0, if "this > other" */
		virtual int compare(const ReadRawDetails* other) const;
		/*! @overload */
		virtual int compare(const ReadRawDetails& other) const;

		/*! Compares two instances for equality */
		virtual bool operator==(const ReadRawDetails& other) const;
		/*! Compares two instances for inequality */
		virtual bool operator!=(const ReadRawDetails& other) const;

		/*! Returns the beginning of the period to read.
		*
		* @see setStartTime() */
		const IDateTime* getStartTime() const;
		/*! Sets the beginning of the period to read.
		* Set the StartTime to the default value of DateTime if no specific StartTime is specified. */
		void setStartTime(const IDateTime* startTime);

		/*! Returns the end of period to read.
		*
		* @see setStartTime() */
		const IDateTime* getEndTime() const;
		/*! Sets the end of the period to read.
		* Set the EndTime to the default value of DateTime if no specific EndTime is specified. */
		void setEndTime(const IDateTime* startTime);

		/*! Returns the maximum number of events returned for any node over the time range.
		*
		* @see setMaxNumberOfValuesPerNode() */
		OTUInt32 getMaxNumberOfValuesPerNode() const;
		/*! Sets the maximum number of events returned for any node over the time range.
		* If only one time is specified, the time range must extend to return this number of values.
		* The default value of 0 indicates that there is no maximum. */
		void setMaxNumberOfValuesPerNode(OTUInt32 maxNumberOfValuesPerNode);

		/*! Returns true, if the server returns bounding values. */
		bool getReturnBounds() const;
		/*! Indicates whether the server shall return bounding values. */
		void setReturnBounds(bool returnBounds);

		/*! Clears the contents of this object to enable its re-use. */
		void clear();

		/*! For internal use only */
		void* getInternHandle() const;

	private:
		/*! For internal use only */
		void initMembers();

		InnerDateTime m_StartTime;
		InnerDateTime m_EndTime;

		ReadRawModifiedDetailsStruct m_content;
	}; // end ReadRawModifiedDetails
} // end namespace

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
#endif	// READRAWMODIFIEDDETAILS_H
