#pragma once

#include <boost/python.hpp>
#include <string>

#include "shadowing/shadowing.h"
#include "shadowing/transformation.h"
#include "shadowing/dataStructures.h"
#include "obj/objFile.h"

using namespace boost::python;

namespace gnssShadowing {
namespace pythonBindings {


    void bindShadowing()
    {
        class_<shadowing::Transformation>("Transformation",init<const cv::Mat&,const cv::Mat&>(
            (arg("SATSEZ")
            ,arg("SEZSAT")
            )))
            .def("getSATSEZ",&shadowing::Transformation::getSATSEZ, return_value_policy<return_by_value>())
            .def("getSEZSAT",&shadowing::Transformation::getSEZSAT, return_value_policy<return_by_value>())
        ;

        class_<shadowing::ContourEdges>("ContourEdges");
        class_<shadowing::ContourFrontFaces>("ContourFrontFaces");
        class_<shadowing::ShadowVolume>("ShadowVolume");
        class_<shadowing::Contour>("Contour",init<shadowing::ContourEdges&,shadowing::ContourFrontFaces&>(
                                            (arg("ContourEdges")
                                            ,arg("ContourFrontFaces")
                                            )))
            .def_readwrite("edges",&shadowing::Contour::first)
            .def_readwrite("frontFaces",&shadowing::Contour::second)
        ;

        shadowing::Contour (shadowing::Shadowing::*computeContourFromTransformation)(shadowing::Transformation) = &shadowing::Shadowing::computeContour;
        shadowing::Contour (shadowing::Shadowing::*computeContourFromHorizontal)(sat::Horizontal) = &shadowing::Shadowing::computeContour;
        class_<shadowing::Shadowing>("Shadowing", init<obj::Object&>((arg("ObjectFromObjFile"))))
            .def("computeTransformation", &shadowing::Shadowing::computeTransformation, (arg("satPosition")))
            .def("computeContourFromTransformation", computeContourFromTransformation, (arg("transformation")))
            .def("computeContourFromHorizontal", computeContourFromHorizontal, (arg("satPosition")))
            .def("computeShadowVolume", &shadowing::Shadowing::computeShadowVolume, (arg("contour"),arg("transformation"),arg("planeLevel"),arg("eps")))
        ;

    }

} // namespace pythonBindings
} // namespace gnssShadowing
