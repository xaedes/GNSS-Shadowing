#pragma once

#include <QApplication>
#include <memory>


namespace gnssShadowing {
namespace gui {

	// forward declarations
	class MainWindow;
	class Keyboard;
	class KeyboardControl;
	class Camera;
	
	/// \brief      Holds all the gui application data.
	class App
	{
	public:
		App();
		~App();
		int run(int argc, char* argv[]);

		std::unique_ptr<QApplication> m_qApp;
		std::unique_ptr<MainWindow> m_mainWindow;
		std::unique_ptr<Camera> m_camera;
		std::unique_ptr<Keyboard> m_keyboard;
		std::unique_ptr<KeyboardControl> m_keyboardControl;

	protected:
	};

} // namespace gui
} // namespace gnssShadowing
