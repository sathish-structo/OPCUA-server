/*****************************************************************************
*                                                                            *
*                     SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                          *
*                                D-85540 Haar                                *
*                        Phone: ++49-89-4 56 56-0                            *
*                          Fax: ++49-89-4 56 56-3 99                         *
*                                                                            *
*                            SOFTING CONFIDENTIAL                            *
*                                                                            *
*                     Copyright (C) SOFTING IA GmbH 2022                     *
*                             All Rights Reserved                            *
*                                                                            *
* NOTICE: All information contained herein is, and remains the property of   *
*   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- *
*   lectual and technical concepts contained herein are proprietary to       *
*   SOFTING Industrial Automation GmbH and its suppliers and may be covered  *
*   by German and Foreign Patents, patents in process, and are protected by  *
*   trade secret or copyright law. Dissemination of this information or      *
*   reproduction of this material is strictly forbidden unless prior         *
*   written permission is obtained from SOFTING Industrial Automation GmbH.  *
******************************************************************************
******************************************************************************
*                                                                            *
* PROJECT_NAME             OPC UA C++ Server & Client SDK                    *
*                                                                            *
* VERSION                  6.00.0                                            *
*                                                                            *
* DATE                     13.6.2022                                            *
*                                                                            *
*****************************************************************************/

#ifndef _PkiStoreConfigurationStruct_H_
#define _PkiStoreConfigurationStruct_H_

#ifdef OT_COMPILATION
#include "opcua_p_pki.h"
#endif

#ifdef SOOS_WINDOWS
#pragma pack(push, 8)
#endif

struct TrustListDataTypeStruct;
struct DateTimeStruct;

