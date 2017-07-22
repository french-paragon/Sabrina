#include "mainwindows.h"

#include "utilsDialogs/aboutdialog.h"

namespace Cathia {

MainWindow::MainWindow(QWidget* parent) :
	Aline::MainWindow(parent)
{
	resize(800, 600);
}

void MainWindow::displayAboutWindows() {

	AboutDialog dialog(this);
	dialog.setModal(true);
	dialog.exec();

}

} // namespace Cathia
