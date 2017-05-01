#include "sat/nmea.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <assert.h>

#include "common/stringUtils.h"

using namespace std;
using namespace gnssShadowing::common;

namespace gnssShadowing {
namespace sat {

    NMEA::NMEA()
    {}

    NMEA::NMEA(std::string filename)
    {
        read(filename);
    }
    void NMEA::purge()
    {
        m_filename = "";
        m_records.clear();
    }
    void NMEA::read(std::string filename)
    {
        purge();
        m_filename = filename;
        ifstream fileStream(m_filename);
        string line;
        unordered_map<string,NMEAStatementReaderFunction> readers({
               {"GGA",bind(&NMEA::readStatement_GGA, this, placeholders::_1, placeholders::_2)},
               {"GNS",bind(&NMEA::readStatement_GNS, this, placeholders::_1, placeholders::_2)},
               {"GSA",bind(&NMEA::readStatement_GSA, this, placeholders::_1, placeholders::_2)},
               {"GSV",bind(&NMEA::readStatement_GSV, this, placeholders::_1, placeholders::_2)},
               {"RMC",bind(&NMEA::readStatement_RMC, this, placeholders::_1, placeholders::_2)},
               {"VTG",bind(&NMEA::readStatement_VTG, this, placeholders::_1, placeholders::_2)}
           });

        std::vector<std::shared_ptr<Record>> records;
        while (getline(fileStream, line))
        {
            istringstream iss(line);

            // cout << line << endl;
            
            if (line.length() == 0) continue; // skip empty lines
            string gpsTime,nachm,date;
            string uselessToken;
            string nmeaStatement;
            iss >> gpsTime;       // e.g. "5:26:47:388"
            iss >> nachm;         // "Nachm."
            iss >> date;          // e.g. "03/26/17"
            iss >> uselessToken;  // "-"
            iss >> nmeaStatement; // e.g. "$GPGSV,4,1,13,12,74,059,27,14,29,303,12,19,16,043,10,24,43,146,17*77"

            if (nmeaStatement.length() == 0) continue; // skip empty lines

            if (nmeaStatement[0] == '$')
            {
                string device = nmeaStatement.substr(1,2);
                string statementType = nmeaStatement.substr(3,3);
                if (readers.count(statementType))
                {
                    vector<string> chk = split(nmeaStatement,'*');
                    vector<string> tokens = split(chk[0],',');
                    std::shared_ptr<Record> record = readers[statementType](device, tokens);
                    if (record.get())
                    {
                        records.push_back(record);
                    }
                    // chk[1] contains checksum
                }
                else
                {
                    cout << "Unknown statement type '" << statementType << "'" << endl;
                }
            }
            else
            {
                cout << "Unknown statement '" << nmeaStatement << "'" << endl;
            }
        }

        // merge GSV items
        m_records = mergeGSVItems(records);

    }
    std::vector<std::shared_ptr<NMEA::Record>> NMEA::mergeGSVItems(std::vector<std::shared_ptr<NMEA::Record>> records)
    {
        std::vector<std::shared_ptr<NMEA::Record>> result;
        GSV* mergedGSV=0;
        for (std::shared_ptr<Record>& record : records)
        {
            GSV* gsv = dynamic_cast<GSV*>(record.get());
            if (gsv)
            {
                if (   mergedGSV
                    &&(mergedGSV->numMessages == gsv->numMessages)
                    &&(mergedGSV->numSatellitesInView == gsv->numSatellitesInView))
                {
                    for (GSVItem& item:gsv->satellitesInView)
                    {
                        mergedGSV->satellitesInView.push_back(item);
                    }
                }
                else
                {
                    if (mergedGSV)
                    {
                        result.push_back(std::shared_ptr<Record>(mergedGSV));
                        mergedGSV = 0;
                    }

                    mergedGSV = new GSV();
                    mergedGSV->device = gsv->device;
                    mergedGSV->numMessages = gsv->numMessages;
                    mergedGSV->numSatellitesInView = gsv->numSatellitesInView;
                    mergedGSV->messageNumber = 0;
                    mergedGSV->satellitesInView = std::vector<GSVItem>(gsv->satellitesInView.begin(),gsv->satellitesInView.end());
                }
            }
            else
            {
                result.push_back(record);
            }
        }
        if (mergedGSV)
        {
            result.push_back(std::shared_ptr<Record>(mergedGSV));
            mergedGSV = 0;
        }
        return result;
    }


    void NMEA::read_int(std::string token, int& value)
    {
        if (token.length() == 0) return;
        istringstream sToken(token);
        sToken >> value;
    }

