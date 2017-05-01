#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>      // std::setprecision
#include <ios>      // std::fixed
#include <stdio.h>

#include "planning/version.h"
#include "planning/astar.h"
#include "planning/problems.h"

#include "mapping/mapperLazyTimesteps.h"
#include "mapping/dopMap.h"
#include "mapping/visibilityMap.h"
#include "mapping/mapProperties.h"
#include "mapping/mapCoordinateConverter.h"
#include "shadowing/shadowing.h"
#include "common/math.h"
#include "common/fileStorageUtil.h"
#include "common/timing.h"
#include "obj/objFile.h"
#include "sat/satStore.h"
#include "sat/sat.h"
#include "sat/coordinateSystems.h"
#include "world/world.h"

#include <stdint.h> // uint16_t etc.

using namespace gnssShadowing::planning;
using namespace gnssShadowing::common;
using namespace gnssShadowing;

using namespace std;

void printDirection(sat::Geodetic pos1, sat::Geodetic pos2)
{
    sat::Geodetic diff;
    diff.latitude  = pos2.latitude  - pos1.latitude;
    diff.longitude = pos2.longitude - pos1.longitude;
    float eps = 0;
    if (abs(diff.latitude) > abs(diff.longitude))
    {
        if (diff.latitude > eps)
        {
            std::cout << "north";
        } 
        else if (diff.latitude < -eps)
        {
            std::cout << "south";
        }
        
    }
    else
    {
        if (diff.longitude > eps)
        {
            std::cout << "east";
        } 
        else if (diff.longitude < -eps)
        {
            std::cout << "west";
        }
    }
}


