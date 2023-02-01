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
#ifndef _CHANGEADDRESSSPACEMETHOD_H_
#define _CHANGEADDRESSSPACEMETHOD_H_

#include "ServerMethod.h"
#include "ServerMethodCallRequest.h"
#include "Statics.h"
#include "Argument.h"
#include "StopWatch.h"
#include "ServerReferenceIterator.h"
#include "Constants.h"

using namespace SoftingOPCToolbox5;
using namespace SoftingOPCToolbox5::Server;

/*! Input class for the scheduler. Used by the input simulation.*/
class ChangeFolderAction
	: public Util_ScheduledAction
{
public:
	ChangeFolderAction()
	{
	}

	virtual void execute()
	{
		cleanTarget();
		fillTarget();
	}

	void fillTarget()
	{
		// a static counter used to craete always different nodeids
		static OTUInt32 cylcleCounter = 0;

		cylcleCounter++;
		for(OTUInt32 i = 0; i < m_variableCount; i++)
		{
			OTChar varName[256];
			_stprintf(varName, _T("DynamicChanging_%08u:_%08u"), cylcleCounter, i);
			TestVariablePtr testVariable = new TestVariable();

			testVariable->setBrowseName(QualifiedName(varName,  getApplicationModule()->getNameSpaceIndex()));
			LocalizedText lt;
			lt.setText(varName);
			lt.setLocale(_T("en"));
			testVariable->setDisplayName(&lt);
			testVariable->setNodeId(NodeId(getApplicationModule()->getNameSpaceIndex(), varName));
			testVariable->setDataType(Statics::DataTypeId_UInt32);

			testVariable->setVariableType(Statics::VariableTypeId_DataItemType);
			testVariable->setAccessLevel(EnumAccessLevel_CurrentRead | EnumAccessLevel_CurrentWrite);

			testVariable->setVariableType(Statics::VariableTypeId_BaseDataVariableType);
			testVariable->insert();
			m_TargetObject->addReference(Statics::ReferenceTypeId_HasComponent, testVariable);
		}
	}

	void cleanTarget()
	{
		if(m_TargetObject)
		{
			vector<BaseNodePtr> referencedNodes;
			// use NULL session (all nodes) to find nodes
			ReferenceIteratorPtr it = m_TargetObject->getReferenceIterator(EnumBrowseDirection_Forward, Statics::ReferenceTypeId_HasComponent, false, NULL);
			while(!it->isEnd())
			{
				referencedNodes.push_back(AddressSpaceRoot::instance()->getNode(it->getReferenceDescription()->getNodeId()));
				it->moveNext();
			}

			for(size_t i = 0; i < referencedNodes.size(); i++)
			{
				// variables only!!!!!
				if(referencedNodes[i]->getNodeClass() == EnumNodeClass_Variable)
				{
					m_TargetObject->removeReferencesTo(referencedNodes[i]);
					referencedNodes[i]->removeTree();
				}
			}
		}
	}

	/*! @brief Number of variables to create in each folder */
	OTUInt32 m_variableCount;

	/** @brief Object wher to change the addressSpace */
	ObjectPtr m_TargetObject;
};


