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

#ifndef SERVERNONEXCLUSIVELEVELALARM_H
#define SERVERNONEXCLUSIVELEVELALARM_H
#if TB5_ALARMS
#include "Base.h"
#include "Enums.h"
#include "InnerNodeId.h"
#include "NodeId.h"
#include "ServerNonExclusiveLimitAlarm.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		/*! A level alarm is commonly used to report when a limit is exceeded. It typically relates to an
		* instrument (e.g. a temperature meter). The level alarm becomes active when the observed value is
		* above a high limit or below a low limit.
		*
		* The NonExclusiveLevelAlarm is a special level alarm utilized with one or more non-exclusive states.
		* If for example both the High and HighHigh states need to be maintained as active at the same time this
		* alarm type should be used. */
		class TBC_EXPORT NonExclusiveLevelAlarm
			: public NonExclusiveLimitAlarm
		{
		public:
			/*! Constructs a new NonExclusiveLevelAlarm instance */
			static ObjectPointer<NonExclusiveLevelAlarm> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			NonExclusiveLevelAlarm(TBHandle otbHandle);

		protected:
			/*! Default constructor */
			NonExclusiveLevelAlarm();

		public:
			/*! Destructor */
			virtual ~NonExclusiveLevelAlarm();

		private:
			/*! Forbid use assignment operator */
			NonExclusiveLevelAlarm& operator=(const NonExclusiveLevelAlarm&);
			/*! Forbid use of copy constructor */
			NonExclusiveLevelAlarm(const NonExclusiveLevelAlarm&);
		};

		typedef ObjectPointer<NonExclusiveLevelAlarm> NonExclusiveLevelAlarmPtr;
		typedef ObjectPointer<const NonExclusiveLevelAlarm> NonExclusiveLevelAlarmConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif
#endif	// SERVERNONEXCLUSIVELEVELALARM_H
