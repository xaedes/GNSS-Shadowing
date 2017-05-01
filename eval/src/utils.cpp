
#include "eval/utils.h"

#include <sstream>
#include <vector>

#include "common/math.h"
#include "common/timing.h"
#include "common/stringUtils.h"

using namespace gnssShadowing::sat;
using namespace gnssShadowing::common;

namespace gnssShadowing {
namespace eval {


    NMEAGeodeticAggregator::NMEAGeodeticAggregator()
        : m_num(0)
    {
        m_sum.latitude = 0;
        m_sum.longitude = 0;
        m_sum.altitude = 0;
    }
        
    NMEA::Geodetic NMEAGeodeticAggregator::getMean()
    {
        NMEA::Geodetic mean;
        int n = m_num ? m_num : 1;
        mean.latitude  = m_sum.latitude / n;
        mean.longitude = m_sum.longitude / n;
        mean.altitude  = m_sum.altitude / n;
        return mean;
    }

    void NMEAGeodeticAggregator::add(NMEA::Geodetic location)
    {
        m_sum.latitude  += location.latitude;
        m_sum.longitude += location.longitude;
        m_sum.altitude  += location.altitude;
        ++m_num;
    }


    DoubleAggregator::DoubleAggregator()
        : m_sum(0)
        , m_num(0)
    {}
        
    double DoubleAggregator::getMean()
    {
        int n = m_num ? m_num : 1;
        return m_sum / n;
    }

    void DoubleAggregator::add(double value)
    {
        m_sum += value;
        ++m_num;
    }

    sat::NMEA::Geodetic parseLocationNMEA(std::string str)
    {
        return convertToNMEA(parseLocation(str));
    }
    sat::NMEA::Geodetic convertToNMEA(sat::Geodetic a)
    {
        sat::NMEA::Geodetic nmeaGeodetic;
        nmeaGeodetic.latitude  = a.latitude * R2D;
        nmeaGeodetic.longitude = a.longitude * R2D;
        nmeaGeodetic.altitude  = a.altitude * R2D;
        return nmeaGeodetic;
        
    }

    sat::Geodetic parseLocation(std::string str)
    {
        std::vector<std::string> locationParts = split(str,',');
        std::istringstream latitudeS(locationParts[0]);
        std::istringstream longitudeS(locationParts[1]);
        double latitude;
        double longitude;
        latitudeS >> latitude;
        longitudeS >> longitude;
        latitude *= D2R;
        longitude *= D2R;
        sat::Geodetic location(latitude, longitude, 0, 0);
        return location;
    }

    sat::Geodetic parseLocation(std::string str, NMEA::Date& date, NMEA::UTC& time)
    {
        sat::Geodetic location = parseLocation(str);
        location.julianDate = julianDateFromUnix(mk_seconds(date.year,date.month,date.day,time.hour,time.minute,time.seconds));
        return location;
    }

    NMEA::Geodetic addGeodetic(NMEA::Geodetic a, NMEA::Geodetic b)
    {
        NMEA::Geodetic result(a);
        result.latitude  += a.latitude;
        result.longitude += a.longitude;
        result.altitude  += a.altitude;
        return result;
    }

    NMEA::Geodetic subGeodetic(NMEA::Geodetic a, NMEA::Geodetic b)
    {
        NMEA::Geodetic result(a);
        result.latitude  -= b.latitude;
        result.longitude -= b.longitude;
        result.altitude  -= b.altitude;
        return result;
    }

    double magDiffGeodetic(NMEA::Geodetic a, NMEA::Geodetic b)
    {
        return magGeodetic(subGeodetic(a,b));
    }

    double magGeodetic(NMEA::Geodetic a)
    {
        return sqrt(a.latitude*a.latitude + a.longitude*a.longitude);
    }


} // namespace eval 
} // namespace gnssShadowing 
