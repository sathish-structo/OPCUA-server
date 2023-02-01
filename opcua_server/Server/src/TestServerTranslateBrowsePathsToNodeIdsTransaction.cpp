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
#include "TestServerTranslateBrowsePathsToNodeIdsTransaction.h"
#include "ServerTranslateBrowsePathsToNodeIdsRequest.h"
#include "DynamicNodeCache.h"
#include "ServerAddressSpaceRoot.h"
#include "Statics.h"

#include <limits>

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

TestServerTranslateBrowsePathsToNodeIdsTransaction::TestServerTranslateBrowsePathsToNodeIdsTransaction(TBHandle otbHandle)
	: TranslateBrowsePathsToNodeIdsTransaction(otbHandle)
{
}

TestServerTranslateBrowsePathsToNodeIdsTransaction::~TestServerTranslateBrowsePathsToNodeIdsTransaction()
{
}

EnumStatusCode TestServerTranslateBrowsePathsToNodeIdsTransaction::handleRequests()
{
	std::vector<TranslateBrowsePathsToNodeIdsRequestPtr> requests = getRequests();

	NodeId startingTmpNodeId;

	for(std::vector<TranslateBrowsePathsToNodeIdsRequestPtr>::const_iterator it = requests.begin();
		it != requests.end(); ++it)
	{
		bool completeRequest = false;	// only complete the requests that are related to the dynamic node (to test the default implementation)
		const TranslateBrowsePathsToNodeIdsRequestPtr& request = (*it);
		EnumStatusCode requestResult;
		std::vector<BaseNodePtr> startingNodes;	// used for the default implementation
		std::vector<NodeId> startingNodeId;		// used for the dynamic node implementation
		RelativePath relativePath;
		unsigned relativePathElementCount;

		// Skip requests that have already been completed
		if(request->getRequestState() != EnumRequestState_Started)
		{
			continue;
		}

		// Check if the request did not pass validation in the core
		requestResult = request->getStatusCode();
		if(StatusCode::isBad(requestResult))
		{
			goto completeRequest;
		}

		requestResult = request->getRelativePath(relativePath);
		if(StatusCode::isBad(requestResult))
		{
			goto completeRequest;
		}
		relativePathElementCount = relativePath.getElementCount();

		// get the first node
		startingNodes.push_back(request->getStartingNode());
		request->getStartingNodeId(startingTmpNodeId);
		startingNodeId.push_back(startingTmpNodeId);

		for(unsigned i = 0; i < relativePathElementCount; i++)
		{
			const IRelativePathElement* pPathElement;
			const INodeId* pReferenceTypeId = NULL;
			EnumBrowseDirection browseDirection;
			bool includeSubtypes;
			const IQualifiedName* pTargetName = NULL;

			// Retrieve request parameters from request object
			pPathElement = relativePath.getElement(i);
			if(pPathElement == NULL)
			{
				requestResult = EnumStatusCode_BadInternalError;
				goto completeRequest;
			}
			pReferenceTypeId = pPathElement->getReferenceTypeId();
			if(pReferenceTypeId == NULL)
			{
				requestResult = EnumStatusCode_BadInternalError;
				goto completeRequest;
			}
			browseDirection = pPathElement->getIsInverse() ? EnumBrowseDirection_Inverse : EnumBrowseDirection_Forward;
			includeSubtypes = pPathElement->getIncludeSubtypes();
			pTargetName = pPathElement->getTargetName();
			if(i != relativePathElementCount-1 && pTargetName == NULL)
			{
				requestResult = EnumStatusCode_BadInternalError;
				goto completeRequest;
			}

			// all target names must not be empty
			if(pTargetName->getName().empty())
			{
				requestResult = EnumStatusCode_BadBrowseNameInvalid;
				goto completeRequest;
			}

			std::vector<BaseNodePtr> tempNodes;
			std::vector<NodeId> tempNodeIds;
			// is this a default, the dynamic or an unknown node?
			if(startingNodes.size() > 0)
			{
				for(size_t ii = 0; ii < startingNodes.size(); ii++)
				{
					if(startingNodes[ii].isNotNull())
					{
						// default case
						std::vector<BaseNodePtr> tempFoundNodes = startingNodes[ii]->findNodes(browseDirection, pReferenceTypeId, includeSubtypes, pTargetName, getSession());

						for(size_t j = 0; j < tempFoundNodes.size(); j++)
						{
							tempNodes.push_back(tempFoundNodes[j]);
						}

						if(!tempNodes.empty())
						{
							for(size_t j = 0; j < tempNodes.size(); j++)
							{
								BaseNodePtr tempNode = tempNodes[j];
								if(tempNode.isNotNull())
								{
									tempNodeIds.push_back(tempNodes[j]->getNodeId());
								}
								else
								{
									// try to find the target using the dynamic node
									NodeId tmpNodeId;
									bool relatedToDynamicNode = DynamicNodeCache::instance()->resolveDynamicNodePath(startingNodeId[ii], browseDirection, pReferenceTypeId, includeSubtypes, pTargetName, &tmpNodeId);
									//_ASSERTION(!relatedToDynamicNode || !tmpNodeId.isNull(), _T("The NodeId of the target should be one of the dynamic node, not Null."));
									tempNodes.push_back(tempNode);
									tempNodeIds.push_back(tempNode->getNodeId());
									if(relatedToDynamicNode && !tmpNodeId.isNull())
									{
										completeRequest = true;
									}
								}
							}
						}
						else
						{
							// try to find the target using the dynamic node
							NodeId tmpNodeId;
							bool relatedToDynamicNode = DynamicNodeCache::instance()->resolveDynamicNodePath(startingNodeId[ii], browseDirection, pReferenceTypeId, includeSubtypes, pTargetName, &tmpNodeId);
							//_ASSERTION(!relatedToDynamicNode || !tmpNodeId.isNull(), _T("The NodeId of the target should be one of the dynamic node, not Null."));
							if(relatedToDynamicNode && !tmpNodeId.isNull())
							{
								tempNodeIds.push_back(tmpNodeId);
								completeRequest = true;
							}
							else
							{
							requestResult = EnumStatusCode_BadNoMatch;
							goto completeRequest;
							}
						}
					}
					else
					{
						// this is the dynamic node or an unknown node
						NodeId tmpNodeId;
						bool relatedToDynamicNode = DynamicNodeCache::instance()->resolveDynamicNodePath(startingNodeId[ii], browseDirection, pReferenceTypeId, includeSubtypes, pTargetName, &tmpNodeId);
						// report a different error in case of an invalid starting node on the first place
						if(!relatedToDynamicNode && ii == 0)
						{
							// not the dynamic node and the starting node is unknown
							tempNodeIds.push_back(tmpNodeId);
							requestResult = EnumStatusCode_BadNodeIdUnknown;
							goto completeRequest;
						}
						else
						{
							completeRequest = true;

							// has a target been found?
							if(tmpNodeId.isNull())
							{
								// startingNodes remains NULL
								requestResult = EnumStatusCode_BadNoMatch;
								goto completeRequest;
							}

							tempNodeIds.push_back(tmpNodeId);
							// try to get the new node -- a not existing node will be checked during the next cycle
							BaseNodePtr tmpNode = AddressSpaceRoot::instance()->getNode(&tmpNodeId);
							tempNodes.push_back(tmpNode);
						}
					}

					startingNodes = tempNodes;
					startingNodeId = tempNodeIds;
					tempNodes.clear();
					tempNodeIds.clear();

					// Add the found node as result if we iterate over the last path element
					if(completeRequest && ii == relativePathElementCount - 1)
					{
						if(!startingNodes.empty())
						{
							ExpandedNodeId targetNodeId;
							std::vector<BrowsePathTarget> targets(startingNodes.size());

							for(size_t k = 0; k < startingNodes.size(); k++)
							{

								requestResult = targetNodeId.set(startingNodeId[k]);
								if(StatusCode::isBad(requestResult))
								{
									goto completeRequest;
								}
								targetNodeId.setServerIndex(0);

								targets[k].setTargetId(targetNodeId);
								targets[k].setRemainingPathIndex(std::numeric_limits<OTUInt32>::max());

							}
							requestResult = request->setTargets(targets);
						}
						else
						{
							ExpandedNodeId targetNodeId;
							std::vector<BrowsePathTarget> targets(startingNodeId.size());

							for(size_t k = 0; k < startingNodeId.size(); k++)
							{

								requestResult = targetNodeId.set(startingNodeId[k]);
								if(StatusCode::isBad(requestResult))
								{
									goto completeRequest;
								}
								targetNodeId.setServerIndex(0);

								targets[k].setTargetId(targetNodeId);
								targets[k].setRemainingPathIndex(std::numeric_limits<OTUInt32>::max());

							}
							requestResult = request->setTargets(targets);
						}
						if(StatusCode::isBad(requestResult))
						{
							goto completeRequest;
						}
					}

				}
			}
			else
			{
				std::vector<NodeId> tmpNodeIds;
				std::vector<bool> relatedToDynamicNodes;
				for(size_t ii = 0; ii < startingNodeId.size(); ii++)
				{
					// this is the dynamic node or an unknown node
					NodeId tmpNodeId;
					bool relatedToDynamicNode = DynamicNodeCache::instance()->resolveDynamicNodePath(startingNodeId[ii], browseDirection, pReferenceTypeId, includeSubtypes, pTargetName, &tmpNodeId);
					tmpNodeIds.push_back(tmpNodeId);
					relatedToDynamicNodes.push_back(relatedToDynamicNode);
				}
				startingNodeId.clear();
				for(size_t ii = 0; ii < tmpNodeIds.size(); ii++)
				{
				// report a different error in case of an invalid starting node on the first place
					if(!relatedToDynamicNodes[ii] && i == 0)
					{
						// not the dynamic node and the starting node is unknown
						// startingNode remains NULL
						startingNodeId.push_back(tmpNodeIds[ii]);
						requestResult = EnumStatusCode_BadNodeIdUnknown;
						goto completeRequest;
					}
					else
					{
						completeRequest = true;

						startingNodeId.push_back(tmpNodeIds[ii]);
						// has a target been found?
						if(tmpNodeIds[ii].isNull())
						{
							// startingNode remains NULL
							requestResult = EnumStatusCode_BadNoMatch;
							goto completeRequest;
						}

						// try to get the new node -- a not existing node will be checked during the next cycle
						startingNodes.push_back(AddressSpaceRoot::instance()->getNode(&startingNodeId[ii]));
					}
				}

			}

		}	// end for

completeRequest:
		if(completeRequest)
		{
			request->setStatusCode(requestResult);
			request->complete();
		}
	}

	// handle all nodes which are not related to the dynamic node by the default implementation
	return TranslateBrowsePathsToNodeIdsTransaction::handleRequests();
}
