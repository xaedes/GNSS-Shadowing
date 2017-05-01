
#include "mapping/visibilityMap.h"
#include "sat/coordinateSystems.h"
// #include "common/timing.h"
// #include "shadowing/shadowing.h"
#include "mapping/shadowMap.h"
#include "shadowing/shadowing.h"
#include <iostream>
namespace gnssShadowing {
namespace mapping {

    VisibilityMap::VisibilityMap(const MapProperties& properties, sat::AvailableSats& availableSats) 
        : m_availableSats(&availableSats)
        , m_capacity(m_availableSats->getMaxNumSats())
        , VectorMap<std::vector<int>>(properties)
    {
    }

    VisibilityMap::VisibilityMap(const MapProperties& properties, sat::AvailableSats& availableSats, shadowing::Shadowing& shadowing) 
        : m_availableSats(&availableSats)
        , m_capacity(m_availableSats->getMaxNumSats())
        , VectorMap<std::vector<int>>(properties)
    {
        populate(shadowing);
    }
    
    VisibilityMap::VisibilityMap(const VisibilityMap& copyFrom)
        : VectorMap<std::vector<int>>(copyFrom.m_properties)
        , m_benchmark(copyFrom.m_benchmark)
        , m_availableSats(copyFrom.m_availableSats 
                            ? new sat::AvailableSats(*copyFrom.m_availableSats) 
                            : (sat::AvailableSats*)0)
        , m_capacity(copyFrom.m_capacity)
    {
        // suboptimal, because VectorMap constructor creates m_items,
        // and then they are discarded for this copy
        m_items = copyFrom.m_items; 
    }

    VisibilityMap& VisibilityMap::operator=(const VisibilityMap& assignFrom)
    {
        m_items = assignFrom.m_items; 
        m_properties = assignFrom.m_properties; 
        m_benchmark = assignFrom.m_benchmark;
        m_availableSats = (assignFrom.m_availableSats 
                                ? new sat::AvailableSats(*assignFrom.m_availableSats) 
                                : (sat::AvailableSats*)0);
        m_capacity = assignFrom.m_capacity;
        return *this;
    }


    void VisibilityMap::applyShadowMap(int satIdx, mapping::ShadowMap map)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        for (int k=0; k < m_properties.m_plane_levels.size(); k++)
        {
            for (int y=0; y < m_properties.m_num_rows; y++)
            {
                for (int x=0; x < m_properties.m_num_cols; x++)
                {
                    if (!map.isShadowed(x,y,k))
                    {
                        // no shadow -> sat is visible
                        m_items[k][x][y].push_back(satIdx);
                    }
                }
            }
        }
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }

    void VisibilityMap::populateWithAll()
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        for(int k=0; k<m_availableSats->getNumSats(); k++)
        {
            for (int i=0; i < m_properties.m_plane_levels.size(); i++)
            {
                for (int y=0; y < m_properties.m_num_rows; y++)
                {
                    for (int x=0; x < m_properties.m_num_cols; x++)
                    {
                        m_items[i][x][y].push_back(k);
                    }
                }
            }
        }
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }
    void VisibilityMap::populate(shadowing::Shadowing& shadowing)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif


        mapping::ShadowMap shadowMap(m_properties);
        // shadowMap.m_benchmark.reset(m_benchmark.get());
        shadowMap.m_benchmark = m_benchmark;

        for(int k=0; k<m_availableSats->getNumSats(); k++)
        {
            sat::Horizontal& sat = m_availableSats->getSatPositionHorizontal(k);
            shadowing::Transformation transformation = shadowing.computeTransformation(sat);
            shadowing::Contour contour = shadowing.computeContour(transformation);
            shadowing::ShadowVolume shadowVolume = shadowing.computeShadowVolume(contour,transformation,m_properties.m_plane_levels[0]);
            shadowMap.computeShadowMap(shadowVolume);
            applyShadowMap(k, shadowMap);
        }
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }

    void VisibilityMap::clear()
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        for (int k=0; k < m_items.size(); k++)
        {
            for (int x=0; x < m_items[k].size(); x++)
            {
                for (int y=0; y < m_items[k][x].size(); y++)
                {
                    m_items[k][x][y].clear();
                }
            }
        }
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }

    void VisibilityMap::initMaps()
    {
        m_items.clear();

        for (int k=0; k < m_properties.m_plane_levels.size(); k++)
        {
            std::vector<std::vector<std::vector<int>>> map;
            for (int x=0; x < m_properties.m_num_cols; x++)
            {
                std::vector<std::vector<int>> column;
                for (int y=0; y < m_properties.m_num_rows; y++)
                {
                    column.push_back(std::vector<int>(m_capacity));
                }
                map.push_back(column);
            }
            m_items.push_back(map);
        }

    }


} // namespace mapping
} // namespace gnssShadowing
