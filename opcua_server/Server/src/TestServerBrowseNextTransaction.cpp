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
#include "TestServerBrowseNextTransaction.h"
#include "ServerBrowseRequest.h"
#include "ServerAddressSpaceRoot.h"
#include "BrowseDescription.h"
#include "ServerReferenceIterator.h"
#include "ServerReferenceType.h"
#include "TestServerBrowseTransaction.h"
#include "DynamicNodeCache.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

TestServerBrowseNextTransaction::TestServerBrowseNextTransaction(TBHandle otbHandle)
	: Server::BrowseNextTransaction(otbHandle)
{
}

TestServerBrowseNextTransaction::~TestServerBrowseNextTransaction()
{
}

EnumStatusCode TestServerBrowseNextTransaction::handleRequests()
{
	std::vector<Server::BrowseNextRequestPtr> requests = getRequests();
	for(size_t i = 0; i < requests.size(); i++)
	{
		BrowseContinuationPointPtr continuationPoint;
		if(StatusCode::isBad(requests[i]->getContinuationPoint(continuationPoint)))
		{
			continue;
		}
		NodeId nodeIdToBrowse;
		continuationPoint->getNodeIdToBrowse(nodeIdToBrowse);

		bool bHandleRequest = DynamicNodeCache::instance()->isNodeIdRelatedToVirtualNode(nodeIdToBrowse);
		NodeId referenceTypeId;

		EnumStatusCode requestResult = requests[i]->getStatusCode();

		// Check if the status code is not good - this means that there
		// was a validation that was not passed in the Toolbox core.
		if(requests[i]->getRequestState() != EnumRequestState_Completed &&
			StatusCode::isGood(requestResult) &&
			bHandleRequest &&
			StatusCode::isGood(continuationPoint->getReferenceTypeId(referenceTypeId)))
		{
			// we use this ContinuationPoint type for the virtual nodes
			CustomBrowseContinuationPointPtr customContinuationPoint;
			{
				customContinuationPoint = static_pointer_cast<CustomBrowseContinuationPoint>(continuationPoint);
			}

			// Copy the remaining references
			ReferenceVectorIteratorDifferenceType remainingReferences = 0;
			_ASSERTION(customContinuationPoint.isNotNull(), _T("CustomContinuationPoint is NULL"));
			if(StatusCode::isGood(requestResult) && customContinuationPoint.isNotNull())
			{
				// Determine the number of remaining (undelivered) references
				remainingReferences = customContinuationPoint->m_browsedReferences.end() - customContinuationPoint->m_currentPosition;

				// Determine the number of references to return with this service call
				ReferenceVectorIteratorDifferenceType referencesToReturn =
					std::min<ReferenceVectorIteratorDifferenceType>(customContinuationPoint->getRequestedMaxReferencesPerNode(), remainingReferences);

				// Copy the references
				ReferenceVector references(customContinuationPoint->m_currentPosition, customContinuationPoint->m_currentPosition + referencesToReturn);
				requestResult = requests[i]->setReferences(references);

				// Advance the iterator and update the remaining number references
				if(StatusCode::isGood(requestResult))
				{
					std::advance(customContinuationPoint->m_currentPosition, referencesToReturn);
					remainingReferences = customContinuationPoint->m_browsedReferences.end() - customContinuationPoint->m_currentPosition;
				}
			}

			// Delete the continuation point if all references have been returned
			// or the browse is aborted by a bad status
			if((StatusCode::isGood(requestResult) && remainingReferences == 0) ||
				StatusCode::isBad(requestResult))
			{
				requests[i]->deleteContinuationPoint();
			}

			requests[i]->setStatusCode(requestResult);
			requests[i]->complete();
		}

	}

	return  BrowseNextTransaction::handleRequests();
}
