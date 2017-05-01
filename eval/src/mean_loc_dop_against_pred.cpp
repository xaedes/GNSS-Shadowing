#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <opencv2/opencv.hpp>

#include "eval/version.h"
#include "eval/utils.h"
#include "sat/nmea.h"
#include "sat/coordinateSystems.h"
#include "sat/dop.h"
#include "sat/wdop.h"
#include "mapping/dopMap.h"
#include "mapping/mapper.h"
#include "mapping/mapProperties.h"
#include "mapping/mapCoordinateConverter.h"
#include "world/world.h"
#include "common/math.h"
#include "common/stringUtils.h"
#include <iomanip>      // std::setprecision
#include <ios>          // std::fixed
#include <memory>

using namespace gnssShadowing::common;
using namespace gnssShadowing::eval;
using namespace gnssShadowing::sat;
using namespace gnssShadowing;
using namespace std;

void doForRes(std::vector<double> planeLevels, world::World& world, sat::Geodetic location, double unixTimeSeconds, float res, int verbosity)
{
    float w = 400;
    float h = 800;
    mapping::MapProperties mapProperties(-w/2,-h/2,w/res,h/res,res,res,planeLevels);


    mapping::Mapper mapper(world, mapProperties);
    mapping::DOPMap map=mapper.computeDOPMap(unixTimeSeconds);
    mapping::MapCoordinateConverter converter(mapProperties, world.m_origin);

    cv::Vec2i coord = converter.geodeticToGridCoordinate(location);
    double predictedHDOP = map.m_items[0][coord[0]][coord[1]].getHorizontal();

    if (verbosity == 2)
    {
        cout << "[coord " << coord;
        cout << mapProperties;
        cout << "predicted HDOP " << predictedHDOP << "]";
    }
    else if (verbosity == 1)
    {
        cout << predictedHDOP << "\t";
    }
}
void doForSingleLocation(std::vector<double> planeLevels, world::World& world, sat::Geodetic location, double unixTimeSeconds, int verbosity)
{
    float res = 1;
    mapping::MapProperties mapProperties = mapping::MapProperties::SingleLocation(world, location, res, planeLevels[0]);
    mapping::Mapper mapper(world, mapProperties);
    mapping::DOPMap map=mapper.computeDOPMap(unixTimeSeconds);
    mapping::MapCoordinateConverter converter(mapProperties, world.m_origin);

    cv::Vec2i coord = converter.geodeticToGridCoordinate(location);
    double predictedHDOP = map.m_items[0][coord[0]][coord[1]].getHorizontal();
    int numSats = mapper.m_visibilityMap.m_items[0][coord[0]][coord[1]].size();
    if (verbosity == 2)
    {
        cout << "[coord " << coord
             << mapProperties
             << "predicted HDOP " << predictedHDOP 
             << "numSats " << numSats 
             << "]";
    }
    else if (verbosity == 1)
    {
        cout << predictedHDOP << "\t"
             << numSats << "\t";
    }
}

