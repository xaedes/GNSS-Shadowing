
#include <opencv2/opencv.hpp>

#include "mapping/mapProperties.h"
#include "sat/coordinateSystems.h"

namespace gnssShadowing {
namespace mapping {

    class MapCoordinateConverter
    {
    public:
        MapCoordinateConverter(MapProperties mapProperties, sat::Geodetic origin);

        sat::Geodetic gridCoordinateToGeodetic(cv::Point2i coord);
        cv::Point2i geodeticToGridCoordinate(sat::Geodetic geodetic);
        cv::Point2f geodeticToGridCoordinateF(sat::Geodetic geodetic);

    protected:
        MapProperties m_mapProperties;
        sat::Geodetic m_origin;
        sat::ECEF m_originECEF;
        sat::ENU m_originENU;
    };

} // namespace mapping
} // namespace gnssShadowing
