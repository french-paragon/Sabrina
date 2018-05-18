#include "mainwindows.h"

#include "utilsDialogs/aboutdialog.h"

#include "model/editableitem.h"

#include "aline/src/editorfactorymanager.h"
#include "aline/src/editableitemeditor.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QDockWidget>

namespace Sabrina {


const QString MainWindow::GEOMETRY_CONFIG_KEY = "geometry";
const QString MainWindow::STATE_CONFIG_KEY = "windowState";

const QString MainWindow::MENU_FILE_NAME = "file_menu";
const QString MainWindow::MENU_DISPLAY_NAME = "display_menu";


MainWindow::MainWindow(QWidget* parent) :
	Aline::MainWindow(parent),
	_currentProject(nullptr)
{
	resize(800, 600);

	//menu

	QMenu* fileMenu = menuBar()->addMenu(tr("fichier"));
	fileMenu->setObjectName(MENU_FILE_NAME);

	QMenu* viewMenu = menuBar()->addMenu(tr("affichage"));
	viewMenu->setObjectName(MENU_DISPLAY_NAME);

	_submenuDock = viewMenu->addMenu(tr("docks"));
}

void MainWindow::displayAboutWindows() {

	AboutDialog dialog(this);
	dialog.setModal(true);
	dialog.exec();

}

void MainWindow::editItem(QString const& itemRef) {

	if (_openedEditors.contains(itemRef)) {
		switchToEditor(_openedEditors.value(itemRef));
		return;
	}

	EditableItem* item = _currentProject->loadItem(itemRef);

	Aline::Editor* editor = Aline::EditorFactoryManager::GlobalEditorFactoryManager.createItemForEditableItem(item, this);

	if (editor != nullptr) {
		addEditor(editor);
	}

	_openedEditors.insert(itemRef, editor);

}
void MainWindow::saveAll() {

	if (_currentProject != nullptr) {
		_currentProject->saveAll();
	}

}

EditableItemManager *MainWindow::currentProject() const
{
    return _currentProject;
}

void MainWindow::setCurrentProject(EditableItemManager *currentProject)
{
	if (currentProject != _currentProject) {
		_currentProject = currentProject;
		emit currentProjectChanged(_currentProject);
	}
}

QMenu* MainWindow::findMenuByName(QString const& name, bool createIfNotExist) {

	QMenu* menu = menuBar()->findChild<QMenu*>(name, Qt::FindDirectChildrenOnly);

	if (menu != nullptr) {
		return menu;
	}

	if (createIfNotExist) {
		return menuBar()->addMenu(name);
	}

	return nullptr;

}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockwidget) {

	_submenuDock->addAction(dockwidget->toggleViewAction());

	QMainWindow::addDockWidget(area, dockwidget);

}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockwidget, Qt::Orientation orientation) {

	_submenuDock->addAction(dockwidget->toggleViewAction());

	QMainWindow::addDockWidget(area, dockwidget, orientation);

}

} // namespace Cathia
