#pragma once

#include <vector>

#include "sat/sat.h"
#include "sat/coordinateSystems.h"
#include "sat/availableSats.h"

namespace gnssShadowing {
namespace sat {

    class SatPositionProvider
    {
    public:
        virtual ENU& getSatPositionENU(int i) = 0;
        virtual int getNumSats() = 0; 
        virtual ~SatPositionProvider() = default;
    };

    class SatPositionProviderVectorENU : public SatPositionProvider
    {
    public:
        SatPositionProviderVectorENU(std::vector<ENU>& vec);
        ENU& getSatPositionENU(int i);
        int getNumSats();
        virtual ~SatPositionProviderVectorENU() = default;
    protected:
        std::vector<ENU>& m_vec;
    };

    class SatPositionProviderAvailable : public SatPositionProvider
    {
    public:
        SatPositionProviderAvailable(AvailableSats& sats);
        ENU& getSatPositionENU(int i);
        int getNumSats();
        virtual ~SatPositionProviderAvailable() = default;
    protected:
        AvailableSats& m_sats;
    };

} //namespace sat
} // namespace gnssShadowing
