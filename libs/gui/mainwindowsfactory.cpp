#include "mainwindowsfactory.h"

#include "mainwindows.h"

#include "dockWidgets/projecttreedockwidget.h"
#include "dockWidgets/projectlabelsdockwidget.h"

#include "utils/app_info.h"

namespace Sabrina {


MainWindowsFactory MainWindowsFactory::GlobalMainWindowsFactory;

MainWindowsFactory::MainWindowsFactory(QObject *parent) : QObject(parent)
{

}

MainWindow* MainWindowsFactory::factorizeMainWindows() const {

	MainWindow* mw = new MainWindow();
	mw->setWindowTitle(APP_NAME);
	mw->setMenuBar(mw->menuBar());

	//preActions

	for(Action const& a : _preActions) {
		a(mw);
	}

	//postActions

	for(Action const& a : _postActions) {
		a(mw);
	}

	return mw;

}

void MainWindowsFactory::registerPreAction(Action const& a) {
	_preActions.append(a);
}

void MainWindowsFactory::registerPostAction(Action const& a) {
	_postActions.append(a);
}

} //namespace Sabrina
