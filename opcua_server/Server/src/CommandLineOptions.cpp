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
#include "CommandLineOptions.h"

using namespace SoftingOPCToolbox5;

CommandLineOptions::CommandReader::CommandReader(size_t argc, OTChar* argv[])
{
	m_numArgs = argc;
	m_args = argv;

	// skip index 0, which is the name of the executable
	m_index = 1;
}

bool CommandLineOptions::CommandReader::parseCommand(const tstring& command)
{
	if(parseCommandInternal(command, m_index))
	{
		m_index++;
		return true;
	}
	return false;
}

bool CommandLineOptions::CommandReader::parseCommand(const tstring& command, tstring& commandArgument)
{
	if(parseCommandInternal(command, m_index))
	{
		if(parseArgumentInternal(m_index + 1, commandArgument))
		{
			m_index += 2;
			return true;
		}
		else
		{
			_tprintf(_T("Missing argument for command /%s\n"), command.c_str());
			return false;
		}
	}
	return false;
}

bool CommandLineOptions::CommandReader::parseColonCommand(const tstring& command, tstring& commandArgument)
{
	if(parseColonCommandInternal(command, commandArgument, m_index))
	{
		m_index ++;
		return true;
	}
	return false;
}

bool CommandLineOptions::CommandReader::parseDefaultArgument(tstring& defaultArgument)
{
	if(parseArgumentInternal(m_index, defaultArgument))
	{
		m_index++;
		return true;
	}
	return false;
}

bool CommandLineOptions::CommandReader::skipUnknownEntry(tstring& skippedEntry)
{
	if(hasMoreElements())
	{
		skippedEntry = m_args[m_index];
		m_index++;
		return true;
	}
	// nothing left to skip
	return false;
}

bool CommandLineOptions::CommandReader::hasMoreElements() const
{
	return m_index < m_numArgs;
}

bool CommandLineOptions::stringMatches(const tstring& s1, const tstring& s2)
{
	return (_tcsicmp(s1.c_str(), s2.c_str()) == 0);
}

void CommandLineOptions::reportWrongCommandArgument(const tstring& commandName, const tstring& commandArgument)
{
	_tprintf(_T("\"%s\" is not an allowed value for command %s\n"), commandArgument.c_str(), commandName.c_str());
}

bool CommandLineOptions::CommandReader::parseCommandInternal(const tstring& command, size_t index)
{
	if(index >= m_numArgs)
	{
		return false;
	}
	// commands shall start with the common command identifier
	OTChar* cmd = m_args[index];
	if(cmd[0] != '/' ||
		cmd[1] == '\0')
	{
		// this is no command
		return false;
	}

	if(_tcsicmp(cmd + 1, command.c_str()) == 0)
	{
		return true;
	}
	return false;
}

bool CommandLineOptions::CommandReader::parseArgumentInternal(size_t index, tstring& argument)
{
	if(index >= m_numArgs)
	{
		return false;
	}

	//Note: Only path arguments are allowed to start with a slash, but paths will have more than one slash
	OTChar* arg = m_args[index];
	if((arg[0] == '/' && _tcschr(&arg[1], _T('/')) == NULL) ||
		arg[1] == '\0')
	{
		// this is no valid argument
		return false;
	}

	argument = arg;
	return true;
}

bool CommandLineOptions::CommandReader::parseColonCommandInternal(const tstring& command, tstring& argument, size_t index)
{
	if(index >= m_numArgs)
	{
		return false;
	}
	// commands shall start with the common command identifier
	OTChar* cmd = m_args[index];
	if(cmd[0] != '/' ||
		cmd[1] == '\0')
	{
		// this is no command
		return false;
	}

	size_t offset = 1;	// the "/"
	if(_tcsnicmp(cmd + offset, command.c_str(), command.size()) != 0)
	{
		// a wrong command name
		return false;
	}
	offset += command.size();

	// "cmd" has at least "offset" characters plus a null terminator
	if(cmd[offset] != _T(':'))
	{
		// missing colon or an even longer command name
		return false;
	}
	offset++;

	argument = tstring(cmd).substr(offset);
	return true;
}