//lint -sem(PkiStoreConfigurationStruct::clear, cleanup)
//lint -sem(PkiStoreConfigurationStruct::init, initializer)
typedef struct TBCORE_IF PkiStoreConfigurationStruct
{
	static const OTChar* c_trustedFolderName;
	static const OTChar* c_issuerFolderName;
	static const OTChar* c_trustedCrlsFolderName;
	static const OTChar* c_issuerCrlsFolderName;
	static const OTChar* c_rejectedFolderName;
	static const OTChar* c_backupFolderName;
	static const OTChar* c_tempBackupFolderName;

	/*! The path where the subdirectories for all other PKI directories are stored */
	OTChar* pBasePath;

	/*! The path to the folder containing trusted client instance certificates
	 * (or the certificate of an issuer in the certificate chain) */
	OTChar* pCertificateTrustListLocation;

	/*! The path to the folder containing certificates needed for validating a
	 * certificate chain to the root certificate. The certificates (and other certificates
	 * issued by those) are not automatically trusted. This parameter can be an empty string
	 * if addUntrustedCertificatesToChainValidation is set to false. */
	OTChar* pCertificateUntrustedListLocation;

	/*! The path to the certificate revocation list. The path can be either a specific file
	 * with suffix .crl or a folder containing those files. This parameter can be an empty
	 * string if no CRL checks are specified. */
	OTChar* pCertificateRevocationListLocation;

	/*! The optional path where the CRLs of the issuers (untrustedList) are stored. */
	OTChar* pIssuerRevocationListLocation;

	/*! The path to the folder where rejected client certificates will be stored
	 * This parameter can be an empty string (e.g. in case of read-only file systems).
	 * Rejected certificates are not stored in this case. */
	OTChar* pRejectedCertificateLocation;

	/*! Bit field for validation errors that shall be ignored. */
	EnumCertificateCheck ignoredCertificateChecks;

	/*! The definition how revocation checks shall be done. */
	EnumCrlCheckOption revocationCheckOption;

	/*! A proxy certificate is a certificate which is not a CA but was used to issue other certificates.
	 * This is an usual way to issue certificates, but sometimes necessary in the case that a global root
	 * CA refuses issuing other CA certificates. With this flag set to false the certificate chain
	 * validation will fail if such a proxy certificate is within the chain. Allowing proxy certificates
	 * on the other hand might downgrade security. */
	bool allowProxyCertificates;

	/*! Indicates whether the signature of self-signed certificates shall be verified. */
	bool checkSelfSignedSignature;

	/*! Indicates whether the key usage of application certificates shall be checked. */
	bool enableStrictKeyUsageCheck;

	/*! Check whether URI in certificate matches URI in application description. */
	bool checkUrlMatch;

	PkiStoreConfigurationStruct();
	PkiStoreConfigurationStruct(const PkiStoreConfigurationStruct&);
	~PkiStoreConfigurationStruct(void);

	void initialize_API();

	EnumStatusCode set(const PkiStoreConfigurationStruct&);
	EnumStatusCode get(PkiStoreConfigurationStruct&) const;
#ifdef OT_COMPILATION
	EnumStatusCode get(OpcUa_P_OpenSSL_CertificateStore_Config& pkiConfig) const;
#endif

	const PkiStoreConfigurationStruct& operator=(const PkiStoreConfigurationStruct& cp);
	bool operator==(const PkiStoreConfigurationStruct& toCompare) const;
	bool operator!=(const PkiStoreConfigurationStruct& toCompare) const;
	bool operator<(const PkiStoreConfigurationStruct& toCompare) const;
	bool operator>(const PkiStoreConfigurationStruct& toCompare) const;

	int compare(const PkiStoreConfigurationStruct* pTarget) const;

	void clear(void);
	EnumStatusCode setBasePath(const OTChar* pPath);
	EnumStatusCode setCertificateTrustListLocation(const OTChar* pPath);
	EnumStatusCode setCertificateUntrustedListLocation(const OTChar* pPath);
	EnumStatusCode setCertificateRevocationListLocation(const OTChar* pPath);
	EnumStatusCode setIssuerRevocationListLocation(const OTChar* pPath);
	EnumStatusCode setRejectedCertificateLocation(const OTChar* pPath);

	EnumStatusCode getTrustedCertificates(std::vector<ByteStringStruct>& certificates) const;
	EnumStatusCode getIssuerCertificates(std::vector<ByteStringStruct>& certificates) const;
	EnumStatusCode getTrustedCrls(std::vector<ByteStringStruct>& crls) const;
	EnumStatusCode getIssuerCrls(std::vector<ByteStringStruct>& crls) const;
	EnumStatusCode getRejectedCertificates(std::vector<ByteStringStruct>& certificates) const;
	EnumStatusCode getFileContentsFromFolder(std::vector<ByteStringStruct>& content, const OTChar* folder, const OTChar* extension, bool isCrl) const;

	DateTimeStruct calculateLastUpdateTime() const;

	/*! Validates whether the provided paths are valid and match the configuration flags
	*
	* This is invoked only when configuring PKI store at application, endpoint or client session. */
	EnumStatusCode validatePaths(bool validateForSecurity) const;

	/*! Generates the names for the PKI sub directories
	*
	* @note The base path must be valid before invoking this method */
	void generateAllPathStringsFromBase();

	/*! Creates all PKI subdirectories if they don't exist yet */
	EnumStatusCode validateOrCreateAllPkiDirectories(bool validateForSecurity) const;

	/*! Converts potential relative paths to absolute paths
	*
	* @note This is only required where the PKI store is provided from the config file where the current dir is expected to be different */
	EnumStatusCode convertPathsToAbsolutePaths();

	EnumStatusCode validateCertificate(const ByteStringStruct& certificate,
		const OTChar* applicationUri,
		bool temporaryCompatibiltyFlag = false /* This flag should be removed again */) const;

	/*! Creates a backup of the current PKI store files
	*
	* The backup can be created while the PKI store is in use */
	EnumStatusCode createBackup() const;
	/*! Restores and removes a previously created backup if any, otherwise does nothing
	*
	* This method can be invoked in any case to restore the backup or to cleanup any pending temporary backup
	* during the initialization phase.
	*
	* @note The PKI store needn't be in use during this operation */
	EnumStatusCode restoreFromBackup() const;
	/*! Deletes a previously created backup or temporary backup */
	EnumStatusCode deleteBackup() const;

#ifdef OT_COMPILATION
	// core-only methods

	/*! If the DH param file is set, it updates the selected CertificateStore config to
	* use this DH param file. */
	static EnumStatusCode setDHParamFileToOpenSSLConfig(OpcUa_P_OpenSSL_CertificateStore_Config& pkiConfig, const tstring& dhParamFileLocation);


	/*! Replaces all contents of all trust-related PKI folders with new files created from the TrustListDataType value.
	*
	* @note Before calling this method the caller has to ensure that the PKI store is not in use
	* @note The caller has to take care to create and restore the backup as creating the backup can be done while the PKI store is in use. */
	EnumStatusCode replaceTrustFiles(const TrustListDataTypeStruct& newContent) const;

	// internal helpers
	static EnumStatusCode createPkiDirectory(const OTChar* pPath, bool reportWarningIfExist = false);
	static EnumStatusCode validateSubPath(const OTChar* pPath, const OTChar* pPathToBase, const OTChar* pSubDirectory, bool mandatory);
	static tstring generatePathStringFromBase(const OTChar* pPathToBase, const OTChar* pSubDirectory);
	static EnumStatusCode createTempBackupFolders(const tstring& tempBackupRootFolder,
		tstring& generatedTrustFolder,
		tstring& generatedIssuerFolder,
		tstring& generatedTrustedCrlsFolder,
		tstring& generatedIssuerCrlsFolder);
	static EnumStatusCode deleteFolder(const tstring& tempBackupRootFolder);
	EnumStatusCode clearTrustRelatedFolders(EnumTrustListMasks specifiedFields) const;

	/*! Checks if the provided TrustList content is the same as the current content of the PKI store
	*
	* pExtraTrustedCert is a workaround for the .Net Sample GDS */
	EnumStatusCode isSamePkiContent(const TrustListDataTypeStruct& newContent, bool& same, const ByteStringStruct* pExtraTrustedCert = NULL) const;

	/* compares if the certificates of the two folder contents are the same, the order does not matter
	*
	* pExtraCert is a workaround for the .Net sample GDS */
	static bool isSameFolderContent(OTUInt32 firstFolderContentCount, const ByteStringStruct* pFirstFolderContent,
		OTUInt32 secondFolderContentCount, const ByteStringStruct* pSecondFolderContent, const ByteStringStruct* pExtraCert = NULL);
	static bool isSameFolderContent(const std::vector<ByteStringStruct>& firstFolderContent,
		OTUInt32 secondFolderContentCount, const ByteStringStruct* pSecondFolderContent, const ByteStringStruct* pExtraCert = NULL);

#endif // OT_COMPILATION

} PkiStoreConfigurationStruct;

#ifdef SOOS_WINDOWS
#pragma pack(pop)
#endif

#endif