// std::ostream& operator<<(std::ostream& stream, const MapTimeNode& node)
// 
bool parseGeolocation(std::string str, sat::Geodetic& geodetic, double time)
{
    float latitude;
    float longitude;
    if (2 == sscanf(str.c_str(),"%f,%f",&latitude,&longitude))
    {
        geodetic = sat::Geodetic(latitude*D2R, longitude*D2R,0,time);
        return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    std::cout << "version " << Version::getString() << std::endl;
    std::cout << "revision " << Version::getRevision() << std::endl;
    
    std::string settingsFilename = "plannerSettings.xml";
    if (argc == 2)
    {
        settingsFilename = argv[1];
    }
    cv::FileStorage settings = openFileStorage(settingsFilename);

    float res = readNodeAndDefaultIfNonexistent("GridResolution", settings, (float)10.0f);
    float w = readNodeAndDefaultIfNonexistent("SouthNorthDimension", settings, (float)400.0f);
    float h = readNodeAndDefaultIfNonexistent("EastWestDimension", settings, (float)800.0f);
    float altitude = readNodeAndDefaultIfNonexistent("GridAltitude", settings, (float)55.5f);
    double timeStep = readNodeAndDefaultIfNonexistent("TimeStepInSeconds", settings, (float)(2*60));
    std::string localTimeString = readNodeAndDefaultIfNonexistent("LocalTime", settings, std::string(""));
    std::string tleFilename = readNodeAndDefaultIfNonexistent("TLEFilename", settings, std::string("data/current.tle"));
    std::string objFilename = readNodeAndDefaultIfNonexistent("ObjFilename", settings, std::string("data/uni.obj"));
    std::string objBuildingKey = readNodeAndDefaultIfNonexistent("BuildingKeyInObjFile", settings, std::string("Building"));
    std::cout
        << "tleFilename '" << tleFilename << "'" << std::endl
        << "objFilename '" << objFilename << "'" << std::endl
        << "objBuildingKey '" << objBuildingKey << "'" << std::endl;
    double startTimeUnixTimeSeconds;
    if (!parseDateTimeAsSeconds(localTimeString, startTimeUnixTimeSeconds))
    {
        startTimeUnixTimeSeconds = now_seconds();
        std::cout << "Couldn't read 'LocalTime' from settings file in format 'YYYY-MM-DD HH:MM:SS'" << std::endl;
        std::cout << "Use current local time." << std::endl;
    }

    std::string startGeolocationStr = readNodeAndDefaultIfNonexistent("StartGeolocation",settings,std::string(""));
    sat::Geodetic startGeolocation;
    if (!parseGeolocation(startGeolocationStr,startGeolocation,startTimeUnixTimeSeconds))
    {
        startGeolocation = sat::Geodetic(52.140992*D2R,11.640801*D2R,altitude,startTimeUnixTimeSeconds);
    }
    std::string goalGeolocationStr = readNodeAndDefaultIfNonexistent("GoalGeolocation",settings,std::string(""));
    sat::Geodetic goalGeolocation;
    if (!parseGeolocation(goalGeolocationStr,goalGeolocation,startTimeUnixTimeSeconds))
    {
        goalGeolocation = sat::Geodetic(52.139854*D2R,11.645191*D2R,altitude,startTimeUnixTimeSeconds);
    }

    float maxHorizontalDOP=readNodeAndDefaultIfNonexistent("MaxHorizontalDOP", settings, (float)5.0);
    float costPerHorizontalDOP=readNodeAndDefaultIfNonexistent("CostPerHorizontalDOP", settings, (float)+1);
    float costPerGridStep=readNodeAndDefaultIfNonexistent("CostPerGridStep", settings, (float)1.0);
    float costPerTimeStep=readNodeAndDefaultIfNonexistent("CostPerTimeStep", settings, (float)1.0);
    float costPerTimeTotal=readNodeAndDefaultIfNonexistent("CostPerTimeTotal", settings, (float)0);
    int timeStepsPerStep=readNodeAndDefaultIfNonexistent("TimeStepsPerStep", settings, (int)0);

    // float costPerHorizontalDOP=+1;
    // float costPerHorizontalDOP=-0.1;
    // float costPerGridStep=0.0;
    // float costPerTimeStep=0.1;
    // float costPerTimeTotal=0.1;
    // float costPerTimeStep=10.1;
    // int timeStepsPerStep=1;

    // float res = 20.0;
    // float res = 10.0;
    // float res = 5.0;
    // float res = 2.0;
    // float res = 1;
    // float w = 400;
    // float h = 800;
    vector<double> planeLevels;
    planeLevels.push_back(altitude);
    mapping::MapProperties mapProperties(-w/2,-h/2,w/res,h/res,res,res,planeLevels);

    // path planning arguments
    // cv::Vec2i start(0,10);
    // cv::Vec2i goal(10,10);
    // double timeStep=2*60; // 2 minutes

    // double startTime = mk_seconds(2017,04,01,16,00,0);
    // double startTime = mk_seconds(2017,04,01,16,15,0);
    // double startTime = now_seconds();

    std::cout << "Start time: ";
    prettyprint_seconds(startTimeUnixTimeSeconds);
    std::cout << std::endl;
    world::World world(tleFilename,startTimeUnixTimeSeconds,objFilename,objBuildingKey);
    mapping::MapperLazyTimesteps maps(world, mapProperties, startTimeUnixTimeSeconds, timeStep);

    // std::cout << maps.m_objOrigin.altitude << std::endl;

    mapping::MapCoordinateConverter converter(mapProperties, world.m_origin);
    
    std::cout << "width: " << mapProperties.m_num_cols << " "
              << "height: " << mapProperties.m_num_rows << std::endl
              << "altitude: " << altitude << std::endl;
    // std::cout << converter.geodeticToGridCoordinate(makeGeodetic(52.140717*D2R,11.640943*D2R))
    //           << std::endl;
    
    std::cout << std::setprecision(6) << std::fixed
              <<  "start " << startGeolocation.latitude*R2D << "," << startGeolocation.longitude*R2D << std::endl
              << "goal " << goalGeolocation.latitude*R2D << "," << goalGeolocation.longitude*R2D << std::endl;

    cv::Vec2i start = converter.geodeticToGridCoordinate(startGeolocation);
    cv::Vec2i goal = converter.geodeticToGridCoordinate(goalGeolocation);

    std::cout << "plan path from " << start << " to " << goal << std::endl;
    std::cout << "distance in grid: " << cv::norm(start-goal) << std::endl;



    std::cout << "Planning parameters:" << std::endl
        << " maxHorizontalDOP " << maxHorizontalDOP << std::endl
        << " costPerHorizontalDOP " << costPerHorizontalDOP << std::endl
        << " costPerGridStep " << costPerGridStep << std::endl
        << " costPerTimeStep " << costPerTimeStep << std::endl
        << " costPerTimeTotal " << costPerTimeTotal << std::endl
        << " timeStepsPerStep " << timeStepsPerStep << std::endl;

    MapTimeProblem pathFindingProblem(maxHorizontalDOP,costPerHorizontalDOP,costPerGridStep,costPerTimeStep,costPerTimeTotal,timeStepsPerStep,maps,
                                      MapTimeNode(0,start),MapTimeNode(-1,goal));
    AStarSolver<MapTimeNode,MapTimeProblem,MapTimeNodeHash,MapTimeNodeEqual> astar;
    std::vector<MapTimeNode> path = astar.findShortestPath(pathFindingProblem);

    cout << "path length: " << path.size() << endl;
    sat::Geodetic lastPos;
    bool first=true;
    float sumDOP=0;
    int nDOP=0;
    for (MapTimeNode node : path)
    {
        double currentTime = maps.getTime(node.first);
        sat::Geodetic pos = converter.gridCoordinateToGeodetic(node.second);
        float dop = maps.getDOPMap(node.first).m_items[0][node.second.x][node.second.y].getHorizontal();
        sumDOP += dop; nDOP++;
        cout << node << "\t";
        cout << "dop: " << dop;
        std::cout << "\tlat,long "  << std::setprecision(6) << std::fixed << pos.latitude*R2D << "," << pos.longitude*R2D;
        if (!first)
        {
            std::cout << "\t";
            printDirection(lastPos, pos);
        }
        else
        {
            std::cout << "\tstart";
        }
        first = false;
        lastPos = pos;

        cout << "\t";
        prettyprint_seconds(currentTime); 
        // cout << endl;
    }
    float meanDOP=sumDOP/nDOP;
    std::cout << "sum dop:  " << sumDOP << std::endl;
    std::cout << "mean dop: " << meanDOP << std::endl;
    std::stringstream google_staticmap_url;
    std::string key="paste google staticmap key here";
    google_staticmap_url << "https://maps.googleapis.com/maps/api/staticmap?";
    // google_staticmap_url << "center=" << maps.m_objOrigin.latitude*R2D << "," << maps.m_objOrigin.longitude*R2D;
    google_staticmap_url << "&zoom=" << 17;
    google_staticmap_url << "&size=" << 1024 << "x" << 1024;
    google_staticmap_url << "&scale=" << 2;
    google_staticmap_url << "&key=" << key;
    
    std::stringstream markers;
    markers << "size:tiny";

    for (MapTimeNode node : path)
    {
        sat::Geodetic pos = converter.gridCoordinateToGeodetic(node.second);
        // https://developers.google.com/maps/documentation/static-maps/intro?hl=de#Latlons
        // Längengrad- und Breitengradangaben werden anhand von Zahlen mit bis zu sechs Nachkommastellen 
        // in einer durch Komma getrennten Zeichenfolge definiert.
        markers << "|" << std::setprecision(6) << std::fixed << pos.latitude*R2D << "," << pos.longitude*R2D;

    }
    google_staticmap_url << "&markers=" << markers.str();

    std::stringstream gpx;
    gpx << std::setprecision(6) << std::fixed
        << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << endl
        << "<gpx version=\"1.1\">" << endl;
    for (MapTimeNode node : path)
    {
        sat::Geodetic pos = converter.gridCoordinateToGeodetic(node.second);
        time_t raw_time = (time_t)maps.getTime(node.first);
        struct tm * timeinfo;
        timeinfo = localtime (&raw_time);
        gpx << "<wpt lat=\"" << pos.latitude*R2D 
            <<    "\" lon=\"" << pos.longitude*R2D 
            << "\">"
            << "<time>"
            // 2011-01-16T23:59:01Z<
            // %F
            << std::put_time( timeinfo, "%Y-%m-%eT%H:%M:%SZ" )
            << "</time>"
            << "</wpt>"
            << endl;
    }
    gpx << "</gpx>" 
        << endl;

    
    std::cout << google_staticmap_url.str() << std::endl;
    std::cout << gpx.str() << std::endl;
}
// https://maps.googleapis.com/maps/api/staticmap?center=47.5952,-122.3316&zoom=16&size=640x400&key=
// https://developers.google.com/maps/documentation/static-maps/intro?hl=de