#pragma once

#include <vector>
#include <utility>
#include <opencv2/opencv.hpp>

namespace gnssShadowing {
namespace shadowing {

    typedef int VertexIndex;       // index into list of vertices
    typedef int UniqueVertexIndex; // index into list of unique vertices
    typedef int VertexIndexIndex;  // index into list of vertex indices
    typedef int FaceIndex;         // index into list of faces
    typedef int ValidFaceIndex;    // index into list of valid faces
    typedef int EdgeIndex;         // index into list of edges
    template <typename T> using Edge = std::pair<T,T>;  // edge

    typedef std::vector<EdgeIndex> ContourEdges;          // contour edges  
    typedef int EdgeContourIndex;                         // index into list of contour edges
    typedef std::vector<FaceIndex> ContourFrontFaces;     // stores front facing faces for each contour edge
    typedef std::pair<ContourEdges,ContourFrontFaces> Contour;
    typedef std::vector<Edge<cv::Point3f> > ShadowEdges;   // projected contour edges of shadow
    typedef std::vector<cv::Point3f> ShadowFace;           // stores a quad of vertices describing one face of a shadow volume
    typedef std::vector<ShadowFace> ShadowVolume;          // stores a list of quads describing the shadow volume
                                                           // 

} // namespace shadowing
} // namespace gnssShadowing
