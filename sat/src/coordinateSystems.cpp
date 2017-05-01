
#include "sat/coordinateSystems.h"
#include "sgp4_vallado/sgp4coord.h"
#include "common/math.h"

#include <cstring>

namespace gnssShadowing {
namespace sat {

    double julianDateFromUnix(double unixSecs)
    {
        return getJulianFromUnix(unixSecs);
    }

    ECEF TEME::toECEF() const
    {
        ECEF ecef;
        teme2ecef(position, velocity, julianDate, ecef.position, ecef.velocity);
        ecef.julianDate = julianDate;
        return ecef;
    }

    Geodetic ECEF::toGeodetic()
    {
        double lla[3];
        ijk2ll(position, lla);
        Geodetic geo;
        geo.latitude  = lla[0];
        geo.longitude = lla[1];
        geo.altitude  = lla[2];
        geo.julianDate = julianDate;
        return geo;
    }

    Geodetic::Geodetic(const Geodetic& copyFrom)
    {
        latitude = copyFrom.latitude;
        longitude = copyFrom.longitude;
        altitude = copyFrom.altitude;
        julianDate = copyFrom.julianDate;
    }
    Horizontal::Horizontal(const Horizontal& copyFrom)
    {
        observer = copyFrom.observer;
        azimuth = copyFrom.azimuth;
        elevation = copyFrom.elevation;
        range = copyFrom.range;
        julianDate = copyFrom.julianDate;

    }
    ECEF Geodetic::toECEF() const
    {
        ECEF ecef;
        site(latitude, longitude, altitude, ecef.position, ecef.velocity);
        ecef.julianDate = julianDate;
        return ecef;
    }

    CarthesianCoordinate::CarthesianCoordinate(const CarthesianCoordinate& copyFrom)
    {
        memcpy(position,copyFrom.position,sizeof(copyFrom.position));
        memcpy(velocity,copyFrom.velocity,sizeof(copyFrom.velocity));
        julianDate=copyFrom.julianDate;
    }

    ECEF::ECEF(const ECEF& copyFrom) : CarthesianCoordinate((CarthesianCoordinate)copyFrom)
    {}

    TEME::TEME(const TEME& copyFrom) : CarthesianCoordinate((CarthesianCoordinate)copyFrom)
    {}

    ENU::ENU(const ENU& copyFrom) : CarthesianCoordinate((CarthesianCoordinate)copyFrom), observer(copyFrom.observer)
    {}

    SEZ::SEZ(const SEZ& copyFrom) : CarthesianCoordinate((CarthesianCoordinate)copyFrom), observer(copyFrom.observer)
    {}
    

    cv::Mat ENU::ECEF_ENU(const Geodetic& observer)
    {
        cv::Mat mat_ENU_ECEF = ENU::ENU_ECEF(observer);
        return mat_ENU_ECEF.t(); // inverse of orthogonal (rotation) matrix is transpose
    }

