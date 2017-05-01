

#include "mapping/dopMap.h"
#include "sat/satPositionProvider.h"
#include <memory>

namespace gnssShadowing {
namespace mapping {

    DOPMap::DOPMap(const DOPMap& copyFrom)
    {
        initMaps();
        if (copyFrom.m_availableSats && copyFrom.m_observer)
        {
            setupItems(*copyFrom.m_availableSats, *copyFrom.m_observer);
        }
        
    }

    DOPMap& DOPMap::operator=(const DOPMap& assignFrom)
    {
        m_properties = assignFrom.m_properties;
        for (int k=0; k < m_properties.m_plane_levels.size(); k++)
        {
            for (int x=0; x < m_properties.m_num_cols; x++)
            {
                for (int y=0; y < m_properties.m_num_rows; y++)
                {
                    m_items[k][x][y] = assignFrom.m_items[k][x][y];
                }
            }
        }
    }

    DOPMap::DOPMap(const MapProperties& properties, sat::AvailableSats& availableSats, sat::ENU observer)
        : VectorMap<sat::DOP>(properties) 
        , m_satProvider((sat::SatPositionProvider*)0)
        , m_availableSats(0)
    {
        setupItems(availableSats, observer);
    }

    void DOPMap::populate(VisibilityMap& visiblity)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif

        for (int k=0; k < m_properties.m_plane_levels.size(); k++)
        {
            for (int x=0; x < m_properties.m_num_cols; x++)
            {
                for (int y=0; y < m_properties.m_num_rows; y++)
                {
                    m_items[k][x][y].update(visiblity.m_items[k][x][y]);
                }
            }
        }
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }
    void DOPMap::forceComputeAll()
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        for (int k=0; k < m_properties.m_plane_levels.size(); k++)
        {
            for (int x=0; x < m_properties.m_num_cols; x++)
            {
                for (int y=0; y < m_properties.m_num_rows; y++)
                {
                    m_items[k][x][y].compute();
                }
            }
        }
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }

    void DOPMap::setupItems(sat::AvailableSats& availableSats, sat::ENU observer)
    {
        m_availableSats = &availableSats;
        m_observer = &observer;
        m_satProvider.reset(new sat::SatPositionProviderAvailable(availableSats));
        for (int k=0; k < m_properties.m_plane_levels.size(); k++)
        {
            for (int x=0; x < m_properties.m_num_cols; x++)
            {
                for (int y=0; y < m_properties.m_num_rows; y++)
                {
                    m_items[k][x][y].setObserver(observer);
                    m_items[k][x][y].setSats(m_satProvider.get());
                }
            }
        }
    }
    
    cv::Mat DOPMap::getHDOPMap(int planeLevelIndex)
    {
        cv::Mat_<float> map(m_properties.m_num_rows,m_properties.m_num_cols);
        for (int x=0; x < m_properties.m_num_cols; x++)
        {
            for (int y=0; y < m_properties.m_num_rows; y++)
            {
                map.at<float>(y,x)=m_items[planeLevelIndex][x][y].getHorizontal();
            }
        }
        return map;
    }

} // namespace mapping
} // namespace gnssShadowing
