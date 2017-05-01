#pragma once

#include <string>

#include "sgp4_vallado/sgp4io.h"
#include "sat/coordinateSystems.h"

namespace gnssShadowing {
namespace sat {

    class Sat
    {
    public:
        Sat(std::string name, std::string tle1, std::string tle2);

        TEME findTEME(double unixTimeSeconds);
        ECEF findECEF(double unixTimeSeconds);

        std::string getName(){return m_name;};
        const std::string& getNameConst(){return m_name;};
        const long int getSatNumber(){return m_satData.satnum;};
        
    protected:
        std::string m_name;
        elsetrec m_satData;
        gravconsttype m_whichConst;
        TEME m_teme0;
    };

    //class SatList
    //{
    //public:
    //    SatList();
    //    ~SatList();
    //    void push_back();
    //    std::vector<Sat> m_sats;
    //};

} //namespace sat
} // namespace gnssShadowing
