
#include "sat/wdop.h"
#include "common/math.h"
#include <iostream>

namespace gnssShadowing {
namespace sat {
    
    WDOP::WDOP(ENU observer, SatPositionProvider* sats, std::vector<int>& visibleSatellites, std::vector<float>& weights)
    {
        setObserver(observer);
        setSats(sats);
        update(visibleSatellites, weights);
    }

    WDOP::WDOP()
        : m_dirty(true)
        , m_sats(0)
        , m_visibleSatellites(0)
    {}
    
    void WDOP::setObserver(ENU observer)
    {
        m_observer = observer;
        m_dirty = true;
    }
    void WDOP::setSats(SatPositionProvider* sats)
    {
        m_sats = sats;
        m_dirty = true;
    }
    void WDOP::update(std::vector<int>& visibleSatellites, std::vector<float>& weights)
    {
        m_visibleSatellites = &visibleSatellites;
        m_weights = weights;
        m_dirty = true;
    }
    void WDOP::compute()
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
        cv::Mat_<float> W(visibleSatellites.size(), visibleSatellites.size());
        W.setTo(cv::Scalar(0));
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
            W.at<float>(k,k) = m_weights[k]; 
        }
        // std::cout << "D " << W << std::endl;
        
        cv::Mat DOP_inv = D.t() * W * D;
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
    float WDOP::getEast()
    {
        if (m_dirty) compute();
        return m_east;
    }
    float WDOP::getNorth()
    {
        if (m_dirty) compute();
        return m_north;
    }
    float WDOP::getVertical()
    {
        if (m_dirty) compute();
        return m_vertical;
    }
    float WDOP::getTime()
    {
        if (m_dirty) compute();
        return m_time;
    }
    float WDOP::getHorizontal()
    {
        if (m_dirty) compute();
        return m_horizontal;
    }
    float WDOP::getPosition()
    {
        if (m_dirty) compute();
        return m_position;
    }
    float WDOP::getGeometric()
    {
        if (m_dirty) compute();
        return m_geometric;
    }
} //namespace sat
} // namespace gnssShadowing
