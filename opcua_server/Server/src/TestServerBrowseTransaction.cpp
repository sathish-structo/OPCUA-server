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
#include "TestServerBrowseTransaction.h"
#include "ServerBrowseRequest.h"
#include "ServerAddressSpaceRoot.h"
#include "BrowseDescription.h"
#include "ServerReferenceIterator.h"
#include "ServerReferenceType.h"
#include "DynamicNodeCache.h"
#include "Statics.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

TestServerBrowseTransaction::TestServerBrowseTransaction(TBHandle otbHandle)
	: BrowseTransaction(otbHandle)
{
}

TestServerBrowseTransaction::~TestServerBrowseTransaction()
{
}

EnumStatusCode TestServerBrowseTransaction::handleRequests()
{
	EnumStatusCode serviceResult = EnumStatusCode_Good;
	// handle views
	ViewDescription viewDesc;
	if(StatusCode::isGood(getView(viewDesc)))
	{
		// only allow the default view Id (Numeric 0:0)
		if(!viewDesc.getViewId()->isNull())
		{
			serviceResult = EnumStatusCode_BadViewIdUnknown;
		}
	}
	else
	{
		_REPORT_ASSERTION(_T("Failed to get the view id."));
		serviceResult = EnumStatusCode_BadInternalError;
	}

	if(StatusCode::isGood(serviceResult))
	{
		std::vector<BrowseRequestPtr> requests = getRequests();
		for(size_t i = 0; i < requests.size(); i++)
		{
			// Check if the status code is not good - this means that there
			// was a validation that was not passed in the Toolbox core.
			EnumStatusCode requestResult = requests[i]->getStatusCode();

			NodeId nodeIdToBrowse;
			requests[i]->getNodeIdToBrowse(nodeIdToBrowse);
			bool bHandleRequest = DynamicNodeCache::instance()->isNodeIdRelatedToVirtualNode(nodeIdToBrowse);
			// in case of a virtual node, it is ok that the node cannot be found by the SDK, we want to handle this node by ourselves
			if(bHandleRequest && requestResult == EnumStatusCode_BadNodeIdUnknown)
			{
				requestResult = EnumStatusCode_Good;
			}

			NodeId referenceTypeId;

			// Match for the right conditions
			if(requests[i]->getRequestState() != EnumRequestState_Completed &&
				StatusCode::isGood(requestResult) &&
				bHandleRequest &&
				StatusCode::isGood(requests[i]->getReferenceTypeId(referenceTypeId)))
			{
				// the null-nodeId is valid but the following test would fail with it
				if(!referenceTypeId.isNull())
				{
					BaseNodePtr referenceType = AddressSpaceRoot::instance()->getReferenceType(&referenceTypeId);
					if(referenceType.isNull())
					{
						// the type is unknown, don't handle this case
						continue;
					}
				}

				std::vector<ReferenceDescription> referencesOfNode;

				// Retrieve NodeId of starting node
				NodeId nIdToBrowse;
				if(StatusCode::isGood(requestResult))
				{
					requestResult = requests[i]->getNodeIdToBrowse(nIdToBrowse);
				}
				// Retrieve NodeId of reference type to follow
				NodeId refTypeId;
				if(StatusCode::isGood(requestResult))
				{
					requestResult = requests[i]->getReferenceTypeId(refTypeId);
				}
				// Retrieve whether to follow subtypes of the provided reference type
				bool includeSubtypes = requests[i]->getIncludeSubtypes();
				// Retrieve the browse direction
				EnumBrowseDirection browseDirection = requests[i]->getBrowseDirection();
				// Retrieve a mask which indicates the node classes to follow
				EnumNodeClass nodeClassMask = requests[i]->getNodeClassMask();
				// Retrieve a mask that indicates which information about a reference to return to the client
				EnumResultMask resultMask = requests[i]->getResultMask();

				// create a local continuationPoint to store the current processing information,
				// we set it to the request only if necessary
				CustomBrowseContinuationPointPtr continuationPoint = new CustomBrowseContinuationPoint();
				// Perform the actual browse and collect all references to return
				if(StatusCode::isGood(requestResult))
				{
					requestResult = collectReferences(
						getRequestedMaxReferencesPerNode(),
						nIdToBrowse,
						refTypeId,
						includeSubtypes,
						browseDirection,
						nodeClassMask,
						resultMask,
						referencesOfNode, /* out parameter */
						continuationPoint, /* in/out parameter */
						getSession());
				}

				// Copy the browse result
				if(StatusCode::isGood(requestResult))
				{
					requestResult = requests[i]->setReferences(referencesOfNode);
				}

				// Store the the continuation point if we didn't complete the request
				if(StatusCode::isGood(requestResult) &&
					continuationPoint->m_currentPosition != continuationPoint->m_browsedReferences.end())
				{
					requestResult = requests[i]->setContinuationPoint(continuationPoint);
				}

				if(StatusCode::isGood(requestResult))
				{
					requests[i]->setReferences(referencesOfNode);
				}
				requests[i]->setStatusCode(requestResult);
				requests[i]->complete();
			}	// end if desired request
		}	// end for requests

		// handle the remaining open requests by the default implementation
		serviceResult = BrowseTransaction::handleRequests();
	}
	return serviceResult;
}

