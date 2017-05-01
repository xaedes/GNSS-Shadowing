#pragma once

#include <opencv2/opencv.hpp>
#include <memory>

#include "common/measureTime.h"
#include "mapping/mapProperties.h"
#include "shadowing/shadowing.h"

namespace gnssShadowing {
namespace mapping {

    typedef unsigned char ShadowMapValue;
    
    class ShadowMap
    {
    public:
        ShadowMap(const MapProperties& properties);
        ShadowMap(const MapProperties& properties, const shadowing::ShadowVolume& shadowVolume);

        MapProperties m_properties;

        void computeShadowMap(const shadowing::ShadowVolume& shadowVolume);
        cv::Mat& getMapForLevel(int idx) { return m_maps[idx]; }

        /**
         * @brief      Determines if cell is shadowed.
         *
         * @param[in]  i     first index in 2d map
         * @param[in]  j     second index in 2d map
         * @param[in]  k     index in planeLevels
         *
         * @return     True if shadowed, False otherwise.
         */
        inline bool isShadowed(int i, int j, int k=0)
        {
            return getCell(i,j,k) == 1;
        }

        std::shared_ptr<common::Benchmark> m_benchmark;
    protected:
        std::vector<cv::Mat_<ShadowMapValue>> m_maps;

        inline ShadowMapValue& getCell(int i, int j, int k)
        {
             return m_maps[k].at<ShadowMapValue>(j,i);
        }
        void rasterizeShadowFace(const shadowing::ShadowFace& shadowFace);

        void initMaps();

    };

} // namespace mapping
} // namespace gnssShadowing
