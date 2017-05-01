#pragma once

#include <vector>

#include "mapping/mapProperties.h"

namespace gnssShadowing {
namespace mapping {

    template <typename Item>
    class VectorMap
    {
    public:
        VectorMap(const MapProperties& properties);
        VectorMap(){}
        virtual ~VectorMap() = default;
        VectorMap(const VectorMap& copyFrom) = default;
        VectorMap& operator=(const VectorMap&) = default;

        MapProperties m_properties;
        std::vector<std::vector<std::vector<Item>>> m_items; // 2d matrix is col-major
        
        Item& getItem(int x, int y, int z) { return m_items[z][x][y]; }

        virtual void initMaps();
    };

} // namespace mapping
} // namespace gnssShadowing

#include "vectorMap.h.cpp"