CommandLineOptions::CommandLineOptions()
{
	m_endpointUrls.clear();
	m_pathToTestServerFiles.clear();
	m_pathUserPKI.clear();
	m_appCertificate.clear();
	m_appPrivateKey.clear();
	m_appPrivateKeyPass = _T("pass");
	m_licenseStr.clear();
	m_clientLicenseStr.clear();
	m_discoveryCertificate.clear();
	m_useDiscovery = false;
	m_discoveryServerURL.clear();
	m_cttCompatible = false;
	m_cttVersion.clear();
	m_restrictEndpoint = false;
	m_skipOwnCertValidation = false;
	m_demoTime = 0;
	m_redundancyMode = EnumRedundancySupport_None;
	m_redundancyUrls = std::vector<tstring>();
	m_validateAddressSpace = false;
	m_callbackTraceFile.clear();
	m_traceEnabled = false;
	m_traceFile.clear();
	// use the initial value 0 to allow to append bits via bit mask, but assign a default value if this command is not used
	m_traceGroupProvided = false;
	m_traceGroup = 0;
	m_traceLevelProvided = false;
	m_traceLevel = EnumTraceLevel_None;

	PkiStoreConfiguration defaultPkiConfig;

	m_pathPKI = defaultPkiConfig.getBasePath();
	m_trustListLocation = defaultPkiConfig.getCertificateTrustListLocation();
	m_issuerCertificateListLocation = defaultPkiConfig.getIssuerCertificateListLocation();
	m_revocationListLocation = defaultPkiConfig.getCertificateRevocationListLocation();
	m_rejectedCertificatePath = defaultPkiConfig.getRejectedCertificateLocation();
	m_crlCheck = defaultPkiConfig.getRevocationCheckOptions();
	// use the initial value 0 to allow to append bits via bit mask, but assign a default value if this command is not used
	m_certificateCheckProvided = false;
	m_certificateCheck = 0;
	m_checkSelfSignedSignature = defaultPkiConfig.isSelfSignedSignatureChecked();
	m_allowProxyCerts = defaultPkiConfig.areProxyCertificatesAllowed();

	m_importDuplicateNodeHandling = EnumImportDuplicateNodeHandling_Default;
	m_achillesCompat = false;
	m_modelDesignerEnable = true;

	m_enableECC = 0;
	m_endpointSecurity1.clear();
	m_endpointSecurity2.clear();
	m_userSecurityPolicy1.clear();
	m_userSecurityPolicy2.clear();

	m_enableAccessPattern = false;
	m_sendOwnCertAsChain = false;

	m_operationLimits.clear();

	// keep the URL empty by default, this identifies whether we start with or without GDS settings
	// which affects the application type
	m_gdsUrl.clear();
	m_gdsUser = _T("appadmin");
	m_gdsPassword = _T("demo");
	m_gdsUpdateCycle = 0;	// disabled

	m_disableLockout = false;
}

CommandLineOptions::~CommandLineOptions()
{
}

