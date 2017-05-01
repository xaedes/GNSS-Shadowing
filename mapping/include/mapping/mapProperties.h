#pragma once
#include <ostream>
#include <vector>
#include "world/world.h"

namespace gnssShadowing {
namespace mapping {
    
    class MapProperties
    {
    public:
        MapProperties(double min_x, double min_y, int num_cols, int num_rows, 
                      double x_resolution, double y_resolution, 
                      std::vector<double> plane_levels=std::vector<double>()) :
            m_min_x(min_x),
            m_min_y(min_y),
            m_num_cols(num_cols),
            m_num_rows(num_rows),
            m_x_resolution(x_resolution),
            m_y_resolution(y_resolution),
            m_plane_levels(plane_levels)
        {}

        MapProperties(const MapProperties& copyFrom, std::vector<double> plane_levels) :
            m_min_x(copyFrom.m_min_x),
            m_min_y(copyFrom.m_min_y),
            m_num_cols(copyFrom.m_num_cols),
            m_num_rows(copyFrom.m_num_rows),
            m_x_resolution(copyFrom.m_x_resolution),
            m_y_resolution(copyFrom.m_y_resolution),
            m_plane_levels(plane_levels)
        {}

        MapProperties(const MapProperties& copyFrom) = default;
        
        static MapProperties SingleLocation(world::World world, sat::Geodetic location, double cellSize, double planeLevel)
        {
            sat::ECEF ecef = location.toECEF();
            sat::SEZ sez = ecef.toSEZ(world.m_origin);
            const int KM2M = (1.0e3);
            double south = sez.position[0] * KM2M;
            double east = sez.position[1] * KM2M;        
            double x = south;
            double y = east;
            double halfCellSize = cellSize/2;
            std::vector<double> planeLevels;
            planeLevels.push_back(planeLevel);
            return MapProperties(x, y, 1, 1, cellSize, cellSize, planeLevels);
        }
        static MapProperties AroundLocation_(world::World world, sat::Geodetic location, double cellSize, int width, int height, double planeLevel)
        {
            sat::ECEF ecef = location.toECEF();
            sat::SEZ sez = ecef.toSEZ(world.m_origin);
            const int KM2M = (1.0e3);
            double south = sez.position[0] * KM2M;
            double east = sez.position[1] * KM2M;        
            double x = south;
            double y = east;
            double halfCellSize = cellSize/2;
            std::vector<double> planeLevels;
            planeLevels.push_back(planeLevel);
            return MapProperties(x-(cellSize*width)/2, y-(cellSize*height)/2, width, height, cellSize, cellSize, planeLevels);
        }
        static MapProperties AroundLocation(world::World world, sat::Geodetic location, double cellSize, int width, int height, std::vector<double> planeLevels)
        {
            sat::ECEF ecef = location.toECEF();
            sat::SEZ sez = ecef.toSEZ(world.m_origin);
            const int KM2M = (1.0e3);
            double south = sez.position[0] * KM2M;
            double east = sez.position[1] * KM2M;        
            double x = south;
            double y = east;
            double halfCellSize = cellSize/2;
            return MapProperties(x-(cellSize*width)/2, y-(cellSize*height)/2, width, height, cellSize, cellSize, planeLevels);
        }
        MapProperties(){}

        double m_min_x;
        double m_min_y;
        int m_num_cols;
        int m_num_rows;
        double m_x_resolution;
        double m_y_resolution; 
        std::vector<double> m_plane_levels; 
        double X(int i) { return m_min_x+i*m_x_resolution; }
        double Y(int j) { return m_min_y+j*m_y_resolution; }
        double I(double x) { return (x-m_min_x)/m_x_resolution; }
        double J(double y) { return (y-m_min_y)/m_y_resolution; }
    };
    std::ostream& operator<<(std::ostream& os, const MapProperties& mapProperties);

} // namespace mapping
} // namespace gnssShadowing
