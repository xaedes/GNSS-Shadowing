#pragma once

#include "pythonBindings/arrayBindings.h"
#include "pythonBindings/cvMatBindings.h"

#include <boost/python.hpp>
#include <string>

#include "world/world.h"

using namespace boost::python;

namespace gnssShadowing {
namespace pythonBindings {


    void bindWorld()
    {

        class_<world::World>("World",init<const std::string&,double,const std::string&,const std::string&>(
                (arg("satStoreFilename")
                ,arg("startTimeUnixTimeSeconds")
                ,arg("objFileFilename")
                ,arg("buildingsGroupName")
                )))
            .def(init<const std::string&,const std::string&,const std::string&>(
            	(arg("satStoreFilename")
                ,arg("objFileFilename")
                ,arg("buildingsGroupName")
                )))
            .def_readwrite("m_satStore", &world::World::m_satStore)
            .def_readwrite("m_startTimeUnixTimeSeconds", &world::World::m_startTimeUnixTimeSeconds)
            .def_readwrite("m_origin", &world::World::m_origin)
            .def_readwrite("m_originECEF", &world::World::m_originECEF)
            .def_readwrite("m_originENU", &world::World::m_originENU)
            .def_readwrite("m_buildings", &world::World::m_buildings)
            .def_readwrite("m_shadowing", &world::World::m_shadowing)
        ;
    }

} // namespace pythonBindings
} // namespace gnssShadowing
