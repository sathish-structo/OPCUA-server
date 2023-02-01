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
#ifndef _TESTSERVERBROWSETRANSACTION_H_
#define _TESTSERVERBROWSETRANSACTION_H_

#include "ServerBrowseTransaction.h"
#include "ReferenceDescription.h"
#include "ServerCondition.h"
#include "ServerAddressSpaceRoot.h"
#include "ServerBrowseContinuationPoint.h"

typedef std::vector<SoftingOPCToolbox5::ReferenceDescription> ReferenceVector;
typedef ReferenceVector::iterator ReferenceVectorIterator;
#ifndef __sun
typedef ReferenceVectorIterator::difference_type ReferenceVectorIteratorDifferenceType;
#else
// Sun's libCstd does not support iterator_traits::difference_type and std::distance()
typedef size_t ReferenceVectorIteratorDifferenceType;
#endif

class CustomBrowseContinuationPoint
	: public SoftingOPCToolbox5::Server::BrowseContinuationPoint
{
public:
	CustomBrowseContinuationPoint();
	virtual ~CustomBrowseContinuationPoint();

	/*! A list of all browsed references. */
	ReferenceVector m_browsedReferences;
	/*! This iterator always points to the first element
	* in the vector of all browsed references that has not yet
	* been returned to the client. */
	ReferenceVectorIterator m_currentPosition;
};
typedef SoftingOPCToolbox5::ObjectPointer<CustomBrowseContinuationPoint> CustomBrowseContinuationPointPtr;

class TestServerBrowseTransaction
	: public SoftingOPCToolbox5::Server::BrowseTransaction
{
public:
	/*! Constructor that shall be used by derived classes whose instances are created by the Creator object factory */
	TestServerBrowseTransaction(TBHandle otbHandle);

public:
	/*! Destructor */
	virtual ~TestServerBrowseTransaction();

	/*! Callback function to handle all read requests of the data transaction in one call.
	* That allows to sort the different requests e.g. depending on the target which contains the requested value.
	*/
	virtual EnumStatusCode handleRequests();

	
private:
	/*! Browses all references that match the criteria provided with
	* the 'Browse' request.
	*
	* @param[in]	maxReferencesPerNode	The maximum number of references to return. If the number
	*										of browsed references exceeds this number, a continuation
	*										point is returned by this method.
	* @param[in]	nodeIdToBrowse			The NodeId of the starting node
	* @param[in]	referenceTypeId			The NodeId of the reference type to follow
	* @param[in]	includeSubtypes			Indicates whether to include subtypes of the provided
	*										reference type
	* @param[in]	browseDirection			Indicates the browse direction relative to the starting node
	* @param[in]	nodeClassMask			This mask indicates which node classes shall be returned
	* @param[in]	resultMask				This mask indicates the information that is returned
	*										about a followed reference (e.g. display name of the target node)
	* @param[out]	references				The browsed references. If the number of browsed references
	*										exceeds @p maxReferencesPerNode, a continuation point is returned.
	* @param[inout]	pContinuationPoint		If the number of browsed references exceeds @p maxReferencesPerNode,
	*										a continuation point is returned. Memory is allocated by the
	*										the 'new' operator. */
	EnumStatusCode collectReferences(OTUInt32 maxReferencesPerNode,
		const SoftingOPCToolbox5::NodeId& nodeIdToBrowse,
		const SoftingOPCToolbox5::NodeId& referenceTypeId,
		bool includeSubtypes,
		EnumBrowseDirection browseDirection,
		EnumNodeClass nodeClassMask,
		EnumResultMask resultMask,
		std::vector<SoftingOPCToolbox5::ReferenceDescription>& references,
		CustomBrowseContinuationPointPtr continuationPoint,
		const SoftingOPCToolbox5::Server::Session* pSession);

	/*! Browses all references that are known to the SDK. Those references may be
	* either have been provided by the application (Server::BaseNode::addReference())
	* or may be references that are created by the SDK on startup (i.e. the standard
	* address space).
	*
	* @param[in]	nodeToBrowse			The starting node
	* @param[in]	referenceTypeId			The NodeId of the reference type to follow
	* @param[in]	includeSubtypes			Indicates whether to include subtypes of the provided
	*										reference type
	* @param[in]	browseDirection			Indicates the browse direction relative to the starting node
	* @param[in]	nodeClassMask			This mask indicates which node classes shall be returned
	* @param[in]	resultMask				This mask indicates the information that is returned
	*										about a followed reference (e.g. display name of the target node)
	* @param[out]	references				The browsed references */
	EnumStatusCode collectToolkitReferences(SoftingOPCToolbox5::Server::BaseNodePtr nodeToBrowse,
		const SoftingOPCToolbox5::NodeId& referenceTypeId,
		bool includeSubtypes,
		EnumBrowseDirection browseDirection,
		EnumNodeClass nodeClassMask,
		EnumResultMask resultMask,
		std::vector<SoftingOPCToolbox5::ReferenceDescription>& references,
		const SoftingOPCToolbox5::Server::Session* pSession);
};

#endif // _TESTSERVERBROWSETRANSACTION_H_
