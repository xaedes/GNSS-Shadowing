
#include "common/timing.h"

#include <iostream>
#include <ctime>
#include <time.h>
#include <math.h>
#include <stdio.h>

namespace gnssShadowing {
namespace common {
	bool nsleep(long nanos)
	{
		struct timespec duration;
		if (nanos < 1e9)
		{
			duration.tv_sec = 0;
			duration.tv_nsec = nanos;
		}
		else
		{
			duration.tv_nsec = nanos % 1000000000; // 1e9 is no integer value
			duration.tv_sec = (nanos-duration.tv_nsec) / 1e9;
		}
		return (nanosleep(&duration,0) == 0);
	}

	bool msleep(long millis)
	{
		return nsleep(millis * 1000000);
	}

	long long now_nanos()
	{
		struct timespec spec;

		clock_gettime(CLOCK_REALTIME, &spec);

		return spec.tv_sec * 1000000000 + spec.tv_nsec;
	}

	long long now_millis()
	{
		struct timespec spec;

		clock_gettime(CLOCK_REALTIME, &spec);

		return spec.tv_sec * 1000 + round(spec.tv_nsec / 1.0e6);
	}
	
	double now_seconds()
	{
		struct timespec spec;

		clock_gettime(CLOCK_REALTIME, &spec);

		return (double)spec.tv_sec + (double)spec.tv_nsec * 1e-9;
	}

	// http://www.catb.org/esr/time-programming/
	double mk_seconds(int year, int month, int day, int hour, int minute, int second)
	{
		struct tm timeinfo;
		timeinfo.tm_sec = second;
		timeinfo.tm_min = minute;
		timeinfo.tm_hour = hour;
		timeinfo.tm_mday = day;
		timeinfo.tm_mon = month-1;
		timeinfo.tm_year = year-1900;
		timeinfo.tm_isdst = -1; // daylight saving unknown

		time_t result = mktime(&timeinfo);
		return (double)result;
	}

	void prettyprint_seconds(double seconds)
	{
		time_t t = (time_t)seconds;
		struct tm * timeinfo;
		timeinfo = localtime(&t);
		std::cout << asctime(timeinfo);
	}

	bool parseDateTime(std::string str, int& year, int& month, int& day, int& hour, int& minutes, int& seconds)
	{
		// 6 items parsed is success
		return 6 == sscanf(str.c_str(),"%d-%d-%d %d:%d:%d",&year,&month,&day,&hour,&minutes,&seconds);
	}

	bool parseDateTimeAsSeconds(std::string str, double& secondsOut)
	{
		int year;
		int month;
		int day;
		int hour;
		int minutes;
		int seconds;
		if (parseDateTime(str,year,month,day,hour,minutes,seconds))
		{
			secondsOut = mk_seconds(year,month,day,hour,minutes,seconds);
			return true;
		}
		return false;
	}

} // namespace common
} // namespace gnssShadowing
