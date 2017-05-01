#pragma once

#include <string>

#include "sat/nmea.h"
#include "sat/coordinateSystems.h"

namespace gnssShadowing {
namespace eval {

    class NMEAGeodeticAggregator
    {
    public:
        NMEAGeodeticAggregator();
        
        sat::NMEA::Geodetic getMean();
        void add(sat::NMEA::Geodetic location);
        inline int num() const {return m_num;};

    protected:
        sat::NMEA::Geodetic m_sum;
        int m_num;
    };

    class DoubleAggregator
    {
    public:
        DoubleAggregator();
        
        double getMean();
        void add(double value);
        inline int num() const {return m_num;};

    protected:
        double m_sum;
        int m_num;
    };

    sat::NMEA::Geodetic addGeodetic(sat::NMEA::Geodetic a, sat::NMEA::Geodetic b);
    sat::NMEA::Geodetic subGeodetic(sat::NMEA::Geodetic a, sat::NMEA::Geodetic b);
    double magDiffGeodetic(sat::NMEA::Geodetic a, sat::NMEA::Geodetic b);
    double magGeodetic(sat::NMEA::Geodetic a);

    sat::NMEA::Geodetic convertToNMEA(sat::Geodetic a);

    sat::NMEA::Geodetic parseLocationNMEA(std::string str);

    sat::Geodetic parseLocation(std::string str);
    sat::Geodetic parseLocation(std::string str, sat::NMEA::Date& date, sat::NMEA::UTC& time);


} // namespace eval 
} // namespace gnssShadowing 
