#include "mainwindows.h"

#include "utilsDialogs/aboutdialog.h"

#include "model/editableitem.h"

#include "aline/src/editorfactorymanager.h"
#include "aline/src/editableitemeditor.h"

namespace Sabrina {

MainWindow::MainWindow(QWidget* parent) :
	Aline::MainWindow(parent),
	_currentProject(nullptr)
{
	resize(800, 600);
}

void MainWindow::displayAboutWindows() {

	AboutDialog dialog(this);
	dialog.setModal(true);
	dialog.exec();

}

void MainWindow::editItem(QString const& itemRef) {
	//TODO: check if an editor is already there.

	EditableItem* item = _currentProject->loadItem(itemRef);

	Aline::Editor* editor = Aline::EditorFactoryManager::GlobalEditorFactoryManager.createItemForEditableItem(item, this);

	if (editor != nullptr) {
		addEditor(editor);
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

} // namespace Cathia
