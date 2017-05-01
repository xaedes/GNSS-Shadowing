
#include "mapping/mapper.h"

#include <iostream>

namespace gnssShadowing {
namespace mapping {

    Mapper::Mapper(world::World& world, MapProperties mapProperties, double minimumSatelliteElevation)
        : m_world(world)
        , m_sats(m_world.get().m_satStore, m_world.get().m_origin, minimumSatelliteElevation)
        , m_mapProperties(mapProperties)
        , m_occupancyMap(m_mapProperties, m_world.get().m_buildings)
        , m_visibilityMap(m_mapProperties, m_sats)
        , m_dopMap(m_mapProperties, m_sats, m_world.get().m_originENU)
    {}

    void Mapper::updateSats(double unixTimeSeconds)
    {
        m_sats.update(unixTimeSeconds);
    }
    DOPMap& Mapper::computeDOPMap(double unixTimeSeconds)
    {
        updateSats(unixTimeSeconds);
        m_visibilityMap.clear();
        m_visibilityMap.populate(m_world.get().m_shadowing);
        m_dopMap.populate(m_visibilityMap);

        return m_dopMap;
    }


} // namespace mapping
} // namespace gnssShadowing
