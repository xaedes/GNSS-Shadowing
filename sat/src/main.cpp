#include <iostream>

#include "sat/version.h"
#include "sat/satStore.h"
#include "sat/sat.h"
#include "common/math.h"
#include "common/timing.h"

using namespace gnssShadowing::sat;
using namespace gnssShadowing::common;
using namespace std;

int main(int argc, char* argv[])
{	
    cout << "version " << Version::getString() << endl;
    cout << "revision " << Version::getRevision() << endl;

    SatStore sats("data/current.tle");
    while(true)
    {
        double now = now_seconds();
        cout << "now: " << now << endl;
        for(Sat sat:sats.m_sats)
        {
            TEME teme = sat.findTEME(now);
            ECEF ecef = teme.toECEF();
            Geodetic geo = ecef.toGeodetic();
            Geodetic geo0(geo);
            Geodetic geo1(geo);
            geo0.altitude=0;
            geo1.altitude=10;
            ECEF ecef0 = geo0.toECEF();
            ECEF ecef1 = geo1.toECEF();
            ENU enu  = ecef.toENU(geo);
            ENU enu0 = ecef0.toENU(geo);
            ENU enu1 = ecef1.toENU(geo);
            ENU origin(enu0);
            enu -= origin;
            enu0 -= origin;
            enu1 -= origin;
            cout << sat.getName();
            cout << "#" << sat.getSatNumber();
            // cout << " teme " << teme.position[0]
            //           << "," << teme.position[1]
            //           << "," << teme.position[2];
            // cout << " ecef " << ecef.position[0]
            //           << "," << ecef.position[1]
            //           << "," << ecef.position[2];
            // cout << " ecef0 " << ecef0.position[0]
            //           << "," << ecef0.position[1]
            //           << "," << ecef0.position[2];
            cout << " enu "  << enu.position[0]
                      << "," << enu.position[1]
                      << "," << enu.position[2];
            cout << " enu0 "  << enu0.position[0]
                      << "," << enu0.position[1]
                      << "," << enu0.position[2];
            cout << " enu1 "  << enu1.position[0]
                      << "," << enu1.position[1]
                      << "," << enu1.position[2];
            cout << " latitude: " << geo.latitude*R2D;
            cout << " longitude: " << geo.longitude*R2D;
            cout << " altitude: " << geo.altitude;

            cout << endl;
        }
        cout << "----" << endl;
    }
}