    void NMEA::read_double(std::string token, double& value)
    {
        if (token.length() == 0) return;
        istringstream sToken(token);
        sToken >> value;
    }
    int NMEA::read_int(std::string token)
    {
        int value;
        read_int(token, value);
        return value;
    }
    double NMEA::read_double(std::string token)
    {
        double value;
        read_double(token, value);
        return value;
    }

    void NMEA::read_UTC(std::string token, int &hours, int& minutes, int& seconds)
    {
        // if (token.length() == 0) return;
        vector<string> utcTokens = split(token,',');
        hours =   read_int(utcTokens[0].substr(0,2));
        minutes = read_int(utcTokens[0].substr(2,2));
        seconds = read_int(utcTokens[0].substr(4,2));
    }

    void NMEA::read_Date(std::string token, int &day, int& month, int& year)
    {
        // if (token.length() == 0) return;
        day   = read_int(token.substr(0,2));
        month = read_int(token.substr(2,2));
        year  = read_int(token.substr(4,2));
        if (year < 50)
            year = 2000 + year;
        else 
            year = 2000 - year;
    }

    void NMEA::read_Latitude(std::string token, std::string followingToken, double& latitude)
    {
        // if (token.length() == 0) return;
        vector<string> subtoken = split(token,'.');
        string degreesString = subtoken[0].substr(0,subtoken[0].size()-2);
        string minutesString = subtoken[0].substr(subtoken[0].size()-2) + "." + subtoken[1];
        double degrees = read_int(degreesString);
        double minutes = read_double(minutesString);
        switch(followingToken[0])
        {
        case 'N':
            latitude = degrees + minutes/60;
            break;
        case 'S':
            latitude = -(degrees + minutes/60);
            break;
        default:
            cout << "Unknown value for following token of latitude '" << followingToken << "'" << endl;
            break;
        }
    }

    void NMEA::read_Longitude(std::string token, std::string followingToken, double& longitude)
    {
        // if (token.length() == 0) return;
        vector<string> subtoken = split(token,'.');
        string degreesString = subtoken[0].substr(0,subtoken[0].size()-2);
        string minutesString = subtoken[0].substr(subtoken[0].size()-2) + "." + subtoken[1];
        double degrees = read_int(degreesString);
        double minutes = read_double(minutesString);
        switch(followingToken[0])
        {
        case 'E':
            longitude = degrees + minutes/60;
            break;
        case 'W':
            longitude = -(degrees + minutes/60);
            break;
        default:
            cout << "Unknown value for following token of longitude '" << followingToken << "'" << endl;
            break;
        }
    }
    void NMEA::read_MagneticVariation(std::string token, std::string followingToken, double& magneticVariation)
    {
        // if (token.length() == 0) return;
        double value = read_double(token);
        switch(followingToken[0])
        {
        case 'E':
            magneticVariation = value;
            break;
        case 'W':
            magneticVariation = -value;
            break;
        default:
            cout << "Unknown value for following token of magnetic variation '" << followingToken << "'" << endl;
            break;
        }
    }
    void NMEA::read_GPSQualityIndicator(std::string token, GPSQualityIndicator& quality)
    {
        // if (token.length() == 0) return;
        int value = read_int(token);
        assert(0 <= value);
        assert(value <= 2);
        quality = (GPSQualityIndicator)value;
    }

