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
#ifndef __STOPWATCH_H__
#define __STOPWATCH_H__

///////////////////////////////////////////////////////////////////////////
// A little util class that allows to make performance measurements
///////////////////////////////////////////////////////////////////////////
#ifdef SOOS_WINDOWS
class StopWatch
{
public:

	// constructor, if bStartMeasurement == true the watch starts immediately.
	StopWatch(bool bStartMeasurement = false);

	virtual ~StopWatch();

	// stops time measurement, makes a reset if already running
	void start();

	// stops time measurement and returns the elapsed time in sesonds (-1 if not running)
	double stop(); // returns time in ms

	// returns the lap time in seconds (-1 if not running)
	double getLapTime(); // returns time in ms

	// returns the lap time in s and makes a reset
	double getLapTimeAndDoReset();

	// writes the current lap time into a trace line
	void traceLapTime(OTChar const * str = NULL);

private:
	// Indicates that the watch is currently running.
	bool m_bStarted;

	// ticks during start()
	OTInt64 m_llStartTime;

	// ticks during last lap()
	OTInt64 m_llLapTime;

	// number of ticks during last stop()
	OTInt64 m_llStopTime;

	// true in case of a missing init of m_llTicksPerSecondhas
	static bool m_bInitOk;

	// Static variable that contains the number of HighPerformanceTicks of the current computer system. This variable will be automatically initialized if the first constructor is called.
	static OTInt64 m_llTicksPerSecond;
};
#endif

#endif

