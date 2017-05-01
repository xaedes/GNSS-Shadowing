#include <iostream>
#include <memory>

#include "eval/version.h"
#include "eval/utils.h"
#include "sat/nmea.h"
#include <iomanip>      // std::setprecision
#include <ios>          // std::fixed
using namespace gnssShadowing::eval;
using namespace gnssShadowing::sat;
using namespace std;

int main(int argc, char* argv[])
{	
    // cout << "version " << Version::getString() << endl;
    // cout << "revision " << Version::getRevision() << endl;

    if (argc < 2) return -1;
    
    NMEA nmea(argv[1]);
    
    NMEAGeodeticAggregator geodeticAggregator;
    DoubleAggregator hDOPAggregator;


    for (shared_ptr<NMEA::Record>& record : nmea.m_records)
    {
        NMEA::GGA* gga = dynamic_cast<NMEA::GGA*>(record.get());
        if (gga)
        {
            geodeticAggregator.add(gga->geodetic);
            hDOPAggregator.add(gga->horizontalDOP);
            // cout << gga->horizontalDOP << endl;
        }
        NMEA::GNS* gns = dynamic_cast<NMEA::GNS*>(record.get());
        if (gns)
        {
            geodeticAggregator.add(gns->geodetic);
            hDOPAggregator.add(gns->horizontalDOP);
            // cout << gns->horizontalDOP << endl;
        }
        NMEA::GSA* gsa = dynamic_cast<NMEA::GSA*>(record.get());
        if (gsa)
        {
            hDOPAggregator.add(gsa->horizontalDOP);
            // cout << gsa->horizontalDOP << endl;
        }
        NMEA::GSV* gsv = dynamic_cast<NMEA::GSV*>(record.get());
        if (gsv)
        {

        }
        NMEA::RMC* rmc = dynamic_cast<NMEA::RMC*>(record.get());
        if (rmc)
        {
            geodeticAggregator.add(rmc->geodetic);
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

    if (hDOPAggregator.num())
    {
        double hDOPMean = hDOPAggregator.getMean();
        cout << "mean HDOP " << hDOPMean << " ";
        output = true;
    }


    if (output) cout << endl;

}