class ChangeAddressSpaceMethod
	: public Server::Method
{
public:
	/*! @brief Overloaded constructur that adds this method to the given parent object */
	ChangeAddressSpaceMethod(Server::Object* parent)
	{
		// add this node to the addresspace....
		setNodeId(NodeId(getApplicationModule()->getNameSpaceIndex(), _T("ChangeAddressSpaceDynamic")));
		setDisplayName(LocalizedText(_T("ChangeAddressSpaceDynamic"), _T("en")));
		setBrowseName(QualifiedName(_T("ChangeAddressSpaceDynamic"), getApplicationModule()->getNameSpaceIndex()));
		setDescription(LocalizedText(_T("Method for changing the content of a folder cyclic."), _T("en")));
		insert();
		setExecutable(true);
		parent->addReference(Statics::ReferenceTypeId_HasComponent, this);

		// variable containing the description of the input args
		VariablePtr pInputArgs = Variable::create();

		// add a list of input args...
		vector<Argument> inputArgs;

		pInputArgs->setDataType(Statics::DataTypeId_Argument);
		pInputArgs->setValueRank(1);

		// define input args for the method ......
		Argument param;

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("FolderCount"));
		param.setDescription(LocalizedText(_T("Number of subfolders to be craeted (use 0 for removing existings)"), _T("en")));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		param.setDataType(NodeId(0, EnumDataTypeId_UInt32));
		param.setName(_T("VariableCount"));
		param.setDescription(LocalizedText(_T("Number of variables to be craeted in each folder (use 0 for removing existings)"), _T("en")));
		param.setValueRank(ValueRank_Scalar);
		inputArgs.push_back(param);

		param.setDataType(Statics::DataTypeId_UInt32);
		param.setName(_T("RefreshInterval"));
		param.setDescription(LocalizedText(_T("Interval for a rebuild of the folder's content (use 0 for disabling))"), _T("en")));
		inputArgs.push_back(param);

		Value containedValue;
		containedValue.setArgumentArray(inputArgs);

		DataValue value;
		value.setValue(containedValue);
		pInputArgs->setDataValue(value);
		pInputArgs->setNodeId(NodeId(getApplicationModule()->getNameSpaceIndex(), _T("ChangeAddressSpaceDynamic_InputArguments")));
		pInputArgs->setBrowseName(QualifiedName(BrowseName_InputArguments,0));
		pInputArgs->setDisplayName(LocalizedText(BrowseName_InputArguments, _T("en")));

		std::vector<unsigned> arrayDims;
		arrayDims = pInputArgs->getArrayDimensions();
		arrayDims.push_back(1);
		pInputArgs->setArrayDimensions(arrayDims);
		arrayDims = pInputArgs->getArrayDimensions();

		pInputArgs->insert();
		pInputArgs->setAccessLevel(EnumAccessLevel_CurrentRead);
		NodeId propTypeNode(0, EnumNumericNodeId_PropertyType);
		pInputArgs->addReference(
			Statics::ReferenceTypeId_HasTypeDefinition,
			Server::AddressSpaceRoot::instance()->getNode(&propTypeNode));
		this->addReference(Statics::ReferenceTypeId_HasProperty, pInputArgs);

		m_changeFolderAction.m_TargetObject = parent;
	}

	virtual EnumStatusCode handleExecuteRequest
		(
		Server::MethodCallTransaction* /* transaction */,
		Server::MethodCallRequest* request
		)
	{
		#ifdef SOOS_WINDOWS
			StopWatch sw(true);
		#endif

		std::vector<Value> inputArgs = request->getInputArguments();

		// there should be always 3 input args.....
		if(inputArgs.size() != 3)
		{
			request->setStatusCode(EnumStatusCode_BadInvalidArgument);
			request->complete();
			return EnumStatusCode_Good;
		}


		// variable 1: Must be an OTUInt32 defining the ID for the varaible set
		if(inputArgs[0].getDataType() != EnumDataTypeId_UInt32)
		{
			request->setStatusCode(EnumStatusCode_BadInvalidArgument);
			request->complete();
		}

		// variable 2: Must be an OTUInt32 defining the number of variables to create
		if(inputArgs[1].getDataType() != EnumDataTypeId_UInt32)
		{
			request->setStatusCode(EnumStatusCode_BadInvalidArgument);
			request->complete();
		}

		// variable 3: Must be an OTUInt32 defining the number of variables to create
		if(inputArgs[2].getDataType() != EnumDataTypeId_UInt32)
		{
			request->setStatusCode(EnumStatusCode_BadInvalidArgument);
			request->complete();
		}

		if(inputArgs[2].getUInt32()) // uinterval != 0?
		{
			if(m_changeFolderAction.isCyclic() == false)
			{
				m_changeFolderAction.setTimeout(inputArgs[2].getUInt32());
				m_changeFolderAction.m_variableCount = inputArgs[1].getUInt32();
				m_changeFolderAction.setCyclic(true);
				getApplicationModule()->addAction(&m_changeFolderAction);
			}
		}
		else
		{
			getApplicationModule()->removeAction(&m_changeFolderAction);
			m_changeFolderAction.setTimeout(0);
			m_changeFolderAction.setCyclic(false);
		}

		request->complete();

		return EnumStatusCode_Good;
	}

	ChangeFolderAction m_changeFolderAction;
};

typedef ObjectPointer<ChangeAddressSpaceMethod> ChangeAddressSpaceMethodPtr;
typedef ObjectPointer<const ChangeAddressSpaceMethod> ChangeAddressSpaceMethodConstPtr;

#endif // _ChangeAddressSpaceMethod_H_
