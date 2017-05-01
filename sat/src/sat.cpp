
#include "sat/sat.h"
#include "sat/coordinateSystems.h"
#include "sgp4_vallado/sgp4coord.h"
#include <cstring>
#include <iostream>

using namespace std;

namespace gnssShadowing {
namespace sat {

    Sat::Sat(string name, string tle1, string tle2) :
        m_name(name)
    {
        // parse Two-line element set
        
        // typerun: type of run              verification 'v', catalog 'c', 
        //                                   manual 'm'
        // typeinput: type of manual input   mfe 'm', epoch 'e', dayofyr 'd'
        // opsmode: mode of operation afspc or improved 'a', 'i'
        char typerun = 'c';
        char typeinput = 'e';
        char opsmode = 'i';
        m_whichConst = wgs72; // one of wgs72old, wgs72, wgs84
        // unused (only in verification mode of twoline2rv used)
        double startmfe, stopmfe, deltamin;

        char longstr1[130];
        char longstr2[130];
        memcpy(longstr1, tle1.c_str(), tle1.length()+1);
        memcpy(longstr2, tle2.c_str(), tle2.length()+1);

        twoline2rv(longstr1, longstr2, typerun, typeinput, opsmode, m_whichConst, startmfe, stopmfe, deltamin, m_satData);

        //Call propogator to get initial state vector value
        sgp4(m_whichConst, m_satData, 0.0, m_teme0.position, m_teme0.velocity);

    }

    TEME Sat::findTEME(double unixTimeSeconds)
    {
        TEME coord(m_teme0);
        coord.julianDate = julianDateFromUnix(unixTimeSeconds);
        // t_since: time since epoch in minutes
        double t_since = (coord.julianDate - m_satData.jdsatepoch) * 24. * 60.;
        bool success=sgp4(m_whichConst, m_satData, t_since, coord.position, coord.velocity);
        return coord;
    }

    ECEF Sat::findECEF(double unixTimeSeconds)
    {
        TEME teme = findTEME(unixTimeSeconds);
        return teme.toECEF();
    }
    
} //namespace sat
} // namespace gnssShadowing
