
#include "mapping/mapCoordinateConverter.h"

#include "common/timing.h"
#include <iostream>

namespace gnssShadowing {
namespace mapping {

#define KM2M (1.0e3)
#define M2KM (1.0e-3)

    MapCoordinateConverter::MapCoordinateConverter(mapping::MapProperties mapProperties, sat::Geodetic origin):
        m_mapProperties(mapProperties),
        m_origin(origin),
        m_originECEF(m_origin.toECEF()),
        m_originENU(m_originECEF.toENU(m_origin))
    {
    }

    sat::Geodetic MapCoordinateConverter::gridCoordinateToGeodetic(cv::Point2i coord)
    {
        double south = m_mapProperties.X(coord.x);
        double east = m_mapProperties.Y(coord.y);
        sat::SEZ sez;
        sez.position[0] = south * M2KM;
        sez.position[1] = east * M2KM;
        sez.position[2] = 0 * M2KM;
        sez.observer = m_origin;
        sez.observer.julianDate = sez.julianDate;
        sat::ECEF ecef = sez.toECEF();
        sat::Geodetic geodetic = ecef.toGeodetic();
        return geodetic;
    }
    cv::Point2f MapCoordinateConverter::geodeticToGridCoordinateF(sat::Geodetic geodetic)
    {
        sat::ECEF ecef = geodetic.toECEF();
        sat::SEZ sez = ecef.toSEZ(m_origin);
        double south = sez.position[0] * KM2M;
        double east =  sez.position[1] * KM2M;
        cv::Point2f coord(
                (m_mapProperties.I(south)),
                (m_mapProperties.J(east))
        );
        return coord;
    }
    cv::Point2i MapCoordinateConverter::geodeticToGridCoordinate(sat::Geodetic geodetic)
    {
        cv::Point2f coordF = geodeticToGridCoordinateF(geodetic);
        cv::Point2i coord(
                (int)(0.5+coordF.x),
                (int)(0.5+coordF.y)
        );
        return coord;
    }



} // namespace mapping
} // namespace gnssShadowing
