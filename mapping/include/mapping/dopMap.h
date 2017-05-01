#pragma once

#include <opencv2/opencv.hpp>
#include <memory>

#include "common/measureTime.h"
#include "mapping/mapProperties.h"
#include "mapping/vectorMap.h"
#include "mapping/visibilityMap.h"
#include "sat/dop.h"
#include "sat/coordinateSystems.h"
#include "sat/availableSats.h"
#include "sat/satPositionProvider.h"

namespace gnssShadowing {
namespace mapping {

    class DOPMap : public VectorMap<sat::DOP>
    {
    public:
        DOPMap(const DOPMap& copyFrom);
        DOPMap& operator=(const DOPMap&);

        DOPMap(const MapProperties& properties, sat::AvailableSats& availableSats, sat::ENU observer);
        virtual ~DOPMap() = default;
        void populate(VisibilityMap& visiblity);
        std::shared_ptr<common::Benchmark> m_benchmark;
        void forceComputeAll();
        void setupItems(sat::AvailableSats& availableSats, sat::ENU observer);

        cv::Mat getHDOPMap(int planeLevelIndex);

    protected:
        std::unique_ptr<sat::SatPositionProvider> m_satProvider;
        sat::AvailableSats* m_availableSats;
        sat::ENU* m_observer;
    };

} // namespace mapping
} // namespace gnssShadowing
