#include "app.h"

#include <QMainWindow>
#include "gui/mainwindows.h"

#include <QMenu>
#include <QMenuBar>

#include "model/editableitemmanager.h"

namespace Sabrina {

App::App(int &argc, char **argv) :
	Aline::App(argc, argv),
	_mainWindow(nullptr),
	_project(nullptr)
{
	connect(this, &QApplication::aboutToQuit, this, &App::quitCathia);
}

App::~App() {

	if (_mainWindow != nullptr) {
		delete _mainWindow;
	}

	if (_project != nullptr) {
		delete _project;
	}

}

bool App::start(QString appCode) {
	int code = Aline::App::start(appCode);

	buildMainWindow();

	_mainWindow->show();

	return code;
}

void App::buildMainWindow() {

	if (_mainWindow != nullptr) {
		return;
	}

	_mainWindow = new MainWindow();
	_mainWindow->setMenuBar(_mainWindow->menuBar());

	QMenu* fileMenu = _mainWindow->menuBar()->addMenu(tr("fichier"));

	QAction* openProjectAction = new QAction(tr("ouvrir un projet"), fileMenu);
	connect(openProjectAction, &QAction::triggered, this, &App::openFileProject);
	fileMenu->addAction(openProjectAction);

	fileMenu->addSeparator();

	QAction* closeProjectAction = new QAction(tr("fermer le projet"), fileMenu);
	connect(closeProjectAction, &QAction::triggered, this, &App::closeProject);
	fileMenu->addAction(closeProjectAction);

	QAction* exitAction = new QAction(tr("fermer le programme"), fileMenu);
	connect(exitAction, &QAction::triggered, this, &App::quit);
	fileMenu->addAction(exitAction);

}

void App::openFileProject() {
	//TODO implement
}

void App::closeProject() {

	if (_project != nullptr) {
		_project->closeAll();
	}

	_project->deleteLater();

	_project = nullptr;
}

void App::quitCathia() {

	if (_project != nullptr) {
		_project->closeAll();
	}

	QApplication::exit();
}

} // namespace Cathia
