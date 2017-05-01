#pragma once
#include <QMainWindow>
#include <memory>



// forward declare Qt generated class
namespace Ui {
class MainWindow;
}

namespace gnssShadowing {
namespace gui {

	// forward declaration
	class App;
	class GLWidget;

	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		explicit MainWindow(App& app, QWidget *parent = 0);
		~MainWindow();

	private:
		App& m_app;
		std::shared_ptr<Ui::MainWindow> m_ui;
		std::shared_ptr<GLWidget> m_glWidget;
	};

} // namespace gui
} // namespace gnssShadowing
