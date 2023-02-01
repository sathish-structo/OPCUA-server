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
#ifndef USERINTERFACE_H
#define USERINTERFACE_H

/*! Class to handle the printing and parsing of a command line user interface commands and menus
*
* The user input is provided via tstringstream */
class UserInterface
{
public:
	static const size_t c_commandIndent;
	static const size_t c_commandWidth;
	static const size_t c_paramWidth;
	static const size_t c_maxParams;
	static const size_t c_maxLineWidth;

	static void printHeader(const tstring& menuName = tstring());

	static void printFooter();

	static void printUnknownCommandMessage(const tstring& command, const tstring& menuName = tstring());

	static void printCommand0(const tstring& commandName, const tstring& description);
	static void printCommand1(const tstring& commandName, const tstring& param1, const tstring& description);
	static void printCommand2(const tstring& commandName, const tstring& param1, const tstring& param2, const tstring& description);
	static void printCommand3(const tstring& commandName, const tstring& param1, const tstring& param2, const tstring& param3, const tstring& description);

	static void printMenu(const tstring& menuName, const tstring& description);


	static bool parseCommand0(const tstring& inputCommandName, const tstring& expectedCommandName);

	template<typename P1>
	static bool parseCommand1(const tstring& inputCommandName, tstringstream& parameterStream,
		const tstring& expectedCommandName, bool& parametersValid, P1& param1)
	{
		if(parseCommand0(inputCommandName, expectedCommandName))
		{
			parametersValid = parseParameter(parameterStream, expectedCommandName, 1, param1);
			return parametersValid;
		}
		return false;
	}

	template<typename P1, typename P2>
	static bool parseCommand2(const tstring& inputCommandName, tstringstream& parameterStream,
		const tstring& expectedCommandName, bool& parametersValid, P1& param1, P2& param2)
	{
		if(parseCommand1(inputCommandName, parameterStream, expectedCommandName, parametersValid, param1))
		{
			parametersValid = parseParameter(parameterStream, expectedCommandName, 2, param2);
			return parametersValid;
		}
		return false;
	}

	template<typename P1, typename P2, typename P3>
	static bool parseCommand3(const tstring& inputCommandName, tstringstream& parameterStream,
		const tstring& expectedCommandName, bool& parametersValid, P1& param1, P2& param2, P3& param3)
	{
		if(parseCommand2(inputCommandName, parameterStream, expectedCommandName, parametersValid, param1, param2))
		{
			parametersValid = parseParameter(parameterStream, expectedCommandName, 3, param3);
			return parametersValid;
		}
		return false;
	}

	static bool parseMenu(const tstring& inputCommandName, const tstring& expectedCommandName);

private:
	static void appendCommand(tstring& sourceString, const tstring& command);
	static void appendParam(tstring& sourceString, size_t paramIndex, const tstring& param);
	static void appendDescription(tstring& sourceString, const tstring& description);

	template<typename T>
	static bool parseParameter(tstringstream& parameterStream,
		const tstring& commandName, size_t paramIndex, T& parameter)
	{
		if(!(parameterStream >> parameter))
		{
			_tprintf(_T("Failed to parse parameter %u of command %s\n"), (OTUInt32)paramIndex, commandName.c_str());
			return false;
		}
		return true;
	}

};

// A proxy class which allows to stream a stringstream to an enumeration value
// The compiler cannot distinguish enums from ints, but classes can be distinguished
template<typename T>
class EnumStreamProxy
{
public:
	typedef T(*EnumParserFromString)(const OTChar*);
	
	EnumStreamProxy(T* pValue, EnumParserFromString pEnumParser)
	{
		m_pValue = pValue;
		m_pEnumParser = pEnumParser;
	}

	T* m_pValue;
	EnumParserFromString m_pEnumParser;
};
template<typename T>
inline tstringstream& operator>>(tstringstream& inputStream, EnumStreamProxy<T>& proxy)
{
	tstring s;
	if(inputStream >> s)
	{
		*proxy.m_pValue = (*proxy.m_pEnumParser)(s.c_str());
	}
	return inputStream;
}

EnumUserTokenType oldGetEnumUserTokenTypeFromString(const OTChar* string);

const OTChar* oldGetEnumUserTokenTypeString(EnumUserTokenType userTokenType);

// A proxy class which allows to stream a stringstream to size_t and perform a range check
// This allows to reject the value during the parsing process and simplifies the command handling
class IndexStreamProxy
{
public:
	IndexStreamProxy(size_t* pIndexVar, size_t containerSize, const tstring& name);

	bool validateRange(size_t containerSize = 0) const;

	size_t* m_pIndexVar;
	size_t m_containerSize;
	tstring m_name;
};
tstringstream& operator>>(tstringstream& inputStream, IndexStreamProxy& proxy);


#endif	// USERINTERFACE_H