    cv::Mat ENU::ENU_ECEF(const Geodetic& observer)
    {
        //#define STEP_BY_STEP
        #ifdef STEP_BY_STEP
        double cs_lat = cos(PI/2-observer.latitude);
        double sn_lat = sin(PI/2-observer.latitude);
        double cs_lon = cos(observer.longitude);
        double sn_lon = sin(observer.longitude);

        cv::Mat rot3 = (cv::Mat_<double>(3,3) << cs_lon, sn_lon, 0,
                                                -sn_lon, cs_lon, 0,
                                                      0,      0, 1);
        cv::Mat rot2 = (cv::Mat_<double>(3,3) << cs_lat, 0, -sn_lat,
                                                      0, 1, 0,
                                                 sn_lat, 0,  cs_lat);
        cv::Mat SEZ_ECEF=rot2*rot3;
        cv::Mat ENU_SEZ=(cv::Mat_<double>(3,3) <<  0, 1, 0, // east  = east
                                                  -1, 0, 0, // north = -south
                                                   0, 0, 1);// up    = zenith
        
        return ENU_SEZ*SEZ_ECEF;
        // The same as the compressed matrix 
        // see:
        // https://www.wolframalpha.com/input/?i=%7B%7B0,1,+0%7D,+%7B-1,+0,+0%7D,+%7B0,+0,+1%7D%7D*%7B%7Bcos(pi%2F2-a),0,+-sin(pi%2F2-a)%7D,+%7B0,+1,+0%7D,+%7Bsin(pi%2F2-a),+0,+cos(pi%2F2-a)%7D%7D%7B%7Bcos(b),sin(b),+0%7D,+%7B-sin(b),+cos(b),+0%7D,+%7B0,+0,+1%7D%7D        
        #else
        // // http://www.navipedia.net/index.php/Transformations_between_ECEF_and_ENU_coordinates
        double cs_lat = cos(observer.latitude);
        double sn_lat = sin(observer.latitude);
        double cs_lon = cos(observer.longitude);
        double sn_lon = sin(observer.longitude);
        cv::Mat mat_ENU_ECEF=(cv::Mat_<double>(3,3) << -sn_lon,       cs_lon, 0,
                                                -cs_lon*sn_lat, -sn_lon*sn_lat, cs_lat,
                                                 cs_lon*cs_lat,  sn_lon*cs_lat, sn_lat);
        return mat_ENU_ECEF;
        #endif
    }

    cv::Mat SEZ::ECEF_SEZ(const Geodetic& observer)
    {
        cv::Mat mat_SEZ_ECEF = SEZ::SEZ_ECEF(observer);
        return mat_SEZ_ECEF.t(); // inverse of orthogonal (rotation) matrix is transpose
    }

    cv::Mat SEZ::SEZ_ECEF(const Geodetic& observer)
    {
        //#define STEP_BY_STEP
        #ifdef STEP_BY_STEP
        double cs_lat = cos(PI/2-observer.latitude);
        double sn_lat = sin(PI/2-observer.latitude);
        double cs_lon = cos(observer.longitude);
        double sn_lon = sin(observer.longitude);

        cv::Mat rot3 = (cv::Mat_<double>(3,3) << cs_lon, sn_lon, 0,
                                                -sn_lon, cs_lon, 0,
                                                      0,      0, 1);
        cv::Mat rot2 = (cv::Mat_<double>(3,3) << cs_lat, 0, -sn_lat,
                                                      0, 1, 0,
                                                 sn_lat, 0,  cs_lat);
        cv::Mat SEZ_ECEF=rot2*rot3;
        return SEZ_ECEF;
        // The same as the compressed matrix 
        // see:
        // https://www.wolframalpha.com/input/?i=%7B%7Bcos(pi%2F2-a),0,+-sin(pi%2F2-a)%7D,+%7B0,+1,+0%7D,+%7Bsin(pi%2F2-a),+0,+cos(pi%2F2-a)%7D%7D%7B%7Bcos(b),sin(b),+0%7D,+%7B-sin(b),+cos(b),+0%7D,+%7B0,+0,+1%7D%7D
        #else
        // // http://www.navipedia.net/index.php/Transformations_between_ECEF_and_ENU_coordinates
        double cs_lat = cos(observer.latitude);//a
        double sn_lat = sin(observer.latitude);//a
        double cs_lon = cos(observer.longitude);//b
        double sn_lon = sin(observer.longitude);//b
        cv::Mat mat_SEZ_ECEF=(cv::Mat_<double>(3,3) << cs_lon*sn_lat, sn_lat*sn_lon, -cs_lat,
                                                      -sn_lon       ,        cs_lon,       0,
                                                       cs_lat*cs_lon, cs_lat*sn_lon,  sn_lat);
        return mat_SEZ_ECEF;
        #endif
    }

