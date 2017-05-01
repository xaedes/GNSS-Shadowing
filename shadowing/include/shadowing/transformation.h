#pragma once

#include <opencv2/opencv.hpp>

namespace gnssShadowing {
namespace shadowing {

    struct Transformation
    {
        Transformation(const cv::Mat& sat_sez, const cv::Mat& sez_sat) : 
            m_sat_sez(sat_sez), m_sez_sat(sez_sat){}
        Transformation(const Transformation& copyFrom) :
            m_sat_sez(copyFrom.m_sat_sez), m_sez_sat(copyFrom.m_sez_sat){}

        cv::Mat& getSATSEZ() {return m_sat_sez;}
        cv::Mat& getSEZSAT() {return m_sez_sat;}
        cv::Mat m_sat_sez;
        cv::Mat m_sez_sat;
    };

} // namespace shadowing
} // namespace gnssShadowing
