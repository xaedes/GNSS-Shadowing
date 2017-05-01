#include "app.h"
#include "mainwindow.h"
#include "keyboard.h"
#include "keyboardControl.h"
#include "camera.h"

namespace gnssShadowing {
namespace gui {

	App::App()
	{
	}

	App::~App()
	{
	}

	int App::run(int argc, char* argv[])
	{
		m_keyboard.reset(new Keyboard());
		m_camera.reset(new Camera(*m_keyboard.get()));
		
		m_keyboardControl.reset(new KeyboardControl(*this));

		m_qApp.reset(new QApplication(argc, argv));
		m_mainWindow.reset(new MainWindow(*this));
		m_mainWindow->show();

		return m_qApp->exec();
	}

} // namespace gui
} // namespace gnssShadowing
