#include "app.h"

#include <QMainWindow>

#include "gui/mainwindows.h"
#include "gui/dockWidgets/projecttreedockwidget.h"

#include <QMenu>
#include <QMenuBar>

#include "model/editableitemmanager.h"
#include "model/editableItems/personnage.h"
#include "model/editableItems/place.h"
#include "model/editableItems/folder.h"

#include "gui/editors/personnageeditor.h"
#include "gui/editors/placeeditor.h"

#include <aline/src/editor.h>
#include <aline/src/editorfactory.h>
#include <aline/src/editorfactorymanager.h>

namespace Sabrina {

App::App(int &argc, char **argv) :
	Aline::App(argc, argv),
	_mainWindow(nullptr),
	_project(nullptr)
{
	QCoreApplication::setOrganizationName(ORG_NAME);
	QCoreApplication::setOrganizationDomain(ORG_DOMAIN);
	QCoreApplication::setApplicationName(APP_NAME);

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

	//menu

	QMenu* fileMenu = _mainWindow->menuBar()->addMenu(tr("fichier"));

	QAction* openProjectAction = new QAction(tr("ouvrir un projet"), fileMenu);
	connect(openProjectAction, &QAction::triggered, this, &App::openFileProject);
	fileMenu->addAction(openProjectAction);

	QAction* createProjectAction = new QAction(tr("créer un projet"), fileMenu);
	connect(createProjectAction, &QAction::triggered, this, &App::createFileProject);
	fileMenu->addAction(createProjectAction);

	fileMenu->addSeparator();

	QAction* closeProjectAction = new QAction(tr("fermer le projet"), fileMenu);
	connect(closeProjectAction, &QAction::triggered, this, &App::closeProject);
	fileMenu->addAction(closeProjectAction);

	QAction* exitAction = new QAction(tr("fermer le programme"), fileMenu);
	connect(exitAction, &QAction::triggered, this, &App::quit);
	fileMenu->addAction(exitAction);

	//Dock widgets.

	ProjectTreeDockWidget* project_dock = new ProjectTreeDockWidget(_mainWindow);
	_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, project_dock);

}

void App::loadEditorsFactories() {

	Aline::EditorFactoryManager::GlobalEditorFactoryManager.installFactory(new PersonnageEditor::PersonnageEditorFactory());
	Aline::EditorFactoryManager::GlobalEditorFactoryManager.installFactory(new PlaceEditor::PlaceEditorFactory());

}

void App::loadEditableFactories() {

	EditableItemFactoryManager::GlobalEditableItemFactoryManager.installFactory(new Personnage::PersonnageFactory());
	EditableItemFactoryManager::GlobalEditableItemFactoryManager.installFactory(new Place::PlaceFactory());
	EditableItemFactoryManager::GlobalEditableItemFactoryManager.installFactory(new Folder::FolderFactory());

}

void App::openFileProject() {
	//TODO implement
}

void App::createFileProject() {

	closeProject(); //ensure previous project is closed.
	//TODO: change the architecture so that we can have multiple projects open at once.



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
