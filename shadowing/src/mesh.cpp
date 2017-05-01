
#include <iostream>

#include "obj/objFile.h"

#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>

#include "sat/coordinateSystems.h"
#include "common/math.h"
#include "common/rotationMatrices.h"
#include "common/setUtils.h"

#include "obj/objFile.h"
#include "shadowing/mesh.h"

using namespace std;
using namespace gnssShadowing::common;

namespace gnssShadowing {
namespace shadowing {

    PreprocessedMesh::PreprocessedMesh(obj::Object& obj)
        : m_obj(obj), m_objFile(*m_obj.objFile)
    {
        // cout << m_objFile.get().name << endl;
        computeUniqueVertices();
        processMesh();
    }
//*
    // @sideffects
    // - m_indicesWithDuplicates: remove duplicate indices (indices of vertices
    //   that are VERY close to each other in space)
    // - m_uniqueIndices: stores unique indices
    // - m_indexMapping:: relates original vertex indices to new vertex indices
    //   of unique vertices
    //
    // @brief      Calculates the unique vertices.
    //
    // @param[in]  threshold  The threshold
    //
    void PreprocessedMesh::computeUniqueVertices(double threshold)
    {
        // m_obj does not access all indices of the whole objFile
        // access the indices related to m_obj
        m_uniqueIndices = vector<VertexIndex>(m_obj.vertexIndices.begin(),m_obj.vertexIndices.end());
        for (UniqueVertexIndex i=0; i<m_uniqueIndices.size(); i++)
        {
            m_indexMapping[m_uniqueIndices[i]] = i;
        }
        
        // m_indicesWithDuplicates = vector<VertexIndex>(m_obj.vertexIndices.begin(),m_obj.vertexIndices.end());
        // m_uniqueIndices = removeDuplicates<VertexIndex>(m_indicesWithDuplicates);
        // set<VertexIndex> indexSet(m_indicesWithDuplicates.begin(),m_indicesWithDuplicates.end());
        // m_uniqueIndices = vector<VertexIndex>(indexSet.begin(),indexSet.end());
        /*
        // flag for each original vertex index whether it is a duplicate or not
        vector<bool> duplicate;

        for (VertexIndexIndex i = 0; i < m_indicesWithDuplicates.size(); ++i)
        {
            duplicate.push_back(false);
        }
        for (VertexIndexIndex i = 0; i < m_indicesWithDuplicates.size(); ++i)
        {
            // index already identified as duplicate
            if (duplicate[m_indicesWithDuplicates[i]]) continue;

            assert(0 <= m_indicesWithDuplicates[i]);
            assert(m_indicesWithDuplicates[i] < m_objFile.get().m_vertices.size());
            cv::Point3f p1 = m_objFile.get().m_vertices[m_indicesWithDuplicates[i]];
            
            // store mapping of old index to new index
            UniqueVertexIndex newIndex = m_uniqueIndices.size();
            m_indexMapping[m_indicesWithDuplicates[i]] = newIndex;
            // store new index
            m_uniqueIndices.push_back(m_indicesWithDuplicates[i]);

            // check for duplicates of m_indicesWithDuplicates[i]
            for (VertexIndexIndex k = i+1; k < m_indicesWithDuplicates.size(); ++k)
            {
                // calculate distance to other vertex
                assert(0 <= m_indicesWithDuplicates[k]);
                assert(m_indicesWithDuplicates[k] < m_objFile.get().m_vertices.size());
                cv::Point3f p2 = m_objFile.get().m_vertices[m_indicesWithDuplicates[k]];
                cv::Point3f diff = p2-p1;
                double d = cv::norm(diff);
                // double d = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
                if(d < threshold)
                {
                    // k is duplicate of i
                    
                    // mark other index as duplicate
                    assert(0 <= m_indicesWithDuplicates[k]);
                    assert(m_indicesWithDuplicates[k] < duplicate.size());
                    duplicate[m_indicesWithDuplicates[k]] = true;

                    // store mapping of old index to new index
                    // m_indexMapping[m_indicesWithDuplicates[k]] = newIndex;
                } 
            }
        }
            /*
            //*/
    }

