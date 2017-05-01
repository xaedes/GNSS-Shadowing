#include "common/math.h"

#include <iostream>

namespace gnssShadowing {
namespace common {
	
	template <typename T>
	SimpleStatistics<T>::SimpleStatistics(size_t maximumNumberOfEntries):
		m_maximumNumberOfEntries(maximumNumberOfEntries),
		m_ringBufferFull(false),
		m_ringBufferPointer(0),
		m_maximum(0),
		m_minimum(0),
		m_sum(0)
	{

	}

	template <typename T>
	void SimpleStatistics<T>::store(T item)
	{
		if (not m_ringBufferFull)
		{
			// fill ringbuffer up to maximum number of entries
			m_items.push_back(item);
			
			// update statistics
			m_sum += item;
			if (m_items.size() == 1)
			{
				m_minimum = item;
				m_maximum = item;
			}
			else 
			{
				if (item < m_minimum)
				{
					m_minimum = item;
				}
				if (item > m_maximum)
				{
					m_maximum = item;
				}
			}

			// check if ring buffer is full
			if (m_items.size() >= m_maximumNumberOfEntries)
			{
				m_ringBufferFull = true;
				m_ringBufferPointer = 0;
			}
		} 
		else
		{
			// full ringbuffer; cycle through elements
			
			// update statistics
			m_sum += item - m_items[m_ringBufferPointer];
			if (m_items[m_ringBufferPointer] == m_minimum)
			{
				// item to be overridden is minimum item
				
				m_items[m_ringBufferPointer] = item;
				if (item < m_minimum)
				{
					// new item is below current minimum
					m_minimum = item;
				}
				else
				{
					// new item does not necessarily set new minimum
					updateMinimum();
				}
			}
			if (m_items[m_ringBufferPointer] == m_maximum)
			{
				// item to be overridden is maximal item
				m_items[m_ringBufferPointer] = item;
				if (item > m_maximum)
				{
					// new item is above current maximum
					m_maximum = item;
				}
				else
				{
					// new item does not necessarily set new maximum
					updateMaximum();
				}
			}
			
			// store item
			m_items[m_ringBufferPointer] = item;

			// advance ring buffer pointer
			m_ringBufferPointer = (m_ringBufferPointer+1) % m_maximumNumberOfEntries;
		}
	}

	template <typename T>
	T SimpleStatistics<T>::mean()
	{
		return (T)m_sum / (T)m_items.size();
	}

	template <typename T>
	T SimpleStatistics<T>::std()
	{
		if (m_items.size()<2)
		{
			return 0;
		}

		T _mean = mean();
		T sum = 0;
		for(size_t k=0; k < m_items.size(); ++k)
		{
			T diff = (m_items[k] - _mean);
			sum += diff*diff;
		}
		T std = sqrt(sum / (m_items.size()-1));
		return std;
	}

	template <typename T>
	void SimpleStatistics<T>::printStats()
	{
		std::cout << "SimpleStatistics "  << std::endl;
		std::cout << "n	   " << m_items.size() << std::endl;
		std::cout << "mean " << mean()         << std::endl;
		std::cout << "std  " << std()          << std::endl;
		std::cout << "min  " << minimum()      << std::endl;
		std::cout << "max  " << maximum()      << std::endl;
	}

	template <typename T>
	T SimpleStatistics<T>::maximum()
	{
		return m_maximum;
	}

	template <typename T>
	T SimpleStatistics<T>::minimum()
	{
		return m_minimum;
	}

	template <typename T>
	void SimpleStatistics<T>::updateMinimum()
	{
		if (m_items.size()==0)
		{
			return;
		}
		T minimum = m_items[0];
		for(size_t k=1; k < m_items.size(); ++k)
		{
			if (m_items[k]<minimum)
			{
				minimum = m_items[k];
			}
		}
		m_minimum = minimum;
	}

	template <typename T>
	void SimpleStatistics<T>::updateMaximum()
	{
		if (m_items.size()==0)
		{
			return;
		}
		T maximum = m_items[0];
		for(size_t k=1; k < m_items.size(); ++k)
		{
			if (m_items[k]>maximum)
			{
				maximum = m_items[k];
			}
		}
		m_maximum = maximum;
	}

} // namespace common
} // namespace gnssShadowing
