#pragma once

#include <vector>

namespace gnssShadowing {
namespace common {

	template <typename T=double>
	class SimpleStatistics
	{
	public:
		SimpleStatistics(size_t maximumNumberOfEntries=1000);

		void store(T item);
		
		T mean();
		T std();
		T maximum();
		T minimum();

		void printStats();

	private:
		void updateMinimum();
		void updateMaximum();

		size_t         m_maximumNumberOfEntries;
		bool           m_ringBufferFull;
		int            m_ringBufferPointer;
		std::vector<T> m_items;

		T m_maximum;
		T m_minimum;
		T m_sum;

	};

} // namespace common
} // namespace gnssShadowing

#include "simpleStatistics.h.cpp"
