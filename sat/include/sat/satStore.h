#pragma once

#include <string>
#include <vector>

#include "sat/sat.h"

namespace gnssShadowing {
namespace sat {

    class SatStore
    {
    public:
        SatStore(const std::string& filename);
        ~SatStore() = default;
        std::vector<Sat> m_sats;
        Sat& getSat(int idx) {return m_sats[idx];}
        int getNumSats() {return m_sats.size();} 
    protected:
        std::string m_filename;
    };

} //namespace sat
} // namespace gnssShadowing
