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

#include "mainwindowsfactory.h"

#include "mainwindows.h"

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
