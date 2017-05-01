
#include "sat/dop.h"
#include "common/math.h"
#include <iostream>
namespace gnssShadowing {
namespace sat {
    
    DOP::DOP(ENU observer, SatPositionProvider* sats, std::vector<int>& visibleSatellites)
        : m_dirty(true)
        , m_sats(0)
        , m_visibleSatellites(0)
    {
        setObserver(observer);
        setSats(sats);
        update(visibleSatellites);
    }

    DOP::DOP()
        : m_dirty(true)
        , m_sats(0)
        , m_visibleSatellites(0)
    {}

    void DOP::setObserver(ENU observer)
    {
        m_observer = observer;
        m_dirty = true;
    }
    void DOP::setSats(SatPositionProvider* sats)
    {
        m_sats = sats;
        m_dirty = true;
    }
    void DOP::update(std::vector<int>& visibleSatellites)
    {
        m_visibleSatellites = &visibleSatellites;
        m_dirty = true;
    }
    void DOP::compute()
    {
        if (!m_visibleSatellites || !m_sats || m_visibleSatellites->size() < 4)
        {
            m_east       = -1;
            m_north      = -1;
            m_vertical   = -1;
            m_time       = -1;
            m_horizontal = -1;
            m_position   = -1;
            m_geometric  = -1;
            return;
        }
        std::vector<int>& visibleSatellites = *m_visibleSatellites;
        cv::Mat_<float> D(visibleSatellites.size(), 4);
        for (int k=0; k<visibleSatellites.size(); k++)
        {
            ENU& sat = m_sats->getSatPositionENU(visibleSatellites[k]);
            ENU diff = sat;
            diff -= m_observer;
            float range = sqrt(diff.position[0]*diff.position[0]+
                               diff.position[1]*diff.position[1]+
                               diff.position[2]*diff.position[2]);
            float* Drow = D.ptr<float>(k,0);
            Drow[0] = diff.position[0] / range;
            Drow[1] = diff.position[1] / range;
            Drow[2] = diff.position[2] / range;
            Drow[3] = -1;
        }
        
        cv::Mat DOP_inv = D.t() * D;
        cv::Mat DOP = DOP_inv.inv();

        m_east     = sqrt(DOP.at<float>(0,0));
        m_north    = sqrt(DOP.at<float>(1,1));
        m_vertical = sqrt(DOP.at<float>(2,2));
        m_time     = sqrt(DOP.at<float>(3,3));

        m_horizontal = sqrt(m_east*m_east + m_north*m_north);
        m_position   = sqrt(m_east*m_east + m_north*m_north + m_vertical*m_vertical);
        m_geometric  = sqrt(m_east*m_east + m_north*m_north + m_vertical*m_vertical + m_time*m_time);

        m_dirty = false;
    }
    float DOP::getEast()
    {
        if (m_dirty) compute();
        return m_east;
    }
    float DOP::getNorth()
    {
        if (m_dirty) compute();
        return m_north;
    }
    float DOP::getVertical()
    {
        if (m_dirty) compute();
        return m_vertical;
    }
    float DOP::getTime()
    {
        if (m_dirty) compute();
        return m_time;
    }
    float DOP::getHorizontal()
    {
        if (m_dirty) compute();
        return m_horizontal;
    }
    float DOP::getPosition()
    {
        if (m_dirty) compute();
        return m_position;
    }
    float DOP::getGeometric()
    {
        if (m_dirty) compute();
        return m_geometric;
    }
} //namespace sat
} // namespace gnssShadowing
