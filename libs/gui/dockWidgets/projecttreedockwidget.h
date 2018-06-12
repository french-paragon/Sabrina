#ifndef PROJECTTREEDOCKWIDGET_H
#define PROJECTTREEDOCKWIDGET_H

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

#include "gui/gui_global.h"
#include <QDockWidget>

#include "gui/mainwindows.h"
#include "model/editableitemmanager.h"
#include <QSortFilterProxyModel>

class QMenu;

namespace Sabrina {

namespace Ui {
class ProjectTreeDockWidget;
}

class CATHIA_GUI_EXPORT ProjectTreeDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit ProjectTreeDockWidget(MainWindow *parent = 0);
	~ProjectTreeDockWidget();

signals:

	void itemCreationTriggered(QString itemTypeRef, QString ref, QString parent_ref);
	void itemDoubleClicked(QString itemRef);
	void itemSuppressionTriggered(QStringList itemRef);

private:

	void selectionChanged();

	void onItemCreationTriggered(QString itemTypeRef);

	void projectChanged(EditableItemManager* project);

	void reselectProject(EditableItemManager* project);
	void rebuildMenuWithoutProject();
	void rebuildMenu(EditableItemManager *project);

	void receiveDoubleClick(const QModelIndex &index);
	void supprButtonClicked();

	void buildTreeContextMenu(QPoint const& pos);

	MainWindow * _mw_parent;

	QMenu* _newItemMenu;
	QSortFilterProxyModel* _internalModel;

	Ui::ProjectTreeDockWidget *ui;

	QMetaObject::Connection _newItemFactoryWatcher;
	QMetaObject::Connection _itemCreationTrigger;
	QMetaObject::Connection _itemSuppresionTrigger;
};

} //namespace Sabrina

#endif // PROJECTTREEDOCKWIDGET_H
