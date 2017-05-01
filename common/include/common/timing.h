#pragma once

#include <string>

namespace gnssShadowing {
namespace common {

	/// \brief      Sleeps for the specified amount of nano seconds.
	/// \param[in]  nanos  The nano seconds
	/// \return     Whether sleep was successful or not.
	bool nsleep(long nanos);
	
	/// \brief      Sleeps for the specified amount of milli seconds.
	/// \param[in]  millis  The milli seconds
	/// \return     Whether sleep was successful or not.
	bool msleep(long millis);
	

	/// \brief      Returns the current time in nano seconds.
	/// \return     nano seconds
	long long now_nanos();

	/// \brief      Returns the current time in milli seconds.
	/// \return     milli seconds
	long long now_millis();

	/// \brief      Returns the current time in seconds.
	/// \return     seconds
	double now_seconds();

	/**
	 * @brief      { function_description }
	 *
	 * @param[in]  year    The year   [e.g. 2017]
	 * @param[in]  month   The month  [1..12]
	 * @param[in]  day     The day    [1..31]
	 * @param[in]  hour    The hour   [0..23]
	 * @param[in]  minute  The minute [0..59]
	 * @param[in]  second  The second [0..59]
	 * @param[in]  timezoneoffset  The timezoneoffset
	 *
	 * @return     { description_of_the_return_value }
	 */
	double mk_seconds(int year, int month, int day, int hour, int minute, int second);

	void prettyprint_seconds(double seconds);
	
	bool parseDateTime(std::string str, int& year, int& month, int& day, int& hour, int& minutes, int& seconds);
	bool parseDateTimeAsSeconds(std::string str, double& secondsOut);


} // namespace common
} // namespace gnssShadowing
