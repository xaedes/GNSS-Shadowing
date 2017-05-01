#pragma once

#include <functional>
#include <vector>
#include <string>
#include <memory>

namespace gnssShadowing {
namespace sat {

    class NMEA
    {
    public:

        struct UTC
        {
            int hour;    // [00..23]
            int minute;  // [00..59]
            int seconds; // [00..59]
        };
        struct Date
        {
            int day;   // [1..31]
            int month; // [1..12]
            int year;  // [1950..2050]
        };
        struct Geodetic
        {
            double latitude;  // in degrees
            double longitude; // in degrees
            double altitude;  // in meters
        };
        enum class GPSQualityIndicator
        {
            no_fix,
            gps_fix,
            dgps_fix
        };
        enum class RMCStatus
        {
            Okay,                      // 'A'
            NavigationReceiverWarning, // 'V'
            Precise                    // 'P'
        };
        struct Record
        {
            std::string device;
            virtual ~Record() {}
        };
        struct GGA : public Record
        {
            UTC utc;
            Geodetic geodetic;
            GPSQualityIndicator quality;
            int numSatellitesInView;
            double horizontalDOP;
            double geoidalSeparation; // in degrees
            virtual ~GGA() {}
        };
        struct GNS : public Record
        {
            UTC utc;
            Geodetic geodetic;
            int mode; // unknown use
            int numSatellitesUsed;
            double horizontalDOP;
            double geoidalSeparation; // in degrees
            virtual ~GNS() {}
        };
        struct GSA : public Record
        {
            std::vector<int> satellitesUsed;
            double positionalDOP;
            double horizontalDOP;
            double verticalDOP;
            virtual ~GSA() {}
        };
        struct GSVItem
        {
            int satelliteNumber;
            int elevation;      // in degrees
            int azimuth;        // in degrees
            int snr;            // in dB
        };
        struct GSV : public Record
        {
            int numMessages;
            int messageNumber;
            int numSatellitesInView;
            std::vector<GSVItem> satellitesInView;
            virtual ~GSV() {}
        };
        struct RMC : public Record
        {
            UTC utc;
            RMCStatus status;
            Geodetic geodetic;
            double speedKnots;      // in knots
            double directionTrue;   // movement direction to geographic north in degree
            Date date;
            double magneticVariation;
            virtual ~RMC() {}
        };
        struct VTG : public Record
        {
            double directionTrue;
            double directionMagnetic;
            double speedKnots;
            double speedKmh;
            virtual ~VTG() {}
        };
    public:
        NMEA();
        NMEA(std::string filename);

        std::vector<std::shared_ptr<Record>> m_records;

        void read(std::string filename);
        void purge();
    protected:
        std::string m_filename;

        std::vector<std::shared_ptr<Record>> mergeGSVItems(std::vector<std::shared_ptr<Record>> records);

        void read_UTC(std::string token, int &hours, int& minutes, int& seconds);
        void read_Date(std::string token, int &day, int& month, int& year);
        void read_Latitude(std::string token, std::string followingToken, double& latitude);
        void read_Longitude(std::string token, std::string followingToken, double& longitude);
        void read_MagneticVariation(std::string token, std::string followingToken, double& magneticVariation);
        void read_GPSQualityIndicator(std::string token, GPSQualityIndicator& quality);
        void read_RMCStatus(std::string token, RMCStatus& status);
        void read_int(std::string token, int& value);
        void read_double(std::string token, double& value);
        NMEA::GSVItem read_gsvItem(std::string token1, std::string token2, std::string token3, std::string token4);
        int read_int(std::string token);
        double read_double(std::string token);

        std::shared_ptr<Record> readStatement_GGA(std::string device,std::vector<std::string> tokens);
        std::shared_ptr<Record> readStatement_GNS(std::string device,std::vector<std::string> tokens);
        std::shared_ptr<Record> readStatement_GSA(std::string device,std::vector<std::string> tokens);
        std::shared_ptr<Record> readStatement_GSV(std::string device,std::vector<std::string> tokens);
        std::shared_ptr<Record> readStatement_RMC(std::string device,std::vector<std::string> tokens);
        std::shared_ptr<Record> readStatement_VTG(std::string device,std::vector<std::string> tokens);
    };

    typedef std::function<std::shared_ptr<NMEA::Record>(std::string,std::vector<std::string>)> NMEAStatementReaderFunction;

} //namespace sat
} // namespace gnssShadowing
