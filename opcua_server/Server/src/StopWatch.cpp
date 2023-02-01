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
#include "StopWatch.h"

#ifdef SOOS_WINDOWS
// static variables

// HighPerformanceTicks per second,
OTInt64 StopWatch::m_llTicksPerSecond = 0;

// true in case of a missing init of m_llTicksPerSecondhas
bool StopWatch::m_bInitOk = QueryPerformanceFrequency((LARGE_INTEGER *)&m_llTicksPerSecond) != FALSE;

StopWatch::StopWatch(bool bStart /* = false */)
	: m_bStarted(false)
{
	if(bStart)
		start();
}

StopWatch::~StopWatch()
{
	// write a trace line if running
	if(m_bStarted)
	{
#ifdef _DEBUG
		// double dStopTime = stop();
		;// TRACE(_T("Stopwatch stop: %g ms\n"), dStopTime * 1000.0);
#endif
	}
}

//Writes the current lap time into the debug trace.
void StopWatch::traceLapTime(OTChar const * str /* = NULL*/)
{
	_UNUSED(str);
	if(m_bStarted)
	{
#ifdef _DEBUG
		//    double dLapTime = getLapTime();

		//if(str)
		//{
		//	TRACE(_T("Stopwatch getLapTime <%s>: %g ms\n"), str, dLapTime * 1000.0);
		//}
		//else
		//{
		//	TRACE(_T("Stopwatch getLapTime: %g ms\n"), dLapTime * 1000.0);
		//}
#endif
	}
}

//Starts the Stopwatch. If the Stopwatch was running before, the start-time will be set again to the current time.
void StopWatch::start()
{
	QueryPerformanceCounter((LARGE_INTEGER *) &m_llStartTime);
	m_bStarted = true;
}

//Stops the watch and returns the elapsed time in sesonds.If the watch was not running, the return value is -1.
double StopWatch::stop()
{
	if(m_bStarted && m_bInitOk)
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &m_llStopTime);
		m_bStarted = false;
		double dTime = (double) (m_llStopTime - m_llStartTime  )/ ((double)m_llTicksPerSecond);
		return dTime;
	}

	return -1.0;
}

//Returns a lap time in seconds. If the watch is not running, the returned time is -1.0
double StopWatch::getLapTime()
{
	if(m_bStarted && m_bInitOk)
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_llLapTime);
		double dTime = (double) (m_llLapTime - m_llStartTime  ) / ((double)m_llTicksPerSecond);
		return dTime;
	}

	return -1.0;
}

double StopWatch::getLapTimeAndDoReset()
{
	if(m_bStarted && m_bInitOk)
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_llLapTime);
		double dTime = (double) (m_llLapTime - m_llStartTime  ) / ((double)m_llTicksPerSecond);
		m_llStartTime  = m_llLapTime ;
		return dTime;
	}

	return -1.0;
}
#endif
