#include <iostream>

#include "shadowing/version.h"
#include "obj/objFile.h"

#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>

#include "sat/coordinateSystems.h"
#include "common/math.h"
#include "common/rotationMatrices.h"

using namespace gnssShadowing::shadowing;
using namespace gnssShadowing;
using namespace std;


template <typename T>
set<T> intersect(set<T>& first, set<T>& second)
{
    set<T> intersection;
    set_intersection(first.begin(),first.end(),second.begin(),second.end(),
                     std::inserter(intersection,intersection.begin()));
    for(auto item:intersection)
    {
        assert(first.count(item));
        assert(second.count(item));
    }
    for(auto item:first)
    {
        if(!intersection.count(item)) assert(!second.count(item));
    }
    for(auto item:second)
    {
        if(!intersection.count(item)) assert(!first.count(item));
    }

    return intersection;
}

typedef int VertexIndex; // index into list of vertices
typedef int UniqueVertexIndex; // index into list of unique vertices
typedef int VertexIndexIndex; // index into list of vertex indices
typedef int FaceIndex; // index into list of faces
typedef int ValidFaceIndex; // index into list of valid faces
typedef int EdgeIndex; // index into list of edges
typedef int EdgeContourIndex; // index into list of contour edges
template <typename T> using Edge = pair<T,T>;  // edge

