#pragma once

#include <opencv2/opencv.hpp>
#include "common/math.h"

namespace gnssShadowing {
namespace common {

    inline cv::Mat rotX(float angle)
    {
        float cs=cos(angle);
        float sn=sin(angle);
        return (cv::Mat_<float>(3,3) << 1,  0,  0,
                                        0, cs,-sn,
                                        0, sn, cs);
    }
    inline cv::Mat rotY(float angle)
    {
        float cs=cos(angle);
        float sn=sin(angle);
        return (cv::Mat_<float>(3,3) << cs, 0, -sn,
                                         0, 1,   0,
                                        sn, 0,  cs);
    }
    inline cv::Mat rotZ(float angle)
    {
        float cs=cos(angle);
        float sn=sin(angle);
        return (cv::Mat_<float>(3,3) << cs, -sn, 0,
                                        sn,  cs, 0,
                                         0,   0, 1);
    }

} //namespace common
} //namespace gnssShadowing
