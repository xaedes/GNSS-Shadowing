
#include "camera.h"
#include "keyboard.h"
#include "common/math.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>


namespace gnssShadowing {
namespace gui {

	Camera::Camera(Keyboard& keyboard, float x, float y, float zoom) :
		m_keyboard(keyboard),
		m_x(x),
		m_y(y),
		m_zoom(zoom),
		m_last_x(0),
		m_last_y(0),
		m_dragging(false),
		m_pitch(-60),
		m_yaw(180),
		m_rotationGain(0.1)
	{}

	Camera::~Camera()
	{}

	void Camera::setProjection(int display_width, int display_height)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		int x_min, x_max, y_min, y_max;
		
		x_min = 0 -(display_width/2);
		y_min = 0 -(display_height/2);
		x_max = x_min + display_width;
		y_max = y_min + display_height;

		glOrtho(x_min,x_max,y_min,y_max,-1280,1280);
	}

	void Camera::setView()
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
        glScalef(m_zoom, m_zoom, 1);
        glTranslatef(-m_x,-m_y,0);
        glRotatef(m_pitch,1,0,0);
        glRotatef(m_yaw,0,0,1);
	}

	void Camera::mousePressEvent(QMouseEvent *event)
	{
		Qt::MouseButtons buttons = event->buttons();
		m_last_x = event->x();
		m_last_y = event->y();
		m_dragging = true;
		m_dx_filtered = m_dy_filtered = 0;
	}
	
	void Camera::mouseReleaseEvent(QMouseEvent *event)
	{
		m_dragging = false;
	}
	
	void Camera::mouseMoveEvent(QMouseEvent *event)
	{
		if (m_dragging)
		{
			Qt::MouseButtons buttons = event->buttons();
			int dx = event->x() - m_last_x;
			int dy = event->y() - m_last_y;
			if (buttons & Qt::LeftButton)
			{
				m_x -= dx / m_zoom;
				m_y -= (-dy) / m_zoom;  // -dy because image y-axis is inverse to world y-axis
			}
			else if (buttons & Qt::RightButton)
			{
				m_dx_filtered = dx*m_rotationGain+(1-m_rotationGain)*m_dx_filtered;
				m_dy_filtered = dy*m_rotationGain+(1-m_rotationGain)*m_dy_filtered;
				m_yaw   += m_dx_filtered;
				m_pitch += m_dy_filtered;
			}
			m_last_x = event->x();
			m_last_y = event->y();			
		}
	}

	void Camera::wheelEvent(QWheelEvent *event)
	{

		// QWheelEvent.angleDelta().y() Returns the distance that the wheel is rotated, in eighths of a degree.

        float deltaDeg = ((float)event->angleDelta().y())/8.;
		//float z_per_deg = 1 / 15.
		if (deltaDeg > 0)
		{
			m_zoom *= 1.1;
		}
		else if (deltaDeg < 0)
		{
			m_zoom *= 0.9;
		}

	}
	

} // namespace gui
} // namespace gnssShadowing
