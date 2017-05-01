#pragma once

#include <vector>
#include <unordered_map>
#include <functional>

#include "mapping/mapProperties.h"
#include "mapping/dopMap.h"
#include "mapping/occupancyMap.h"
#include "mapping/visibilityMap.h"

#include "shadowing/shadowing.h"
#include "common/math.h"
#include "common/timing.h"
#include "obj/objFile.h"
#include "sat/satStore.h"
#include "sat/sat.h"
#include "sat/coordinateSystems.h"
#include "sat/availableSats.h"
#include "world/world.h"

namespace gnssShadowing {
namespace mapping {

    class Mapper
    {
    public:
        Mapper(world::World& world, MapProperties mapProperties, double minimumSatelliteElevation=5*D2R);

        void updateSats(double unixTimeSeconds);
        DOPMap& computeDOPMap(double unixTimeSeconds);
        
        std::reference_wrapper<world::World> m_world;

        world::World& getWorld() {return m_world;};
        void setWorld(world::World& world) {m_world = world;};

        sat::AvailableSats m_sats;
        
        MapProperties m_mapProperties;

        OccupancyMap m_occupancyMap;
        VisibilityMap m_visibilityMap;
        DOPMap m_dopMap;

    };

} // namespace mapping
} // namespace gnssShadowing