    ENU ECEF::toENU(const Geodetic& observer) const
    {

        ECEF observerECEF=observer.toECEF();
        ECEF rangeECEF(*this);
        rangeECEF -= observerECEF;

        cv::Mat mat_ENU_ECEF = ENU::ENU_ECEF(observer);
        cv::Mat mat_ecef_p = (cv::Mat_<double>(3,1) << rangeECEF.position[0], rangeECEF.position[1], rangeECEF.position[2]);
        cv::Mat mat_ecef_v = (cv::Mat_<double>(3,1) << rangeECEF.velocity[0], rangeECEF.velocity[1], rangeECEF.velocity[2]);
        cv::Mat mat_enu_p = mat_ENU_ECEF*mat_ecef_p;
        cv::Mat mat_enu_v = mat_ENU_ECEF*mat_ecef_v;

        ENU enu;
        enu.julianDate = julianDate;
        enu.observer = observer;
        enu.position[0] = mat_enu_p.at<double>(0,0);
        enu.position[1] = mat_enu_p.at<double>(1,0);
        enu.position[2] = mat_enu_p.at<double>(2,0);
        enu.velocity[0] = mat_enu_v.at<double>(0,0);
        enu.velocity[1] = mat_enu_v.at<double>(1,0);
        enu.velocity[2] = mat_enu_v.at<double>(2,0);
        return enu;
    }
    
    SEZ ECEF::toSEZ(const Geodetic& observer) const
    {

        ECEF observerECEF=observer.toECEF();
        ECEF rangeECEF(*this);
        rangeECEF -= observerECEF;

        cv::Mat mat_SEZ_ECEF = SEZ::SEZ_ECEF(observer);
        cv::Mat mat_ecef_p = (cv::Mat_<double>(3,1) << rangeECEF.position[0], rangeECEF.position[1], rangeECEF.position[2]);
        cv::Mat mat_ecef_v = (cv::Mat_<double>(3,1) << rangeECEF.velocity[0], rangeECEF.velocity[1], rangeECEF.velocity[2]);
        cv::Mat mat_sez_p = mat_SEZ_ECEF*mat_ecef_p;
        cv::Mat mat_sez_v = mat_SEZ_ECEF*mat_ecef_v;

        SEZ sez;
        sez.julianDate = julianDate;
        sez.observer = observer;
        sez.position[0] = mat_sez_p.at<double>(0,0);
        sez.position[1] = mat_sez_p.at<double>(1,0);
        sez.position[2] = mat_sez_p.at<double>(2,0);
        sez.velocity[0] = mat_sez_v.at<double>(0,0);
        sez.velocity[1] = mat_sez_v.at<double>(1,0);
        sez.velocity[2] = mat_sez_v.at<double>(2,0);
        return sez;
    }
    
    ECEF ENU::toECEF() const
    {
        cv::Mat mat_ECEF_ENU = ENU::ECEF_ENU(observer);
        cv::Mat mat_enu_p = (cv::Mat_<double>(3,1) << position[0], position[1], position[2]);
        cv::Mat mat_enu_v = (cv::Mat_<double>(3,1) << velocity[0], velocity[1], velocity[2]);

        cv::Mat mat_ecef_p = mat_ECEF_ENU*mat_enu_p;
        cv::Mat mat_ecef_v = mat_ECEF_ENU*mat_enu_v;

        ECEF ecef;
        ecef.julianDate = julianDate;
        ecef.position[0] = mat_ecef_p.at<double>(0,0);
        ecef.position[1] = mat_ecef_p.at<double>(1,0);
        ecef.position[2] = mat_ecef_p.at<double>(2,0);
        ecef.velocity[0] = mat_ecef_v.at<double>(0,0);
        ecef.velocity[1] = mat_ecef_v.at<double>(1,0);
        ecef.velocity[2] = mat_ecef_v.at<double>(2,0);

        ECEF observerECEF=observer.toECEF();
        ecef += observerECEF;

        return ecef;
    }
    
