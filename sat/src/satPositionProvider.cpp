
#include "sat/satPositionProvider.h"

namespace gnssShadowing {
namespace sat {

    SatPositionProviderVectorENU::SatPositionProviderVectorENU(std::vector<ENU>& vec)
        : m_vec(vec)
    {}
    ENU& SatPositionProviderVectorENU::getSatPositionENU(int i)
    {
        return m_vec[i];
    }
    int SatPositionProviderVectorENU::getNumSats()
    {
        return m_vec.size();
    }

    SatPositionProviderAvailable::SatPositionProviderAvailable(AvailableSats& sats)
        : m_sats(sats)
    {}
    ENU& SatPositionProviderAvailable::getSatPositionENU(int i)
    {
        return m_sats.getSatPositionENU(i);
    }
    int SatPositionProviderAvailable::getNumSats()
    {
        return m_sats.getNumSats();
    }

} //namespace sat
} // namespace gnssShadowing
