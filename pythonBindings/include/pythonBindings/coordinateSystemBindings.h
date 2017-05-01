#pragma once

#include "pythonBindings/arrayBindings.h"
#include "pythonBindings/cvMatBindings.h"

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "common/timing.h"
#include "sat/coordinateSystems.h"

using namespace boost::python;

namespace gnssShadowing {
namespace pythonBindings {

    void bindCoordinateSystems()
    {
        def("julianDateFromUnix",&sat::julianDateFromUnix, (arg("unixTimeInSeconds")));
        def("unixTimeInSeconds",&common::now_seconds);
        def("mkSeconds",&common::mk_seconds, (arg("year"),arg("month"),arg("day"),arg("hour"),arg("minutes"),arg("seconds")));

        class_<sat::CarthesianCoordinate>("CarthesianCoordinate", init<>())
            .def(init<const sat::CarthesianCoordinate&>((arg("copyFrom"))))
            .def_readwrite("julianDate", &sat::CarthesianCoordinate::julianDate)
            .add_property("position", make_array(&sat::CarthesianCoordinate::position))
            .add_property("velocity", make_array(&sat::CarthesianCoordinate::velocity))
            .def(self + self)
            .def(self - self)
            .def(self += self)
            .def(self -= self)
        ;

        class_<sat::TEME, bases<sat::CarthesianCoordinate>>("TEME", init<>())
            .def(init<const sat::TEME&>((arg("copyFrom"))))
            .def("toECEF", &sat::TEME::toECEF)
            .def("toHorizontal", &sat::TEME::toHorizontal)
        ;

        class_<sat::ECEF, bases<sat::CarthesianCoordinate>>("ECEF", init<>())
            .def(init<const sat::ECEF&>((arg("copyFrom"))))
            .def("toSEZ", &sat::ECEF::toSEZ, (arg("observer")))
            .def("toENU", &sat::ECEF::toENU, (arg("observer")))
            .def("toGeodetic", &sat::ECEF::toGeodetic)
        ;

        class_<sat::Geodetic>("Geodetic", init<>())
            .def(init<const sat::Geodetic&>((arg("copyFrom"))))
            .def(init<double,double,double,double>((arg("latitude"),arg("longitude"),arg("altitude"),arg("julianDate"))))
            .def_readwrite("julianDate", &sat::Geodetic::julianDate)
            .def_readwrite("latitude", &sat::Geodetic::latitude)
            .def_readwrite("longitude", &sat::Geodetic::longitude)
            .def_readwrite("altitude", &sat::Geodetic::altitude)
            .def("toECEF", &sat::Geodetic::toECEF)
        ;

        class_<sat::ENU, bases<sat::CarthesianCoordinate>>("ENU", init<>())
            .def(init<const sat::ENU&>((arg("copyFrom"))))
            .def_readwrite("observer", &sat::ENU::observer)
            .def("toECEF", &sat::ENU::toECEF)
            .def("toSEZ", &sat::ENU::toSEZ)
            .def("toHorizontal", &sat::ENU::toHorizontal)
            .def("ENU_ECEF", &sat::ENU::ENU_ECEF, (arg("observer")))
            .staticmethod("ENU_ECEF")
            .def("ECEF_ENU", &sat::ENU::ECEF_ENU, (arg("observer")))
            .staticmethod("ECEF_ENU")
        ;

        class_<sat::SEZ, bases<sat::CarthesianCoordinate>>("SEZ", init<>())
            .def(init<const sat::SEZ&>((arg("copyFrom"))))
            .def_readwrite("observer", &sat::SEZ::observer)
            .def("toECEF", &sat::SEZ::toECEF)
            .def("toENU", &sat::SEZ::toENU)
            .def("toHorizontal", &sat::SEZ::toHorizontal)
            .def("SEZ_ECEF", &sat::SEZ::SEZ_ECEF, (arg("observer")))
            .staticmethod("SEZ_ECEF")
            .def("ECEF_SEZ", &sat::SEZ::ECEF_SEZ, (arg("observer")))
            .staticmethod("ECEF_SEZ")
        ;

        class_<sat::Horizontal>("Horizontal", init<>())
            .def(init<const sat::Horizontal&>((arg("copyFrom"))))
            .def_readwrite("observer", &sat::Horizontal::observer)
            .def_readwrite("julianDate", &sat::Horizontal::julianDate)
            .def_readwrite("azimuth", &sat::Horizontal::azimuth)
            .def_readwrite("elevation", &sat::Horizontal::elevation)
            .def_readwrite("range", &sat::Horizontal::range)
            .def("toENU", &sat::Horizontal::toENU)
            .def("toSEZ", &sat::Horizontal::toSEZ)
        ;
    }

} // namespace pythonBindings
} // namespace gnssShadowing