EnumStatusCode TestServerBrowseTransaction::collectReferences(OTUInt32 maxReferencesPerNode, const NodeId& nodeIdToBrowse,
	const NodeId& referenceTypeId, bool includeSubtypes, EnumBrowseDirection browseDirection, EnumNodeClass nodeClassMask,
	EnumResultMask resultMask, std::vector<SoftingOPCToolbox5::ReferenceDescription>& references,
	CustomBrowseContinuationPointPtr continuationPoint,
	const SoftingOPCToolbox5::Server::Session* pSession)
{
	EnumStatusCode result = EnumStatusCode_Good;
	
	//////////////////////////////////////////////////////////////////////////
	// Verify that the provided NodeId is known to the application
	//////////////////////////////////////////////////////////////////////////
	Server::BaseNodePtr nodeToBrowse;
	if(StatusCode::isGood(result))
	{
		nodeToBrowse = Server::AddressSpaceRoot::instance()->getNode(&nodeIdToBrowse);
	}

	//////////////////////////////////////////////////////////////////////////
	// Validate the provided reference type id.
	//
	// A null reference type id indicates that all are to be returned and
	// includeSubtypes is to be ignored.
	//////////////////////////////////////////////////////////////////////////
	if(StatusCode::isGood(result) && !referenceTypeId.isNull())
	{
		Server::ReferenceTypePtr referenceTypeNode = Server::AddressSpaceRoot::instance()->getReferenceType(&referenceTypeId);
		if(referenceTypeNode.isNull())
		{
			result = EnumStatusCode_BadReferenceTypeIdInvalid;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Collect both the references provided by/to the SDK and the
	// custom references that are only known to the application.
	//////////////////////////////////////////////////////////////////////////
	std::vector<ReferenceDescription> allReferences;
	if(StatusCode::isGood(result))
	{
		if(nodeToBrowse.isNotNull())
		{
			result = collectToolkitReferences(nodeToBrowse, referenceTypeId, includeSubtypes, browseDirection, nodeClassMask, resultMask, allReferences, pSession);
		}
	}

	DynamicNodeCache::instance()->addVirtualReferences(nodeIdToBrowse, referenceTypeId, includeSubtypes, browseDirection, nodeClassMask, resultMask, allReferences);

	//////////////////////////////////////////////////////////////////////////
	// Create a continuation point if the number of collected references exceeds
	// the maximum requested number of references. If the maximum requested
	// number of references per node is 0, all references shall be returned.
	//////////////////////////////////////////////////////////////////////////
	if(StatusCode::isGood(result))
	{
		if(maxReferencesPerNode > 0 && allReferences.size() > maxReferencesPerNode)
		{
			continuationPoint->m_browsedReferences = allReferences;
			continuationPoint->m_currentPosition = continuationPoint->m_browsedReferences.begin() + maxReferencesPerNode;

			references.assign(continuationPoint->m_browsedReferences.begin(), continuationPoint->m_currentPosition);
		}
		else
		{
			references = allReferences;
		}
	}

	return result;
}

EnumStatusCode TestServerBrowseTransaction::collectToolkitReferences(SoftingOPCToolbox5::Server::BaseNodePtr nodeToBrowse,
	const SoftingOPCToolbox5::NodeId& referenceTypeId, bool includeSubtypes, EnumBrowseDirection browseDirection,
	EnumNodeClass nodeClassMask, EnumResultMask resultMask, std::vector<SoftingOPCToolbox5::ReferenceDescription>& references,
	const SoftingOPCToolbox5::Server::Session* pSession)
{
	EnumStatusCode result = EnumStatusCode_Good;

	Server::ReferenceIteratorPtr referenceIterator = nodeToBrowse->getReferenceIterator(browseDirection, &referenceTypeId, includeSubtypes, nodeClassMask, resultMask, pSession);
	if(referenceIterator.isNotNull())
	{
		while(referenceIterator->isEnd() == false)
		{
			references.push_back(referenceIterator->getReferenceDescription());
			referenceIterator->moveNext();
		}
	}
	else
	{
		result = EnumStatusCode_BadInternalError;
	}

	return result;
}

CustomBrowseContinuationPoint::CustomBrowseContinuationPoint()
{
	m_browsedReferences.clear();
	m_currentPosition = m_browsedReferences.end();
}

CustomBrowseContinuationPoint::~CustomBrowseContinuationPoint()
{
	m_browsedReferences.clear();
	m_currentPosition = m_browsedReferences.end();
}
