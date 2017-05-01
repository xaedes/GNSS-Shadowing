
#include "mapping/mapperLazyTimesteps.h"

#include <iostream>

namespace gnssShadowing {
namespace mapping {

    MapperLazyTimesteps::MapperLazyTimesteps(world::World& world, MapProperties mapProperties, double startTimeUnixTimeSeconds, double timePerStep, double minimumSatelliteElevation)
        : m_world(world)
        , m_mapProperties(mapProperties)
        , m_minimumSatelliteElevation(minimumSatelliteElevation)
        , m_startTimeUnixTimeSeconds(startTimeUnixTimeSeconds)
        , m_timePerStep(timePerStep)
    {}

    double MapperLazyTimesteps::getTime(int timeStep)
    {
        return m_startTimeUnixTimeSeconds + m_timePerStep*timeStep;
    }

    DOPMap& MapperLazyTimesteps::getDOPMap(int timeStep)
    {
        if (m_mappers.count(timeStep) && m_mappers[timeStep].get())
        {
            return m_mappers[timeStep]->m_dopMap;
        }
        else
        {
            return computeDOPMap(timeStep);
        }
    }

    OccupancyMap& MapperLazyTimesteps::getOccupancyMap(int timeStep)
    {
        if (m_mappers.count(timeStep) && m_mappers[timeStep].get())
        {
            return m_mappers[timeStep]->m_occupancyMap;
        }
        else
        {
            computeDOPMap(timeStep);
            return m_mappers[timeStep]->m_occupancyMap;
        }
    }

    void MapperLazyTimesteps::clear()
    {
        m_mappers.clear();
    }
    
    DOPMap& MapperLazyTimesteps::computeDOPMap(int timeStep)
    {
        // std::cout << __PRETTY_FUNCTION__ << " timeStep " << timeStep << std::endl;
        
        double now = getTime(timeStep);

        m_mappers[timeStep].reset(new Mapper(m_world, m_mapProperties, m_minimumSatelliteElevation));
        m_mappers[timeStep]->computeDOPMap(now);

        return m_mappers[timeStep]->m_dopMap;
    }


} // namespace mapping
} // namespace gnssShadowing
