#include "mainwindows.h"

#include "utilsDialogs/aboutdialog.h"

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

void MainWindow::editItem(QString const& itemRef, QString const& itemType) {
	//TODO: implement
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