    void NMEA::read_RMCStatus(std::string token, RMCStatus& status)
    {
        assert(token.length() == 1);
        switch(token[0])
        {
        case 'A':
            status = RMCStatus::Okay;
            break;
        case 'V':
            status = RMCStatus::NavigationReceiverWarning;
            break;
        case 'P':
            status = RMCStatus::Precise;
            break;
        default:
            cout << "Unknown value for RMCStatus '" << token << "'" << endl;
            break;
        }
    }
    std::shared_ptr<NMEA::Record> NMEA::readStatement_GGA( std::string device, std::vector<std::string> tokens)
    {
        // http://www.nmea.de/nmea0183datensaetze.html#gga
        // GGA - Global Positioning System Fix Data, Time, Position and fix related data fora GPS receiver.
        //                                                      11 
        //        1         2       3 4        5 6 7  8   9  10 |  12 13  14   15 
        //        |         |       | |        | | |  |   |   | |   | |   |    | 
        // $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>
        // 
        //  1) Universal Time Coordinated (UTC) 
        //  2) Latitude 
        //  3) N or S (North or South) 
        //  4) Longitude 
        //  5) E or W (East or West) 
        //  6) GPS Quality Indicator, 
        //     0 - fix not available, 
        //     1 - GPS fix, 
        //     2 - Differential GPS fix 
        //  7) Number of satellites in view, 00 - 12 
        //  8) Horizontal Dilution of precision 
        //  9) Antenna Altitude above/below mean-sea-level (geoid)  
        // 10) Units of antenna altitude, meters 
        // 11) Geoidal separation, the difference between the WGS-84 earth 
        //     ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level 
        //     below ellipsoid 
        // 12) Units of geoidal separation, meters 
        // 13) Age of differential GPS data, time in seconds since last SC104 
        //     type 1 or 9 update, null field when DGPS is not used 
        // 14) Differential reference station ID, 0000-1023 
        // 15) Checksum

        if (tokens[1].length() == 0) return std::shared_ptr<Record>(0);

        GGA* record = new GGA();
        record->device = device;
        
        read_UTC(tokens[1], record->utc.hour, record->utc.minute, record->utc.seconds);
        read_Latitude(tokens[2], tokens[3], record->geodetic.latitude);
        read_Longitude(tokens[4], tokens[5], record->geodetic.longitude);
        read_GPSQualityIndicator(tokens[6], record->quality);
        read_int(tokens[7], record->numSatellitesInView);
        read_double(tokens[8], record->horizontalDOP);
        read_double(tokens[9], record->geodetic.altitude);
        // assert(tokens[10] == "M"); // 10) Units of antenna altitude, meters 
        read_double(tokens[11], record->geoidalSeparation);
        // assert(tokens[12] == "M"); // 10) Units of antenna altitude, meters 

        // 13) Age of differential GPS data, time in seconds since last SC104 
        //     type 1 or 9 update, null field when DGPS is not used 
        // DGPS not used!
        // assert(tokens[13] == "");
        
        return std::shared_ptr<Record>((Record*)record);
    }   

    std::shared_ptr<NMEA::Record> NMEA::readStatement_GNS(std::string device, std::vector<std::string> tokens)
    {
        // GNS - GNSS fixed data
        //
        //            1         2          3           4    5  6   7   8   9   10  11 
        //        |         |          |           |    |  |   |   |   |   |   | 
        // $--GNS,hhmmss.ss,llll.lll,a,yyyyy.yyy,a,c--c,xx,x.x,x.x,x.x,x.x,x.x*hh<CR><LF>
        // 
        // Field Number:  
        // 1) UTC of position 
        // 2) Latitude, N/S 
        // 3) Longitude, E/W 
        // 4) Mode indicator 
        // 5) Total number of satllite in use,00-99 
        // 6) HDOP 
        // 7) Antenna altitude, metres, re:mean-sea-level(geoid) 
        // 8) Geoidal separation 
        // 9) Age of differential data 
        // 10) Differential reference station ID 
        // 11) Checksum
        if (tokens[1].length() == 0) return std::shared_ptr<Record>(0);

        GNS* record = new GNS();
        record->device = device;

        read_UTC(tokens[1], record->utc.hour, record->utc.minute, record->utc.seconds);
        read_Latitude(tokens[2], tokens[3], record->geodetic.latitude);
        read_Longitude(tokens[4], tokens[5], record->geodetic.longitude);
        // read_int(tokens[6], record->mode);
        read_int(tokens[7], record->numSatellitesUsed);
        read_double(tokens[8], record->horizontalDOP);
        read_double(tokens[9], record->geodetic.altitude);
        read_double(tokens[10], record->geoidalSeparation);

        return std::shared_ptr<Record>((Record*)record);
    }

    std::shared_ptr<NMEA::Record> NMEA::readStatement_GSA(std::string device, std::vector<std::string> tokens)
    {
        // GSA - GPS DOP and active satellites
        //
        //             1 2 3                    14 15  16  17  18 
        //         | | |                    |  |   |   |   | 
        //  $--GSA,a,a,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x.x,x.x,x.x*hh<CR><LF>
        //
        //  Field Number:  
        //   1) Selection mode 
        //   2) Mode 
        //   3) ID of 1st satellite used for fix 
        //   4) ID of 2nd satellite used for fix 
        //   ... 
        //   14) ID of 12th satellite used for fix 
        //   15) PDOP 
        //   16) HDOP 
        //   17) VDOP 
        //   18) checksum
        if (tokens.size() < 16 || tokens[15].length() == 0) return std::shared_ptr<Record>(0);

        GSA* record = new GSA();
        record->device = device;

        for (int i=3; i<3+12;i++)
        {
            if (tokens[i] == "") break;
            record->satellitesUsed.push_back(read_int(tokens[i]));
        }
        if (tokens.size() >= 18)
        {
            read_double(tokens[15], record->positionalDOP);
            read_double(tokens[16], record->horizontalDOP);
            read_double(tokens[17], record->verticalDOP);
        }

        return std::shared_ptr<Record>((Record*)record);
    }
    