bool CommandLineOptions::read(size_t argc, OTChar* argv[])
{
	CommandReader reader(argc, argv);
	tstring commandArgument;
	static bool usageShown = false;

	while(reader.hasMoreElements())
	{
		if(reader.parseCommand(_T("endpointurl"), commandArgument))
		{
			m_endpointUrls.push_back(commandArgument);
		}
		else if(reader.parseCommand(_T("demo"), commandArgument))
		{
			m_demoTime = _tcstoul(commandArgument.c_str(), NULL, 10);
		}
		else if(reader.parseCommand(_T("lic"), commandArgument))
		{
			m_licenseStr = commandArgument;
		}
		else if(reader.parseCommand(_T("licClient"), commandArgument))
		{
			m_clientLicenseStr = commandArgument;
		}
		else if(reader.parseCommand(_T("pkipath"), commandArgument))
		{
			m_pathPKI = commandArgument;
		}
		else if(reader.parseCommand(_T("userpkipath"), commandArgument))
		{
			m_pathUserPKI = commandArgument;
		}
		else if(reader.parseCommand(_T("appcertificate"), commandArgument))
		{
			m_appCertificate = commandArgument;
		}
		else if(reader.parseCommand(_T("appprivatekey"), commandArgument))
		{
			m_appPrivateKey = commandArgument;
		}
		else if(reader.parseCommand(_T("appprivatekeypass"), commandArgument))
		{
			m_appPrivateKeyPass = commandArgument;
		}
		else if(reader.parseColonCommand(_T("disc"), commandArgument))
		{
			if(stringMatches(commandArgument, _T("ON")))
			{
				m_useDiscovery = true;
			}
			else if(stringMatches(commandArgument, _T("OFF")))
			{
				// pretty useless as this is the default
				m_useDiscovery = false;
			}
			else
			{
				reportWrongCommandArgument(_T("disc"), commandArgument);
			}
		}
		else if(reader.parseCommand(_T("discURL"), commandArgument))
		{
			m_discoveryServerURL = commandArgument;
		}
		else if(reader.parseCommand(_T("discCert"), commandArgument))
		{
			m_discoveryCertificate = commandArgument;
		}
		else if(reader.parseCommand(_T("ctt")))
		{
			m_cttCompatible = true;
			if(m_cttVersion.empty())
			{
				m_cttVersion = _T("1.3");
			}
		}
		else if(reader.parseCommand(_T("cttversion"), commandArgument))
		{
			m_cttCompatible = true;
			m_cttVersion = commandArgument;
		}
		else if(reader.parseCommand(_T("restrictEndpoint")))
		{
			m_restrictEndpoint = true;
		}
		else if(reader.parseCommand(_T("skipOwnCertValidation")))
		{
			m_skipOwnCertValidation = true;
		}
		else if(reader.parseCommand(_T("redmode"), commandArgument))
		{
			m_redundancyMode = getEnumRedundancySupportFromString(commandArgument.c_str());
		}
		else if(reader.parseCommand(_T("redurls"), commandArgument))
		{
			tstring redUrls = commandArgument;

			size_t nextPos, charToRead, pos = 0;
			do
			{
				nextPos = redUrls.find(_T(";"), pos);
				if(nextPos == tstring::npos)
				{
					charToRead = tstring::npos;
				}
				else
				{
					charToRead = nextPos - pos;
				}
				tstring url = redUrls.substr(pos, charToRead);
				m_redundancyUrls.push_back(url);
				pos = nextPos + 1;
			} while(nextPos != tstring::npos);
		}
		else if(reader.parseCommand(_T("trustlistpath"), commandArgument))
		{
			m_trustListLocation = commandArgument;
		}
		else if(reader.parseCommand(_T("issuercertslocation"), commandArgument))
		{
			m_issuerCertificateListLocation = commandArgument;
		}
		else if(reader.parseCommand(_T("revoclistpath"), commandArgument))
		{
			m_revocationListLocation = commandArgument;
		}
		else if(reader.parseCommand(_T("rejectcertspath"), commandArgument))
		{
			m_rejectedCertificatePath = commandArgument;
		}
		else if(reader.parseColonCommand(_T("suppcertcheck"), commandArgument))
		{
			m_certificateCheckProvided = true;	// don't report the default value
			// allow these strings case insensitive for backward compatibility
			if(stringMatches(commandArgument, _T("certperiod")))
			{
				m_certificateCheck |= EnumCertificateCheck_SuppressCertificateValidityPeriodCheck;
			}
			else if(stringMatches(commandArgument, _T("crlperiod")))
			{
				m_certificateCheck |= EnumCertificateCheck_SuppressCrlValidityPeriodCheck;
			}
			else if(stringMatches(commandArgument, _T("crlnotfound")))
			{
				m_certificateCheck |= EnumCertificateCheck_SuppressCrlNotFoundError;
			}
			else if(parseBitMaskEnum(commandArgument, &getEnumCertificateCheckFromString, m_certificateCheck))
			{
				// the value is already assigned by the method call
			}
			else
			{
				reportWrongCommandArgument(_T("suppcertcheck"), commandArgument);
			}
		}
		else if(reader.parseColonCommand(_T("crlcheck"), commandArgument))
		{
			// allow these strings case insensitive for backward compatibility
			if(stringMatches(commandArgument, _T("none")))
			{
				m_crlCheck = EnumCrlCheckOption_None;
			}
			else if(stringMatches(commandArgument, _T("onlyleaf")))
			{
				m_crlCheck = EnumCrlCheckOption_OnlyLeaf;
			}
			else if(stringMatches(commandArgument, _T("exceptselfsigned")))
			{
				m_crlCheck = EnumCrlCheckOption_AllExceptSelfSigned;
			}
			else if(stringMatches(commandArgument, _T("all")))
			{
				m_crlCheck = EnumCrlCheckOption_All;
			}
			else if(parseEnum(commandArgument, &getEnumCrlCheckOptionFromString, m_crlCheck))
			{
				// the value is already assigned by the method call
			}
			else
			{
				reportWrongCommandArgument(_T("crlcheck"), commandArgument);
			}
		}
		else if(reader.parseCommand(_T("allowProxyCert")))
		{
			m_allowProxyCerts = true;
		}
		else if(reader.parseCommand(_T("checkselfsignedsignature")))
		{
			m_checkSelfSignedSignature = true;
		}
		else if(reader.parseCommand(_T("importNodeSet"), commandArgument))
		{
			m_nodeSet2XmlFile.push_back(commandArgument);
		}
		else if(reader.parseCommand(_T("exportNamespace"), commandArgument))
		{
			m_exportNamespaces.push_back(commandArgument);
		}
		else if(reader.parseCommand(_T("exportFile"), commandArgument))
		{
			m_xmlExportFile = commandArgument;
		}
		else if(reader.parseCommand(_T("validateAddressSpace")))
		{
			m_validateAddressSpace = true;
		}
		else if(reader.parseCommand(_T("traceEnabled")))
		{
			m_traceEnabled = true;
		}
		else if(reader.parseCommand(_T("callbackTraceFile"), commandArgument))
		{
			m_callbackTraceFile = commandArgument;
			m_traceEnabled = true;
		}
		else if(reader.parseCommand(_T("traceFile"), commandArgument))
		{
			m_traceFile = commandArgument;
			m_traceEnabled = true;
		}
		else if(reader.parseCommand(_T("traceGroup"), commandArgument))
		{
			m_traceGroupProvided = true;	// don't report the default value
			if(parseBitMaskEnum(commandArgument, &getEnumTraceGroupFromString, m_traceGroup))
			{
				// the value is already assigned by the method call
			}
			else
			{
				reportWrongCommandArgument(_T("traceGroup"), commandArgument);
			}
		}
		else if(reader.parseCommand(_T("traceLevel"), commandArgument))
		{
			m_traceLevelProvided = true;	// don't report the default value
			if(parseBitMaskEnum(commandArgument, &getEnumTraceLevelFromString, m_traceLevel))
			{
				// the value is already assigned by the method call
			}
			else
			{
				reportWrongCommandArgument(_T("traceLevel"), commandArgument);
			}
		}
		else if(reader.parseColonCommand(_T("importDuplicateNode"), commandArgument))
		{
			if(stringMatches(commandArgument, _T("IgnoreImport")))
			{
				m_importDuplicateNodeHandling = EnumImportDuplicateNodeHandling_IgnoreImport;
			}
			else if(stringMatches(commandArgument, _T("RemoveExisting")))
			{
				m_importDuplicateNodeHandling = EnumImportDuplicateNodeHandling_RemoveExisting;
			}
			else
			{
				reportWrongCommandArgument(_T("importDuplicateNode"), commandArgument);
			}
		}
		else if(reader.parseCommand(_T("achilles")))
		{
			m_achillesCompat = true;
		}
		else if(reader.parseColonCommand(_T("modelDesigner"), commandArgument))
		{
			if(stringMatches(commandArgument, _T("enable")))
			{
				m_modelDesignerEnable = true;
			}
			else if(stringMatches(commandArgument, _T("disable")))
			{
				m_modelDesignerEnable = false;
			}
			else
			{
				reportWrongCommandArgument(_T("modelDesigner"), commandArgument);
			}
		}
		else if(reader.parseCommand(_T("dhParamFile"), commandArgument))
		{
			m_dhParamFile = commandArgument;
		}
		else if(reader.parseCommand(_T("ecc")))
		{
			m_enableECC = 1;
		}
		else if(reader.parseCommand(_T("ecx")))
		{
			m_enableECC = 2;
		}
		else if(reader.parseCommand(_T("endpointSecurity1"), commandArgument))
		{
			m_endpointSecurity1 = commandArgument;
		}
		else if(reader.parseCommand(_T("endpointSecurity2"), commandArgument))
		{
			m_endpointSecurity2 = commandArgument;
		}
		else if(reader.parseCommand(_T("userSecurityPolicy1"), commandArgument))
		{
			m_userSecurityPolicy1 = commandArgument;
		}
		else if(reader.parseCommand(_T("userSecurityPolicy2"), commandArgument))
		{
			m_userSecurityPolicy2 = commandArgument;
		}
		else if(reader.parseCommand(_T("config"), commandArgument))
		{
			m_configurationFile = commandArgument;
		}
		else if(reader.parseCommand(_T("serverFilesPath"), commandArgument))
		{
			m_pathToTestServerFiles = commandArgument;
		}
		else if(reader.parseCommand(_T("enableAccessPattern")))
		{
			m_enableAccessPattern = true;
		}
		else if(reader.parseCommand(_T("sendOwnCertAsChain")))
		{
			m_sendOwnCertAsChain = true;
		}
		else if(reader.parseCommand(_T("operationLimit"), commandArgument))
		{
			// Syntax:
			// Browse=500
			// Write=10
			// ...
			size_t separatorPos = commandArgument.find('=');
			if(separatorPos != tstring::npos)
			{
				tstring serviceTypeString = commandArgument.substr(0, separatorPos);
				tstring limitString = commandArgument.substr(separatorPos + 1);

				EnumServiceType serviceType = getEnumServiceTypeFromString(serviceTypeString.c_str());
				OTUInt32 limitValue = _tcstoul(limitString.c_str(), NULL, 10);
				m_operationLimits[serviceType] = limitValue;
			}
		}
		else if(reader.parseCommand(_T("gdsUrl"), commandArgument))
		{
			m_gdsUrl = commandArgument;
		}
		else if(reader.parseCommand(_T("gdsUser"), commandArgument))
		{
			m_gdsUser = commandArgument;
		}
		else if(reader.parseCommand(_T("gdsPassword"), commandArgument))
		{
			m_gdsPassword = commandArgument;
		}
		else if(reader.parseCommand(_T("gdsUpdateCycle"), commandArgument))
		{
			m_gdsUpdateCycle = _tcstoul(commandArgument.c_str(), NULL, 10);
		}
		else if(reader.parseCommand(_T("disableLockout")))
		{
			m_disableLockout = true;
		}
		else if(reader.skipUnknownEntry(commandArgument))
		{
			// unknown command....
			showCommands(usageShown, commandArgument);
		}
		else
		{
			// nothing left to parse
			break;
		}
	}

	// set the the default values to the bit masks if no value was provided
	if(!m_traceGroupProvided)
	{
		m_traceGroup = EnumTraceGroup_All;
	}
	if(!m_traceLevelProvided)
	{
		m_traceLevel = EnumTraceLevel_ErrorAndHigher;
	}
	if(!m_certificateCheckProvided)
	{
		PkiStoreConfiguration defaultPkiStore;
		m_certificateCheck = defaultPkiStore.getSuppressedCertificateChecks();
	}

	return true;
}

