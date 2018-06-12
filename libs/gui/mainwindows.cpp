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

#include "mainwindows.h"

#include "utilsDialogs/aboutdialog.h"

#include "model/editableitem.h"

#include "aline/src/view/editorfactorymanager.h"
#include "aline/src/view/editableitemeditor.h"

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

	connect(this, &Aline::MainWindow::editorAboutToBeRemoved,
			this, &MainWindow::onEditorAboutToBeRemoved);
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

	Aline::EditableItem* item = _currentProject->loadItem(itemRef);

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

void MainWindow::onEditorAboutToBeRemoved(Aline::Editor* editor) {

	if (_openedEditors.values().contains(editor)) {

		QString key = _openedEditors.key(editor);

		_openedEditors.remove(key);

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
