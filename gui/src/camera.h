#pragma once


#include <QMouseEvent>
#include <QWheelEvent>

namespace gnssShadowing {
namespace gui {

	class Keyboard;

	class Camera
	{
	public:
		Camera(Keyboard& keyboard, float x=0, float y=0, float zoom=1);
		~Camera();
		void setProjection(int display_width, int display_height);
		void setView();

		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void wheelEvent(QWheelEvent *event);
	protected:
		Keyboard& m_keyboard;
		float m_x;
		float m_y;
		float m_zoom;

		int m_last_x;
		int m_last_y;

		float m_rotationGain;
		float m_dx_filtered;
		float m_dy_filtered;

		// TODO: convert into sat::Horizontal

		float m_yaw;  // in degree
		float m_pitch;// in degree

		bool m_dragging;
	};

} // namespace gui
} // namespace gnssShadowing
