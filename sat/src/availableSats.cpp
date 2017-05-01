
#include "sat/availableSats.h"


using namespace std;

namespace gnssShadowing {
namespace sat {

    AvailableSats::AvailableSats(SatStore& satStore, Geodetic observer, double minimumSatelliteElevation)
        : m_satStore(satStore)
        , m_observer(observer)
        , m_minimumSatelliteElevation(minimumSatelliteElevation)
    {
    }

    void AvailableSats::setSatStore(SatStore& satStore)
    {
        m_satStore=satStore;
    }

    void AvailableSats::setObserver(Geodetic observer)
    {
        m_observer=observer;
    }

    void AvailableSats::update(double unixTimeSeconds)
    {
        m_indicesAvailable.clear();
        m_positionsHorizontal.clear();
        m_positionsENU.clear();
        for(int i=0; i<m_satStore.get().m_sats.size(); i++)
        {
            Sat& sat = m_satStore.get().m_sats[i];
            TEME teme = sat.findTEME(unixTimeSeconds);
            Horizontal hor = teme.toHorizontal(m_observer);
            if (hor.elevation > m_minimumSatelliteElevation)
            {
                m_indicesAvailable.push_back(i);
                m_positionsHorizontal.push_back(hor);
                m_positionsENU.push_back(hor.toENU());
            }
        }       
    }
    int AvailableSats::getNumSats()
    {
        return m_indicesAvailable.size();
    }
    int AvailableSats::getMaxNumSats()
    {
        return m_satStore.get().getNumSats();
    }
    int AvailableSats::getSatIndex(int i)
    {
        return m_indicesAvailable[i];
    }
    Sat& AvailableSats::getSat(int i)
    {
        return m_satStore.get().getSat(m_indicesAvailable[i]);
    }
    Horizontal& AvailableSats::getSatPositionHorizontal(int i)
    {
        return m_positionsHorizontal[i];
    }
    ENU& AvailableSats::getSatPositionENU(int i)
    {
        return m_positionsENU[i];
    }

} //namespace sat
} // namespace gnssShadowing