/*!
 * @brief Shows available commands for the executable
 * @param alreadyShown Boolean flag if available commands have been already shown
 * @param command Command as argument
 * @return void
 */
void CommandLineOptions::showCommands(bool& alreadyShown, const tstring& command) const
{
	_tprintf(_T("Unexpected command line argument: \"%s\"\n"), command.c_str());
	if(!alreadyShown)
	{
		_tprintf(_T("Command line arguments:\n"));
		_tprintf(_T("/endpointurl <OPC Server URL>\n"));
		_tprintf(_T("   Clients must use this URL for connection to the server.\n"));
		_tprintf(_T("   This parameter can be specified multiple times for multiple endpoints.\n"));
		_tprintf(_T("/pkipath <path to PKI folder>\n"));
		_tprintf(_T("   PKI folder must contain connection certificates related files.\n"));
		_tprintf(_T("/userpkipath <path to user PKI folder>\n"));
		_tprintf(_T("   PKI folder must contain the certificates used for the user authentication.\n"));
		_tprintf(_T("   Default: %s\n"), m_pathUserPKI.c_str());
		_tprintf(_T("/appcertificate <path application certificate>\n"));
		_tprintf(_T("   If set it will overwrite the hard-coded one.\n"));
		_tprintf(_T("/appprivatekey <path application private key>\n"));
		_tprintf(_T("   If set it will overwrite the hard-coded one.\n"));
		_tprintf(_T("/appprivatekeypass <path to application private key password>\n"));
		_tprintf(_T("   If set it will overwrite the hard-coded one.\n"));
		_tprintf(_T("/lic <license>\n"));
		_tprintf(_T("   Server license string.\n"));
		_tprintf(_T("/licClient <license>\n"));
		_tprintf(_T("   Client license string.\n"));
		_tprintf(_T("/disc:[ON/OFF]\n"));
		_tprintf(_T("    Register or do not register the server to the DiscoveryServer.\n"));
		_tprintf(_T("/discURL <DiscoveryServer url>\n"));
		_tprintf(_T("   The address of the Discovery Server used.\n"));
		_tprintf(_T("/discCert <path discovery server cert>\n"));
		_tprintf(_T("   The certificate of the Discovery Server specified by /discURL.\n"));
		_tprintf(_T("/ctt\n"));
		_tprintf(_T("   Try to be compatible to the CTT Test.\n"));
		_tprintf(_T("/cttversion <CTT version>\n"));
		_tprintf(_T("   The used CTT version (different options can be exposed by code)\n"));
		_tprintf(_T("/restrictEndpoint\n"));
		_tprintf(_T("   Restricts all endpoints to the IP in endpoint URL.\n"));
		_tprintf(_T("/skipOwnCertValidation\n"));
		_tprintf(_T("   Do not validate own certificate.\n"));
		_tprintf(_T("/demo\n"));
		_tprintf(_T("   Reduce default demo time (ms). Max is 90 minutes.\n"));
		_tprintf(_T("/redmode <redundancy mode>\n"));
		_tprintf(_T("   The redundancy mode to use.\n"));
		_tprintf(_T("/redurls <URL>[;<URL>]+\n"));
		_tprintf(_T("   List of URLs for redundant servers.\n"));
		_tprintf(_T("/trustlistpath <path to Trust List Location>\n"));
		_tprintf(_T("   If set it will overwrite the hard-coded one.\n"));
		_tprintf(_T("/issuercertslocation <path to Untrust List Location>\n"));
		_tprintf(_T("   Set the path for the Untrust List and enable this certificate validation option.\n"));
		_tprintf(_T("/revoclistpath <path to a Revocation List file or directory>\n"));
		_tprintf(_T("   If set it will overwrite the hard-coded one.\n"));
		_tprintf(_T("/rejectcertspath <path to Rejected Certificate Location>\n"));
		_tprintf(_T("   Set the Rejected Certificate Location and activate the related check.\n"));
		_tprintf(_T("/suppcertcheck:[certperiod/crlperiod/crlnotfound/AllowPartialChain]\n"));
		_tprintf(_T("   Suppress certificate checks.\n"));
		_tprintf(_T("/crlcheck:[none/onlyleaf/exceptselfsigned/all]\n"));
		_tprintf(_T("   Modify the Certificate Revocation List check options.\n"));
		_tprintf(_T("/clientcertintrustlistreq\n"));
		_tprintf(_T("   Enables the certificate validation option that the client certificate must be in the Trust List.\n"));
		_tprintf(_T("/allowProxyCert\n"));
		_tprintf(_T("   Enables the certificate validation option to allow proxy certificates.\n"));
		_tprintf(_T("/checkselfsignedsignature\n"));
		_tprintf(_T("   Enables the certificate validation option check the signature of self signed certificates.\n"));
		_tprintf(_T("/importNodeSet <NodeSet 2 XML file>\n"));
		_tprintf(_T("   Imports the given file and suppresses build-in creation of default nodes.\n"));
		_tprintf(_T("/validateAddressSpace\n"));
		_tprintf(_T("   Validate address space.\n"));
		_tprintf(_T("/exportFile <NodeSet 2 XML file>\n"));
		_tprintf(_T("   The specified namespaces are exported to this file.\n"));
		_tprintf(_T("/exportNamespace \n"));
		_tprintf(_T("   Specify a namespace URI to be exported (can be used multiple times).\n"));
		_tprintf(_T("/traceEnabled\n"));
		_tprintf(_T("   Enable tracing.\n"));
		_tprintf(_T("/traceFile <traceFile>\n"));
		_tprintf(_T("   File tracing to this file.\n"));
		_tprintf(_T("/traceGroup <traceGroup>\n"));
		_tprintf(_T("   Specify trace group.\n"));
		_tprintf(_T("/traceLevel <traceLevel>\n"));
		_tprintf(_T("   Specify trace level.\n"));
		_tprintf(_T("/callbackTraceFile\n"));
		_tprintf(_T("   Specify callback trace file and enable callback tracing\n"));
		_tprintf(_T("/importDuplicateNode:IgnoreImport\n"));
		_tprintf(_T("   Ignore duplicate nodes during import\n"));
		_tprintf(_T("/importDuplicateNode:RemoveExisting\n"));
		_tprintf(_T("   Remove the existing node when importing a duplicate node\n"));
		_tprintf(_T("/achilles\n"));
		_tprintf(_T("   For testing with Achilles\n"));
		_tprintf(_T("/modelDesigner:enable or disable\n"));
		_tprintf(_T("   For enable or disable modelDesigner (default = enable\n"));
		_tprintf(_T("/dhParamFile <path to DH Param File Location>\n"));
		_tprintf(_T("   Set the DH Param File Location\n"));
		_tprintf(_T("/ecc\n"));
		_tprintf(_T("   Enable the elliptic curve security\n"));
		_tprintf(_T("/ecx\n"));
		_tprintf(_T("   Enable the elliptic curve security using curve25519 and curve448\n"));
		_tprintf(_T("/endpointSecurity1 <policyUri>\n"));
		_tprintf(_T("/endpointSecurity2 <policyUri>\n"));
		_tprintf(_T("/userSecurityPolicy1 <policyUri>\n"));
		_tprintf(_T("/userSecurityPolicy2 <policyUri>\n"));
		_tprintf(_T("   Specify endpoint/user Security Profiles to use.\n"));
		_tprintf(_T("   Default: Aes128_Sha256_nistP256 when /ecc is in effect,\n"));
		_tprintf(_T("            otherwise a fixed set of RSA profiles are used.\n"));
		_tprintf(_T("/config <configationfile>\n"));
		_tprintf(_T("   Load configuration settings from an XML file.\n"));
		_tprintf(_T("/serverFilesPath <path to server files>\n"));
		_tprintf(_T("/enableAccessPattern\n"));
		_tprintf(_T("/sendOwnCertAsChain\n"));
		_tprintf(_T("/operationLimit <ServiceType>=<value>\n"));
		_tprintf(_T("/gdsUrl <GdsEndpointUrl>\n"));
		_tprintf(_T("/gdsUser <Username>\n"));
		_tprintf(_T("/gdsPassword <Password>\n"));
		_tprintf(_T("/gdsUpdateCycle <milliseconds>\n"));
		_tprintf(_T("/disableLockout\n"));
		_tprintf(_T("\n"));

		alreadyShown = true;
	}
}

