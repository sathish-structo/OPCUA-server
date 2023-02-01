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

#ifndef SERVERCONDITIONENABLE_H
#define SERVERCONDITIONENABLE_H
#if TB5_ALARMS
#include "Base.h"
#include "Enums.h"
#include "ServerMethod.h"

namespace SoftingOPCToolbox5
{
	// Forward declarations
	namespace Server
	{
		class MethodCallTransaction;
		class MethodCallRequest;
	}

	namespace Server
	{
		/*! This class represents the 'Enable' method that clients may call
		* to enable a condition.
		*
		* @see Condition */
		class TBC_EXPORT ConditionEnable
			: public Method
		{
		public:
			/*! Constructs a new ConditionEnable instance */
			static ObjectPointer<ConditionEnable> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			ConditionEnable(TBHandle otbHandle);

		protected:
			/*! Default constructor */
			ConditionEnable();

		public:
			/*! Destructor */
			virtual ~ConditionEnable();

			/*! Callback method that is called whenever the 'Enable' method is executed by a client.
			* This method may be overridden by server applications.
			*
			* @note	When overriding this method you mustn't use blocking calls within this method since
			* this blocks the UA communication. */
			virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);

		private:
			/*! Forbid use assignment operator */
			ConditionEnable& operator=(const ConditionEnable&);
			/*! Forbid use of copy constructor */
			ConditionEnable(const ConditionEnable&);
		};
		typedef ObjectPointer<ConditionEnable> ConditionEnablePtr;
		typedef ObjectPointer<const ConditionEnable> ConditionEnableConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif
#endif	// SERVERCONDITIONENABLE_H
