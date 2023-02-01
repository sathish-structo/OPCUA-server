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
#ifndef _TEST_SERVER_ADDRESS_SPACE_ROOW_H_
#define _TEST_SERVER_ADDRESS_SPACE_ROOW_H_

#include "ServerAddressSpaceRoot.h"
#include "ServerBaseNode.h"

typedef enum tagEnumImportDuplicateNodeHandling
{
	EnumImportDuplicateNodeHandling_Default,
	EnumImportDuplicateNodeHandling_IgnoreImport,
	EnumImportDuplicateNodeHandling_RemoveExisting
}
EnumImportDuplicateNodeHandling;

class TestServerAddressSpaceRoot
	: public SoftingOPCToolbox5::Server::AddressSpaceRoot
{
public:
	TestServerAddressSpaceRoot(void* otbHandle);

	void setImportDuplicateNodeHandling(EnumImportDuplicateNodeHandling handling);

	//! @see AddressSpaceRoot::onIncrementNodeVersion()
	virtual void onIncrementNodeVersion(const std::vector<SoftingOPCToolbox5::Server::BaseNode*>& changedNodes);

	/*! @brief This method is called when the Application is initialized and allows server
	 *        applications to provide translations for attribute values of type LocalizedText. */
	virtual EnumStatusCode defineTranslations();

	virtual void onImportNodesError(const tstring& fileName, int lineNumber, EnumTraceLevel severity, EnumModelDesignerStatus errorCode, const tstring& errorMessage);
	virtual EnumModelDesignerStatus onImportDuplicateNode(SoftingOPCToolbox5::Server::BaseNodePtr existingNode, const SoftingOPCToolbox5::Server::NodeSet2Import::ImportUANodeDescription* pImportedNodeDescription, const SoftingOPCToolbox5::XmlElement &xmlTag);
	virtual EnumModelDesignerStatus onModelDesignerMethod(const SoftingOPCToolbox5::INodeId* methodId, const std::vector<SoftingOPCToolbox5::Value>& inputArguments);
	virtual EnumModelDesignerStatus onResetAddressSpace();
	virtual void handleExportCustomNodes(SoftingOPCToolbox5::Server::NodeSet2Export::ExportCustomNodesTransactionPtr transaction);

	virtual void onValidateError(const SoftingOPCToolbox5::INodeId* nodeId, OTUInt16 severity, const tstring& errorMessage);

private:
	EnumImportDuplicateNodeHandling m_importDuplicateNodeHandling;
};

#endif
