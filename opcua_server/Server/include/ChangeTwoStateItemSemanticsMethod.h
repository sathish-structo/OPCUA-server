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
#ifndef _CHANGE_TWO_STATE_ITEM_SEMANTICS_METHOD_H_
#define _CHANGE_TWO_STATE_ITEM_SEMANTICS_METHOD_H_

#include "ServerMethod.h"
#include "ServerObject.h"

using namespace SoftingOPCToolbox5;

/**
* This method enables clients to change the semantics ('true' and 'false' texts) of TwoStateItems
*/
class ChangeTwoStateItemSemanticsMethod : public Server::Method
{
public:
	ChangeTwoStateItemSemanticsMethod (Server::ObjectPtr parent);
	virtual ~ChangeTwoStateItemSemanticsMethod ();

	virtual EnumStatusCode handleExecuteRequest(Server::MethodCallTransaction* transaction, Server::MethodCallRequest* request);

	EnumStatusCode extractArguments(Server::MethodCallRequest* request, std::vector<NodeId>& twoStateItemIds, LocalizedText& trueText, LocalizedText& falseText, bool& summarizeChanges);

};

typedef ObjectPointer<ChangeTwoStateItemSemanticsMethod> ChangeTwoStateItemSemanticsMethodPtr;
typedef ObjectPointer<const ChangeTwoStateItemSemanticsMethod> ChangeTwoStateItemSemanticsMethodConstPtr;

#endif //_CHANGE_TWO_STATE_ITEM_SEMANTICS_METHOD_H_
