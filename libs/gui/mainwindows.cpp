/*
This file is part of the project Sabrina
Copyright (C) 2018-2023  Paragon <french.paragon@gmail.com>

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

#include <Aline/view/editorfactorymanager.h>
#include <Aline/view/editableitemeditor.h>

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QDockWidget>

namespace Sabrina {


const QString MainWindow::GEOMETRY_CONFIG_KEY = "geometry";
const QString MainWindow::STATE_CONFIG_KEY = "windowState";


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
