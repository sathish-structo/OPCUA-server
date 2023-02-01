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
#pragma once

// Allow use of insecure CRT functions
#if defined _MSC_VER || defined _WIN32_WCE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <cstdlib>
using namespace std;

//-----------------------------------------------------------------------------
// Operating systems and default features                                     |
//-----------------------------------------------------------------------------

#include "Toolbox.h"

#ifdef SOOS_WINDOWS
#ifndef _WIN32_WCE
#include <conio.h>
#endif
#endif

#include <stack>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>

#include "OTTypes.h"
#include "ByteString.h"

// #define DEBUG_SIMCHANGES 1