const std::vector<tstring>& CommandLineOptions::getEndPointURLs() const
{
	return m_endpointUrls;
}

OTUInt32 CommandLineOptions::getDemoTime() const
{
	return m_demoTime;
}

const tstring& CommandLineOptions::getPathPKI() const
{
	return m_pathPKI;
}

const tstring& CommandLineOptions::getPathUserPKI() const
{
	return m_pathUserPKI;
}

const tstring& CommandLineOptions::getAppCertificate() const
{
	return m_appCertificate;
}

const tstring& CommandLineOptions::getAppPrivateKey() const
{
	return m_appPrivateKey;
}

const tstring& CommandLineOptions::getAppPrivateKeyPass() const
{
	return m_appPrivateKeyPass;
}

const tstring& CommandLineOptions::getDiscoveryServerURL() const
{
	return m_discoveryServerURL;
}

const tstring& CommandLineOptions::getDiscoveryCertificate() const
{
	return m_discoveryCertificate;
}

bool CommandLineOptions::useDiscovery() const
{
	return m_useDiscovery;
}

bool CommandLineOptions::getCttCompatible() const
{
	return m_cttCompatible;
}

const tstring& CommandLineOptions::getCttVersion() const
{
	return m_cttVersion;
}

bool CommandLineOptions::getRestrictEndpoint() const
{
	return m_restrictEndpoint;
}

