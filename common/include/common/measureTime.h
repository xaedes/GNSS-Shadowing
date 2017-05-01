#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "common/simpleStatistics.h"

namespace gnssShadowing {
namespace common {

	class MeasureTime : public SimpleStatistics<double>
	{
	public:
		MeasureTime(size_t maximumNumberOfEntries=1000);

		void measureStart();
		void measureEnd();


	private:
        bool     m_started;
        double   m_startTime;
	};

	typedef std::unordered_map<std::string, MeasureTime> Benchmark;
	
} // namespace common
} // namespace gnssShadowing
