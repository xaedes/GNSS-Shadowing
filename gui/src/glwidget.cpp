
#include "glwidget.h"

#include <QCoreApplication>
// #include <QOpenGLContext>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <vector>

#include <iostream>

#include "app.h"
#include "keyboard.h"
#include "keyboardControl.h"
#include "camera.h"

#include "common/math.h"
#include "common/timing.h"

#include "shadowing/shadowing.h"

#include "mapping/mapProperties.h"
#include "mapping/shadowMap.h"


using namespace Qt;
using namespace std;
using namespace gnssShadowing::common;

namespace gnssShadowing {
namespace gui {

    GLWidget::GLWidget(App& app, QWidget *parent) :
        QOpenGLWidget(parent),
        m_app(app),
        m_benchmark( new common::Benchmark()),
        m_world("data/current.tle",now_seconds(),"data/uni.obj","Building"),
        m_maxDOP(2)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        m_timeStart = now_seconds();

        std::vector<double> planeLevels;
        planeLevels.clear();
        // m_planeLevels.push_back(0.10);
        planeLevels.push_back(55.50);
        // // m_planeLevels.push_back(5.01);
        // // m_planeLevels.push_back(10.01);

        
        float res = 10.0;
        // float res = 5.0;
        // float res = 2.0;
        // float res = 1;
        // float res = 0.5;
        float w = 400;
        float h = 600;
        m_mapProperties = mapping::MapProperties(-w/2,-h/2,w/res,h/res,res,res,planeLevels);

        m_mapper.reset(new mapping::Mapper(m_world, m_mapProperties));
        
        
        m_computeMapsEnabled = true;

        setFocusPolicy(Qt::StrongFocus);
        setFocus();
        startTimer(10);


        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }

    GLWidget::~GLWidget()
    {
    }

    QSize GLWidget::minimumSizeHint() const
    {
        return QSize(50, 50);
    }

    QSize GLWidget::sizeHint() const
    {
        return QSize(400, 400);
    }

    void GLWidget::actionComputeDOPMap()
    {
        m_mapper->computeDOPMap(now_seconds());
    }

    void GLWidget::actionBenchmarkResults()
    {
        cout << "actionBenchmarkResults" << endl;
        if (!m_benchmark.get()) return;
        for (auto item:*m_benchmark.get())
        {
            cout << item.first << endl;
            item.second.printStats();
        }
    }


    void GLWidget::cleanup()
    {
        makeCurrent();
        // do cleanup
        doneCurrent();
    }

    void GLWidget::initializeGL()
    {
        glewInit();
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(0, 0, 0, 0);

        initObjects();

    }

