
#include <opencv2/opencv.hpp>
#include <vector>

#include "mapping/shadowMap.h"
#include "mapping/mapProperties.h"

using namespace gnssShadowing::shadowing;
using namespace std;
namespace gnssShadowing {
namespace mapping {

    
    ShadowMap::ShadowMap(const MapProperties& properties)
        : m_properties(properties)
    {
        initMaps();
    }
    ShadowMap::ShadowMap(const MapProperties& properties, const ShadowVolume& shadowVolume)
        : m_properties(properties)
    {
        initMaps();
        computeShadowMap(shadowVolume);
    }

    void ShadowMap::initMaps()
    {
        m_maps.clear();
        for (double planeLevel : m_properties.m_plane_levels)
        {
            m_maps.push_back(cv::Mat_<ShadowMapValue>(m_properties.m_num_rows, m_properties.m_num_cols, (ShadowMapValue)(0)));
        }
    }

    void ShadowMap::computeShadowMap(const ShadowVolume& shadowVolume)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif

        // clear maps
        for (int planeLevelIndex=0; planeLevelIndex<m_properties.m_plane_levels.size(); planeLevelIndex++)
        {
            m_maps[planeLevelIndex].setTo(cv::Scalar(0));
        }
            
        // rasterize each face
        for (const ShadowFace& shadowFace : shadowVolume)
        {
            rasterizeShadowFace(shadowFace);
        }

        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }

    void ShadowMap::rasterizeShadowFace(const ShadowFace& shadowFace)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        // determine Y bounds of face and edges
        float minY, maxY;
        assert(shadowFace.size()==4);
        minY = shadowFace[0].y;
        maxY = shadowFace[0].y;
        vector<Edge<cv::Point3f>> edges;
        for (int k=1; k<shadowFace.size(); k++)
        {
            minY = min(minY, shadowFace[k].y);
            maxY = max(maxY, shadowFace[k].y);
            edges.push_back(Edge<cv::Point3f>(shadowFace[k-1],shadowFace[k]));
        }
        edges.push_back(Edge<cv::Point3f>(shadowFace[3],shadowFace[0]));

        // face normal
        cv::Point3f v1 = shadowFace[1]-shadowFace[0];
        cv::Point3f v2 = shadowFace[3]-shadowFace[0];
        cv::Point3f n = v1.cross(v2);
        n = n / cv::norm(n);
        // x*nx+y*ny+z*nz = d
        float d = shadowFace[0].dot(n);
        if (n.z == 0)
        {
            // if n.z == 0 face is orthogonal to view direction and not visible
            return;
        }

        // convert Y Bounds into map coordinates
        int minJ = (int)ceil(m_properties.J(minY));
        int maxJ = (int)floor(m_properties.J(maxY));
        minJ = min(max(minJ, 0), m_properties.m_num_rows-1);
        maxJ = min(max(maxJ, 0), m_properties.m_num_rows-1);
        // rasterize face line by line
        for (int j=minJ; j<=maxJ; j++)
        {
            float y = m_properties.Y(j);
            
            // calculate intersections with edges on this line
            vector<float> intersections;
            for (int k=0; k<edges.size(); k++)
            {
                cv::Point3f diff = edges[k].second - edges[k].first;
                if (diff.y == 0) continue;
                float progress = (y - edges[k].first.y)/diff.y;
                if((0 <= progress) && (progress <= 1))
                {
                    // intersection is on edge (0 <= progress <= 1)
                    float x = progress*diff.x+edges[k].first.x;
                    intersections.push_back(x);
                }
            }
            // if there is more than one intersection (i.e. a segment):
            if (intersections.size() == 2) 
            {
                float minX = min(intersections[0],intersections[1]);
                float maxX = max(intersections[0],intersections[1]);
                int minI = (int)ceil(m_properties.I(minX));
                int maxI = (int)floor(m_properties.I(maxX));
                minI = max(minI, 0);
                maxI = min(maxI, m_properties.m_num_cols-1);
                //minI = min(max(minI, 0), m_properties.m_num_cols-1);
                //maxI = min(max(maxI, 0), m_properties.m_num_cols-1);
                for (int i=minI; i<=maxI; i++)
                {
                    // calculate z at this point and check if below or above plane
                    // x*nx+y*ny+z*nz = d
                    // z = (d-x*nx-y*ny)/nz
                    // if nz == 0 face is orthogonal to view direction and not visible
                    float x = m_properties.X(i);
                    if (minX <= x && x <= maxX)
                    {
                        // sample point is between between intersections
                        float z = (d-x*n.x-y*n.y)/n.z;
                        for (int planeLevelIndex=0; planeLevelIndex<m_properties.m_plane_levels.size(); planeLevelIndex++)
                        {
                            if (z>=m_properties.m_plane_levels[planeLevelIndex])
                            {
                                getCell(i,j,planeLevelIndex) = 1;
                            }
                        }
                    }
                }
            }
        }
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }



} // namespace mapping
} // namespace gnssShadowing