    // @sideffects
    // - m_validFaces: stores unique not malformed faces
    // - m_vertexNeighbors: stores vertices that are reachable over the edge of
    //   a face from a vertex in canonical form i.e. lower index maps to its
    //   higher values neighbor indices
    // - m_vertexFaces: stores all faces a vertex is part of
    // - m_edges: contains edges
    // - m_vertexEdges: stores edges adjacent to vertices
    // - m_edgeFaces: stores faces adjacent to edges
    // - m_faceEdges: stores edges that are part of a face
    // - m_edgeNeighborsSharingVertex: useful to navigate on edges
    // - m_faceNeighborsSharingEdge: useful to navigate on faces
    // - m_faceNormals: stores normal vectors for each valid face
    // - m_edgesPossibleContour: contains edges that could possibly be a contour
    //
    void PreprocessedMesh::processMesh()
    {
        // build valid faces m_validFaces
        for (FaceIndex k=0; k < m_obj.faces.size(); k++)
        {
            obj::Face& face = m_obj.faces[k];
            assert(face.vertexIndices.size()==3);
            vector<UniqueVertexIndex> indices;
            // vector<UniqueVertexIndex> indices(face.vertexIndices.begin(), face.vertexIndices.end());
            // apply m_indexMapping from original vertices to unique vertices
            for (VertexIndexIndex i=0; i < face.vertexIndices.size(); i++)
            {
                indices.push_back(m_indexMapping[face.vertexIndices[i]]);
            }
            // sort indices, so we can store vertex neighbors in canonical order
            // i.e. the lower index maps to its higher valued neighbor indices
            vector<UniqueVertexIndex> sortedIndices(indices);
            sort(sortedIndices.begin(), sortedIndices.end());
            VertexIndex a = sortedIndices[0];
            VertexIndex b = sortedIndices[1];
            VertexIndex c = sortedIndices[2];
            
            // check if malformed
            if ((a!=b) && (b!=c) && (a!=c))
            {
                // check if face is duplicate
                bool duplicate = false;
                int n=m_validFaces.size();
                for (ValidFaceIndex i=0;i<n;i++)
                {
                    std::vector<VertexIndex>& current = m_validFaces[i];
                    if (  ((current[0] == a) || (current[1] == a) || (current[2] == a))
                        &&((current[0] == b) || (current[1] == b) || (current[2] == b))
                        &&((current[0] == c) || (current[1] == c) || (current[2] == c)))
                    {
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate)
                {
                    // store valid face
                    ValidFaceIndex index = m_validFaces.size();
                    m_validFaces.push_back(indices);
                    // store links
                    m_vertexNeighbors[a].insert(b);
                    m_vertexNeighbors[a].insert(c);
                    m_vertexNeighbors[b].insert(c);
                    m_vertexFaces[a].insert(index);
                    m_vertexFaces[b].insert(index);
                    m_vertexFaces[c].insert(index);
                }
            }
        }
        // erase neighbor links to self
        for (UniqueVertexIndex k=0; k < m_uniqueIndices.size(); k++)
        {
            m_vertexNeighbors[k].erase(k);
        }

        // int nWeird = 0;
        // int nNormal = 0;
        
        // 
        for (auto item : m_vertexNeighbors)
        {
            for (auto b : item.second)
            {
                // create new edge
                EdgeIndex index = m_edges.size();
                Edge<UniqueVertexIndex> edge(item.first,b);
                m_edges.push_back(edge);

                // add mapping from vertex indices to edge index
                m_vertexEdges[edge.first].insert(index);
                m_vertexEdges[edge.second].insert(index);
                set<ValidFaceIndex>& firstFaces = m_vertexFaces[edge.first];
                set<ValidFaceIndex>& secondFaces = m_vertexFaces[edge.second];

                // all faces shared(hence intersect) by both edge vertices are adjacent to edge
                m_edgeFaces[index] = intersect<ValidFaceIndex>(firstFaces,secondFaces);
                for(ValidFaceIndex k:m_edgeFaces[index])
                {
                    m_faceEdges[k].insert(index);
                }

                // if(m_edgeFaces[index].size()>2)
                // // those edges are weird, but are in the data!
                // {
                //     set<ValidFaceIndex>& faceSet = m_edgeFaces[index];
                //     cout << "edge #" << index << " " << edge.first << ":" << edge.second << endl;
                //     cout << "faces as indices:" << endl;
                //     for (ValidFaceIndex k:faceSet)
                //     {
                //         cout << "#" << k 
                //              << " " << m_validFaces[k][0] 
                //              << " " << m_validFaces[k][1] 
                //              << " " << m_validFaces[k][2] 
                //              << endl;
                //     }
                //     cout << "faces:" << endl;
                //     for (ValidFaceIndex k:faceSet)
                //     {
                //         cout << "#" << k 
                //              << " " << objFile.m_vertices[m_uniqueIndices[m_validFaces[k][0]]]
                //              << " " << objFile.m_vertices[m_uniqueIndices[m_validFaces[k][1]]]
                //              << " " << objFile.m_vertices[m_uniqueIndices[m_validFaces[k][2]]]
                //              << endl;
                //     }
                //     cout << endl;
                //     nWeird++;
                // }
                // else
                // {
                //     nNormal++;
                // }
            }
        }
        // m_edgeNeighborsSharingVertex: useful to navigate on edges
        for (EdgeIndex k=0; k<m_edges.size(); k++)
        {
            Edge<UniqueVertexIndex>& edge=m_edges[k];
            m_edgeNeighborsSharingVertex[k].insert(m_vertexEdges[edge.first].begin(),m_vertexEdges[edge.first].end());
            m_edgeNeighborsSharingVertex[k].insert(m_vertexEdges[edge.second].begin(),m_vertexEdges[edge.second].end());
        }
        // m_faceNeighborsSharingEdge: useful to navigate on faces
        for (ValidFaceIndex i=0; i<m_validFaces.size(); i++)
        {
            for (EdgeIndex k:m_faceEdges[i])
            {
                m_faceNeighborsSharingEdge[i].insert(m_edgeFaces[k].begin(),m_edgeFaces[k].end());
            }
            m_faceNeighborsSharingEdge[i].erase(i);
            assert(m_faceEdges[i].size()==3);
        }
        // m_faceNormals: stores normal vectors for each valid face
        for (ValidFaceIndex i=0; i<m_validFaces.size(); i++)
        {
            cv::Point3f a = m_objFile.get().m_vertices[m_uniqueIndices[m_validFaces[i][0]]];
            cv::Point3f b = m_objFile.get().m_vertices[m_uniqueIndices[m_validFaces[i][1]]];
            cv::Point3f c = m_objFile.get().m_vertices[m_uniqueIndices[m_validFaces[i][2]]];
            cv::Point3f ab = b-a;
            cv::Point3f ac = c-a;
            cv::Point3f normal = ab.cross(ac);
            m_faceNormals.push_back(normal / cv::norm(normal));
        }
        // m_edgesPossibleContour: contains edges that could possibly be a contour
        for (EdgeIndex i=0; i<m_edges.size(); i++)
        {
            if (m_edgeFaces[i].size()==1)
            {
                m_edgesPossibleContour.push_back(i);
            }
            if (m_edgeFaces[i].size()==2)
            {
                vector<FaceIndex> faces(m_edgeFaces[i].begin(),m_edgeFaces[i].end());
                FaceIndex a = faces[0];
                FaceIndex b = faces[1];
                cv::Point3f normalA = m_faceNormals[a];
                cv::Point3f normalB = m_faceNormals[b];
                cv::Point3f diff = normalA-normalB;
                if (cv::norm(diff) < 1e-3) continue;
                m_edgesPossibleContour.push_back(i);
            }
        }
/*/
//*/
    }
} // namespace shadowing
} // namespace gnssShadowing

