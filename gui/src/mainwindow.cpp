#include "mainwindow.h"
#include "ui_mainwindow.h"

// #include "app.h"
#include "glwidget.h"
#include <QAction>

namespace gnssShadowing {
namespace gui {

	MainWindow::MainWindow(App& app, QWidget *parent) :
		QMainWindow(parent),
		m_app(app),
		m_ui(new Ui::MainWindow),
		m_glWidget(new GLWidget(m_app))
	{
		m_ui->setupUi(this);
		//m_glWidget = new GLWidget(m_app);
		m_ui->container->addWidget(m_glWidget.get());
		// m_ui->actionBenchmarkResults->
		connect(m_ui->actionBenchmarkResults, SIGNAL(triggered()), m_glWidget.get(), SLOT(actionBenchmarkResults()));
		connect(m_ui->actionComputeDOPMap, SIGNAL(triggered()), m_glWidget.get(), SLOT(actionComputeDOPMap()));
	}

	MainWindow::~MainWindow()
	{
	}

} // namespace gui
} // namespace gnssShadowing
