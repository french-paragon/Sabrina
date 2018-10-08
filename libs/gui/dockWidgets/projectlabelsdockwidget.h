#ifndef SABRINA_PROJECTLABELSDOCKWIDGET_H
#define SABRINA_PROJECTLABELSDOCKWIDGET_H

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

namespace Aline {
	class LabelSelectionForItemProxyModel;
	class EditableItemManager;
}

namespace Sabrina {

namespace Ui {
class ProjectLabelsDockWidget;
}

class MainWindow;
class EditableItemManager;

class CATHIA_GUI_EXPORT ProjectLabelsDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit ProjectLabelsDockWidget(MainWindow *parent = 0);
	~ProjectLabelsDockWidget();

private:

	void projectChanged(Aline::EditableItemManager* project);

	void labelAdditionButtonClicked();
	void labelDeletionButtonClicked();

	void buildTreeContextMenu(QPoint const& pos);

	void selectionChanged();

	Ui::ProjectLabelsDockWidget *ui;

	Aline::EditableItemManager* _currentProject;
	Aline::LabelSelectionForItemProxyModel* _proxy;

	MainWindow* _mw_parent;
};


} // namespace Sabrina
#endif // SABRINA_PROJECTLABELSDOCKWIDGET_H
