#pragma once

#include <iostream>
#include <memory>

#include "obj/objFile.h"

#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>

#include <opencv2/opencv.hpp>

#include "common/measureTime.h"
#include "sat/coordinateSystems.h"
#include "common/math.h"
#include "common/rotationMatrices.h"

#include "obj/objFile.h"

#include "shadowing/mesh.h"
#include "shadowing/transformation.h"
#include "shadowing/dataStructures.h"


namespace gnssShadowing {
namespace shadowing {

    class Shadowing
    {
    public:
        Shadowing(obj::Object& obj);

        Transformation computeTransformation(sat::Horizontal sat);
        Contour computeContour(Transformation transformation);
        Contour computeContour(sat::Horizontal sat);
        ShadowVolume computeShadowVolume(Contour contour, Transformation transformation, double planeLevel=0, double eps = 1e-3);
        std::shared_ptr<common::Benchmark> m_benchmark;

    protected:
        PreprocessedMesh m_mesh;
    };

} // namespace shadowing
} // namespace gnssShadowing

