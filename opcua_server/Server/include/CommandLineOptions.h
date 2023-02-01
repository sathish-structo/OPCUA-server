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
#ifndef _COMMANDLINEOPTIONS_H_
#define _COMMANDLINEOPTIONS_H_

#include "Application.h"

// to get EnumImportDuplicateNodeHandling
#include "TestServerAddressSpaceRoot.h"


/**
 * @brief Class for storing command line arguments.
 */
class CommandLineOptions
{
public:
	CommandLineOptions();
	~CommandLineOptions();
	bool read(size_t argc, OTChar* argv[]);
	void showCommands(bool& alreadyShown, const tstring& command) const;
	const std::vector<tstring>& getEndPointURLs() const;
	const tstring& getPathPKI() const;
	const tstring& getPathUserPKI() const;
	const tstring& getAppCertificate() const;
	const tstring& getAppPrivateKey() const;
	const tstring& getAppPrivateKeyPass() const;
	void loadLicense(SoftingOPCToolbox5::Application* pApp) const;
	bool useDiscovery() const;
	const tstring& getDiscoveryServerURL() const;
	const tstring& getDiscoveryCertificate() const;
	bool getCttCompatible() const;
	const tstring& getCttVersion() const;
	bool getRestrictEndpoint() const;
	bool getSkipOwnCertValidation() const;
	OTUInt32 getDemoTime() const;
	EnumRedundancySupport getRedundancySupport() const;
	std::vector<tstring> getRedundantUrls() const;
	const tstring& getTrustListLocation() const;
	const tstring& getIssuerCertificateListLocation() const;
	const tstring& getRevocationListLocation() const;
	const tstring& getRejectedCertificatePath() const;
	EnumCertificateCheck getCertificateCheckOption() const;
	EnumCrlCheckOption getCrlCheckOption() const;
	bool getAllowProxyCerts() const;
	bool getCheckSelfSignedSignature() const;
	const std::vector<tstring>& getNodeSet2XMLFiles() const;
	bool getValidateAddressSpace() const;
	const tstring& getExportFile() const;
	const std::vector<tstring>& getExportNamespaces() const;
	bool getTraceEnabled() const;
	const tstring& getTraceFile() const;
	EnumTraceGroup getTraceGroup() const;
	EnumTraceLevel getTraceLevel() const;
	const tstring& getCallbackTraceFile() const;
	EnumImportDuplicateNodeHandling getImportDuplicateNodeHandling() const;
	bool getAchillesCompat() const;
	bool getModelDesignerEnable() const;
	const tstring& getDhParamFile() const;
	int getEnableECC() const;
	const tstring& getEndpointSecurity1() const;
	const tstring& getEndpointSecurity2() const;
	const tstring& getUserSecurityPolicy1() const;
	const tstring& getUserSecurityPolicy2() const;
	const tstring& getConfigurationFilePath() const;
	const tstring& getTestServerFilesPath() const;
	bool getEnableAccessPattern() const;
	bool getSendOwnCertAsChain() const;
	std::map<EnumServiceType, OTUInt32> getOperationLimits() const;
	const tstring& getGdsUrl() const;
	const tstring& getGdsUser() const;
	const tstring& getGdsPassword() const;
	OTUInt32 getGdsUpdateCycle() const;
	bool getDisableLockout() const;

private:
	/*! Tries to parse the input string as number or enum string and assigns the value on success or reports an error. */
	template<typename T>
	static bool parseEnum(const tstring& commandArgument, T(*pGetEnumFromString)(const OTChar*), T& enumValue)
	{
		T tempVal;

		// try to parse the value as a number (UInt32)
		OTChar* pEnd = NULL;
		const OTChar* pStr = commandArgument.c_str();
		OTUInt32 numberVal = _tcstoul(pStr, &pEnd, 10);
		if(pEnd == pStr)
		{
			// the string is not a number, try to parse as EnumString
			// Note: This will produce an error log on failure
			tempVal = (pGetEnumFromString(pStr));
			// -1 indicates an error during parse
			if(tempVal == (T)-1)
			{
				return false;
			}
		}
		else
		{
			// the string represents a number
			tempVal = (T)numberVal;
		}
		enumValue = tempVal;
		return true;
	}

	/*! Tries to parse the input string as number or enum string and adds the related bits to the currently assigned bits
	*
	* @note This method only works as expected if the initial value is 0 */
	template<typename T>
	static bool parseBitMaskEnum(const tstring& commandArgument, T(*pGetEnumFromString)(const OTChar*), T& enumValue)
	{
		T tempVal;

		// try to parse the value as a number (UInt32)
		OTChar* pEnd = NULL;
		const OTChar* pStr = commandArgument.c_str();
		OTUInt32 numberVal = _tcstoul(pStr, &pEnd, 10);
		if(pEnd == pStr)
		{
			// the string is not a number, try to parse as EnumString
			tempVal = (pGetEnumFromString(pStr));
			// -1 indicates an error during parse, there is no bitmask enum which uses all bits of the data type
			if(tempVal == (T)-1)
			{
				return false;
			}
		}
		else
		{
			// the string represents a number
			tempVal = (T)numberVal;
		}
		// add the new bits to the current value
		enumValue = (T)((int)enumValue | (int)tempVal);
		return true;
	}

	/*! Compares two strings case insensitive */
	static bool stringMatches(const tstring& s1, const tstring& s2);

