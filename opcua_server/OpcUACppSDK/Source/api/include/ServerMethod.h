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

#ifndef SERVERMETHOD_H
#define SERVERMETHOD_H
#include "Base.h"
#include "Enums.h"
#include "InnerNodeId.h"
#include "NodeId.h"
#include "ServerBaseNode.h"

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
		/*! The Method class is used to define a Method node in the address space of a server.
		* A method can be called by a client using the 'Call' service and will be executed
		* on the server (see handleExecuteRequest()).
		*
		* A method is part of an object in the address space. An object that owns a method needs to
		* reference the method by a 'HasComponent' reference. */
		class TBC_EXPORT Method
			: public BaseNode
		{
		public:
			/*! Constructs a new Method instance */
			static ObjectPointer<Method> create();

			/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
			Method(TBHandle otbHandle);

			/*! A generic helper method to test that the provided input argument values match the criteria defined by the InputArguments property of the current OPC UA Method.
			*
			* When this method reports an error, this means that the client made a mistake when providing the input arguments.
			*
			* This method generates the specified operation result codes and inputArgument result codes.
			* When this C++ method reports a good status, the caller is usually safe to invoke the inputArgumentValues array at the expected indexes and
			* with the expected getter methods.
			* The only exception are input argument definitions that allow multiple representations, like
			* @li abstract data types to allow different subtypes
			* @li variable value ranks, e.g. ValueRank_OneOrMoreDimensions or ValueRank_Any
			* @li variable array dimensions (any dimensions with value 0)
			* For those exceptions, the caller still needs to test the value to be able to select the correct Value getter.
			*
			* @param inputArgumentValues	The input argument values provided from the client
			* @param inputArgumentResults	The status codes related to the input argument values. An empty vector is returned when no error is found.
			* @return						Reports a bad status code if any input argument values don't match. This status shall be used as
			*								operation result if the validation fails.
			*
			* @note	Due to specification, A Server shall accept a ByteString if an array of Byte is expected.
			*		This validation method accepts this special case, so keep in mind the argument value handling when dealing with byte arrays */
			EnumStatusCode validateInputArguments(const std::vector<Value>& inputArgumentValues, std::vector<EnumStatusCode>& inputArgumentResults) const;

			/*! A generic helper method to test that the output arguments to send match the criteria defined by the OutputArguments property of the current OPC UA Method.
			*
			* When this method reports an error, this means that this server application made a configuration error between the described arguments and provided values.
			*
			* @param outputArgumentValues	The output argument values which shall be returned to the client.
			* @return						Returns Good or an error which indicates the type of the first encountered mismatch.
			*
			* @note	The return value of this method should not be used as response of the service operation.
			*		A bad result means a server internal problem and BadInternalError should be reported to the client. */
			EnumStatusCode validateOutputArguments(const std::vector<Value>& outputArgumentValues) const;

			/*! A helper method to get the default value of an optional input argument.
			*
			* The default value is available only if this method has argument Metadata referenced by a HasOptionalInputArgumentDescription reference.
			*
			* @param index		The index of the optional input argument.
			* @param[out] value	The default value for the requested input argument.
			* @return			Returns Good if the default value was successfully retrieved. */
			EnumStatusCode getDefaultInputArgumentValue(size_t index, Value& value) const;

			/*! A helper method to get the default value of an optional input argument.
			*
			* The default value is available only if this method has argument Metadata referenced by a HasOptionalInputArgumentDescription reference.
			*
			* @param argumentName	The name of the optional input argument.
			* @param[out] value		The default value for the requested input argument.
			* @return				Returns Good if the default value was successfully retrieved. */
			EnumStatusCode getDefaultInputArgumentValue(const tstring& argumentName, Value& value) const;

		protected:
			/*! Default constructor */
			Method();
			/*! For internal use only */
			Method(bool suppressCoreInstanceCreate);

		public:
			/*! Destructor */
			virtual ~Method();

			/*! Defines the input arguments of the method. Clients are able to identify the input arguments
			* of a method by reading the 'InputArgument' property of the method.
			*
			* The input arguments cannot be modified once they have been defined.
			*
			* @param nodeId			The NodeId that is assigned to the InputArgument property
			* @param arguments		A vector of Arguments, defining the number and properties for the InputArguments
			*
			* @note	This method creates the InputArgument property that holds the input arguments of the method.
			*
			* @see Argument */
			EnumStatusCode setInputArguments(const NodeId& nodeId, const std::vector<Argument>& arguments);
			/*! @overload */
			EnumStatusCode setInputArguments(const INodeId* pNodeId, const std::vector<Argument>& arguments);

			/*! Defines the output arguments of the method. Clients are able to identify the output arguments
			* of a method by reading the 'OutputArgument' property of the method.
			*
			* The output arguments cannot be modified once they have been defined.
			*
			* @param nodeId			The NodeId that is assigned to the OutputArgument property
			* @param arguments		A vector of Arguments, defining the number and properties for the OutputArguments
			*
			* @note	This method creates the OutputArgument property that holds the output arguments of the method.
			*
			* @see Argument */
			EnumStatusCode setOutputArguments(const NodeId& nodeId, const std::vector<Argument>& arguments);
			/*! @overload */
			EnumStatusCode setOutputArguments(const INodeId* pNodeId, const std::vector<Argument>& arguments);

			/*! Returns whether a client is (currently) allowed to execute this method. */
			bool getExecutable() const;
			/*! Sets whether a client is allowed to execute this method. */
			virtual EnumStatusCode setExecutable(bool executable);

			/*! Returns the 'UserExecutable' attribute value of the method that indicates whether the
			* method can be executed by a user belonging to the given session.
			*
			* @param session	The client session where the user is assigned to
			*
			* @see getExecutable() */
			bool getUserExecutable(ObjectPointer<Server::Session> session) const;

			/*! Assigns the method a previously defined AccessPattern.
			*
			* The bit EnumAccessLevel_Executable is used to restrict execution of this method
			* for a user. All other bits are ignored.
			*
			* @note	This method returns an error if the key is unknown to the server.
			*		Keys are limited to values from 0...0xFFFF.
			*
			* @see IAccessPattern Application::setAccessPattern()
			*
			* @note This method is deprecated. Please use BaseNode::setRolePermissions instead.
			*		If you still like to use this method, please take care that Application::enableUserAccessPattern is invoked
			*		to enable the former way of user access right configuration. */
			DEPRECATED(EnumStatusCode setAccessPatternKey(OTUInt32 accessPatternKey));

			/*! Returns the key of the assigned AccessPattern.
			*
			* @see setAccessPatternKey
			*
			* @note This method is deprecated. Please use BaseNode::getRolePermissions instead.
			*		If you still like to use this method, please take care that Application::enableUserAccessPattern is invoked
			*		to enable the former way of user access right configuration. */
			DEPRECATED(OTUInt32 getAccessPatternKey() const);

			/*! Callback method that is triggered when the method is executed by a client.
			* This method should be overridden by server applications.
			*
			* @param transaction	The transaction which contains the single request
			* @param request		The single request which shall be handled
			*
			* @note	When overriding this method you mustn't use blocking calls within this method since
			* this blocks the UA communication. */
			virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);

			/*! Callback method for a single read request to a value.
			*
			* This method handles requests for the 'Executable' and 'UserExecutable' attribute of methods.
			*
			* @param transaction	The whole transaction which contains the single ReadRequest
			* @param request		The single ReadRequest which shall be handled
			*
			* @note	When overriding this method you mustn't use blocking calls within this method since
			* this blocks the UA communication. */
			EnumStatusCode handleReadRequest(Server::ReadTransaction* transaction, Server::ReadRequest* request);

		protected:
			/*! Forwards the method call to the callbacks Object::handleCallRequest(), ObjectType::handleCallRequest() or to the SDK core.
			*
			* @param transaction	The whole transaction which contains the single request
			* @param request		The single request which shall be executed */
			EnumStatusCode executeRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);

		private:
			/*! Forbid use assignment operator */
			Method& operator=(const Method&);
			/*! Forbid use of copy constructor */
			Method(const Method&);

		};
		typedef ObjectPointer<Method> MethodPtr;
		typedef ObjectPointer<const Method> MethodConstPtr;

	} // end Server namespace
} // toolbox namespace end

#endif	// SERVERMETHOD_H