bool CommandLineOptions::getSkipOwnCertValidation() const
{
	return m_skipOwnCertValidation;
}

void CommandLineOptions::loadLicense(SoftingOPCToolbox5::Application* pApp) const
{
	// check if the license is set
	if (m_clientLicenseStr.length() != 0)
	{
		if (pApp->activateLicense(EnumFeature_Client, m_clientLicenseStr) != EnumStatusCode_Good)
		{
			_tprintf(_T("The client license is not valid: %s\n"), m_clientLicenseStr.c_str());
		}
	}
	if (m_licenseStr.length() != 0)
	{
		if (pApp->activateLicense(EnumFeature_Server, m_licenseStr) != EnumStatusCode_Good)
		{
			_tprintf(_T("The server license is not valid: %s\n"), m_licenseStr.c_str());
		}
	}
}

EnumRedundancySupport CommandLineOptions::getRedundancySupport() const
{
	return m_redundancyMode;
}

std::vector<tstring> CommandLineOptions::getRedundantUrls() const
{
	return m_redundancyUrls;
}

const tstring& CommandLineOptions::getTrustListLocation() const
{
	return m_trustListLocation;
}

const tstring& CommandLineOptions::getIssuerCertificateListLocation() const
{
	return m_issuerCertificateListLocation;
}

