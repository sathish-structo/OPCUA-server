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

#ifndef SERVERROLE_H
#define SERVERROLE_H
#include "Base.h"
#include "Enums.h"
#include "ServerObject.h"

namespace SoftingOPCToolbox5
{
	namespace Server
	{
		class Session;

		/*! The Role class is used to define a node in the servers address space of type RoleType.
		*
		* A role node has optional properties which define rules, how this role is assigned to a client session.
		* The properties contain sensitive security related information and shall only be browseable, readable and writeable
		* by authorized administrators through an encrypted channel.
		*
		* The role node itself shall be also configured with permissions, so UA methods cannot be invoked by everybody.
		*
		* The rules defined in the properties are logically anded.
		*
		* The RoleType also defines optional methods to change the role properties. Those methods are not yet supported by the SDK. */
		class TBC_EXPORT Role
			: public Object
		{
		public:
			/*! Constructs a new Role instance */
			static ObjectPointer<Role> create();

			/*! Gets a well-known Role instance
			*
			* The well-known roles, predefined by specification, are not yet configured an are not added to the address space.
			* If an application wants to use these roles, it is necessary to configure (at least Identities property) the role
			* and add it to the address space
			*
			* Well-known roles are
			* @li Anonymous
			* @li AuthenticatedUser
			* @li Observer
			* @li Operator
			* @li Engineer
			* @li Supervisor
			* @li ConfigureAdmin
			* @li SecurityAdmin
			*
			* @return The role object, Null if role object is not available.
			*
			* @see AddressSpaceRoot::addRole */
			static ObjectPointer<Role> getWellKnown(EnumNumericNodeId wellknownRoleId);

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator Role factory */
			Role(TBHandle otbHandle);

			/*! Sets the Identities property of the role
			*
			* @param identities		The identities specifies the currently configured rules for mapping a UserIdentityToken to the Role.
			*						If this Property is an empty array, then the Role cannot be granted to any Session by the SDK. */
			EnumStatusCode setIdentities(const std::vector<IdentityMappingRuleType>& identities);
			/*! Gets the Identities property of the role */
			EnumStatusCode getIdentities(std::vector<IdentityMappingRuleType>& identities);

			/*! Sets the Applications and ApplicationsExclude property of the role
			*
			* @param applications	The Applications property specifies the application instance certificates of clients which shall
			*						be included or excluded from this Role. Each element in the array is an ApplicationUri from a
			*						client certificate which is trusted by the Server.
			*						An empty list of application implies that this filter is not active for this role.
			*
			* @param excludeApplications	The ApplicationsExclude property defines the applications property as an include list or exclude list.
			*								If this property is has a value of FALSE then only application instance certificates included in the
			*								applications property shall be included in this Role.
			*								All other application instance certificates shall not be included in this Role.
			*								If this property has a value of TRUE then all application instance certificates included in the applications
			*								property shall be excluded from this Role.
			*								All other application instance certificates shall be included in this Role. */
			EnumStatusCode setApplications(const std::vector<tstring>& applications, bool excludeApplications);
			/*! Gets the Applications and ApplicationsExclude property of the role */
			EnumStatusCode getApplications(std::vector<tstring>& applications, bool& excludeApplications);

			/*! Sets the Endpoints and EndpointsExclude property of the role
			*
			* @param endpoints		The Endpoints property specifies the endpoints which shall be included or excluded from this Role.
			*						The value is an EndpointType array which contains one or more endpoint descriptions.
			*						An empty list of endpoints implies that this filter is not active for this role.
			*
			* @param excludeEndpoints	The EndpointsExclude property defines Endpoints property as an include list or exclude list.
			*							If this property is has a value of FALSE then only endpoints with matching descriptions in the
			*							Endpoints property shall be included in this Role.
			*							All other endpoints shall not be included in this Role.
			*							If this property has a value of TRUE then all endpoints included in the Endpoints
			*							property shall be excluded from this Role.
			*							All other endpoints shall be included in this Role. */
			EnumStatusCode setEndpoints(const std::vector<EndpointType>& endpoints, bool excludeEndpoints);
			/*! Gets the Endpoints and EndpointsExclude property of the role */
			EnumStatusCode getEndpoints(std::vector<EndpointType>& endpoints, bool& excludeEndpoints);

		protected:
			/*! For internal use only */
			Role();

		public:
			/*! Destructor */
			virtual ~Role();

			/*! Initialize the Role object and create properties
			*
			* The NodeIds of the property nodes are generated by the SDK. An application provide own NodeIds by overwriting
			* the method generateNodeIdForSubNode. Default implementation will provide unique NodeIds for the properties generated by SDK. */
			virtual EnumStatusCode init(const INodeId* typeId);


		private:
			/*! For internal use only */
			EnumStatusCode initPropertyId(const tstring& name, const NodeId& nodeId);

			/*! Forbid use assignment operator */
			Role& operator=(const Role&);
			/*! Forbid use of copy constructor */
			Role(const Role&);
		};
		typedef ObjectPointer<Role> RolePtr;
		typedef ObjectPointer<const Role> RoleConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif	// SERVERROLE_H