int main(int argc, char* argv[])
{	
    // cout << "version " << Version::getString() << endl;
    // cout << "revision " << Version::getRevision() << endl;

    if (argc < 2) return -1;

    world::World world("data/2017-03-28.tle",0,"data/uni.obj","Building");


    std::vector<double> planeLevels;
    planeLevels.push_back(55.5);

    NMEA nmea(argv[1]);

    int verbosity = 2;
    if (argc == 4)
    {
        istringstream iss(argv[3]);
        iss >> verbosity;
        assert(verbosity==0 || verbosity==1 || verbosity==2);
    }
    
    NMEA::Geodetic geodeticSum;
    int nGeodetic;
    
    NMEAGeodeticAggregator geodeticAggregator;
    DoubleAggregator hdopObservedAggregator;
    DoubleAggregator hdopVisibleAggregator;
    DoubleAggregator hdopSelectedAggregator;
    DoubleAggregator whdopVisibleAggregator;
    DoubleAggregator whdopSelectedAggregator;


    unique_ptr<NMEA::Date> date;
    unique_ptr<NMEA::UTC> time;


    unordered_map<int,NMEA::GSVItem> satellites;
    for (shared_ptr<NMEA::Record>& record : nmea.m_records)
    {
        NMEA::GGA* gga = dynamic_cast<NMEA::GGA*>(record.get());
        if (gga)
        {
            geodeticAggregator.add(gga->geodetic);
            hdopObservedAggregator.add(gga->horizontalDOP);
            // cout << gga->horizontalDOP << endl;
        }
        NMEA::GNS* gns = dynamic_cast<NMEA::GNS*>(record.get());
        if (gns)
        {
            geodeticAggregator.add(gns->geodetic);
            hdopObservedAggregator.add(gns->horizontalDOP);
            // cout << gns->horizontalDOP << endl;
        }
        NMEA::GSA* gsa = dynamic_cast<NMEA::GSA*>(record.get());
        if (gsa)
        {
            // cout << gsa->horizontalDOP << endl;

            // cout << "gsa---" << endl;
            bool valid=true;
            std::vector<sat::ENU> sats;
            std::vector<float> weights;
            for (auto idx : gsa->satellitesUsed)
            {
                // cout << "satelliteNumber " << idx << " ";
                if (satellites.count(idx))
                {
                    NMEA::GSVItem& item = satellites[idx];
                    // cout << 
                        // "elevation " << item.elevation << " "
                        // "azimuth " << item.azimuth << " ";
                    sat::Horizontal hor;
                    hor.observer = world.m_origin;
                    hor.azimuth = item.azimuth * D2R;
                    hor.elevation = item.elevation * D2R;
                    hor.range = 20000;
                    sat::ENU enu = hor.toENU();
                    sats.push_back(enu);
                    float estimatedSignalVariance = 10;
                    float weight = item.snr / estimatedSignalVariance;
                    weights.push_back(weight);
                } 
                else 
                {
                    valid=false;
                    break;
                }
                // cout << endl;
            }
            if (valid)
            {
                std::vector<int> visible;
                std::unique_ptr<SatPositionProvider> satsProvider(new SatPositionProviderVectorENU(sats));
                for (int k=0; k<satsProvider->getNumSats(); k++) visible.push_back(k);
                sat::DOP dop(world.m_originENU, satsProvider.get(), visible);
                // cout << "gsa->horizontalDOP " << gsa->horizontalDOP << endl;
                // cout << "dop.getHorizontal " << dop.getHorizontal() << endl;
                double hdop = dop.getHorizontal();
                if (hdop<0) hdop=20;
                hdopSelectedAggregator.add(hdop);

                sat::WDOP wdop(world.m_originENU, satsProvider.get(), visible, weights);
                // cout << "gsa->horizontalDOP " << gsa->horizontalDOP << endl;
                // cout << "wdop.getHorizontal " << wdop.getHorizontal() << endl;
                double whdop = wdop.getHorizontal();
                if (whdop<0) whdop=20;
                whdopSelectedAggregator.add(whdop);
            }

        }
        NMEA::GSV* gsv = dynamic_cast<NMEA::GSV*>(record.get());
        if (gsv)
        {
            // cout << "gsv---" << endl;
            std::vector<sat::ENU> sats;
            std::vector<float> weights;
            for(auto item : gsv->satellitesInView)
            {
                // cout << 
                    // "satelliteNumber " << item.satelliteNumber << " "
                    // "elevation " << item.elevation << " "
                    // "azimuth " << item.azimuth << endl;
                satellites[item.satelliteNumber] = item;
                sat::Horizontal hor;
                hor.observer = world.m_origin;
                hor.azimuth = item.azimuth * D2R;
                hor.elevation = item.elevation * D2R;
                hor.range = 20000;
                sat::ENU enu = hor.toENU();
                sats.push_back(enu);
                float estimatedSignalVariance = 10;
                float weight = item.snr / estimatedSignalVariance;
                weights.push_back(weight);
            }
            std::vector<int> visible;
            std::unique_ptr<SatPositionProvider> satsProvider(new SatPositionProviderVectorENU(sats));
            for (int k=0; k<satsProvider->getNumSats(); k++) visible.push_back(k);
            sat::DOP dop(world.m_originENU, satsProvider.get(), visible);
            // cout << "dop.getHorizontal " << dop.getHorizontal() << endl;
            double hdop = dop.getHorizontal();
            if (hdop<0) hdop=20;
            hdopVisibleAggregator.add(hdop);

            sat::WDOP wdop(world.m_originENU, satsProvider.get(), visible, weights);
            // cout << "wdop.getHorizontal " << wdop.getHorizontal() << endl;
            double whdop = wdop.getHorizontal();
            if (whdop<0) whdop=20;
            whdopVisibleAggregator.add(whdop);
        }
        NMEA::RMC* rmc = dynamic_cast<NMEA::RMC*>(record.get());
        if (rmc)
        {
            geodeticAggregator.add(rmc->geodetic);
            nGeodetic++;
            if (!date.get())
            {
                date.reset(new NMEA::Date());
                date->day   = rmc->date.day;
                date->month = rmc->date.month;
                date->year  = rmc->date.year;
            }
            if (!time.get())
            {
                time.reset(new NMEA::UTC());
                time->hour    = rmc->utc.hour;
                time->minute  = rmc->utc.minute;
                time->seconds = rmc->utc.seconds;
            }
        }
        NMEA::VTG* vtg = dynamic_cast<NMEA::VTG*>(record.get());
        if (vtg)
        {

        }
    }
    bool output = false;
    cout << std::setprecision(6) << std::fixed;
    if (geodeticAggregator.num())
    {
        NMEA::Geodetic geodeticMean = geodeticAggregator.getMean();
        cout << geodeticMean.latitude << "," << geodeticMean.longitude << " ";
        output = true;
    }

    if (hdopObservedAggregator.num())
    {
        double meanHDOPObserved = hdopObservedAggregator.getMean();
        if (verbosity == 2)
        {
            cout << "[mean Observed HDOP " << meanHDOPObserved << "]";
        }
        else if (verbosity == 1)
        {
            cout << meanHDOPObserved << "\t";
        }

        output = true;
    }

    if (hdopVisibleAggregator.num())
    {
        double meanHDOPVisible = hdopVisibleAggregator.getMean();
        if (verbosity == 2)
        {
            cout << "[mean Visible HDOP " << meanHDOPVisible << "]";
        }
        else if (verbosity == 1)
        {
            cout << meanHDOPVisible << "\t";
        }
        output = true;
    }

    if (hdopSelectedAggregator.num())
    {
        double meanHDOPSelected = hdopSelectedAggregator.getMean();
        if (verbosity == 2)
        {
            cout << "[mean Selected HDOP " << meanHDOPSelected << "]";
        }
        else if (verbosity == 1)
        {
            cout << meanHDOPSelected << "\t";
        }
        output = true;
    }

    if (whdopVisibleAggregator.num())
    {
        double meanWHDOPVisible = whdopVisibleAggregator.getMean();
        if (verbosity == 2)
        {
            cout << "[mean Visible WHDOP " << meanWHDOPVisible << "]";
        }
        else if (verbosity == 1)
        {
            cout << meanWHDOPVisible << "\t";
        }
        output = true;
    }

    if (whdopSelectedAggregator.num())
    {
        double meanWHDOPSelected = whdopSelectedAggregator.getMean();
        if (verbosity == 2)
        {
            cout << "[mean Selected WHDOP " << meanWHDOPSelected << "]";
        }
        else if (verbosity == 1)
        {
            cout << meanWHDOPSelected << "\t";
        }
        output = true;
    }

    if ((argc > 2) && date.get() && time.get())
    {
        double unixTimeSeconds = mk_seconds(date->year,date->month,date->day,time->hour,time->minute,time->seconds);
        sat::Geodetic location = parseLocation(string(argv[2]), *date.get(), *time.get());
        if (verbosity==2)
        {
            cout << "[location " << location.latitude*R2D << "," << location.longitude*R2D << "]";
        }


        // doForRes(planeLevels,world,location,startTime,20,verbosity);
        // doForRes(planeLevels,world,location,startTime,10,verbosity);
        // doForRes(planeLevels,world,location,startTime,5,verbosity);
        // doForRes(planeLevels,world,location,startTime,2,verbosity);
        // doForRes(planeLevels,world,location,startTime,1,verbosity);
        doForSingleLocation(planeLevels,world,location,unixTimeSeconds,verbosity);

        output = true;
        if (verbosity==2)
        {
            cout << "start time: ";
            prettyprint_seconds(unixTimeSeconds);
        }
    }
    if (output) cout << endl;

}
