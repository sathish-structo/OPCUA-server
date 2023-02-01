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
#include "Util_Timer.h"

#ifdef __VXWORKS__
#include <version.h>
#endif

/*! @brief Constructor
 */
Util_Timer::Util_Timer():
	m_startTickCount(0),
	m_stopTickCount(0),
	m_isRunning(false)
{
}

/*! @brief Destructor
 */
Util_Timer::~Util_Timer()
{
}

unsigned long Util_Timer::currentTickCount()
{
#ifdef SOOS_POSIX
	struct timespec now;
	int getTimeResult = 0;
#if defined (CLOCK_MONOTONIC)
	clock_gettime(CLOCK_MONOTONIC,&now);
#else
	// monotonic clock is not supported - use real time clock
	clock_gettime(CLOCK_REALTIME,&now);
#endif
	if(getTimeResult != 0)
	{
#if defined(_WRS_VXWORKS_MAJOR) && ((_WRS_VXWORKS_MAJOR < 6) || ((_WRS_VXWORKS_MAJOR == 6) && (_WRS_VXWORKS_MINOR <= 8)))
		// VxWorks 6.8 does not support gettimeofday
		_REPORT_ASSERTION(_T("clock_gettime failed"));
#else
		struct timeval timevalnow;
		gettimeofday(&timevalnow, NULL);
		now.tv_sec = timevalnow.tv_sec;
		now.tv_nsec = 1000 * timevalnow.tv_usec;
#endif
	}
	return now.tv_sec*1000 + now.tv_nsec/1000000;
#endif

#ifdef SOOS_WINDOWS
	return GetTickCount();
#endif
}

#ifdef SOOS_WINDOWS
// Windows CE and Windows XP do not support GetTickCount64
static OTUInt64 UtilGetTickCount64()
{
	OTUInt64 currentTick = 0;
	static OTUInt32 tickCount64HighPart = 0;
	static bool tickCount64Initialized = false;
	static OTUInt32 tickCount64LastTickCount = 0;
	static HMODULE hKernelDll = NULL;
	static FARPROC getTickCountProc = NULL;
	if(!tickCount64Initialized)
	{
#ifndef SOOS_WINDOWS_CE
		// Windows CE does definitely not support GetTickCount64 - and no GetProcAddress
		hKernelDll = LoadLibraryA("Kernel32.dll");
		if(hKernelDll != NULL)
		{
			getTickCountProc = GetProcAddress(hKernelDll, "GetTickCount64");
		}
#endif
		if(getTickCountProc == NULL)
		{
			tickCount64LastTickCount = GetTickCount();
		}
		tickCount64Initialized = true;
	}
	if(getTickCountProc != NULL)
	{
		currentTick = getTickCountProc();
	}
	else
	{
		OTUInt32 actTickCount = GetTickCount();
		// the following lines should be protected!
		if(actTickCount < tickCount64LastTickCount)
		{
			tickCount64HighPart++;
			tickCount64LastTickCount = actTickCount;
		}
		currentTick = (OTUInt64)tickCount64HighPart << 32 & (OTUInt64)actTickCount;
	}
	return currentTick;
}
#endif

OTUInt64 Util_Timer::currentTickCount64()
{
#ifdef SOOS_POSIX
	unsigned long long result = 0;
	struct timespec now;
	int getTimeResult = 0;
#if defined (CLOCK_MONOTONIC)
	getTimeResult = clock_gettime(CLOCK_MONOTONIC,&now);
#else
	// monotonic clock is not supported - use real time clock
	getTimeResult = clock_gettime(CLOCK_REALTIME,&now);
#endif
	if(getTimeResult != 0)
	{
#if defined(_WRS_VXWORKS_MAJOR) && ((_WRS_VXWORKS_MAJOR < 6) || ((_WRS_VXWORKS_MAJOR == 6) && (_WRS_VXWORKS_MINOR <= 8)))
		// VxWorks 6.8 does not support gettimeofday
		_REPORT_ASSERTION(_T("clock_gettime failed"));
#else
		struct timeval timevalnow;
		gettimeofday(&timevalnow, NULL);
		now.tv_sec = timevalnow.tv_sec;
		now.tv_nsec = 1000 * timevalnow.tv_usec;
#endif
	}
	result = (unsigned long long)now.tv_sec*1000 + (unsigned long long)now.tv_nsec/1000000;
	return result;
#endif

#ifdef SOOS_WINDOWS
	static bool performanceCounterInitialized = false;
	static bool usePerformanceCounter = true;
	static LARGE_INTEGER performanceCounterFrequency;
	static LARGE_INTEGER startPerformanceCounter;
	LARGE_INTEGER counter;

	// get frequency and start count only once
	if(!performanceCounterInitialized)
	{
		// try to use use performance counter to get a better resolution than 16 ms
		if(!QueryPerformanceFrequency(&performanceCounterFrequency))
		{
			usePerformanceCounter = false;
		}
		if(usePerformanceCounter)
		{
			if(!QueryPerformanceCounter(&startPerformanceCounter))
			{
				_REPORT_ASSERTION(_T("QueryPerformanceCounter failed"));
				usePerformanceCounter = false;
			}
		}
		performanceCounterInitialized = true;
	}

	if(usePerformanceCounter)
	{
		QueryPerformanceCounter(&counter);
		return (OTUInt64)(counter.QuadPart - startPerformanceCounter.QuadPart) / (OTUInt64)performanceCounterFrequency.QuadPart;
	}
	else
	{
		return UtilGetTickCount64();
	}
#endif
}