    void GLWidget::initObjects()
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        // create render objects
        for (obj::Object* obj_ptr : m_world.m_objFile.m_objectsVector)
        {
            obj::Object& obj = *obj_ptr;
            std::shared_ptr<RenderObj> renderObj;
            renderObj.reset(new RenderObj(obj));
            m_renderObjects.push_back(renderObj);
        }
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }

    void GLWidget::timerEvent(QTimerEvent *event)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        update();
        m_app.m_keyboardControl->update();

        m_mapper->updateSats(now_seconds());

        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif

        // cout << "---" << endl;
    }
    void GLWidget::paintSats(sat::AvailableSats& sats, double length)
    {
        if (sats.getNumSats() == 0) return;
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        glBegin(GL_LINES);
        glColor3f(1,1,0);
        for (int k=0; k<sats.getNumSats(); k++)
        {
            sat::Horizontal& hor = sats.getSatPositionHorizontal(k);
            glVertex3f(0,0,0);

            // coordsys is south-east-up
            sat::SEZ sez = hor.toSEZ();
            glVertex3f(sez.position[0],sez.position[1],sez.position[2]);

            // cout << "sat.azimuth " << sat.azimuth*R2D << endl;
            // cout << "sat.elevation " << sat.elevation*R2D << endl;
        }
        glEnd();
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }

    void GLWidget::paintMapProperties(mapping::MapProperties mapProperties, int planelevelIndex)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        if (mapProperties.m_plane_levels.size() == 0) return;
        double planeLevel = mapProperties.m_plane_levels[planelevelIndex];
        float halfTileSize = ((mapProperties.m_x_resolution+mapProperties.m_y_resolution)/2)/2;
        glBegin(GL_LINES);
        glColor3f(1,1,1);
        glVertex3f(mapProperties.X(0)-halfTileSize,mapProperties.Y(0)-halfTileSize,planeLevel);
        glVertex3f(mapProperties.X(mapProperties.m_num_cols-1)+halfTileSize,mapProperties.Y(0)-halfTileSize,planeLevel);

        glVertex3f(mapProperties.X(mapProperties.m_num_cols-1)+halfTileSize,mapProperties.Y(0)-halfTileSize,planeLevel);
        glVertex3f(mapProperties.X(mapProperties.m_num_cols-1)+halfTileSize,mapProperties.Y(mapProperties.m_num_rows-1)+halfTileSize,planeLevel);

        glVertex3f(mapProperties.X(mapProperties.m_num_cols-1)+halfTileSize,mapProperties.Y(mapProperties.m_num_rows-1)+halfTileSize,planeLevel);
        glVertex3f(mapProperties.X(0)-halfTileSize,mapProperties.Y(mapProperties.m_num_rows-1)+halfTileSize,planeLevel);

        glVertex3f(mapProperties.X(0)-halfTileSize,mapProperties.Y(mapProperties.m_num_rows-1)+halfTileSize,planeLevel);
        glVertex3f(mapProperties.X(0)-halfTileSize,mapProperties.Y(0)-halfTileSize,planeLevel);
        glEnd();
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }
    void GLWidget::paintVisibilityMap(mapping::VisibilityMap& map, int planelevelIndex)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        
        if (map.m_properties.m_plane_levels.size() == 0) return;

        glBegin(GL_QUADS);
        float halfTileSize = ((map.m_properties.m_x_resolution+map.m_properties.m_y_resolution)/2)/2;
        float planeLevel = map.m_properties.m_plane_levels[planelevelIndex];
        for (int i=0; i<map.m_properties.m_num_cols; i++)
        {
            float x = map.m_properties.X(i);
            for (int j=0; j<map.m_properties.m_num_rows; j++)
            {

                // int satCount = 0; 
                int satCount = map.m_items[planelevelIndex][i][j].size();
                // if (satCount>0)
                {
                    float y = map.m_properties.Y(j);
                    float c = 1-1*(float)satCount/m_mapper->m_sats.getNumSats();
                    glColor3f(c,1-c,0);

                    glVertex3f(x-halfTileSize,y-halfTileSize,planeLevel);
                    glVertex3f(x-halfTileSize,y+halfTileSize,planeLevel);
                    glVertex3f(x+halfTileSize,y+halfTileSize,planeLevel);
                    glVertex3f(x+halfTileSize,y-halfTileSize,planeLevel);
                }
                
            }
        }
        glEnd();
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }
    void GLWidget::paintDOPMap(mapping::DOPMap& map, int planelevelIndex)
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        glBegin(GL_QUADS);
        float halfTileSize = ((map.m_properties.m_x_resolution+map.m_properties.m_y_resolution)/2)/2;
        float planeLevel = map.m_properties.m_plane_levels[planelevelIndex];

        float maxDOP = 2;
        for (int i=0; i<map.m_properties.m_num_cols; i++)
        {
            float x = map.m_properties.X(i);
            for (int j=0; j<map.m_properties.m_num_rows; j++)
            {
                // int satCount = 0; 
                float dop = map.m_items[planelevelIndex][i][j].getHorizontal();
                if (dop > maxDOP) maxDOP = dop;
                // if (satCount>0)
                {
                    float y = map.m_properties.Y(j);
                    if (dop >= 0)
                    {
                        float c = dop/m_maxDOP;
                        glColor3f(c,1-c,0);
                    }
                    else
                    {
                        glColor3f(0,0.5,0.5);
                    }

                    glVertex3f(x-halfTileSize,y-halfTileSize,planeLevel);
                    glVertex3f(x-halfTileSize,y+halfTileSize,planeLevel);
                    glVertex3f(x+halfTileSize,y+halfTileSize,planeLevel);
                    glVertex3f(x+halfTileSize,y-halfTileSize,planeLevel);
                }
            }
        }
        // float gain = 0.5;
        // if (maxDOP>2) maxDOP=2;
        // m_maxDOP = maxDOP*gain+(1-gain)*m_maxDOP;
        glEnd();
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }


    void GLWidget::paintGL()
    {
        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureStart();
        #endif
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_app.m_camera->setView();
        

        for(std::shared_ptr<RenderObj>& renderObj : m_renderObjects)
        {
            renderObj->paint();
        }

        paintAxes(200);

        double now = now_seconds();

        if (m_mapper->m_sats.getNumSats()>0)
        {
            paintSats(m_mapper->m_sats, 1000);

            if (m_computeMapsEnabled)
            {
                actionComputeDOPMap();
                m_computeMapsEnabled = false;                
            }

            for (int k=0; k<m_mapProperties.m_plane_levels.size(); k++)
            {
                paintMapProperties(m_mapProperties, k);
                // paintVisibilityMap(m_mapper->m_visibilityMap, k);
                paintDOPMap(m_mapper->m_dopMap, k);
            }

            checkGlError();
        }

        #ifdef BENCHMARK
        if(m_benchmark.get())(*m_benchmark.get())[__PRETTY_FUNCTION__].measureEnd();
        #endif
    }

    void GLWidget::paintAxes(double length)
    {
        glBegin(GL_LINES);
            glColor3f(1,0,0);
            glVertex3f(0,0,0);
            glVertex3f(length,0,0);
            glColor3f(0,1,0);
            glVertex3f(0,0,0);
            glVertex3f(0,length,0);
            glColor3f(0,0,1);
            glVertex3f(0,0,0);
            glVertex3f(0,0,length);
        glEnd();
    }

    void GLWidget::checkGlError()
    {
        int err = glGetError();
        if (err != GL_NO_ERROR)
        {
            cout << "OpenGL Error Number: "  << err << endl;
            switch(err)
            {
                case GL_NO_ERROR:
                    cout << "GL_NO_ERROR" << endl;
                    break;
                case GL_INVALID_ENUM:
                    cout << "GL_INVALID_ENUM" << endl;
                    break;
                case GL_INVALID_VALUE:
                    cout << "GL_INVALID_VALUE" << endl;
                    break;
                case GL_INVALID_OPERATION:
                    cout << "GL_INVALID_OPERATION" << endl;
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl;
                    break;
                case GL_OUT_OF_MEMORY:
                    cout << "GL_OUT_OF_MEMORY" << endl;
                    break;
                case GL_STACK_UNDERFLOW:
                    cout << "GL_STACK_UNDERFLOW" << endl;
                    break;
                case GL_STACK_OVERFLOW:
                    cout << "GL_STACK_OVERFLOW" << endl;
                    break;
            }
        }
    }

    void GLWidget::resizeGL(int width, int height)
    {
        glViewport(0, 0, width, height);
        m_app.m_camera->setProjection(width, height);
    }

    void GLWidget::mousePressEvent(QMouseEvent* event)
    {
        m_app.m_camera->mousePressEvent(event);
    }

    void GLWidget::mouseReleaseEvent(QMouseEvent* event)
    {
        m_app.m_camera->mouseReleaseEvent(event);
    }

    void GLWidget::mouseMoveEvent(QMouseEvent* event)
    {
        m_app.m_camera->mouseMoveEvent(event);
        update();
    }

    void GLWidget::wheelEvent(QWheelEvent* event)
    {
        m_app.m_camera->wheelEvent(event);
    }

    void GLWidget::keyPressEvent(QKeyEvent* event)
    {
        m_app.m_keyboard->keyPressEvent(event);
    }

    void GLWidget::keyReleaseEvent(QKeyEvent* event)
    {
        m_app.m_keyboard->keyReleaseEvent(event);
    }


} // namespace gui
} // namespace gnssShadowing
