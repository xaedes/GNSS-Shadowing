
#pragma once


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <QOpenGLWidget>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimerEvent>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "common/measureTime.h"

#include "obj/objFile.h"
#include "obj/mtllib.h"
#include "renderObject.h"

#include "sat/coordinateSystems.h"
#include "sat/satStore.h"
#include "sat/availableSats.h"
#include "shadowing/shadowing.h"
#include "mapping/visibilityMap.h"
#include "mapping/dopMap.h"
#include "mapping/mapProperties.h"
#include "mapping/mapper.h"
#include "world/world.h"

namespace gnssShadowing {
namespace gui {

	// forward declaration
	class App;
    
	class GLWidget : public QOpenGLWidget
	{
		Q_OBJECT

	public:
		GLWidget(App& app, QWidget *parent = 0);
		~GLWidget();

		QSize minimumSizeHint() const Q_DECL_OVERRIDE;
		QSize sizeHint() const Q_DECL_OVERRIDE;
	public slots:
		void cleanup();
        void actionBenchmarkResults();
        void actionComputeDOPMap();
	protected:
		App& m_app;
        
        std::shared_ptr<common::Benchmark> m_benchmark;

        world::World m_world;

        double m_timeStart;

        float m_maxDOP;


        mapping::MapProperties m_mapProperties;
        std::shared_ptr<mapping::Mapper> m_mapper;

        bool m_computeMapsEnabled;

		std::vector<std::shared_ptr<RenderObj> > m_renderObjects;

		void initializeGL() Q_DECL_OVERRIDE;
		void paintGL() Q_DECL_OVERRIDE;	
		void resizeGL(int width, int height) Q_DECL_OVERRIDE;

        void paintAxes(double length);
        void paintSats(sat::AvailableSats& sats, double length);
        void paintMapProperties(mapping::MapProperties mapProperties, int planelevelIndex);
        void paintVisibilityMap(mapping::VisibilityMap& map, int planelevelIndex);
        void paintDOPMap(mapping::DOPMap& map, int planelevelIndex);
        void initObjects();

        void checkGlError();

		virtual void timerEvent(QTimerEvent *event);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void wheelEvent(QWheelEvent *event);
		virtual void keyPressEvent(QKeyEvent *event);
		virtual void keyReleaseEvent(QKeyEvent *event);
	};

} // namespace gui
} // namespace gnssShadowing