	static void reportWrongCommandArgument(const tstring& commandName, const tstring& commandArgument);

	/*! End point URl */
	std::vector<tstring> m_endpointUrls;

	/*! time (in ms) after application stops working if no license is activated */
	OTUInt32 m_demoTime;

	/*! The license / license file */
	tstring m_licenseStr;
	tstring m_clientLicenseStr;
	
	/*! The URL to the discovery server used for registration */
	tstring m_discoveryServerURL;

	/*! The certificate file of the discovery server */
	tstring m_discoveryCertificate;

	/*! Relative path to PKI folder */
	tstring m_pathPKI;

	/*! Relative path to user PKI folder */
	tstring m_pathUserPKI;

	/*! If set it will overwrite the hard-coded one */
	tstring m_appCertificate;

	/*! If set it will overwrite the hard-coded one */
	tstring m_appPrivateKey;

	/*! If set it will overwrite the hard-coded one */
	tstring m_appPrivateKeyPass;

	/*! Register the server to the DiscoveryServer? */
	bool m_useDiscovery;

	/*! CTT test compatibility  */
	bool m_cttCompatible;
	tstring m_cttVersion;

	/*! restrict endpoint to specific IP */
	bool m_restrictEndpoint;

	/*! skip validation of own certificate */
	bool m_skipOwnCertValidation;

	/*! Mode of redundancy */
	EnumRedundancySupport m_redundancyMode;

	/*! Array of redundant server URLs */
	std::vector<tstring>m_redundancyUrls;

	/*! Path to the Trust List Location folder */
	tstring m_trustListLocation;
	
	/*! Path to the Issuer Certificate List Location folder */
	tstring m_issuerCertificateListLocation;
	
	/*! Path to the Revocation List Location file or folder */
	tstring m_revocationListLocation;

	/*! The directory where the rejected certificates will be stored */
	tstring m_rejectedCertificatePath;

	/*! Suppressed certificate checks */
	bool m_certificateCheckProvided;
	EnumCertificateCheck m_certificateCheck;
	
	/*! Suppressed crl checks */
	EnumCrlCheckOption m_crlCheck;
	
	/*! All the PKI store to use proxy certificates? */
	bool m_allowProxyCerts;

	/*! Check the signature of self signed certificates? */
	bool m_checkSelfSignedSignature;

	/*! The XML to be imported */
	std::vector<tstring> m_nodeSet2XmlFile;

	/*! flag to validate the AddressSpace */
	bool m_validateAddressSpace;

	std::vector<tstring> m_exportNamespaces;
	tstring m_xmlExportFile;

	/*! Trace settings */
	bool m_traceEnabled;
	tstring m_traceFile;
	bool m_traceGroupProvided;
	EnumTraceGroup m_traceGroup;
	bool m_traceLevelProvided;
	EnumTraceLevel m_traceLevel;
	tstring m_callbackTraceFile;

	EnumImportDuplicateNodeHandling m_importDuplicateNodeHandling;

	/*! flag for Achilles compatibility */
	bool m_achillesCompat;

	/*! flag for ModelDesigner enable*/
	bool m_modelDesignerEnable;

	/*! DH parameter file */
	tstring m_dhParamFile;

	/*! flag for enable ECC */
	int m_enableECC;

	/*! flag for enabling selected SecurityProfiles */
	tstring m_endpointSecurity1;
	tstring m_endpointSecurity2;
	tstring m_userSecurityPolicy1;
	tstring m_userSecurityPolicy2;

	tstring m_configurationFile;

	/*! Relative path to server TestFiles folder */
	tstring m_pathToTestServerFiles;

	bool m_enableAccessPattern;
	bool m_sendOwnCertAsChain;


	std::map<EnumServiceType, OTUInt32> m_operationLimits;

	tstring m_gdsUrl;
	tstring m_gdsUser;
	tstring m_gdsPassword;
	OTUInt32 m_gdsUpdateCycle;

	bool m_disableLockout;

	class CommandReader
	{
	public:
		CommandReader(size_t argc, OTChar* argv[]);

		/*! Checks if the current argument matches to the given command.
		*
		* This increments the internal index on success */
		bool parseCommand(const tstring& command);
		/*! Checks if the current argument matches to the given command and returns the command related parameter
		*
		* This increments the internal index on success */
		bool parseCommand(const tstring& command, tstring& commandArgument);

		/*! Same as parseCommand, but the argument value is part of the command parameter, separated by a colon.
		*
		* Syntax: /<command>:<value> */
		bool parseColonCommand(const tstring& command, tstring& commandArgument);

		/*! Parses the current argument as default parameter, which is not bound to any command.
		*
		* This increments the internal index on success */
		bool parseDefaultArgument(tstring& defaultArgument);

		/*! Retrieves the current command or argument and increments the internal index.
		*
		* Returns false if there are no commands or arguments left */
		bool skipUnknownEntry(tstring& skippedEntry);

		/*! Indicates if there are more arguments to parse */
		bool hasMoreElements() const;

	private:
		bool parseCommandInternal(const tstring& command, size_t index);
		bool parseArgumentInternal(size_t index, tstring& argument);
		bool parseColonCommandInternal(const tstring& command, tstring& argument, size_t index);

		size_t m_numArgs;
		OTChar** m_args;
		size_t m_index;
	};
};

#endif