const tstring& CommandLineOptions::getRevocationListLocation() const
{
	return m_revocationListLocation;
}

const tstring& CommandLineOptions::getRejectedCertificatePath() const
{
	return m_rejectedCertificatePath;
}

EnumCertificateCheck CommandLineOptions::getCertificateCheckOption() const
{
	return m_certificateCheck;
}

EnumCrlCheckOption CommandLineOptions::getCrlCheckOption() const
{
	return m_crlCheck;
}

bool CommandLineOptions::getAllowProxyCerts() const
{
	return m_allowProxyCerts;
}

bool CommandLineOptions::getCheckSelfSignedSignature() const
{
	return m_checkSelfSignedSignature;
}

const std::vector<tstring>& CommandLineOptions::getNodeSet2XMLFiles() const
{
	return m_nodeSet2XmlFile;
}

bool CommandLineOptions::getValidateAddressSpace() const
{
	return m_validateAddressSpace;
}

const std::vector<tstring>& CommandLineOptions::getExportNamespaces() const
{
	return m_exportNamespaces;
}

const tstring& CommandLineOptions::getExportFile() const
{
	return m_xmlExportFile;
}

bool CommandLineOptions::getTraceEnabled() const
{
	return m_traceEnabled;
}

const tstring& CommandLineOptions::getTraceFile() const
{
	return m_traceFile;
}

