/*
This file is part of the project Sabrina
Copyright (C) 2018  Paragon <french.paragon@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "app.h"

#include <QMainWindow>

#include "gui/mainwindows.h"
#include "gui/mainwindowsfactory.h"
#include "gui/dockWidgets/projecttreedockwidget.h"
#include "gui/dockWidgets/projectlabelsdockwidget.h"
#include "gui/dockWidgets/labelitemsdockwidget.h"
#include "gui/dockWidgets/projectnotesdockwidget.h"

#include <QUrl>

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>

#include <QDebug>
#include <QSettings>
#include <QByteArray>

#include "model/editableitemmanager.h"
#include "model/editableItems/personnage.h"
#include "model/editableItems/place.h"
#include "model/editableItems/folder.h"
#include "model/editableItems/cartography.h"

#include "model/editableItemsManagers/jsoneditableitemmanager.h"

#include "gui/editors/personnageeditor.h"
#include "gui/editors/placeeditor.h"
#include "gui/editors/cartographyeditor.h"

#include "gui/utilsDialogs/aboutdialog.h"
#include "gui/utilsDialogs/licensedialog.h"

#include <aline/src/view/editor.h>
#include <aline/src/view/editorfactory.h>
#include <aline/src/view/editorfactorymanager.h>

#include "utils/app_info.h"

namespace Sabrina {

App::App(int &argc, char **argv) :
	Aline::App(argc, argv),
	_mainWindow(nullptr),
	_project(nullptr)
{
	QCoreApplication::setOrganizationName(ORG_NAME);
	QCoreApplication::setOrganizationDomain(ORG_DOMAIN);
	QCoreApplication::setApplicationName(APP_NAME);

	QCoreApplication::setApplicationVersion(appTag());

	connect(this, &QApplication::aboutToQuit, this, &App::quitCathia);

	setWindowIcon(QIcon(":/icons/icons/app_icon.svg"));
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

	MainWindowsFactory::GlobalMainWindowsFactory.registerPreAction([this] (MainWindow* mw) {this->addAppActionsToMainWindows(mw);});

	loadEditableFactories();
	loadEditorsFactories();

	MainWindowsFactory::GlobalMainWindowsFactory.registerPostAction([this] (MainWindow* mw) {this->addAboutActionsToMainWindows(mw);});
	MainWindowsFactory::GlobalMainWindowsFactory.registerPostAction([] (MainWindow* mw) {

		QSettings settings;
		QVariant geometry = settings.value(MainWindow::GEOMETRY_CONFIG_KEY);
		QVariant state = settings.value(MainWindow::STATE_CONFIG_KEY);

		if (geometry.isValid() && geometry.canConvert(qMetaTypeId<QByteArray>())) {
			mw->restoreGeometry(geometry.toByteArray());
		}

		if (state.isValid() && state.canConvert(qMetaTypeId<QByteArray>())) {
			mw->restoreState(state.toByteArray());
		}

	}); //last post action is to restore state.

	buildMainWindow();

	_mainWindow->show();

	return code;
}

void App::openProject(QString const& projectFile) {

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

void App::buildMainWindow() {

	if (_mainWindow != nullptr) {
		return;
	}

	_mainWindow = MainWindowsFactory::GlobalMainWindowsFactory.factorizeMainWindows();

}

void App::addAppActionsToMainWindows(MainWindow* mw) {

	//menu

	QMenu* fileMenu = mw->findMenuByName(MainWindow::MENU_FILE_NAME);

	QAction* openProjectAction = new QAction(QIcon(":/icons/icons/editable_item_folder.svg"), tr("ouvrir un projet"), mw);
	openProjectAction->setShortcut(QKeySequence::Open);
	connect(openProjectAction, &QAction::triggered, this, &App::openFileProject);
	fileMenu->addAction(openProjectAction);

	QAction* createProjectAction = new QAction(QIcon(":/icons/icons/new_simple.svg"), tr("créer un projet"), mw);
	createProjectAction->setShortcut(QKeySequence::New);
	connect(createProjectAction, &QAction::triggered, this, &App::createFileProject);
	fileMenu->addAction(createProjectAction);

	fileMenu->addSeparator();

	QAction* saveAction = new QAction(QIcon(":/icons/icons/save_simple.svg"), tr("sauver le projet"), mw);
	saveAction->setShortcut(QKeySequence::Save);
	connect(saveAction, &QAction::triggered, mw, &MainWindow::saveAll);

	fileMenu->addAction(saveAction);

	fileMenu->addSeparator();

	QAction* closeProjectAction = new QAction(tr("fermer le projet"), fileMenu);
	connect(closeProjectAction, &QAction::triggered, this, &App::closeProject);
	fileMenu->addAction(closeProjectAction);

	QAction* exitAction = new QAction(tr("fermer le programme"), fileMenu);
	connect(exitAction, &QAction::triggered, this, &App::quit);
	fileMenu->addAction(exitAction);

	//tool bar

	QToolBar* mainToolBar = new QToolBar(mw);
	mainToolBar->setObjectName("main_tool_bar");

	mainToolBar->addAction(createProjectAction);
	mainToolBar->addAction(openProjectAction);
	mainToolBar->addAction(saveAction);

	mw->addToolBar(Qt::TopToolBarArea, mainToolBar);

	//Dock widgets.

	ProjectTreeDockWidget* project_dock = new ProjectTreeDockWidget(mw);
	mw->addDockWidget(Qt::LeftDockWidgetArea, project_dock);

	connect(project_dock, &ProjectTreeDockWidget::itemDoubleClicked,
			mw, &MainWindow::editItem);

	ProjectLabelsDockWidget* labels_dock = new ProjectLabelsDockWidget(mw);
	mw->addDockWidget(Qt::LeftDockWidgetArea, labels_dock);

	LabelItemsDockWidget* label_item_dock = new LabelItemsDockWidget(mw);
	mw->addDockWidget(Qt::RightDockWidgetArea, label_item_dock);

	connect(label_item_dock, &LabelItemsDockWidget::itemDoubleClicked,
			mw, &MainWindow::editItem);

	ProjectNotesDockWidget* notes_dock = new ProjectNotesDockWidget(mw);
	mw->addDockWidget(Qt::RightDockWidgetArea, notes_dock);



}

void App::addAboutActionsToMainWindows(MainWindow* mw) {

	//menu

	QMenu* aboutMenu = mw->menuBar()->addMenu(tr("à propos"));

	QAction* openAboutAction = new QAction(tr("à propos de %1").arg(APP_NAME), mw);
	connect(openAboutAction, &QAction::triggered, [mw] () {
		AboutDialog dialog(mw);
		dialog.exec();
	});
	aboutMenu->addAction(openAboutAction);

	QAction* openLicenseDialog = new QAction(tr("license"), mw);

	connect(openLicenseDialog, &QAction::triggered, [mw] () {
		LicenseDialog ld(mw);
		ld.exec();
	});

	aboutMenu->addAction(openLicenseDialog);
}

void App::loadEditorsFactories() {

	Aline::EditorFactoryManager::GlobalEditorFactoryManager.installFactory(new PersonnageEditor::PersonnageEditorFactory());
	Aline::EditorFactoryManager::GlobalEditorFactoryManager.installFactory(new PlaceEditor::PlaceEditorFactory());
	Aline::EditorFactoryManager::GlobalEditorFactoryManager.installFactory(new CartographyEditor::CartographyEditorFactory());

}

void App::loadEditableFactories() {

	Aline::EditableItemFactoryManager::GlobalEditableItemFactoryManager.installFactory(new Personnage::PersonnageFactory());
	Aline::EditableItemFactoryManager::GlobalEditableItemFactoryManager.installFactory(new Place::PlaceFactory());
	Aline::EditableItemFactoryManager::GlobalEditableItemFactoryManager.installFactory(new Folder::FolderFactory());
	Aline::EditableItemFactoryManager::GlobalEditableItemFactoryManager.installFactory(new Cartography::CartographyFactory());

	Aline::EditableItemFactoryManager::GlobalEditableItemFactoryManager.installSubItemFactory(new CartographyItem::CartographyItemFactory());
	Aline::EditableItemFactoryManager::GlobalEditableItemFactoryManager.installSubItemFactory(new CartographyCategory::CartographyCategoryFactory());

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

	openProject(projectFile);

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

	QSettings settings;
	settings.setValue(MainWindow::GEOMETRY_CONFIG_KEY, _mainWindow->saveGeometry());
	settings.setValue(MainWindow::STATE_CONFIG_KEY, _mainWindow->saveState());

	QApplication::exit();
}

} // namespace Cathia
