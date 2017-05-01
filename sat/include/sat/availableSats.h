#pragma once

#include <vector>
#include <functional>

#include "sat/sat.h"
#include "sat/satStore.h"
#include "sat/coordinateSystems.h"


namespace gnssShadowing {
namespace sat {

    class AvailableSats
    {
    public:
        AvailableSats(SatStore& satStore, Geodetic observer, double minimumSatelliteElevation);
        AvailableSats(const AvailableSats& copyFrom) = default;
        AvailableSats& operator=(const AvailableSats&) = default;

        void setSatStore(SatStore& satStore);
        void setObserver(Geodetic observer);
        void update(double unixTimeSeconds);
        int getMaxNumSats();
        int getNumSats();
        int getSatIndex(int i);
        Sat& getSat(int i);
        Horizontal& getSatPositionHorizontal(int i);
        ENU& getSatPositionENU(int i);

        double m_minimumSatelliteElevation;
    protected:
        std::reference_wrapper<SatStore> m_satStore;
        Geodetic m_observer;

        std::vector<int> m_indicesAvailable;
        std::vector<Horizontal> m_positionsHorizontal;
        std::vector<ENU> m_positionsENU;
    };

} //namespace sat
} // namespace gnssShadowing
