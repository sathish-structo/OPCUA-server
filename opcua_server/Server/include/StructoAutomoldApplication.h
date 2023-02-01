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
#ifndef TEST_APPLICATION_H
#define TEST_APPLICATION_H

#include "Application.h"
#include "Util_Event.h"

class GdsCreateSignedCertificateInfo
{
public:
	GdsCreateSignedCertificateInfo();
	virtual ~GdsCreateSignedCertificateInfo();
	Util_Event m_gdsEvent;
	EnumStatusCode m_creationResult;
	SoftingOPCToolbox5::ByteString m_certificate;
	SoftingOPCToolbox5::ByteString m_privateKey;
	tstring m_privateKeyPassword;
	bool m_isUserCertificate;
	SoftingOPCToolbox5::NodeId m_certificateGroup;
	std::vector<SoftingOPCToolbox5::ByteString> m_issuerCertificates;
	SoftingOPCToolbox5::NodeId m_pendingRequestId;
};

class GdsGetTrustListInfo
{
public:
	GdsGetTrustListInfo();
	virtual ~GdsGetTrustListInfo();
	Util_Event m_gdsEvent;
	SoftingOPCToolbox5::NodeId m_certificateGroup;
	EnumStatusCode m_getResult;
	std::vector<SoftingOPCToolbox5::ByteString> m_trustedCertificates;
	std::vector<SoftingOPCToolbox5::ByteString> m_trustedCrls;
	std::vector<SoftingOPCToolbox5::ByteString> m_issuerCertificates;
	std::vector<SoftingOPCToolbox5::ByteString> m_issuerCrls;
	SoftingOPCToolbox5::DateTime m_lastUpdateTime;
};

class StructoAutomoldApplication : public SoftingOPCToolbox5::Application
{
public:
	virtual EnumStatusCode handleMonitoringChange(const std::vector<SoftingOPCToolbox5::Server::BaseNode*>& nodes, const std::vector<SoftingOPCToolbox5::Server::MonitoringChange>& changes);
	StructoAutomoldApplication(TBHandle otbHandle);

	virtual void gdsCreateSignedCertificateCompleted(
		void* pCookie,
		EnumStatusCode creationResult,
		const SoftingOPCToolbox5::IByteString& certificate,
		const SoftingOPCToolbox5::IByteString& privateKey,
		const tstring& privateKeyPassword,
		bool isUserCertificate,
		const SoftingOPCToolbox5::INodeId& certificateGroup,
		const std::vector<SoftingOPCToolbox5::ByteString> issuerCertificates,
		const SoftingOPCToolbox5::INodeId& pendingRequestId);

	virtual void gdsGetTrustListCompleted(
		void* pCookie,
		const SoftingOPCToolbox5::INodeId& certificateGroup,
		EnumStatusCode getResult,
		const std::vector<SoftingOPCToolbox5::ByteString>& trustedCertificates,
		const std::vector<SoftingOPCToolbox5::ByteString>& trustedCrls,
		const std::vector<SoftingOPCToolbox5::ByteString>& issuerCertificates,
		const std::vector<SoftingOPCToolbox5::ByteString>& issuerCrls,
		const SoftingOPCToolbox5::IDateTime& lastUpdateTime);

};

#endif //TEST_APPLICATION_H