int main(int argc, char* argv[])
{	
    cout << "version " << Version::getString() << endl;
    cout << "revision " << Version::getRevision() << endl;
    
    obj::ObjFile objFile("data/uni.obj");
    obj::Group& buildings = objFile.m_groups["Building"];
    obj::Object& building = *buildings.objects[0];

    cout << building.name << endl;

    // remove duplicate indices (indices of vertices that are VERY close to each other in space)
    double threshold  = 1e-3;
    vector<VertexIndex> indicesWithDuplicates(building.vertexIndices.begin(),building.vertexIndices.end());
    // stores unique indices
    vector<VertexIndex> uniqueIndices;
    // relates original vertex indices to new vertex indices of unique vertices
    unordered_map<VertexIndex,UniqueVertexIndex> indexMapping;
    // flag for each original vertex index whether it is a duplicate or not
    vector<bool> duplicate;

    for (VertexIndexIndex i = 0; i < indicesWithDuplicates.size(); ++i)
    {
        duplicate.push_back(false);
    }

    for (VertexIndexIndex i = 0; i < indicesWithDuplicates.size(); ++i)
    {
        // index already identified as duplicate
        if (duplicate[indicesWithDuplicates[i]]) continue;

        cv::Point3f p1 = objFile.m_vertices[indicesWithDuplicates[i]];
        
        // store mapping of old index to new index
        UniqueVertexIndex newIndex = uniqueIndices.size();
        indexMapping[indicesWithDuplicates[i]] = newIndex;
        // store new index
        uniqueIndices.push_back(indicesWithDuplicates[i]);

        // check for duplicates of indicesWithDuplicates[i]
        for (VertexIndexIndex k = i+1; k < indicesWithDuplicates.size(); ++k)
        {
            // calculate distance to other vertex
            cv::Point3f p2 = objFile.m_vertices[indicesWithDuplicates[k]];
            cv::Point3f diff = p2-p1;
            double d = cv::norm(diff);
            // double d = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
            if(d < threshold)
            {
                // k is duplicate of i
                
                // mark other index as duplicate
                duplicate[indicesWithDuplicates[k]] = true;

                // store mapping of old index to new index
                indexMapping[indicesWithDuplicates[k]] = newIndex;
            } 
        }
    }
    // stores unique not malformed faces
    vector<vector<UniqueVertexIndex> > validFaces;
    // stores vertices that are reachable over the edge of a face from a vertex in canonical form 
    // i.e. lower index maps to its higher values neighbor indices
    unordered_map<UniqueVertexIndex, set<UniqueVertexIndex> > vertexNeighbors;
    // stores all faces a vertex is part of
    unordered_map<UniqueVertexIndex, set<ValidFaceIndex> > vertexFaces;
    for (FaceIndex k=0; k < building.faces.size(); k++)
    {
        obj::Face& face = building.faces[k];
        assert(face.vertexIndices.size()==3);
        vector<UniqueVertexIndex> indices;
        // apply indexMapping from original vertices to unique vertices
        for (VertexIndexIndex i=0; i < face.vertexIndices.size(); i++)
        {
            indices.push_back(indexMapping[face.vertexIndices[i]]);
        }
        // sort indices, so we can store vertex neighbors in canonical order
        // i.e. the lower index maps to its higher valued neighbor indices
        vector<UniqueVertexIndex> sortedIndices(indices);
        sort(sortedIndices.begin(), sortedIndices.end());
        UniqueVertexIndex a = sortedIndices[0];
        UniqueVertexIndex b = sortedIndices[1];
        UniqueVertexIndex c = sortedIndices[2];
        
        // check if malformed
        if ((a!=b) && (b!=c) && (a!=c))
        {
            // check if face is duplicate
            bool duplicate = false;
            for (ValidFaceIndex i=0;i<validFaces.size();i++)
            {

                if (  ((validFaces[i][0] == a) || (validFaces[i][1] == a) || (validFaces[i][2] == a))
                    &&((validFaces[i][0] == b) || (validFaces[i][1] == b) || (validFaces[i][2] == b))
                    &&((validFaces[i][0] == c) || (validFaces[i][1] == c) || (validFaces[i][2] == c)))
                {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate)
            {
                // store valid face
                ValidFaceIndex index = validFaces.size();
                validFaces.push_back(indices);
                // store links
                vertexNeighbors[a].insert(b);
                vertexNeighbors[a].insert(c);
                vertexNeighbors[b].insert(c);
                vertexFaces[a].insert(index);
                vertexFaces[b].insert(index);
                vertexFaces[c].insert(index);
            }
        }
    }
    for (VertexIndex k=0; k < uniqueIndices.size(); k++)
    {
        vertexNeighbors[k].erase(k);
    }

    // contains edges
    vector<Edge<UniqueVertexIndex> > edges;
    // stores edges adjacent to vertices
    unordered_map<UniqueVertexIndex, set<EdgeIndex> > vertexEdges;
    // stores faces adjacent to edges
    unordered_map<EdgeIndex, set<ValidFaceIndex> > edgeFaces;
    // stores edges that are part of a face
    unordered_map<ValidFaceIndex, set<EdgeIndex> > faceEdges;
    int nWeird = 0;
    int nNormal = 0;
    for (auto item:vertexNeighbors)
    {
        for (auto b:item.second)
        {
            // create new edge
            EdgeIndex index = edges.size();
            Edge<UniqueVertexIndex> edge(item.first,b);
            edges.push_back(edge);

            // add mapping from vertex indices to edge index
            vertexEdges[item.first].insert(index);
            vertexEdges[b].insert(index);
            set<ValidFaceIndex>& firstFaces = vertexFaces[edge.first];
            set<ValidFaceIndex>& secondFaces = vertexFaces[edge.second];

            // all faces shared(hence intersect) by both edge vertices are adjacent to edge
            edgeFaces[index] = intersect<ValidFaceIndex>(firstFaces,secondFaces);
            for(ValidFaceIndex k:edgeFaces[index])
            {
                faceEdges[k].insert(index);
            }

            if(edgeFaces[index].size()>2)
            // those edges are weird, but are in the data!
            {
                set<ValidFaceIndex>& faceSet = edgeFaces[index];
                cout << "edge #" << index << " " << edge.first << ":" << edge.second << endl;
                cout << "faces as indices:" << endl;
                for (ValidFaceIndex k:faceSet)
                {
                    cout << "#" << k 
                         << " " << validFaces[k][0] 
                         << " " << validFaces[k][1] 
                         << " " << validFaces[k][2] 
                         << endl;
                }
                cout << "faces:" << endl;
                for (ValidFaceIndex k:faceSet)
                {
                    cout << "#" << k 
                         << " " << objFile.m_vertices[uniqueIndices[validFaces[k][0]]]
                         << " " << objFile.m_vertices[uniqueIndices[validFaces[k][1]]]
                         << " " << objFile.m_vertices[uniqueIndices[validFaces[k][2]]]
                         << endl;
                }
                cout << endl;
                nWeird++;
            }
            else
            {
                nNormal++;
            }
        }
    }
    // useful to navigate on edges
    unordered_map<EdgeIndex, set<EdgeIndex> > edgeNeighborsSharingVertex;
    for (EdgeIndex k=0; k<edges.size(); k++)
    {
        Edge<UniqueVertexIndex>& edge=edges[k];
        edgeNeighborsSharingVertex[k].insert(vertexEdges[edge.first].begin(),vertexEdges[edge.first].end());
        edgeNeighborsSharingVertex[k].insert(vertexEdges[edge.second].begin(),vertexEdges[edge.second].end());
    }
    // useful to navigate on faces
    unordered_map<ValidFaceIndex, set<ValidFaceIndex> > faceNeighborsSharingEdge;
    for (ValidFaceIndex i=0;i<validFaces.size();i++)
    {
        for (EdgeIndex k:faceEdges[i])
        {
            faceNeighborsSharingEdge[i].insert(edgeFaces[k].begin(),edgeFaces[k].end());
        }
        faceNeighborsSharingEdge[i].erase(i);
        assert(faceEdges[i].size()==3);
    }
    // stores normal vectors for each valid face
    vector<cv::Point3f> faceNormals;
    for (ValidFaceIndex i=0;i<validFaces.size();i++)
    {
        cv::Point3f a = objFile.m_vertices[uniqueIndices[validFaces[i][0]]];
        cv::Point3f b = objFile.m_vertices[uniqueIndices[validFaces[i][1]]];
        cv::Point3f c = objFile.m_vertices[uniqueIndices[validFaces[i][2]]];
        cv::Point3f ab = b-a;
        cv::Point3f ac = c-a;
        cv::Point3f normal = ab.cross(ac);
        faceNormals.push_back(normal / cv::norm(normal));
    }
    // contains edges that could possibly be a contour
    vector<EdgeIndex> edgesPossibleContour;
    for (EdgeIndex i=0;i<edges.size();i++)
    {
        if(edgeFaces[i].size()==1)
        {
            edgesPossibleContour.push_back(i);
        }
        if(edgeFaces[i].size()==2)
        {
            vector<FaceIndex> faces(edgeFaces[i].begin(),edgeFaces[i].end());
            FaceIndex a = faces[0];
            FaceIndex b = faces[1];
            cv::Point3f normalA = faceNormals[a];
            cv::Point3f normalB = faceNormals[b];
            cv::Point3f diff = normalA-normalB;
            if(cv::norm(diff) > 1e-3)
            {
                edgesPossibleContour.push_back(i);
            }
        }
    }
    
    // south-east-up coordinate system
    sat::Horizontal sat;
    sat.azimuth = 90*D2R; // north
    sat.elevation = 45*D2R;
    // after rot sat looks from -x axis (=north) onto scene
    cv::Mat rot=common::rotY(-sat.elevation)*common::rotZ(-sat.azimuth);
    cv::Mat rot_inv=rot.t();
    // for (ValidFaceIndex i=0;i<validFaces.size();i++)
    // {
    //     cv::Mat normal(faceNormals[i]);
    //     normal = rot*normal;
    //     // cout << normal << endl;
    // }
    vector<EdgeIndex> edgesContour;
    for (EdgeIndex i:edgesPossibleContour)
    {
        vector<FaceIndex> faces(edgeFaces[i].begin(),edgeFaces[i].end());
        cout << "faces.size()" << faces.size() << endl;
    
        if (faces.size()>=1)
        {
            FaceIndex a = faces[0];
            cv::Mat normalA(faceNormals[a]);
            normalA = rot*normalA;
            if ((faces.size()==1) && (normalA.at<float>(0) > 0)) // back side of face
            {
                continue;
            }
            if (faces.size()>=2)
            {
                FaceIndex b = faces[1];
                cv::Mat normalB(faceNormals[b]);
                normalB = rot*normalB;
                if  ((normalA.at<float>(0)  < 0 && normalB.at<float>(0)  < 0)  // both front
                  || (normalA.at<float>(0)  > 0 && normalB.at<float>(0)  > 0)  // both back
                  || (normalA.at<float>(0) == 0 && normalB.at<float>(0) == 0)  // both parallel to sight
                  || (normalA.at<float>(0) == 0 && normalB.at<float>(0)  > 0)  // parallel to sight and back
                  || (normalA.at<float>(0)  > 0 && normalB.at<float>(0) == 0)) // parallel to sight and back
                {
                    continue;
                }
                // else 
                // {
                    // parallel to sight and front
                    //   or
                    // front and back
                // }
            }
            edgesContour.push_back(i);
        }
    }
    
    // plane is pointing up
    cv::Mat normalPlane = (cv::Mat_<float>(3,1) << 0,0,1);
    
    // distance part of hesse normal form of plane equation
    double planeLevel = 1;

    // rotate ground plane into sat view system
    cv::Mat rotNormalPlane = rot*normalPlane;
    cout << "normalPlane " << normalPlane << endl;
    cout << "rotNormalPlane " << rotNormalPlane << endl;

    float eps = 1e-3;
    // project parallel sat view shadow of contour onto ground plane
    vector<Edge<cv::Point3f> > contour;
    vector<cv::Point3f > contourNormals;
    for (EdgeIndex i:edgesContour)
    {
        cv::Mat a(objFile.m_vertices[uniqueIndices[edges[i].first]]);
        cv::Mat b(objFile.m_vertices[uniqueIndices[edges[i].second]]);
        // rotate contour vertices into sat view system
        a=rot*a;
        b=rot*b;
        // sat looks from negative x-axis in direction of positive x-axis
        // casting shadow parallel to the view direction unlocks the x-values of the vertex coordinates
        // i.e. they can be changed to project the contour onto the rotated ground plane
        // this is done by computing x from y&z and the hesse normal form of the ground plane
        // x*nx+y*ny+z*nz = d
        // x=(d-y*ny-z*nz)/nx
        if(abs(rotNormalPlane.at<float>(0))>eps)
        {
            a.at<float>(0) = (planeLevel - a.at<float>(1)*rotNormalPlane.at<float>(1)
                                         - a.at<float>(2)*rotNormalPlane.at<float>(2))
                                / rotNormalPlane.at<float>(0);
            b.at<float>(0) = (planeLevel - b.at<float>(1)*rotNormalPlane.at<float>(1)
                                         - b.at<float>(2)*rotNormalPlane.at<float>(2))
                                / rotNormalPlane.at<float>(0);

            // now the rotation can be undone to get the coordinates in south-east-up
            a=rot_inv*a;
            b=rot_inv*b;
            contour.push_back(Edge<cv::Point3f>(cv::Point3f(a),cv::Point3f(b)));
            cout << a.t() << b.t() << endl;
        } 
        // else
        // {
        //     // cout << abs(rotNormalPlane.at<float>(0)) << endl;
        // }
        // else:
        // if rotated plane normal vector x is zero it freely spins on the y-z plane
        // in a parallel projection along x-axis this plane is parallel to view, it reduces to a line
        // in the view of the satellite. this makes the shadow infinetly small so it is discarded
    }


    cout << "edgesContour: " << edgesContour.size() << endl;
    cout << "edgesPossibleContour: " << edgesPossibleContour.size() << endl;
    cout << "edges: " << edges.size() << endl;
    cout << "nWeird " << nWeird << endl;
    cout << "nNormal " << nNormal << endl;
    cout << "done" << endl;




    // unordered_map<int, set<int> > edgeIndexNeighbors;
    // for (int k=0;k < edges.size(); k++)
    // {
    //     const pair<int,int>& edge = edges[k];
    //     set<int>& firstNeighborEdges = edgeIndicesByVertexIndex[edge.first];
    //     set<int>& secondNeighborEdges = edgeIndicesByVertexIndex[edge.second];
    //     // add mapping from edge indices to edge indices
    //     edgeIndexNeighbors[k].insert(firstNeighborEdges.begin(),
    //                                  firstNeighborEdges.end());
    //     edgeIndexNeighbors[k].insert(secondNeighborEdges.begin(),
    //                                  secondNeighborEdges.end());

    //     // cout << faceIndicesByEdgeIndex[k].size() << endl;
    // }

    // for (auto item:edgeFaces)
    // {
    //     // cout << item.first << ":" << item.second.size() << endl;
    //     // erase self connection (from edge k to edge k)
    //     // item.second.erase(item.first);
    //     // for (int neighbor:item.second)
    //     // {
    //     //     cout << item.first << " " << neighbor << endl;
    //     // }
    // }

}

