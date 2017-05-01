#pragma once

#include <boost/python.hpp>
#include <string>

#include "obj/objFile.h"
#include "obj/mtllib.h"

using namespace boost::python;

namespace gnssShadowing {
namespace pythonBindings {


    void bindObj()
    {

        class_<cv::Point3f>("cvPoint3f", init<>())
        	.def(init<float,float,float>())
            .def_readwrite("x", &cv::Point3f::x)
            .def_readwrite("y", &cv::Point3f::y)
            .def_readwrite("z", &cv::Point3f::z)
        ;

        class_<obj::Material>("Material", init<>())
            .def_readwrite("name", &obj::Material::name)
            .def_readwrite("ambient", &obj::Material::ambient)
            .def_readwrite("diffuse", &obj::Material::diffuse)
        ;

        class_<obj::Mtllib>("Mtllib", init<>())
            .def("purge", &obj::Mtllib::purge)
            .def("read", &obj::Mtllib::read, (arg("filename"))) 
            .def("getMaterial", &obj::Mtllib::getMaterial, return_value_policy<reference_existing_object>(), (arg("materialName")))
        ;

        class_<obj::ObjFile>("ObjFile", init<std::string>())
            .def("purge", &obj::ObjFile::purge)
            .def("read", &obj::ObjFile::read, (arg("filename")))
            .def_readwrite("m_mtllib", &obj::ObjFile::m_mtllib)
        ;

        class_<obj::Face>("ObjFace", init<>());
        class_<obj::Object>("ObjObject", init<>());
        class_<obj::Group>("ObjGroup", init<>());

    }

} // namespace pythonBindings
} // namespace gnssShadowing