    NMEA::GSVItem NMEA::read_gsvItem(std::string token1, std::string token2, std::string token3, std::string token4)
    {
        GSVItem item;
        item.satelliteNumber = read_int(token1);
        item.elevation       = read_int(token2);
        item.azimuth         = read_int(token3);
        item.snr             = read_int(token4);
        return item;
    }

    std::shared_ptr<NMEA::Record> NMEA::readStatement_GSV(std::string device, std::vector<std::string> tokens)
    {
        // GSV - Satellites in view
        //
        //        1 2 3 4 5 6 7     n 
        //         | | | | | | |     | 
        //  $--GSV,x,x,x,x,x,x,x,...*hh<CR><LF>
        // 
        // Field Number:  
        //  1) total number of messages 
        //  2) message number 
        //  3) satellites in view 
        //  4) satellite number 
        //  5) elevation in degrees 
        //  6) azimuth in degrees to true 
        //  7) SNR in dB 
        //  more satellite infos like 4)-7) 
        //  n) checksum
        GSV* record = new GSV();
        record->device = device;

        record->numMessages = read_int(tokens[1]);
        record->messageNumber = read_int(tokens[2]);
        record->numSatellitesInView = read_int(tokens[3]);
        int n = (tokens.size()-4)/4;
        for (int i=0; i<n; i++)
        {
            GSVItem item = read_gsvItem(tokens[4+i*4],tokens[5+i*4],tokens[6+i*4],tokens[7+i*4]);
            record->satellitesInView.push_back(item);
        }

        return std::shared_ptr<Record>((Record*)record);
    }

    std::shared_ptr<NMEA::Record> NMEA::readStatement_RMC(std::string device, std::vector<std::string> tokens)
    {
        // RMC - Recommended Minimum Navigation Information
        // 
        //                                                             12 
        //         1         2 3       4 5        6 7   8   9    10  11| 
        //         |         | |       | |        | |   |   |    |   | | 
        //  $--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxx,x.x,a*hh<CR><LF>
        // 
        //  Field Number:  
        //   1) UTC Time 
        //   2) Status, V = Navigation receiver warning, P = Precise
        //   3) Latitude 
        //   4) N or S 
        //   5) Longitude 
        //   6) E or W 
        //   7) Speed over ground, knots 
        //   8) Track made good, degrees true 
        //   9) Date, ddmmyy 
        //  10) Magnetic Variation, degrees 
        //  11) E or W 
        //  12) Checksum
        if (tokens[1].length() == 0) return std::shared_ptr<Record>(0);
        
        RMC* record = new RMC();
        record->device = device;

        read_UTC(tokens[1], record->utc.hour, record->utc.minute, record->utc.seconds);
        read_RMCStatus(tokens[2], record->status);
        read_Latitude(tokens[3], tokens[4], record->geodetic.latitude);
        read_Longitude(tokens[5], tokens[6], record->geodetic.longitude);
        read_double(tokens[7], record->speedKnots);
        read_double(tokens[8], record->directionTrue);
        read_Date(tokens[9], record->date.day, record->date.month, record->date.year);
        read_MagneticVariation(tokens[10], tokens[11], record->magneticVariation);

        return std::shared_ptr<Record>((Record*)record);
    }

    std::shared_ptr<NMEA::Record> NMEA::readStatement_VTG(std::string device, std::vector<std::string> tokens)
    {
        // VTG - Track made good and Ground speed
        // 
        //         1   2 3   4 5  6 7   8 9 
        //         |   | |   | |  | |   | | 
        //  $--VTG,x.x,T,x.x,M,x.x,N,x.x,K*hh<CR><LF>
        // 
        //  Field Number:  
        //   1) Track Degrees 
        //   2) T = True 
        //   3) Track Degrees 
        //   4) M = Magnetic 
        //   5) Speed Knots 
        //   6) N = Knots 
        //   7) Speed Kilometers Per Hour 
        //   8) K = Kilometers Per Hour 
        //   9) Checksum
        VTG* record = new VTG();
        record->device = device;

        read_double(tokens[1], record->directionTrue);
        assert(tokens[2] == "T");
        read_double(tokens[3], record->directionMagnetic);
        assert(tokens[4] == "M");
        read_double(tokens[5], record->speedKnots);
        assert(tokens[6] == "N");
        read_double(tokens[7], record->speedKmh);
        assert(tokens[8] == "K");

        return std::shared_ptr<Record>((Record*)record);
    }


} //namespace sat
} // namespace gnssShadowing
