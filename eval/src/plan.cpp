#include <iostream>
#include <vector>
#include <memory>
#include <fstream>

#include "eval/version.h"
#include "eval/utils.h"
#include "sat/nmea.h"
#include <iomanip>      // std::setprecision
#include <ios>          // std::fixed
using namespace gnssShadowing::eval;
using namespace gnssShadowing::sat;
using namespace std;

int findClosest(vector<NMEA::Geodetic> locations, NMEA::Geodetic needle)
{
    if (locations.size() == 0) return -1; // nothing to find
    if (locations.size() == 1) return 0;  // only item is closest item

    double minDifference = magDiffGeodetic(locations[0],needle);
    int bestIndex = 0;
    for (int i=1; i<locations.size(); i++)
    {
        double diff = magDiffGeodetic(locations[i],needle);
        if (diff < minDifference) 
        {
            minDifference = diff;
            bestIndex = i;
        }
    }
    return bestIndex;
}

int main(int argc, char* argv[])
{	
    // cout << "version " << Version::getString() << endl;
    // cout << "revision " << Version::getRevision() << endl;

    if (argc < 3)
    {
        cout << "usage: program <nmea-filename> <plan-filename> <datetime YYYY-MM-DD HH:MM:SS>" << endl;
        return -1;
    }
    
    NMEA nmea(argv[1]);
    string planFilename(argv[2]);
    // string dateTimeStr(argv[3]);
    typedef int PlanIndex;
    typedef double HDOP;
    
    ifstream fileStream(planFilename);
    string line;
    vector<HDOP> planHDOPs;
    vector<int> planXs;
    vector<int> planYs;
    vector<NMEA::Geodetic> planLocations;
    while (getline(fileStream, line))
    {
        istringstream iss(line);
        HDOP hdop;
        string locationStr;
        int x,y;
        iss >> hdop;
        iss >> locationStr;
        iss >> x;
        iss >> y;
        planHDOPs.push_back(hdop);
        planLocations.push_back(parseLocationNMEA(locationStr));
        planXs.push_back(x);
        planYs.push_back(y);
    }
    if (planLocations.size() ==0)
    {
        cout << "Plan contains no items" << endl;
        return -1;
    }

    // for (auto item:planLocations)
    // {
    //     cout << item.latitude << "," << item.longitude << endl;
    // }


    vector<NMEAGeodeticAggregator> geodeticAggregators;
    vector<DoubleAggregator> hDOPAggregators;
    vector<DoubleAggregator> geodeticDiffAggregators;
    for (int i=0;i<planLocations.size();i++)
    {
        NMEAGeodeticAggregator geodeticAggregator;
        DoubleAggregator hDOPAggregator;
        DoubleAggregator geodeticDiffAggregator;
        geodeticAggregators.push_back(geodeticAggregator);
        hDOPAggregators.push_back(hDOPAggregator);
        geodeticDiffAggregators.push_back(geodeticDiffAggregator);
    }

    // maps records to closest
    vector<PlanIndex> closest;

    cout << std::setprecision(6) << std::fixed;

    cout << "\"#\"" 
        << "\t" << "\"latitude\""
        << "\t" << "\"longitude\""
        << "\t" << "\"hdop\""
        << "\t" << "\"plan#\""
        << endl;
    int i=0;
    for (shared_ptr<NMEA::Record>& record : nmea.m_records)
    {
        NMEA::GGA* gga = dynamic_cast<NMEA::GGA*>(record.get());
        if (gga)
        {
            int idx = findClosest(planLocations, gga->geodetic);
            // cout << "closest idx " << idx << endl;
            geodeticAggregators[idx].add(gga->geodetic);
            hDOPAggregators[idx].add(gga->horizontalDOP);
            geodeticDiffAggregators[idx].add(magDiffGeodetic(gga->geodetic, planLocations[idx]));
            cout << i << "\t" << gga->geodetic.latitude 
                      << "\t" << gga->geodetic.longitude 
                      << "\t" << gga->horizontalDOP 
                      << "\t" << idx
                      << endl;
            ++i;
        }
        NMEA::GNS* gns = dynamic_cast<NMEA::GNS*>(record.get());
        if (gns)
        {
            int idx = findClosest(planLocations, gns->geodetic);
            // cout << "closest idx " << idx << endl;
            geodeticAggregators[idx].add(gns->geodetic);
            hDOPAggregators[idx].add(gns->horizontalDOP);
            geodeticDiffAggregators[idx].add(magDiffGeodetic(gns->geodetic, planLocations[idx]));
            cout << i << "\t" << gns->geodetic.latitude 
                      << "\t" << gns->geodetic.longitude 
                      << "\t" << gns->horizontalDOP 
                      << "\t" << idx
                      << endl;
            ++i;
        }
        NMEA::GSA* gsa = dynamic_cast<NMEA::GSA*>(record.get());
        if (gsa)
        {
            // hDOPAggregator.add(gsa->horizontalDOP);
            // cout << gsa->horizontalDOP << endl;
        }
        NMEA::GSV* gsv = dynamic_cast<NMEA::GSV*>(record.get());
        if (gsv)
        {

        }
        NMEA::RMC* rmc = dynamic_cast<NMEA::RMC*>(record.get());
        if (rmc)
        {
            // geodeticAggregator.add(rmc->geodetic);
        }
        NMEA::VTG* vtg = dynamic_cast<NMEA::VTG*>(record.get());
        if (vtg)
        {

        }
    }
    cout << "---" << endl;
    cout << "\"#\"" 
        << "\t" << "\"plan latitude\""
        << "\t" << "\"plan longitude\""
        << "\t" << "\"plan hdop\""
        << "\t" << "\"x\""
        << "\t" << "\"y\""
        << "\t" << "\"num corresponding\""
        << "\t" << "\"geodetic diff\""
        << "\t" << "\"mean latitude\""
        << "\t" << "\"mean longitude\""
        << "\t" << "\"mean hdop\""
        << "\t" << "\"plan hdop-mean hdop\""
        << endl;
    for (int i=0; i<planLocations.size(); i++)
    {
        cout << i << "\t" << planLocations[i].latitude << "\t" << planLocations[i].longitude << "\t";
        cout << planHDOPs[i] << "\t";
        // cout << "|\t";
        cout << planXs[i] << "\t";
        cout << planYs[i] << "\t";
        cout << geodeticAggregators[i].num() << "\t";
        bool correspondence = geodeticAggregators[i].num() && hDOPAggregators[i].num();
        if (correspondence)
        {
            cout << geodeticDiffAggregators[i].getMean() << "\t";
            NMEA::Geodetic geodeticMean = geodeticAggregators[i].getMean();
            cout << geodeticMean.latitude << "\t" << geodeticMean.longitude << "\t";
            double hDOPMean = hDOPAggregators[i].getMean();
            cout << hDOPMean << "\t";
            cout << hDOPMean-planHDOPs[i] << "\t";
        }
        cout << endl;
    }


}
