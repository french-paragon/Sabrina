#ifndef SABRINA_PROJECTNOTESDOCKWIDGET_H
#define SABRINA_PROJECTNOTESDOCKWIDGET_H

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

#include <QDockWidget>

#include "../gui_global.h"

namespace Aline {
	class EditableItemManager;
}

namespace Sabrina {

namespace Ui {
class ProjectNotesDockWidget;
}

class MainWindow;
class EditableItemManager;

class CATHIA_GUI_EXPORT ProjectNotesDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit ProjectNotesDockWidget(MainWindow *parent = 0);
	~ProjectNotesDockWidget();

private:

	void projectChanged(Aline::EditableItemManager* project);
	void projectCleared();


	MainWindow * _mw_parent;

	Ui::ProjectNotesDockWidget *ui;

	QMetaObject::Connection _projectDeletedConnection;
};


} // namespace Sabrina
#endif // SABRINA_PROJECTNOTESDOCKWIDGET_H
