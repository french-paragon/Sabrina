#ifndef SABRINA_LABELITEMSDOCKWIDGET_H
#define SABRINA_LABELITEMSDOCKWIDGET_H

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

#include "gui/gui_global.h"

namespace Sabrina {

class MainWindow;
class ProjectFilterByLabelProxyModel;
class EditableItemManager;

namespace Ui {
class LabelItemsDockWidget;
}

class CATHIA_GUI_EXPORT LabelItemsDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit LabelItemsDockWidget(MainWindow *parent = 0);
	~LabelItemsDockWidget();

signals:

	void itemDoubleClicked(QString itemRef);

private:

	void selectionChanged();

	void projectChanged(EditableItemManager* project);

	void receiveDoubleClick(const QModelIndex &index);

	MainWindow * _mw_parent;

	Ui::LabelItemsDockWidget *ui;

	ProjectFilterByLabelProxyModel* _proxy;

	EditableItemManager* _currentProject;
};


} // namespace Sabrina
#endif // SABRINA_LABELITEMSDOCKWIDGET_H
