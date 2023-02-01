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
#include "StructoAutomoldApplication.h"
#include "CustomCallbackEvent.h"

using namespace SoftingOPCToolbox5;

StructoAutomoldApplication::StructoAutomoldApplication(TBHandle otbHandle) : Application(otbHandle)
{

}

void StructoAutomoldApplication::gdsCreateSignedCertificateCompleted(
	void* pCookie,
	EnumStatusCode creationResult,
	const IByteString& certificate,
	const IByteString& privateKey,
	const tstring& privateKeyPassword,
	bool isUserCertificate,
	const INodeId& certificateGroup,
	const std::vector<ByteString> issuerCertificates,
	const INodeId& pendingRequestId)
{
	GdsCreateSignedCertificateInfo* gdsCompleteInfo = (GdsCreateSignedCertificateInfo*)pCookie;

	gdsCompleteInfo->m_creationResult = creationResult;
	gdsCompleteInfo->m_certificate = certificate;
	gdsCompleteInfo->m_privateKey = privateKey;
	gdsCompleteInfo->m_privateKeyPassword = privateKeyPassword;
	gdsCompleteInfo->m_isUserCertificate = isUserCertificate;
	gdsCompleteInfo->m_certificateGroup = certificateGroup;
	gdsCompleteInfo->m_issuerCertificates = issuerCertificates;
	gdsCompleteInfo->m_pendingRequestId = pendingRequestId;

	gdsCompleteInfo->m_gdsEvent.signal();
}

void StructoAutomoldApplication::gdsGetTrustListCompleted(void* pCookie,
	const INodeId& certificateGroup,
	EnumStatusCode getResult,
	const std::vector<ByteString>& trustedCertificates,
	const std::vector<ByteString>& trustedCrls,
	const std::vector<ByteString>& issuerCertificates,
	const std::vector<ByteString>& issuerCrls,
	const IDateTime& lastUpdateTime)
{
	GdsGetTrustListInfo* pGdsGetTrustListInfo = (GdsGetTrustListInfo*)pCookie;

	pGdsGetTrustListInfo->m_getResult = getResult;
	pGdsGetTrustListInfo->m_certificateGroup = certificateGroup;
	pGdsGetTrustListInfo->m_trustedCertificates = trustedCertificates;
	pGdsGetTrustListInfo->m_trustedCrls = trustedCrls;
	pGdsGetTrustListInfo->m_issuerCertificates = issuerCertificates;
	pGdsGetTrustListInfo->m_issuerCrls = issuerCrls;
	pGdsGetTrustListInfo->m_issuerCertificates = issuerCertificates;
	pGdsGetTrustListInfo->m_lastUpdateTime = lastUpdateTime;

	pGdsGetTrustListInfo->m_gdsEvent.signal();
}

EnumStatusCode StructoAutomoldApplication::handleMonitoringChange(const std::vector<Server::BaseNode*>& nodes, const std::vector<Server::MonitoringChange>& changes)
{
	_UNUSED(nodes);
	_UNUSED(changes);
#if TB5_ALARMS

	// _tprintf(_T("Sending %u events\n"), (unsigned int)nodes.size());
	for(size_t i=0; i<nodes.size(); i++)
	{
		CustomCallbackEventPtr cb = CustomCallbackEvent::create(_T("handleMonitoringChange"));

		DateTime time;
		time = DateTime::getUtcNow();
		cb->setReceiveTime(time); // the time when the event occurred (on the device)
		cb->setTime(time);
		Value propertyValue;
		propertyValue.setNodeId(nodes[i]->getNodeId());
		cb->setProperty(QualifiedName(_T("NodeId"), 0), propertyValue);
		propertyValue.setInt32((OTInt32)changes[i].getCurrentState());
		cb->setProperty(QualifiedName(_T("CurrentState"), 0), propertyValue);
		propertyValue.setInt32((OTInt32)changes[i].getPreviousState());
		cb->setProperty(QualifiedName(_T("PreviousState"), 0), propertyValue);
		propertyValue.setInt32((OTInt32)changes[i].getAttributeId());
		cb->setProperty(QualifiedName(_T("AttributeId"), 0), propertyValue);
		propertyValue.setDouble(changes[i].getMinimumSamplingInterval());
		cb->setProperty(QualifiedName(_T("MinimumSamplingInterval"), 0), propertyValue);
		NodeId id(EnumNumericNodeId_Server);
		//_tprintf(_T("   NodeId %s: %s -> %s (%5.1g)\n"), nodes[i]->getNodeId()->toString().c_str(), 
		//	getEnumMonitoringStateString(changes[i].getPreviousState()), getEnumMonitoringStateString(changes[i].getCurrentState()), changes[i].getMinimumSamplingInterval());
		cb->report(&id);
	}
#else
	_UNUSED(nodes);
	_UNUSED(changes);
#endif

	return EnumStatusCode_Good;
}

GdsCreateSignedCertificateInfo::GdsCreateSignedCertificateInfo()
{
	m_gdsEvent.create();
	m_creationResult = EnumStatusCode_Good;
	m_isUserCertificate = false;
}

GdsCreateSignedCertificateInfo::~GdsCreateSignedCertificateInfo()
{

}

GdsGetTrustListInfo::GdsGetTrustListInfo()
{
	m_gdsEvent.create();
	m_getResult = EnumStatusCode_Good;
}

GdsGetTrustListInfo::~GdsGetTrustListInfo()
{

}
