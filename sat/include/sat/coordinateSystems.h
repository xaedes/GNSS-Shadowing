#pragma once

#include <opencv2/opencv.hpp>
#include <cstring>

namespace gnssShadowing {
namespace sat {

    double julianDateFromUnix(double unixSecs);

    struct CarthesianCoordinate
    {
        CarthesianCoordinate(const CarthesianCoordinate& copyFrom);
        CarthesianCoordinate(){}

        double position[3]; // km
        double velocity[3]; // km/s
        double julianDate;  // days
        CarthesianCoordinate& operator+=(const CarthesianCoordinate& rhs);
        CarthesianCoordinate& operator-=(const CarthesianCoordinate& rhs);
    };

    struct ECEF; /*forward declaration*/
    struct Geodetic
    {
        Geodetic(const Geodetic& copyFrom);
        Geodetic(){}
        Geodetic(double latitude, double longitude, double altitude=0, double julianDate=0)
            : latitude(latitude)
            , longitude(longitude)
            , altitude(altitude)
            , julianDate(julianDate)
        {}
        double latitude;    // -pi/2 to pi/2 in radians
        double longitude;   // -2pi to 2pi in radians
        double altitude;    // km
        double julianDate;  // days
        ECEF toECEF() const;
    };
    


    struct SEZ; /*forward declaration*/
    struct Horizontal; /*forward declaration*/
    /**
     * @brief      East North Up - Local Coordinate System
     *
     *             Units in km and km/s
     *
     *             "Topocentric-Horizon Coordinate System"
     *
     *             http://www.navipedia.net/index.php/Transformations_between_ECEF_and_ENU_coordinates
     *             https://celestrak.com/columns/v02n02/
     */
    struct ENU : public CarthesianCoordinate
    {
        Geodetic observer;
        ENU(){}
        ENU(const ENU& copyFrom);

        ECEF toECEF() const;
        SEZ toSEZ() const;
        Horizontal toHorizontal() const;

        // @brief      3x3 Transformation matrix to transform from ECEF to ENU.
        // @param[in]  observer  The observer point at which to instantiate the local east-north-up coordinate system
        static cv::Mat ENU_ECEF(const Geodetic& observer);
        // @brief      3x3 Transformation matrix to transform from ENU to ECEF.
        // @param[in]  observer  The observer point at which to instantiate the local east-north-up coordinate system
        static cv::Mat ECEF_ENU(const Geodetic& observer);

        ENU& operator+=(const ENU& rhs);
        ENU& operator-=(const ENU& rhs);
    };

    /**
     * @brief      South East Zenith - Local Coordinate System
     *
     *             Units in km and km/s
     *
     *             "Topocentric-Horizon Coordinate System"
     *
     *             http://www.navipedia.net/index.php/Transformations_between_ECEF_and_ENU_coordinates
     *             https://celestrak.com/columns/v02n02/
     */
    struct SEZ : public CarthesianCoordinate
    {
        Geodetic observer;
        SEZ(){}
        SEZ(const SEZ& copyFrom);

        ECEF toECEF() const;
        ENU toENU() const;
        Horizontal toHorizontal() const;

        // @brief      3x3 Transformation matrix to transform from ECEF to SEZ.
        // @param[in]  observer  The observer point at which to instantiate the local east-north-up coordinate system
        static cv::Mat SEZ_ECEF(const Geodetic& observer);
        // @brief      3x3 Transformation matrix to transform from SEZ to ECEF.
        // @param[in]  observer  The observer point at which to instantiate the local east-north-up coordinate system
        static cv::Mat ECEF_SEZ(const Geodetic& observer);

        SEZ& operator+=(const SEZ& rhs);
        SEZ& operator-=(const SEZ& rhs);
    };

    /**
     * The horizontal coordinate system is a celestial coordinate system that
     * uses the observer's local horizon as the fundamental plane. It is
     * expressed in terms of altitude (or elevation) angle and azimuth.
     *
     * topocentric horizon coordinate system
     */
    struct Horizontal
    {
        Horizontal(const Horizontal& copyFrom);
        Horizontal(){}
        Geodetic observer;
        double azimuth;     // [-2pi to 2pi in radians] angle between north and this, positive in east direction
        double elevation;   // [0 to pi/2 in radians]   angle between ground plane and this, positive upwards
        double range;       // in km
        double julianDate;  // days
        ENU toENU() const;
        SEZ toSEZ() const;
    };

    /**
     * @brief      Earth Centered Earth Fixed
     */
    struct ECEF : public CarthesianCoordinate
    {
        ECEF(){}
        ECEF(const ECEF& copyFrom);
        Geodetic toGeodetic();
        ENU toENU(const Geodetic& observer) const;
        SEZ toSEZ(const Geodetic& observer) const;

        ECEF& operator+=(const ECEF& rhs);
        ECEF& operator-=(const ECEF& rhs);
    };

    /**
     * @brief      True Equator, Mean Qquinox
     */
    struct TEME : public CarthesianCoordinate
    {
        TEME(){}
        TEME(const TEME& copyFrom);
        ECEF toECEF() const;
        Horizontal toHorizontal(const Geodetic& observer) const;

        TEME& operator+=(const TEME& rhs);
        TEME& operator-=(const TEME& rhs);
    };

    inline void add_double3(const double a[3],const double b[3],double result[3])
    {
        result[0] = a[0]+b[0];
        result[1] = a[1]+b[1];
        result[2] = a[2]+b[2];
    }

