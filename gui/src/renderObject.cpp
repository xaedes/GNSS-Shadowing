
#include <vector>
#include "renderObject.h"

using namespace std;

namespace gnssShadowing {
namespace gui {

    RenderObj::RenderObj(obj::Object& obj) :
        m_obj(obj)
    {
        obj::ObjFile& objFile = *obj.objFile;

        int idx=0;
        // collect all vertices, colors and indices for this object
        for(obj::Face face : obj.faces)
        {
            for(int index : face.vertexIndices)
            {
                cv::Point3f cvPoint = objFile.m_vertices[index];
                cv::Point3f cvColor = objFile.m_mtllib.m_materials[face.materialName].ambient;

                Vertex vertex;
                vertex.x = cvPoint.x;
                vertex.y = cvPoint.y;
                vertex.z = cvPoint.z;


                Vertex color;
                color.x = cvColor.x;
                color.y = cvColor.y;
                color.z = cvColor.z;
                m_vertices.push_back(vertex);
                m_colors.push_back(color);
                m_indices.push_back(idx);
                ++idx;
            }
        }

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        // create vertex buffer
        glGenBuffers(1, &m_vboVertices);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices); 
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*m_vertices.size(), &m_vertices[0].x, GL_STATIC_DRAW); 
        glVertexPointer(3, GL_FLOAT, 0, 0);

        // create color buffer
        glGenBuffers(1, &m_vboColor);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboColor); 
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*m_colors.size(), &m_colors[0].x, GL_STATIC_DRAW); 
        glColorPointer(3, GL_FLOAT, 0, 0);

        // create buffer to store indices into vertex buffer
        glGenBuffers(1, &m_vboIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

        // number of triangles
        m_numElements = obj.faces.size()*3;
    }

    void RenderObj::paint()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices); 
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboColor); 
        glColorPointer(3, GL_FLOAT, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndices);
    

        // glDisableClientState(GL_COLOR_ARRAY);
        // glColor3f(0,0,0);
        // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        // glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, 0);
        // glEnableClientState(GL_COLOR_ARRAY);
        
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, 0);

    }

} // namespace gui
} // namespace gnssShadowing

