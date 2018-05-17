#include "app.h"

#include <QMainWindow>

#include "gui/mainwindows.h"
#include "gui/dockWidgets/projecttreedockwidget.h"
#include "gui/dockWidgets/projectlabelsdockwidget.h"

#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QToolBar>

#include <QUrl>

#include "model/editableitemmanager.h"
#include "model/editableItems/personnage.h"
#include "model/editableItems/place.h"
#include "model/editableItems/folder.h"

#include "model/editableItemsManagers/jsoneditableitemmanager.h"

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

	loadEditableFactories();
	loadEditorsFactories();

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

	QAction* openProjectAction = new QAction(QIcon(":/icons/icons/editable_item_folder.svg"), tr("ouvrir un projet"), _mainWindow);
	openProjectAction->setShortcut(QKeySequence::Open);
	connect(openProjectAction, &QAction::triggered, this, &App::openFileProject);
	fileMenu->addAction(openProjectAction);

	QAction* createProjectAction = new QAction(QIcon(":/icons/icons/new_simple.svg"), tr("créer un projet"), _mainWindow);
	createProjectAction->setShortcut(QKeySequence::New);
	connect(createProjectAction, &QAction::triggered, this, &App::createFileProject);
	fileMenu->addAction(createProjectAction);

	fileMenu->addSeparator();

	QAction* saveAction = new QAction(QIcon(":/icons/icons/save_simple.svg"), tr("sauver le projet"), _mainWindow);
	saveAction->setShortcut(QKeySequence::Save);
	connect(saveAction, &QAction::triggered, _mainWindow, &MainWindow::saveAll);

	fileMenu->addAction(saveAction);

	fileMenu->addSeparator();

	QAction* closeProjectAction = new QAction(tr("fermer le projet"), fileMenu);
	connect(closeProjectAction, &QAction::triggered, this, &App::closeProject);
	fileMenu->addAction(closeProjectAction);

	QAction* exitAction = new QAction(tr("fermer le programme"), fileMenu);
	connect(exitAction, &QAction::triggered, this, &App::quit);
	fileMenu->addAction(exitAction);

	//tool bar

	QToolBar* mainToolBar = new QToolBar(_mainWindow);

	mainToolBar->addAction(createProjectAction);
	mainToolBar->addAction(openProjectAction);
	mainToolBar->addAction(saveAction);

	_mainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);

	//Dock widgets.

	ProjectTreeDockWidget* project_dock = new ProjectTreeDockWidget(_mainWindow);
	_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, project_dock);

	connect(project_dock, &ProjectTreeDockWidget::itemDoubleClicked,
			_mainWindow, &MainWindow::editItem);

	ProjectLabelsDockWidget* labels_dock = new ProjectLabelsDockWidget(_mainWindow);
	_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, labels_dock);

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

	QString projectFile = QFileDialog::getOpenFileName(
				_mainWindow,
				tr("Ouvrir un projet Sabrina."),
				QDir::homePath(),
				QString("*") + JsonEditableItemManager::PROJECT_FILE_EXT);

	if (projectFile == "") {
		return;
	}

	JsonEditableItemManager* project = new JsonEditableItemManager(this);

	project->connectProject(projectFile);

	if (!project->hasDataSource()) {
		delete project;
		return;
	}

	closeProject(); //ensure previous project is closed.
	//TODO: change the architecture so that we can have multiple projects open at once.

	_project = project;

	_mainWindow->setCurrentProject(_project);

}

void App::createFileProject() {

	QString projectFile = QFileDialog::getSaveFileName(
				_mainWindow,
				tr("Créer un projet Sabrina."),
				QDir::homePath(),
				QString("*") + JsonEditableItemManager::PROJECT_FILE_EXT);

	if (projectFile == "") {
		return;
	}

	if (!projectFile.endsWith(JsonEditableItemManager::PROJECT_FILE_EXT)) {
		projectFile += JsonEditableItemManager::PROJECT_FILE_EXT;
	}

	JsonEditableItemManager* project = new JsonEditableItemManager(this);

	project->connectProject(projectFile);

	if (!project->hasDataSource()) {
		delete project;
		return;
	}

	closeProject(); //ensure previous project is closed.
	//TODO: change the architecture so that we can have multiple projects open at once.

	_project = project;

	_mainWindow->setCurrentProject(_project);

}

void App::closeProject() {

	if (_project != nullptr) {
		_project->closeAll();
	}

	_project->deleteLater();

	_project = nullptr;

	_mainWindow->setCurrentProject(nullptr);
}

void App::quitCathia() {

	if (_project != nullptr) {
		_project->closeAll();
	}

	QApplication::exit();
}

} // namespace Cathia
