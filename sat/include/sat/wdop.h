#pragma once

#include <memory>

#include "sgp4_vallado/sgp4io.h"
#include "sat/coordinateSystems.h"
#include "sat/satPositionProvider.h"

namespace gnssShadowing {
namespace sat {

    class WDOP
    {
        // Ming Fatt Yuen. (n.d.). DILUTION OF PRECISION (DOP) CALCULATION FOR MISSION PLANNING PURPOSES. 
        // Retrieved from http://www.dtic.mil/dtic/tr/fulltext/u2/a497140.pdf
    public:
        WDOP(ENU observer, SatPositionProvider* sats, std::vector<int>& visibleSatellites, std::vector<float>& weights);
        WDOP();
        
        void setSats(SatPositionProvider* sats);
        void setObserver(ENU observer);
        void update(std::vector<int>& visibleSatellites, std::vector<float>& weights);
        void compute();

        float getEast();
        float getNorth();
        float getVertical();
        float getTime();
        float getHorizontal();
        float getPosition();
        float getGeometric();
    protected:
        float m_east;
        float m_north;
        float m_vertical;
        float m_time;
        float m_horizontal;
        float m_position;
        float m_geometric;

        bool m_dirty;
        ENU m_observer;
        SatPositionProvider* m_sats;
        std::vector<int>* m_visibleSatellites;
        std::vector<float> m_weights;
    };

} //namespace sat
} // namespace gnssShadowing
