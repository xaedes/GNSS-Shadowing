#include <iostream>
#include <sstream>
#include <memory>
#include <iomanip>      // std::setprecision
#include <ios>          // std::fixed

#include "eval/version.h"

#include "mapping/mapProperties.h"
#include "mapping/mapper.h"
#include "mapping/dopMap.h"
#include "world/world.h"
#include "common/timing.h"

using namespace gnssShadowing::eval;
using namespace gnssShadowing::common;
using namespace gnssShadowing;
using namespace std;

int main(int argc, char* argv[])
{	
    // cout << "version " << Version::getString() << endl;
    // cout << "revision " << Version::getRevision() << endl;

    world::World world("data/2017-03-28.tle",0,"data/uni.obj","Building");

    int numParameter=7;
    if (argc < numParameter+1) return -1;

    istringstream isss[numParameter];
    for (int k=0; k<numParameter; k++)
    {
        isss[k] = istringstream(argv[1+k]);
    }

    double basePlaneLevel;
    double interval;
    int numPlaneLevels;
    double width;
    double height;
    double resolution;
    double maxDuration;

    int k=0;
    isss[k++] >> basePlaneLevel;
    isss[k++] >> interval;
    isss[k++] >> numPlaneLevels;
    isss[k++] >> width;
    isss[k++] >> height;
    isss[k++] >> resolution;
    isss[k++] >> maxDuration;

    double startTime = now_seconds();

    int numIterations = 0;
    int numRows,numCols;
    while((numIterations == 0) || (now_seconds() - startTime < maxDuration))
    {
        std::vector<double> planeLevels;
        for (int k=0; k<numPlaneLevels; k++)
        {
            planeLevels.push_back(basePlaneLevel + k*interval);
        }
        
        float w = width;
        float h = height;
        mapping::MapProperties mapProperties(-w/2,-h/2,w/resolution,h/resolution,resolution,resolution,planeLevels);
        numCols = mapProperties.m_num_cols;
        numRows = mapProperties.m_num_rows;
        mapping::Mapper mapper(world, mapProperties, 5*D2R);
        mapper.updateSats(startTime);
        mapper.m_visibilityMap.clear();
        mapper.m_visibilityMap.populateWithAll();
        mapper.m_dopMap.populate(mapper.m_visibilityMap);

        numIterations++;
    }

    double endTime = now_seconds();
    double totalDuration = endTime - startTime;
    double meanDuration = totalDuration / (numIterations?numIterations:1);
    int numCells = numCols*numRows;
    int numCellsTotal = numCells*numPlaneLevels;
    cout
        << basePlaneLevel   << "\t"        
        << interval         << "\t"
        << numPlaneLevels   << "\t"        
        << width            << "\t"
        << height           << "\t"
        << resolution       << "\t"    
        << numCols          << "\t"
        << numRows          << "\t"
        << numCells         << "\t"
        << numCellsTotal    << "\t"
        << maxDuration      << "\t"    
        << numIterations    << "\t"
        << meanDuration     << "\t"
        << totalDuration    << "\t"
        << endl;

}
