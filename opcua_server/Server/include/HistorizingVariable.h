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
#ifndef _HISTORIZINGVARIABLE_H_
#define _HISTORIZINGVARIABLE_H_
#if TB5_HISTORY

#include "ServerVariable.h"
#include "Util_Sync.h"
#include "AggregateConfiguration.h"
#include "NumericRange.h"
#include "ServerHistoryReadRawContinuationPoint.h"
#include "ServerHistoryReadProcessedContinuationPoint.h"
#include <list>

using namespace SoftingOPCToolbox5;

class HistorizingVariable
	: public Server::Variable
{
public:
	/*!
	 * @brief Constructs a variable which holds a history of the 100 last
	 *        written values
	 */
	HistorizingVariable();

	/*!
	 * @brief Constructs a variable which holds a history of the last
	 *        written @p maxNumberOfHistoricalValues values
	 *
	 * @param maxNumberOfHistoricalValues the maximum number of data values
	 *        the variable shall hold as historical values. The oldest values
	 *        are discarded on queue overflow.
	 */
	HistorizingVariable(OTUInt32 maxNumberOfHistoricalValues);

	/*! @brief Constructor */
	HistorizingVariable(void* otbHandle);

	/*! @brief Destructor */
	virtual ~HistorizingVariable();

	/*! @brief Overloaded method which stores a history of written values */
	virtual EnumStatusCode handleWriteRequest(Server::WriteTransaction* transaction, Server::WriteRequest* request);

	/*! @brief Overloaded method for handling read raw request */
	EnumStatusCode virtual handleHistoryReadRawRequest(Server::HistoryReadRawTransaction* transaction, Server::HistoryReadRawRequest* request);

	/*! @brief Overloaded method for handling access to aggregated historical values */
	EnumStatusCode virtual handleHistoryReadProcessedRequest(Server::HistoryReadProcessedTransaction* transaction, Server::HistoryReadProcessedRequest* request);

	/*! @brief Adds a value to the variable's history */
	void addHistoricalValue(const DataValue& dataValue);

	void enableHistorizing(bool enable);

private:
#ifdef MODULE_TEST_HISTORYREAD_TRANSACTION
	EnumStatusCode transactionModuleTest(Server::HistoryReadRawTransaction* transaction, Server::HistoryReadRawRequest* request);
#endif

	mutable Util_Sync sync;

	bool m_historizingActive;
	OTUInt32 m_maxNumberOfHistoricalValues;
	std::list<DataValue> m_historicalValues;

	static const OTUInt64 cMaxNumberOfTimeSlicesPerRequest;
	static const OTUInt32 cMaxNumberOfHistoricalValuesDefault;

	//////////////////////////////////////////////////////////////////////////
	// Processing of 'raw' data
	//////////////////////////////////////////////////////////////////////////

	// Continuation Point state information
	typedef enum tagEnumReadRawProcessingState
	{
		EnumReadRawProcessingState_LowerBound,
		EnumReadRawProcessingState_WithinBounds,
		EnumReadRawProcessingState_UpperBound,
		EnumReadRawProcessingState_Finished
	} EnumReadRawProcessingState;

	class HistorizingVariableReadRawContinuationPoint
		: public Server::HistoryReadRawContinuationPoint
	{
	public:
		HistorizingVariableReadRawContinuationPoint();

		EnumReadRawProcessingState m_processingState;
	};
	typedef ObjectPointer<HistorizingVariableReadRawContinuationPoint> HistorizingVariableReadRawContinuationPointPtr;

	EnumStatusCode getRawHistoricalValues(HistorizingVariableReadRawContinuationPointPtr continuationPoint, const DateTime& rawStartTime, DateTime rawEndTime, bool returnBoundingValues, OTUInt32 maxNumberOfValuesPerNode,
		std::list<DataValue>& historicalValues);

	//////////////////////////////////////////////////////////////////////////
	// Processing of 'processed' data
	//////////////////////////////////////////////////////////////////////////

	class HistorizingVariableReadProcessedContinuationPoint
		: public Server::HistoryReadProcessedContinuationPoint
	{
	public:
		HistorizingVariableReadProcessedContinuationPoint();
		bool m_isFinished;
		DateTime m_startTime;
		DateTime m_endTime;
		SoftingOPCToolbox5::AggregateConfiguration m_aggregateConfiguration;
	};
	typedef ObjectPointer<HistorizingVariableReadProcessedContinuationPoint> HistorizingVariableReadProcessedContinuationPointPtr;

	EnumStatusCode getProcessedHistoricalValues(HistorizingVariableReadProcessedContinuationPointPtr continuationPoint, const NumericRange& indexRange, OTDouble processingInterval, const NodeId& aggregateType,
		std::list<DataValue>& historicalValues);

	EnumStatusCode verifyAverageAggregate(const NumericRange &indexRange);
	EnumStatusCode calculateAverageAggregate(HistorizingVariableReadProcessedContinuationPointPtr continuationPoint, OTDouble processingInterval, std::list<DataValue> &aggregateValues);
	EnumStatusCode calculateAverage(const std::list<const DataValue*>& numericValues, double& average, size_t& numberOfSamples);
	EnumStatusCode calculateAverageStatusCode(HistorizingVariableReadProcessedContinuationPointPtr continuationPoint, const std::list<const DataValue*>& numericValues);
};
typedef ObjectPointer<HistorizingVariable> HistorizingVariablePtr;
typedef ObjectPointer<const HistorizingVariable> HistorizingVariableConstPtr;

#endif
#endif

