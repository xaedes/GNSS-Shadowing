#pragma once

#include <boost/python.hpp>
#include <string>

#include "sat/sat.h"
#include "sat/satStore.h"
#include "sat/availableSats.h"
#include "sat/coordinateSystems.h"
#include "sat/dop.h"
#include "sat/wdop.h"

using namespace boost::python;

namespace gnssShadowing {
namespace pythonBindings {


    void bindSat()
    {

        class_<sat::SatStore>("SatStore", init<const std::string&>((arg("filename"))))
            .def("getSat", &sat::SatStore::getSat, return_value_policy<reference_existing_object>(), (arg("idx")))
            .def("getNumSats", &sat::SatStore::getNumSats)
        ;

        class_<sat::Sat>("Sat", init<std::string,std::string,std::string>(
                (arg("name")
                ,arg("tleLine1")
                ,arg("tleLine2")
                )))
            .def("getName", &sat::Sat::getName)
            .def("getSatNumber", &sat::Sat::getSatNumber)
            .def("findTEME", &sat::Sat::findTEME, (arg("unixTimeInSeconds")))
            .def("findECEF", &sat::Sat::findECEF, (arg("unixTimeInSeconds")))
        ;

        class_<sat::AvailableSats>("AvailableSats", init<sat::SatStore&, sat::Geodetic, double>(
                    (arg("satStore")
                    ,arg("observer")
                    ,arg("minimumSatelliteElevation")
                    )))
            .def(init<const sat::AvailableSats&>((arg("copyFrom"))))
            .def("setSatStore", &sat::AvailableSats::setSatStore)
            .def("setObserver", &sat::AvailableSats::setObserver)
            .def("update", &sat::AvailableSats::update, arg("unixTimeSeconds"))
            .def("getNumSats", &sat::AvailableSats::getNumSats)
            .def("getMaxNumSats", &sat::AvailableSats::getMaxNumSats)
            .def("getSatIndex", &sat::AvailableSats::getSatIndex, (arg("idx")))
            .def("getSat", &sat::AvailableSats::getSat, return_value_policy<reference_existing_object>(), (arg("idx")))
            .def("getSatPositionHorizontal", &sat::AvailableSats::getSatPositionHorizontal, return_value_policy<reference_existing_object>(), (arg("idx")))
            .def("getSatPositionENU", &sat::AvailableSats::getSatPositionENU, return_value_policy<reference_existing_object>(), (arg("idx")))
            .def_readwrite("m_minimumSatelliteElevation", &sat::AvailableSats::m_minimumSatelliteElevation)
        ;

        sat::ENU& (std::vector<sat::ENU>::*vectorENUNodeAt)(size_t) = &std::vector<sat::ENU>::at;
        void (std::vector<sat::ENU>::*vectorENUNodePushback)(const sat::ENU&) = &std::vector<sat::ENU>::push_back;
        class_<std::vector<sat::ENU>>("ENUList", init<>())
            .def("at",vectorENUNodeAt,return_value_policy<return_by_value>())
            .def("size",&std::vector<sat::ENU>::size)
            .def("clear",&std::vector<sat::ENU>::clear)
            .def("append",vectorENUNodePushback)
        ;

        //http://stackoverflow.com/a/37543673/798588
        class_<sat::SatPositionProvider, boost::noncopyable>("SatPositionProvider", no_init)
            .def("getNumSats", &sat::SatPositionProvider::getNumSats)
            .def("getSatPositionENU", &sat::SatPositionProvider::getSatPositionENU, return_value_policy<reference_existing_object>(), (arg("idx")))
        ;
        class_<sat::SatPositionProviderVectorENU, bases<sat::SatPositionProvider> >("SatPositionProviderVectorENU",init<std::vector<sat::ENU>&>((arg("ENUList"))));
        class_<sat::SatPositionProviderAvailable, bases<sat::SatPositionProvider> >("SatPositionProviderAvailable",init<sat::AvailableSats&>((arg("AvailableSats"))));

        typedef std::vector<int> SatelitteIndexList;

        class_<SatelitteIndexList>("SatelitteIndexList")
            .def(vector_indexing_suite<SatelitteIndexList>())
            .def("clear",&SatelitteIndexList::clear)
            .def("size",&SatelitteIndexList::size)
        ;

        class_<sat::DOP>("DOP", init<>())
            .def(init<sat::ENU,sat::SatPositionProvider*,SatelitteIndexList&>(
                    (arg("observer")
                    ,arg("satPositionProvider")
                    ,arg("visibleSatelliteIndices")
                    )))
            .def("setObserver", &sat::DOP::setObserver, (arg("observer")))
            .def("setSats", &sat::DOP::setSats, (arg("satPositionProvider")))
            .def("update", &sat::DOP::update, (arg("visibleSatelliteIndices")))
            .def("compute", &sat::DOP::compute)
            .def("getEast", &sat::DOP::getEast)
            .def("getNorth", &sat::DOP::getNorth)
            .def("getVertical", &sat::DOP::getVertical)
            .def("getTime", &sat::DOP::getTime)
            .def("getHorizontal", &sat::DOP::getHorizontal)
            .def("getPosition", &sat::DOP::getPosition)
            .def("getGeometric", &sat::DOP::getGeometric)
        ;

        typedef std::vector<float> WDOPWeightList;
        class_<WDOPWeightList>("WDOPWeightList")
            .def(vector_indexing_suite<WDOPWeightList>())
            .def("clear",&WDOPWeightList::clear)
            .def("size",&WDOPWeightList::size)
        ;

        class_<sat::WDOP>("WDOP", init<>())
            .def(init<sat::ENU,sat::SatPositionProvider*,SatelitteIndexList&,WDOPWeightList&>(
                    (arg("observer")
                    ,arg("satPositionProvider")
                    ,arg("visibleSatelliteIndices")
                    ,arg("weights")
                    )))
            .def("setObserver", &sat::WDOP::setObserver, (arg("observer")))
            .def("setSats", &sat::WDOP::setSats, (arg("satPositionProvider")))
            .def("update", &sat::WDOP::update, (arg("visibleSatelliteIndices"), arg("weights")))
            .def("compute", &sat::WDOP::compute)
            .def("getEast", &sat::WDOP::getEast)
            .def("getNorth", &sat::WDOP::getNorth)
            .def("getVertical", &sat::WDOP::getVertical)
            .def("getTime", &sat::WDOP::getTime)
            .def("getHorizontal", &sat::WDOP::getHorizontal)
            .def("getPosition", &sat::WDOP::getPosition)
            .def("getGeometric", &sat::WDOP::getGeometric)
        ;
    }

} // namespace pythonBindings
} // namespace gnssShadowing
