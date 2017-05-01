
#include <iostream>

#include "shadowing/shadowing.h"
#include "obj/objFile.h"

#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>

#include "sat/coordinateSystems.h"
#include "common/math.h"
#include "common/rotationMatrices.h"

#include "obj/objFile.h"

using namespace std;

namespace gnssShadowing {
namespace shadowing {
    Shadowing::Shadowing(obj::Object& obj) 
        : m_mesh(obj)
    {
    }

//*

    Transformation Shadowing::computeTransformation(sat::Horizontal sat)
    {
        // if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();

        // vertices are in south-east-up coordinate system

        // transform sez into sat view
        // sat view looks from -x axis (=north) onto scene
        // positive y-axis points left in sat view
        // positive z-axis points up in sat view
        cv::Mat sat_sez = common::rotY(sat.elevation)*common::rotZ(sat.azimuth);
        cv::Mat sez_sat = sat_sez.t();
        
        // if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        return Transformation(sat_sez,sez_sat);
    }

    Contour Shadowing::computeContour(Transformation transformation)
    // Contour Shadowing::computeContour(sat::Horizontal sat, double planeLevel, double eps)
    {   
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        ContourEdges contourEdges;
        ContourFrontFaces contourFrontFace;
        for (EdgeIndex i:m_mesh.m_edgesPossibleContour)
        {

            vector<FaceIndex> faces(m_mesh.m_edgeFaces[i].begin(),m_mesh.m_edgeFaces[i].end());
            // cout << "faces.size()" << faces.size() << endl;
            EdgeContourIndex index = contourEdges.size();
            if (faces.size()>=1)
            {
                FaceIndex a = faces[0];
                cv::Mat normalA(m_mesh.m_faceNormals[a]);
                normalA = transformation.m_sat_sez*normalA;
                if (faces.size()==1) // edge is on outer face
                {
                    if (normalA.at<float>(0) > 0) // back side of face
                    {
                        continue;
                    }
                    else
                    {
                        contourFrontFace.push_back(a);
                    }
                }
                else if (faces.size()>=2)
                {
                    FaceIndex b = faces[1];
                    cv::Mat normalB(m_mesh.m_faceNormals[b]);
                    normalB = transformation.m_sat_sez*normalB;
                    if  ((normalA.at<float>(0)  < 0 && normalB.at<float>(0)  < 0)  // both front
                      || (normalA.at<float>(0)  > 0 && normalB.at<float>(0)  > 0)  // both back
                      || (normalA.at<float>(0) == 0 && normalB.at<float>(0) == 0)  // both parallel to sight
                      || (normalA.at<float>(0) == 0 && normalB.at<float>(0)  > 0)  // parallel to sight and back
                      || (normalA.at<float>(0)  > 0 && normalB.at<float>(0) == 0)) // parallel to sight and back
                    {
                        continue;
                    }
                    else 
                    {
                        // front and parallel to sight
                        //   or
                        // front and back
                        if (normalA.at<float>(0) > 0)
                        {
                            contourFrontFace.push_back(a);
                        }
                        else if (normalB.at<float>(0) > 0)
                        {
                            contourFrontFace.push_back(b);
                        }
                    }
                }
                else 
                {
                    continue;
                }
                if (contourFrontFace.size() == contourEdges.size() + 1 )
                {
                    contourEdges.push_back(i);
                }
            }
        }
        assert(contourEdges.size() == contourFrontFace.size());
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
        return Contour(contourEdges, contourFrontFace);
    }

    Contour Shadowing::computeContour(sat::Horizontal sat)
    {
        Transformation transformation=computeTransformation(sat);
        
        return computeContour(transformation);
    }
   
    ShadowVolume Shadowing::computeShadowVolume(Contour contour, Transformation transformation, double planeLevel, double eps)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        // std::cout << __FILE__ << ":" << __LINE__ << std::endl;
        ContourEdges& contourEdges = contour.first;
        ContourFrontFaces& contourFrontFace = contour.second;
        // plane is pointing up
        cv::Mat sez_normalPlane = (cv::Mat_<float>(3,1) << 0,0,1);
        
        // planeLevel: distance part of hesse normal form of plane equation

        // rotate ground plane into sat view system
        cv::Mat sat_NormalPlane = transformation.m_sat_sez*sez_normalPlane;
        // cout << "normalPlane " << normalPlane << endl;
        // cout << "rotNormalPlane " << rotNormalPlane << endl;