EnumTraceGroup CommandLineOptions::getTraceGroup() const
{
	return m_traceGroup;
}

EnumTraceLevel CommandLineOptions::getTraceLevel() const
{
	return m_traceLevel;
}

const tstring& CommandLineOptions::getCallbackTraceFile() const
{
	return m_callbackTraceFile;
}

EnumImportDuplicateNodeHandling CommandLineOptions::getImportDuplicateNodeHandling() const
{
	return m_importDuplicateNodeHandling;
}

bool CommandLineOptions::getAchillesCompat() const
{
	return m_achillesCompat;
}

bool CommandLineOptions::getModelDesignerEnable() const
{
	return m_modelDesignerEnable;
}

const tstring& CommandLineOptions::getDhParamFile() const
{
	return m_dhParamFile;
}

int CommandLineOptions::getEnableECC() const
{
	return m_enableECC;
}

const tstring& CommandLineOptions::getEndpointSecurity1() const
{
	return m_endpointSecurity1;
}

const tstring& CommandLineOptions::getEndpointSecurity2() const
{
	return m_endpointSecurity2;
}

const tstring& CommandLineOptions::getUserSecurityPolicy1() const
{
	return m_userSecurityPolicy1;
}

const tstring& CommandLineOptions::getUserSecurityPolicy2() const
{
	return m_userSecurityPolicy2;
}

const tstring& CommandLineOptions::getConfigurationFilePath() const
{
	return m_configurationFile;
}

std::map<EnumServiceType, OTUInt32> CommandLineOptions::getOperationLimits() const
{
	return m_operationLimits;
}

const tstring& CommandLineOptions::getGdsUrl() const
{
	return m_gdsUrl;
}

const tstring& CommandLineOptions::getGdsUser() const
{
	return m_gdsUser;
}

const tstring& CommandLineOptions::getGdsPassword() const
{
	return m_gdsPassword;
}

OTUInt32 CommandLineOptions::getGdsUpdateCycle() const
{
	return m_gdsUpdateCycle;
}

bool CommandLineOptions::getDisableLockout() const
{
	return m_disableLockout;
}

/*!
 * @brief Getter for the relative path to server TestFiles folder
 */
const tstring& CommandLineOptions::getTestServerFilesPath() const
{
	return m_pathToTestServerFiles;
}

bool CommandLineOptions::getEnableAccessPattern() const
{
	return m_enableAccessPattern;

}

bool CommandLineOptions::getSendOwnCertAsChain() const
{
	return m_sendOwnCertAsChain;
}
