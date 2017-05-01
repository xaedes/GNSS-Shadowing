#pragma once

// #include <opencv2/opencv.hpp>
// #include <memory>
#include <vector>
#include <memory>

#include "common/measureTime.h"
#include "mapping/mapProperties.h"
#include "mapping/vectorMap.h"
#include "mapping/shadowMap.h"
#include "sat/coordinateSystems.h"
#include "shadowing/shadowing.h"
#include "sat/availableSats.h"

namespace gnssShadowing {
namespace mapping {
    
    //forward declarations
    class ShadowMap;

    class VisibilityMap : public VectorMap<std::vector<int>>
    {
    public:
        VisibilityMap(const MapProperties& properties, sat::AvailableSats& availableSats);
        VisibilityMap(const MapProperties& properties, sat::AvailableSats& availableSats, shadowing::Shadowing& shadowing);
        virtual ~VisibilityMap() = default;
        VisibilityMap(const VisibilityMap& copyFrom);
        VisibilityMap& operator=(const VisibilityMap&);
        
        // void populate(shadowing::ShadowVolume& shadowVolume);
        void populate(shadowing::Shadowing& shadowing);
        void populateWithAll();
        void applyShadowMap(int satIdx, mapping::ShadowMap map);
        void clear();

        std::shared_ptr<common::Benchmark> m_benchmark;
        sat::AvailableSats* m_availableSats;
        sat::AvailableSats& getAvailableSats() {return *m_availableSats;}
        virtual void initMaps();
    protected:
        int m_capacity;
    };

} // namespace mapping
} // namespace gnssShadowing
