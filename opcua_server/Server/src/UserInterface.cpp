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
#include "UserInterface.h"

//0         10        20        30        40        50        60        70        80
//012345678901234567890123456789012345678901234567890123456789012345678901234567890
//[  ][     cmd  ][    p1     ][    p2     ][    p3     ][          desc          ]
//    myCommand   <myParam1>   <myParam2>   <myParam3>   description of myCommand
const size_t UserInterface::c_commandIndent = 4;
const size_t UserInterface::c_commandWidth = 12;
const size_t UserInterface::c_paramWidth = 13;
const size_t UserInterface::c_maxParams = 3;
const size_t UserInterface::c_maxLineWidth = 80;	// terminals usually have a width of 80 or more (80 was the width of punch cards) 

void UserInterface::printHeader(const tstring& menuName /* = tstring() */)
{
	tstring s = _T("Usage: [SubMenu1] ... [SubMenuN] <Command | Menu> [<Parameter1> ... <ParameterN>]\n")
		_T("Available commands");
	if(!menuName.empty())
	{
		s.append(_T(" of menu "));
		s.append(menuName);
	}
	s.append(_T(":"));
	_tprintf(_T("%s\n"), s.c_str());
}

void UserInterface::printFooter()
{
	_tprintf(_T("\n"));
}

void UserInterface::printUnknownCommandMessage(const tstring& command, const tstring& menuName /* = tstring() */)
{
	tstring s = _T("Unknown command ");
	s.append(command);
	if(!menuName.empty())
	{
		s.append(_T(" for menu "));
		s.append(menuName);
	}
	_tprintf(_T("%s\n"), s.c_str());
}

void UserInterface::printCommand0(const tstring& commandName, const tstring& description)
{
	tstring s;
	appendCommand(s, commandName);
	appendDescription(s, description);
	_tprintf(_T("%s\n"), s.c_str());
}

void UserInterface::printCommand1(const tstring& commandName, const tstring& param1, const tstring& description)
{
	tstring s;
	appendCommand(s, commandName);
	appendParam(s, 1, param1);
	appendDescription(s, description);
	_tprintf(_T("%s\n"), s.c_str());
}

void UserInterface::printCommand2(const tstring& commandName, const tstring& param1, const tstring& param2, const tstring& description)
{
	tstring s;
	appendCommand(s, commandName);
	appendParam(s, 1, param1);
	appendParam(s, 2, param2);
	appendDescription(s, description);
	_tprintf(_T("%s\n"), s.c_str());
}

void UserInterface::printCommand3(const tstring& commandName, const tstring& param1, const tstring& param2, const tstring& param3, const tstring& description)
{
	tstring s;
	appendCommand(s, commandName);
	appendParam(s, 1, param1);
	appendParam(s, 2, param2);
	appendParam(s, 3, param3);
	appendDescription(s, description);
	_tprintf(_T("%s\n"), s.c_str());
}

void UserInterface::printMenu(const tstring& menuName, const tstring& description)
{
	return printCommand0(menuName, description);
}

bool UserInterface::parseCommand0(const tstring& inputCommandName, const tstring& expectedCommandName)
{
	// use an explicit if-else to easily set breakpoints
	if(_tcsicmp(inputCommandName.c_str(), expectedCommandName.c_str()) == 0)
	{
		return true;
	}
	return false;
}

bool UserInterface::parseMenu(const tstring& inputCommandName, const tstring& expectedCommandName)
{
	return parseCommand0(inputCommandName, expectedCommandName);
}

void UserInterface::appendCommand(tstring& sourceString, const tstring& command)
{
	sourceString.reserve(c_maxLineWidth);
	sourceString.resize(c_commandIndent, _T(' '));
	sourceString.append(command);
}

void UserInterface::appendParam(tstring& sourceString, size_t paramIndex, const tstring& param)
{
	size_t pos = c_commandIndent + c_commandWidth + ((paramIndex - 1) * c_paramWidth);
	sourceString.resize(pos, _T(' '));
	sourceString.append(_T("<"));
	sourceString.append(param);
	sourceString.append(_T(">"));
}

void UserInterface::appendDescription(tstring& sourceString, const tstring& description)
{
	sourceString.resize(c_commandIndent + c_commandWidth + c_maxParams * c_paramWidth, _T(' '));
	sourceString.append(description);
}

EnumUserTokenType oldGetEnumUserTokenTypeFromString(const OTChar* string)
{
	EnumUserTokenType token = EnumUserTokenType_Anonymous;
	tstring s = string;
	if(s == _T("Anonymous"))
	{
		token = EnumUserTokenType_Anonymous;
	}
	else if(s == _T("UserName"))
	{
		token = EnumUserTokenType_UserName;
	}
	else if(s == _T("Certificate"))
	{
		token = EnumUserTokenType_Certificate;
	}
	else if(s == _T("IssuedToken"))
	{
		token = EnumUserTokenType_IssuedToken;
	}
	return token;
}

const OTChar* oldGetEnumUserTokenTypeString(EnumUserTokenType userTokenType)
{
	switch(userTokenType)
	{
	case EnumUserTokenType_Anonymous:
		return _T("Anonymous");
	case EnumUserTokenType_UserName:
		return _T("UserName");
	case EnumUserTokenType_Certificate:
		return _T("Certificate");
	case EnumUserTokenType_IssuedToken:
		return _T("IssuedToken");
	default:
		return _T("Invalid");
	}
}

IndexStreamProxy::IndexStreamProxy(size_t* pIndexVar, size_t containerSize, const tstring& name)
{
	m_pIndexVar = pIndexVar;
	m_containerSize = containerSize;
	m_name = name;
}

bool IndexStreamProxy::validateRange(size_t containerSize /* = 0 */) const
{
	// a non-default value means it is used during manual validation
	if(containerSize == 0)
	{
		containerSize = m_containerSize;
	}
	if(*m_pIndexVar >= containerSize)
	{
		// e.g. "endpointIndex 8 is out of range (0-2)"
		if(containerSize > 0)
		{
			_tprintf(_T("%s %u is out of range (0-%u)\n"), m_name.c_str(), (OTUInt32)*m_pIndexVar, (OTUInt32)(containerSize - 1));
		}
		else
		{
			_tprintf(_T("%s %u is out of range (none)\n"), m_name.c_str(), (OTUInt32)*m_pIndexVar);
		}
		return false;
	}
	return true;
}

tstringstream& operator>>(tstringstream& inputStream, IndexStreamProxy& proxy)
{
	size_t* pIndexVar = proxy.m_pIndexVar;
	if(inputStream >> (*pIndexVar))
	{
		if(!proxy.validateRange())
		{
			// return the error for the stream operation
			inputStream.setstate(tstringstream::failbit);
		}
	}
	return inputStream;
}
