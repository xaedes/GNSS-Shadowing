#pragma once

#include "pythonBindings/arrayBindings.h"
// #include "pythonBindings/cvMatBindings.h"

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <string>

#include "mapping/mapProperties.h"
#include "mapping/mapper.h"
#include "mapping/mapperLazyTimesteps.h"
#include "mapping/visibilityMap.h"
#include "mapping/dopMap.h"
#include "mapping/mapCoordinateConverter.h"
#include "sat/availableSats.h"
#include "world/world.h"

using namespace boost::python;

namespace gnssShadowing {
namespace pythonBindings {


    void bindMapping()
    {
        typedef std::vector<double> PlaneLevelList;

        class_<PlaneLevelList>("PlaneLevelList")
            .def(vector_indexing_suite<PlaneLevelList>())
            .def("clear",&PlaneLevelList::clear)
            .def("size",&PlaneLevelList::size)
        ;

        class_<mapping::MapProperties>("MapProperties",init<>())
            .def(init<const mapping::MapProperties&>((arg("copyFrom"))))
            .def(init<double,double,int,int,double,double,PlaneLevelList>(
                (arg("min_x")
                ,arg("min_y")
                ,arg("num_cols")
                ,arg("num_cols")
                ,arg("x_resolution")
                ,arg("y_resolution")
                ,arg("plane_levels")
                )))
            .def(init<const mapping::MapProperties&,PlaneLevelList>(
                (arg("copyFrom")
                ,arg("plane_levels")
                )))
            .def("SingleLocation", &mapping::MapProperties::SingleLocation, (arg("world"), arg("location"), arg("cellSize"), arg("planeLevel")))
            .def("AroundLocation", &mapping::MapProperties::AroundLocation, (arg("world"), arg("location"), arg("cellSize"), arg("width"), arg("height"), arg("planeLevels")))
            .def("AroundLocation_", &mapping::MapProperties::AroundLocation_, (arg("world"), arg("location"), arg("cellSize"), arg("width"), arg("height"), arg("planeLevel")))
            .staticmethod("SingleLocation")
            .staticmethod("AroundLocation")
            .staticmethod("AroundLocation_")
            .def("X", &mapping::MapProperties::X, (arg("i")))
            .def("Y", &mapping::MapProperties::Y, (arg("j")))
            .def("I", &mapping::MapProperties::I, (arg("x")))
            .def("J", &mapping::MapProperties::J, (arg("y")))
            .def_readwrite("m_min_x", &mapping::MapProperties::m_min_x)
            .def_readwrite("m_min_y", &mapping::MapProperties::m_min_y)
            .def_readwrite("m_num_cols", &mapping::MapProperties::m_num_cols)
            .def_readwrite("m_num_rows", &mapping::MapProperties::m_num_rows)
            .def_readwrite("m_x_resolution", &mapping::MapProperties::m_x_resolution)
            .def_readwrite("m_y_resolution", &mapping::MapProperties::m_y_resolution)
            .def_readwrite("m_plane_levels", &mapping::MapProperties::m_plane_levels)
        ;

        class_<mapping::ShadowMap>("ShadowMap",init<const mapping::MapProperties&>((arg("mapProperties"))))
            .def(init<const mapping::MapProperties&,const shadowing::ShadowVolume&>((arg("mapProperties"),arg("shadowVolume"))))
            .def_readwrite("m_properties",&mapping::ShadowMap::m_properties)
            .def("computeShadowMap", &mapping::ShadowMap::computeShadowMap, (arg("shadowVolume")))
            .def("isShadowed", &mapping::ShadowMap::isShadowed, (arg("x"),arg("y"),arg("z")))
            .def("getMapForLevel", &mapping::ShadowMap::getMapForLevel, return_value_policy<return_by_value>(), (arg("z")))
        ;

        class_<mapping::OccupancyMap>("OccupancyMap",init<const mapping::MapProperties&>((arg("mapProperties"))))
            .def(init<const mapping::MapProperties&,const shadowing::ShadowVolume&>((arg("mapProperties"),arg("shadowVolume"))))
            .def_readwrite("m_properties",&mapping::OccupancyMap::m_properties)
            .def("isOccupied", &mapping::OccupancyMap::isOccupied, (arg("x"),arg("y"),arg("z")))
            .def("getMapForLevel", &mapping::OccupancyMap::getMapForLevel, return_value_policy<return_by_value>(), (arg("z")))
        ;

        class_<mapping::VisibilityMap>("VisibilityMap",init<const mapping::MapProperties&,sat::AvailableSats&>((arg("mapProperties"),arg("availableSats"))))
            .def(init<const mapping::MapProperties&,sat::AvailableSats&,shadowing::Shadowing&>((arg("mapProperties"),arg("availableSats"),arg("shadowing"))))
            .def(init<const mapping::VisibilityMap&>((arg("copyFrom"))))
            .def("populate", &mapping::VisibilityMap::populate, (arg("shadowing")))
            .def("populateWithAll", &mapping::VisibilityMap::populateWithAll)
            .def("applyShadowMap", &mapping::VisibilityMap::applyShadowMap, (arg("satIdx"),arg("shadowMap")))
            .def("clear", &mapping::VisibilityMap::clear)
            .def("getItem", &mapping::VisibilityMap::getItem, return_value_policy<reference_existing_object>())
            .add_property("m_availableSats", make_function(&mapping::VisibilityMap::getAvailableSats, return_value_policy<reference_existing_object>()))
        ;

        class_<mapping::DOPMap>("DOPMap",init<const mapping::MapProperties&,sat::AvailableSats&,sat::ENU>((arg("mapProperties"),arg("availableSats"),arg("observer"))))
            .def(init<const mapping::DOPMap&>((arg("copyFrom"))))
            .def("populate", &mapping::DOPMap::populate, (arg("visiblityMap")))
            .def("forceComputeAll", &mapping::DOPMap::forceComputeAll)
            .def("setupItems", &mapping::DOPMap::setupItems)
            .def("getItem", &mapping::DOPMap::getItem, return_value_policy<reference_existing_object>(), (arg("x"),arg("y"),arg("z")))
            .def("getHDOPMap", &mapping::DOPMap::getHDOPMap, return_value_policy<return_by_value>(), (arg("z")))
        ;

        class_<mapping::Mapper>("Mapper",init<world::World&,mapping::MapProperties,double>((arg("world"),arg("mapProperties"),arg("minimumSatelliteElevation"))))
            .def(init<const mapping::Mapper&>((arg("copyFrom"))))
            .def("updateSats", &mapping::Mapper::updateSats, (args("unixTimeSeconds")))
            .def("computeDOPMap", &mapping::Mapper::computeDOPMap, return_value_policy<reference_existing_object>(), (args("unixTimeSeconds")))
            .add_property("m_world", make_function(&mapping::Mapper::getWorld, return_value_policy<reference_existing_object>()), 
                                    &mapping::Mapper::setWorld)
            .def_readwrite("m_sats", &mapping::Mapper::m_sats)
            .def_readwrite("m_mapProperties", &mapping::Mapper::m_mapProperties)
            .def_readwrite("m_occupancyMap", &mapping::Mapper::m_occupancyMap)
            .def_readwrite("m_visibilityMap", &mapping::Mapper::m_visibilityMap)
            .def_readwrite("m_dopMap", &mapping::Mapper::m_dopMap)
        ;

        class_<mapping::MapperLazyTimesteps>("MapperLazyTimesteps",init<world::World&,mapping::MapProperties,double,double,double>(
                (arg("world")
                ,arg("mapProperties")
                ,arg("startTimeUnixTimeSeconds")
                ,arg("timePerStep")
                ,arg("minimumSatelliteElevation")
                )))
            .def("getTime", &mapping::MapperLazyTimesteps::getTime, (args("timestep")))
            .def("getDOPMap", &mapping::MapperLazyTimesteps::getDOPMap, return_value_policy<reference_existing_object>(), (args("timestep")))
            .def("getOccupancyMap", &mapping::MapperLazyTimesteps::getOccupancyMap, return_value_policy<reference_existing_object>(), (args("timestep")))
            .def("computeDOPMap", &mapping::MapperLazyTimesteps::computeDOPMap, return_value_policy<reference_existing_object>(), (args("timestep")))
            .def("clear", &mapping::MapperLazyTimesteps::clear)
            .def_readwrite("m_world", &mapping::MapperLazyTimesteps::m_world)
            .def_readwrite("m_mapProperties", &mapping::MapperLazyTimesteps::m_mapProperties)
            .def_readwrite("m_minimumSatelliteElevation", &mapping::MapperLazyTimesteps::m_minimumSatelliteElevation)
            .def_readwrite("m_timePerStep", &mapping::MapperLazyTimesteps::m_timePerStep)
            .def_readwrite("m_startTimeUnixTimeSeconds", &mapping::MapperLazyTimesteps::m_startTimeUnixTimeSeconds)
        ;

        class_<cv::Point2f>("MapNodeF", init<>())
            .def(init<float,float>())
            .def_readwrite("x", &cv::Point2f::x)
            .def_readwrite("y", &cv::Point2f::y)
        ;
        class_<mapping::MapCoordinateConverter>("MapCoordinateConverter", init<mapping::MapProperties,sat::Geodetic>(
                    (arg("mapProperties")
                    ,arg("origin")
                    )))
            .def("gridCoordinateToGeodetic", &mapping::MapCoordinateConverter::gridCoordinateToGeodetic, (arg("gridCoordinates")))
            .def("geodeticToGridCoordinate", &mapping::MapCoordinateConverter::geodeticToGridCoordinate, (arg("geodeticCoordinates")))
            .def("geodeticToGridCoordinateF", &mapping::MapCoordinateConverter::geodeticToGridCoordinateF, (arg("geodeticCoordinates")))
        ;
    }

} // namespace pythonBindings
} // namespace gnssShadowing
