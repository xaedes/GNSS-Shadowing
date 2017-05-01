
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "obj/objFile.h"

namespace gnssShadowing {
namespace gui {

    struct Vertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };

    class RenderObj
    {
    public:
        RenderObj(obj::Object& obj);
    
        void paint();
    protected:
        obj::Object& m_obj;
        GLuint m_vboVertices;
        GLuint m_vboColor;
        GLuint m_vboIndices;
        int m_numElements;
        std::vector<Vertex> m_vertices;
        std::vector<Vertex> m_colors;
        std::vector<GLuint> m_indices;        
    };

} // namespace gui
} // namespace gnssShadowing