        // std::cout << __FILE__ << ":" << __LINE__ << std::endl;
        // project parallel sat view shadow of contour onto ground plane
        ShadowVolume shadowVolume;
        // vector<cv::Point3f > contourNormals;
        for (EdgeContourIndex k=0; k<contourEdges.size(); k++)
        {
            EdgeIndex i = contourEdges[k];
            cv::Mat a(m_mesh.m_objFile.get().m_vertices[m_mesh.m_uniqueIndices[m_mesh.m_edges[i].first]]);
            cv::Mat b(m_mesh.m_objFile.get().m_vertices[m_mesh.m_uniqueIndices[m_mesh.m_edges[i].second]]);
            if ((a.at<float>(2) < planeLevel) && (b.at<float>(2) < planeLevel))
            {
                // edge completely below plane, can't cast shadow
                // continue;
            }
            else if ((a.at<float>(2) < planeLevel) || (b.at<float>(2) < planeLevel))
            {
                // edge partly below plane, clip it and only project part above plane
                // TODO
                // continue; // for now just skip it
            }

            vector<FaceIndex> faces(m_mesh.m_edgeFaces[i].begin(),m_mesh.m_edgeFaces[i].end());
            //cv::Mat normalA(m_mesh.m_faceNormals[faces[0]]);
            //cout << __FILE__ << ":" << __LINE__ << endl;
            //cv::Mat normalB(m_mesh.m_faceNormals[faces[1]]);
            //cout << __FILE__ << ":" << __LINE__ << endl;
            //cv::Mat normalAB = (normalA+normalB)/2;

            cv::Mat frontNormalSEZMat(m_mesh.m_faceNormals[contourFrontFace[k]]);

            cv::Mat frontNormalSATMat(transformation.m_sat_sez*frontNormalSEZMat);
            cv::Point3f frontNormalSAT(frontNormalSATMat);
            // sat looks from negative x-axis in direction of positive x-axis
            // casting shadow parallel to the view direction unlocks the x-values of the vertex coordinates
            // i.e. they can be changed to project the contour onto the rotated ground plane
            // this is done by computing x from y&z and the hesse normal form of the ground plane
            // x*nx+y*ny+z*nz = d
            // x=(d-y*ny-z*nz)/nx
            if (abs(sat_NormalPlane.at<float>(0))>eps)
            {
                // only increase the x-value, i.e. cast the shadow edge from its
                // material edge and the satellite away 
                // hence the use of max
                
                // rotate contour vertices into sat view system
                cv::Point3f A(cv::Mat(transformation.m_sat_sez*a));
                cv::Point3f B(cv::Mat(transformation.m_sat_sez*b));

                A.x = max((double)A.x, (planeLevel - A.y*sat_NormalPlane.at<float>(1)
                                                   - A.z*sat_NormalPlane.at<float>(2))
                                       / sat_NormalPlane.at<float>(0));
                B.x = max((double)B.x, (planeLevel - B.y*sat_NormalPlane.at<float>(1)
                                                   - B.z*sat_NormalPlane.at<float>(2))
                                       / sat_NormalPlane.at<float>(0));


                // now the rotation can be undone to get the coordinates in south-east-up
                A=cv::Point3f(cv::Mat(transformation.m_sez_sat*cv::Mat(A)));
                B=cv::Point3f(cv::Mat(transformation.m_sez_sat*cv::Mat(B)));

                // if the edge is below the plane its shadow can't be cast on it 
                // due to the max operation above
                // in this case cast the edge along the z-axis in south-east-up onto the plane
                // in any case, the z-value of the edge is now at plane level
                A.z = planeLevel;
                B.z = planeLevel;

                // cv::Point3f mid((A+B)/2);
                cv::Point3f AB(B-A);
                cv::Point3f normalOnPlaneSEZ(-AB.y,AB.x,0);
                cv::Mat normalOnPlaneSEZMat(normalOnPlaneSEZ);
                cv::Point3f normalOnPlaneSAT(cv::Mat(transformation.m_sat_sez*normalOnPlaneSEZMat));
                // cv::Point3f normal(normalAB);
                
                // project normals on SAT YZ plane
                normalOnPlaneSAT.x = 0;
                // determine angle between normals
                double cos_angle = normalOnPlaneSAT.dot(frontNormalSAT) / (cv::norm(normalOnPlaneSAT)*cv::norm(frontNormalSAT));
                if (cos_angle < 0)
                {
                    // angle is bigger than 90 degree 
                    // shadow contour edge points in wrong direction
                    normalOnPlaneSEZ *= -1;
                    // swap A & B
                    // cv::Point3f tmp = A;
                    // A = B;
                    // B = tmp;

                }
                // normalize normal to unit length
                normalOnPlaneSEZ = normalOnPlaneSEZ / cv::norm(normalOnPlaneSEZ);
                
                // if (cv::norm(AB) > 1e-3)
                // some points are projected onto the same point on the plane
                // skip those
                {
                    std::vector<cv::Point3f> shadowFace;
                    shadowFace.push_back(cv::Point3f(a));
                    shadowFace.push_back(A);
                    shadowFace.push_back(B);
                    shadowFace.push_back(cv::Point3f(b));
                    shadowVolume.push_back(shadowFace);
                    // shadow.first.push_back(Edge<cv::Point3f>(mid,mid+normalOnPlaneSEZ));
                }
                // cout << a.t() << b.t() << endl;
            } 
            // else:
            // if rotated plane normal vector x is zero it freely spins on the y-z plane
            // in a parallel projection along x-axis this plane is parallel to view, it reduces 
            // to a line in the view of the satellite.
            // this makes the shadow infinetly small so it is discarded
        }

        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
        return shadowVolume;
    }

//*/
} // namespace shadowing
} // namespace gnssShadowing

