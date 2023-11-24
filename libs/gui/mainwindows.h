#ifndef CATHIA_MAINWINDOWS_H
#define CATHIA_MAINWINDOWS_H

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

#include "gui_global.h"

#include <Aline/view/mainwindow.h>

#include "model/editableitemmanager.h"

namespace Sabrina {

class CATHIA_GUI_EXPORT MainWindow : public Aline::MainWindow
{
	Q_OBJECT
public:

	static const QString GEOMETRY_CONFIG_KEY;
	static const QString STATE_CONFIG_KEY;

	explicit MainWindow(QWidget* parent = nullptr);

public slots:

	void displayAboutWindows();

protected:

	void onEditorAboutToBeRemoved(Aline::Editor* editor);

};

} // namespace Cathia

#endif // MAINWINDOWS_H
