#include "common/measureTime.h"
#include "common/timing.h"

namespace gnssShadowing {
namespace common {

    MeasureTime::MeasureTime(size_t maximumNumberOfEntries) 
        : SimpleStatistics<double>(maximumNumberOfEntries)
        , m_started(false)
    {}

    void MeasureTime::measureStart()
    {
        m_started = true;
        m_startTime = now_seconds();
    }

    void MeasureTime::measureEnd()
    {
        if (m_started)
        {
            store(now_seconds() - m_startTime);
            m_started = false;
        }
    }
    
} // namespace common
} // namespace gnssShadowing