    ECEF SEZ::toECEF() const
    {
        cv::Mat mat_ECEF_SEZ = SEZ::ECEF_SEZ(observer);
        cv::Mat mat_sez_p = (cv::Mat_<double>(3,1) << position[0], position[1], position[2]);
        cv::Mat mat_sez_v = (cv::Mat_<double>(3,1) << velocity[0], velocity[1], velocity[2]);

        cv::Mat mat_ecef_p = mat_ECEF_SEZ*mat_sez_p;
        cv::Mat mat_ecef_v = mat_ECEF_SEZ*mat_sez_v;

        ECEF ecef;
        ecef.julianDate = julianDate;
        ecef.position[0] = mat_ecef_p.at<double>(0,0);
        ecef.position[1] = mat_ecef_p.at<double>(1,0);
        ecef.position[2] = mat_ecef_p.at<double>(2,0);
        ecef.velocity[0] = mat_ecef_v.at<double>(0,0);
        ecef.velocity[1] = mat_ecef_v.at<double>(1,0);
        ecef.velocity[2] = mat_ecef_v.at<double>(2,0);

        ECEF observerECEF=observer.toECEF();
        ecef += observerECEF;

        return ecef;
    }

    ENU SEZ::toENU() const
    {
        ENU enu;
        enu.position[0] = position[1];
        enu.position[1] = -position[0];
        enu.position[2] = position[2];
        enu.velocity[0] = velocity[1];
        enu.velocity[1] = -velocity[0];
        enu.velocity[2] = velocity[2];
        enu.observer = observer;
        enu.julianDate = julianDate;
        return enu;
    }

    SEZ ENU::toSEZ() const
    {
        SEZ sez;
        sez.position[0] = -position[1];
        sez.position[1] = position[0];
        sez.position[2] = position[2];
        sez.velocity[0] = -velocity[1];
        sez.velocity[1] = velocity[0];
        sez.velocity[2] = velocity[2];
        sez.observer = observer;
        sez.julianDate = julianDate;
        return sez;
    }


    Horizontal TEME::toHorizontal(const Geodetic& observer) const
    {
        double razel[3];
        double razelrates[3];
        rv2azel(position, velocity, 
            observer.latitude, observer.longitude, observer.altitude, 
            julianDate, 
            razel, razelrates);
        Horizontal hor;
        hor.observer = observer;
        hor.julianDate = julianDate;
        hor.range = razel[0];
        hor.azimuth = razel[1];
        hor.elevation = razel[2];
        return hor;
    }



    ENU Horizontal::toENU() const
    {
        ENU enu;
        enu.julianDate = julianDate;
        
        double cs_az = cos(azimuth);
        double sn_az = sin(azimuth);
        double cs_el = cos(elevation);
        double sn_el = sin(elevation);

        double r = cs_el * range;

        enu.position[0] = sn_az*r;
        enu.position[1] = cs_az*r;
        enu.position[2] = sn_el*range;

        // ignore velocity
        enu.velocity[0] = 0;
        enu.velocity[1] = 0;
        enu.velocity[2] = 0;

        return enu;
    }

    SEZ Horizontal::toSEZ() const
    {
        return toENU().toSEZ();
    }

    Horizontal SEZ::toHorizontal() const
    {
        Horizontal hor;
        hor.observer = observer;
        hor.julianDate = julianDate;
        double sum_sq_xy = position[0]*position[0]+position[1]*position[1];
        double sum_sq_xyz = sum_sq_xy+position[2]*position[2];
        double mag_xy = sqrt(sum_sq_xy);
        double mag_xyz = sqrt(sum_sq_xyz);
        double north = -position[0];
        const double& east = position[1];
        const double& zenith = position[2];
        hor.azimuth = atan2(east/mag_xy,north/mag_xy);
        hor.elevation = asin(zenith/mag_xyz);
        hor.range = mag_xyz;
        return hor;
    }

    Horizontal ENU::toHorizontal() const
    {
        Horizontal hor;
        hor.observer = observer;
        hor.julianDate = julianDate;
        double sum_sq_xy = position[0]*position[0]+position[1]*position[1];
        double sum_sq_xyz = sum_sq_xy+position[2]*position[2];
        double mag_xy = sqrt(sum_sq_xy);
        double mag_xyz = sqrt(sum_sq_xyz);
        const double& north = position[1];
        const double& east = position[0];
        const double& zenith = position[2];
        hor.azimuth = atan2(east/mag_xy,north/mag_xy);
        hor.elevation = asin(zenith/mag_xyz);
        hor.range = mag_xyz;
        return hor;
    }


} //namespace sat
} // namespace gnssShadowing
