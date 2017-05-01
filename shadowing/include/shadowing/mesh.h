#pragma once

#include <iostream>
#include <functional>

#include "obj/objFile.h"

#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>

#include <opencv2/opencv.hpp>

#include "sat/coordinateSystems.h"
#include "common/math.h"
#include "common/rotationMatrices.h"

#include "obj/objFile.h"

#include "shadowing/dataStructures.h"

namespace gnssShadowing {
namespace shadowing {

    class PreprocessedMesh
    {
    public:
        PreprocessedMesh(obj::Object& obj);

        virtual ~PreprocessedMesh(){}
    protected:
        void computeUniqueVertices(double threshold  = 1e-3);
/*
*/
        void processMesh();


    public:
        obj::Object m_obj;
        std::reference_wrapper<obj::ObjFile> m_objFile;
        // contains edges that could possibly be a contour
        std::vector<EdgeIndex> m_edgesPossibleContour;
        // stores normal vectors for each valid face
        std::vector<cv::Point3f> m_faceNormals;
        // useful to navigate on faces
        std::unordered_map<ValidFaceIndex, std::set<ValidFaceIndex> > m_faceNeighborsSharingEdge;
        // useful to navigate on edges
        std::unordered_map<EdgeIndex, std::set<EdgeIndex> > m_edgeNeighborsSharingVertex;
        // stores unique indices
        std::vector<VertexIndex> m_uniqueIndices;
        // relates original vertex indices to new vertex indices of unique vertices
        std::unordered_map<VertexIndex,UniqueVertexIndex> m_indexMapping;
        // stores vertex indices (that point into m_objFile.m_vertices) related to m_obj
        std::vector<VertexIndex> m_indicesWithDuplicates;
        // stores unique not malformed faces
        std::vector<std::vector<VertexIndex> > m_validFaces;
        // stores vertices that are reachable over the edge of a face from a vertex in canonical form 
        // i.e. lower index maps to its higher values neighbor indices
        std::unordered_map<VertexIndex, std::set<VertexIndex> > m_vertexNeighbors;
        // stores all faces a vertex is part of
        std::unordered_map<VertexIndex, std::set<ValidFaceIndex> > m_vertexFaces;
        // contains edges
        std::vector<Edge<VertexIndex> > m_edges;
        // stores edges adjacent to vertices
        std::unordered_map<VertexIndex, std::set<EdgeIndex> > m_vertexEdges;
        // stores faces adjacent to edges
        std::unordered_map<EdgeIndex, std::set<ValidFaceIndex> > m_edgeFaces;
        // stores edges that are part of a face
        std::unordered_map<ValidFaceIndex, std::set<EdgeIndex> > m_faceEdges;

/*
*/
    };
    // std::vector<Edge<cv::Point3f> > computeContour(obj::Object& building, sat::Horizontal sat, double planeLevel=0);


} // namespace shadowing
} // namespace gnssShadowing

