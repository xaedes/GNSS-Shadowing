#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "mapping/dopMap.h"
#include "mapping/visibilityMap.h"

#include "mapping/mapper.h"
#include "mapping/version.h"
#include "world/world.h"
#include "common/timing.h"
#include "common/fileStorageUtil.h"
#include "sat/coordinateSystems.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


using namespace cv;

using namespace gnssShadowing::mapping;
using namespace gnssShadowing::common;
using namespace gnssShadowing;
using namespace std;


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
    

    std::string settingsFilename = "fourierSettings.xml";
    if (argc == 2)
    {
        settingsFilename = argv[1];
    }
    cv::FileStorage settings = openFileStorage(settingsFilename);

    float minElevation = readNodeAndDefaultIfNonexistent("MinimumSatelliteElevation", settings, (float)5.0f);
    float w = readNodeAndDefaultIfNonexistent("SouthNorthDimension", settings, (float)400.0f);
    float h = readNodeAndDefaultIfNonexistent("EastWestDimension", settings, (float)800.0f);
    float altitude = readNodeAndDefaultIfNonexistent("GridAltitude", settings, (float)(0.0f));
    vector<double> altitudes;
    altitudes.push_back(altitude);
    std::string resolutionsString = readNodeAndDefaultIfNonexistent("GridResolutions", settings, std::string("1.0"));
    std::string localTimeString = readNodeAndDefaultIfNonexistent("LocalTime", settings, std::string(""));
    std::string tleFilename = readNodeAndDefaultIfNonexistent("TLEFilename", settings, std::string("data/current.tle"));
    std::string objFilename = readNodeAndDefaultIfNonexistent("ObjFilename", settings, std::string("data/uni.obj"));
    std::string objBuildingKey = readNodeAndDefaultIfNonexistent("BuildingKeyInObjFile", settings, std::string("Building"));
    std::cout
        << "tleFilename '" << tleFilename << "'" << std::endl
        << "objFilename '" << objFilename << "'" << std::endl
        << "objBuildingKey '" << objBuildingKey << "'" << std::endl;
    double localTimeUnixTimeSeconds;
    if (!parseDateTimeAsSeconds(localTimeString, localTimeUnixTimeSeconds))
    {
        localTimeUnixTimeSeconds = now_seconds();
        std::cout << "Couldn't read 'LocalTime' from settings file in format 'YYYY-MM-DD HH:MM:SS'" << std::endl;
        std::cout << "Using current local time." << std::endl;
    }
    std::istringstream resolutionsStream(resolutionsString);
    std::ostringstream resolutionsStreamOut;
    vector<float> resolutions;
    float resolution;
    while(resolutionsStream >> resolution)
    {
        resolutions.push_back(resolution);
        resolutionsStreamOut << resolution << " ";
    }
    std::cout << "time: ";
    prettyprint_seconds(localTimeUnixTimeSeconds);
    world::World world(tleFilename,localTimeUnixTimeSeconds,objFilename,objBuildingKey);
    sat::Geodetic gridCenter;
    std::string gridCenterStr = readNodeAndDefaultIfNonexistent("GridCenter",settings,std::string(""));
    bool display = (bool)readNodeAndDefaultIfNonexistent("Display",settings,int(0));
    if (!parseGeolocation(gridCenterStr,gridCenter,localTimeUnixTimeSeconds))
    {
        gridCenter = world.m_origin;
    }
    sat::SEZ gridCenterSEZ = gridCenter.toECEF().toSEZ(world.m_origin);

    cout << "center: " << gridCenter.latitude*R2D << "," << gridCenter.longitude*R2D << std::endl;
    cout << "altitude: " << altitude << std::endl;


    cout << std::setprecision(6) << std::fixed;
    cout << "\"resolution\"" 
         << "\t" << "\"radius\"" 
         << "\t" << "\"frequency\"" 
         << "\t" << "\"mean\"" 
         << endl;

    for (float resolution : resolutions)
    {
        mapping::MapProperties mapProperties(
            gridCenterSEZ.position[0]-w/2,
            gridCenterSEZ.position[1]-h/2,
            w/resolution,h/resolution,
            resolution,resolution,
            altitudes);

        double sampleFrequency = 1/resolution;
        double fourierN = 0.5 * sqrt((double)mapProperties.m_num_cols*mapProperties.m_num_cols
                                    +(double)mapProperties.m_num_rows*mapProperties.m_num_rows);
        double frequencyStep = sampleFrequency/(fourierN);
        double frequencyMax = 0.5 * sampleFrequency - frequencyStep;

        // std::cout << std::setprecision(6) << std::fixed
        //           << "center: " << gridCenter.latitude*R2D << "," << gridCenter.longitude*R2D << std::endl
        //           << "width: " << mapProperties.m_num_cols << " "
        //           << "height: " << mapProperties.m_num_rows << std::endl
        //           << "altitudes: " << altitudesStreamOut.str() << std::endl
        //           << "minimum sat elevation: " << minElevation << std::endl;
        Mapper mapper(world, mapProperties, minElevation*D2R);
        DOPMap& dopMap = mapper.computeDOPMap(localTimeUnixTimeSeconds);

        Mat_<float> hdopMat(mapProperties.m_num_rows, mapProperties.m_num_cols);
        for (int y=0; y<mapProperties.m_num_rows; y++)
        {
            float* row = hdopMat.ptr<float>(y);
            for (int x=0; x<mapProperties.m_num_cols; x++)
            {
                double hdop = dopMap.m_items[0][x][y].getHorizontal();
                row[x] = hdop;
            }
        }
        // Mat hdopMat = imread("lena.png", CV_LOAD_IMAGE_GRAYSCALE);
        if (hdopMat.rows > hdopMat.cols)
        {
            hdopMat = hdopMat.t();
        }
        Mat padded;                            //expand input image to optimal size
        int m = getOptimalDFTSize( hdopMat.rows );
        int n = getOptimalDFTSize( hdopMat.cols ); // on the border add zero values
        copyMakeBorder(hdopMat, padded, 0, m - hdopMat.rows, 0, n - hdopMat.cols, BORDER_CONSTANT, Scalar::all(0));

        Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
        Mat complexI;
        merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
     
        dft(complexI, complexI);            // this way the result may fit in the source matrix

        // compute the magnitude and switch to logarithmic scale
        // => log(1 + sqrt(Re(DFT(hdopMat))^2 + Im(DFT(hdopMat))^2))
        split(complexI, planes);                   // planes[0] = Re(DFT(hdopMat), planes[1] = Im(DFT(hdopMat))
        magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
        Mat magI = planes[0];

        magI += Scalar::all(1);                    // switch to logarithmic scale
        log(magI, magI);

        // crop the spectrum, if it has an odd number of rows or columns
        magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
    //*

        // rearrange the quadrants of Fourier image  so that the origin is at the image center
        int cx = magI.cols/2;
        int cy = magI.rows/2;

        Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
        Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
        Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
        Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

        Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);

        q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
        q2.copyTo(q1);
        tmp.copyTo(q2);

        // normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
                                                // viewable image form (float between values 0 and 1).



        // double us = 1/mapProperties.m_x_resolution;
        // double vs = 1/mapProperties.m_y_resolution;
        unordered_map<int, vector<float>> circular;
        for (int y=0; y<magI.rows; y++)
        {
            float* row = magI.ptr<float>(y);
            for (int x=0; x<magI.cols; x++)
            {
                int i = x-cx;
                int j = y-cy;
                int r = (int)ceil(sqrt((float)i*i+(float)j*j));
                // double u1 = us/mapProperties.m_num_cols;
                // double v1 = vs/mapProperties.m_num_rows;
                circular[r].push_back(row[x]);
            }
        }

        for (auto item:circular)
        {
            int r = item.first;
            float sum = 0;
            vector<float> values = item.second;
            for (auto value : values) sum += value;
            float mean = sum;
            if (values.size()) mean /= values.size();
            double frequency = r * frequencyStep;

            cout<< resolution
                << "\t" << r
                << "\t" << frequency
                << "\t" << mean 
                << endl;
        }

        if (display)
        {
            normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
                                                    // viewable image form (float between values 0 and 1).
            imshow("Input Image"       , hdopMat  );    // Show the result
            imshow("spectrum magnitude", magI);
            waitKey();
        }
    }


}