    inline void sub_double3(const double a[3],const double b[3],double result[3])
    {
        result[0] = a[0]-b[0];
        result[1] = a[1]-b[1];
        result[2] = a[2]-b[2];
    }

    inline CarthesianCoordinate operator+(const CarthesianCoordinate& a, const CarthesianCoordinate& b)
    {
        CarthesianCoordinate result(a);
        add_double3(a.position,b.position,result.position);
        add_double3(a.velocity,b.velocity,result.velocity);
        return result;
    }

    inline CarthesianCoordinate operator-(const CarthesianCoordinate& a, const CarthesianCoordinate& b)
    {
        CarthesianCoordinate result(a);
        sub_double3(a.position,b.position,result.position);
        sub_double3(a.velocity,b.velocity,result.velocity);
        return result;
    }

    inline ECEF operator+(const ECEF& a, const ECEF& b)
    {
        ECEF result(a);
        add_double3(a.position,b.position,result.position);
        add_double3(a.velocity,b.velocity,result.velocity);
        return result;
    }

    inline ECEF operator-(const ECEF& a, const ECEF& b)
    {
        ECEF result(a);
        sub_double3(a.position,b.position,result.position);
        sub_double3(a.velocity,b.velocity,result.velocity);
        return result;
    }

    inline TEME operator+(const TEME& a, const TEME& b)
    {
        TEME result(a);
        add_double3(a.position,b.position,result.position);
        add_double3(a.velocity,b.velocity,result.velocity);
        return result;
    }

    inline TEME operator-(const TEME& a, const TEME& b)
    {
        TEME result(a);
        sub_double3(a.position,b.position,result.position);
        sub_double3(a.velocity,b.velocity,result.velocity);
        return result;
    }

    inline ENU operator+(const ENU& a, const ENU& b)
    {
        ENU result(a);
        add_double3(a.position,b.position,result.position);
        add_double3(a.velocity,b.velocity,result.velocity);
        return result;
    }

    inline ENU operator-(const ENU& a, const ENU& b)
    {
        ENU result(a);
        sub_double3(a.position,b.position,result.position);
        sub_double3(a.velocity,b.velocity,result.velocity);
        return result;
    }

    inline SEZ operator+(const SEZ& a, const SEZ& b)
    {
        SEZ result(a);
        add_double3(a.position,b.position,result.position);
        add_double3(a.velocity,b.velocity,result.velocity);
        return result;
    }

    inline SEZ operator-(const SEZ& a, const SEZ& b)
    {
        SEZ result(a);
        sub_double3(a.position,b.position,result.position);
        sub_double3(a.velocity,b.velocity,result.velocity);
        return result;
    }

    //template <typename T>
    //inline T operator+(const T& a, const T& b)
    //{
    //    CarthesianCoordinate result((CarthesianCoordinate)a-(CarthesianCoordinate)b);
    //    return (T)(result);
    //}

    //template <typename T>
    //inline T operator-(const T& a, const T& b)
    //{
    //    CarthesianCoordinate result((CarthesianCoordinate)a-(CarthesianCoordinate)b);
    //    return (T)(result);
    //}

    inline CarthesianCoordinate& CarthesianCoordinate::operator+=(const CarthesianCoordinate& rhs)
    {
        add_double3(this->position,rhs.position,this->position);
        add_double3(this->velocity,rhs.velocity,this->velocity);
        return *this;
    }

    inline CarthesianCoordinate& CarthesianCoordinate::operator-=(const CarthesianCoordinate& rhs)
    {
        sub_double3(this->position,rhs.position,this->position);
        sub_double3(this->velocity,rhs.velocity,this->velocity);
        return *this;
    }

    inline ECEF& ECEF::operator+=(const ECEF& rhs)
    {
        add_double3(this->position,rhs.position,this->position);
        add_double3(this->velocity,rhs.velocity,this->velocity);
        return *this;
    }

    inline ECEF& ECEF::operator-=(const ECEF& rhs)
    {
        sub_double3(this->position,rhs.position,this->position);
        sub_double3(this->velocity,rhs.velocity,this->velocity);
        return *this;
    }

    inline ENU& ENU::operator+=(const ENU& rhs)
    {
        add_double3(this->position,rhs.position,this->position);
        add_double3(this->velocity,rhs.velocity,this->velocity);
        return *this;
    }

    inline ENU& ENU::operator-=(const ENU& rhs)
    {
        sub_double3(this->position,rhs.position,this->position);
        sub_double3(this->velocity,rhs.velocity,this->velocity);
        return *this;
    }

    inline SEZ& SEZ::operator+=(const SEZ& rhs)
    {
        add_double3(this->position,rhs.position,this->position);
        add_double3(this->velocity,rhs.velocity,this->velocity);
        return *this;
    }

    inline SEZ& SEZ::operator-=(const SEZ& rhs)
    {
        sub_double3(this->position,rhs.position,this->position);
        sub_double3(this->velocity,rhs.velocity,this->velocity);
        return *this;
    }

    inline TEME& TEME::operator+=(const TEME& rhs)
    {
        add_double3(this->position,rhs.position,this->position);
        add_double3(this->velocity,rhs.velocity,this->velocity);
        return *this;
    }

    inline TEME& TEME::operator-=(const TEME& rhs)
    {
        sub_double3(this->position,rhs.position,this->position);
        sub_double3(this->velocity,rhs.velocity,this->velocity);
        return *this;
    }

} //namespace sat
} // namespace gnssShadowing
