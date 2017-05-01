#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "mapping/mapProperties.h"
#include "mapping/dopMap.h"
#include "mapping/occupancyMap.h"
#include "mapping/visibilityMap.h"
#include "mapping/mapper.h"

#include "shadowing/shadowing.h"
#include "common/math.h"
#include "common/timing.h"
#include "obj/objFile.h"
#include "sat/satStore.h"
#include "sat/sat.h"
#include "sat/coordinateSystems.h"
#include "world/world.h"

namespace gnssShadowing {
namespace mapping {

    class MapperLazyTimesteps
    {
    public:
        MapperLazyTimesteps(world::World& world, MapProperties mapProperties, double startTimeUnixTimeSeconds, double timePerStep, double minimumSatelliteElevation=5*D2R);

        double getTime(int timeStep);

        DOPMap& getDOPMap(int timeStep);
        OccupancyMap& getOccupancyMap(int timeStep);


        DOPMap& computeDOPMap(int timeStep);
        void clear();
        
        double m_startTimeUnixTimeSeconds;
        double m_timePerStep;

        std::reference_wrapper<world::World> m_world;
        MapProperties m_mapProperties;
        double m_minimumSatelliteElevation;


        std::unordered_map<int,std::shared_ptr<Mapper>> m_mappers;
        // std::unordered_map<int,std::shared_ptr<VisibilityMap>> m_visibilityMaps;
        // std::unordered_map<int,std::shared_ptr<DOPMap>> m_dopMaps;

        // Mapper m_mapper;
    };

} // namespace mapping
} // namespace gnssShadowing
