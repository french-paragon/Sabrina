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

#include "projectnotesdockwidget.h"
#include "ui_projectnotesdockwidget.h"

#include "mainwindows.h"
#include "model/editableitemmanager.h"

namespace Sabrina {

ProjectNotesDockWidget::ProjectNotesDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	_mw_parent(parent),
	ui(new Ui::ProjectNotesDockWidget)
{
	ui->setupUi(this);

	projectChanged(_mw_parent->currentProject());

	connect(_mw_parent, &MainWindow::currentProjectChanged,
			this, &ProjectNotesDockWidget::projectChanged);
}

ProjectNotesDockWidget::~ProjectNotesDockWidget()
{
	delete ui;
}


void ProjectNotesDockWidget::projectChanged(Aline::EditableItemManager* aproject) {

	EditableItemManager* project = qobject_cast<EditableItemManager*>(aproject);

	if (_projectDeletedConnection) {
		disconnect(_projectDeletedConnection);
	}

	if (project != nullptr) {

		ui->notesWidgets->setCurrentNoteList(project->noteList());

		_projectDeletedConnection = connect(project, &QObject::destroyed,
											this, &ProjectNotesDockWidget::projectCleared);
	} else {
		ui->notesWidgets->setCurrentNoteList(nullptr);
	}

}

void ProjectNotesDockWidget::projectCleared() {

	if (_projectDeletedConnection) {
		disconnect(_projectDeletedConnection);
	}

	ui->notesWidgets->setCurrentNoteList(nullptr);

}

} // namespace Sabrina
