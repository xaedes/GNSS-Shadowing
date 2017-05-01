#pragma once

#include <opencv2/opencv.hpp>
#include <memory>
#include <vector>

#include "common/measureTime.h"
#include "mapping/mapProperties.h"
#include "obj/objFile.h"

namespace gnssShadowing {
namespace mapping {

    typedef unsigned char OccupancyMapValue;
    
    class OccupancyMap
    {
    public:
        typedef std::vector<cv::Point3f> Face;
        template <typename T> using Edge = std::pair<T,T>;  // edge
        OccupancyMap(const MapProperties& properties);
        OccupancyMap(const MapProperties& properties, obj::Object& object);
        OccupancyMap(const MapProperties& properties, const std::vector<Face>& faces);

        MapProperties m_properties;

        void computeOccupancyMap(const std::vector<Face>& faces);
        cv::Mat& getMapForLevel(int idx) { return m_maps[idx]; }

        std::shared_ptr<common::Benchmark> m_benchmark;

        /**
         * @brief      Determines if cell is occupied.
         *
         * @param[in]  i     first index in 2d map
         * @param[in]  j     second index in 2d map
         * @param[in]  k     index in planeLevels
         *
         * @return     True if occupied, False otherwise.
         */
        inline bool isOccupied(int i, int j, int k=0)
        {
            return getCell(i,j,k) == 1;
        }
    
    protected:
        std::vector<cv::Mat_<OccupancyMapValue>> m_maps;

        inline OccupancyMapValue& getCell(int i, int j, int k)
        {
            return m_maps[k].at<OccupancyMapValue>(j,i);
        }

        void rasterizeFace(const Face& face);
        void initMaps();
    };

} // namespace mapping
} // namespace gnssShadowing
