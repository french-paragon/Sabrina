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

#include "noteseditwidget.h"
#include "ui_noteseditwidget.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>

namespace Sabrina {

NotesEditWidget::NotesEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::NotesEditWidget),
	_currentList(nullptr)
{
	ui->setupUi(this);

	_proxy = new QSortFilterProxyModel(this);

	QQmlContext* ctx = ui->quickWidget->engine()->rootContext();
	ctx->setContextProperty("notesModel", _proxy);

	ui->quickWidget->setSource(QUrl("qrc:/qml/qml/noteseditorwidget.qml"));

	connect(ui->addButton, &QPushButton::clicked,
			this, &NotesEditWidget::addItemToModel);

	connect(ui->removeButton, &QPushButton::clicked,
			this, &NotesEditWidget::removeCurrentItemFromModel);
}

void NotesEditWidget::setCurrentNoteList(NotesList* list) {

	_currentList = list;
	_proxy->setSourceModel(list);

}

NotesEditWidget::~NotesEditWidget()
{
	delete ui;
}

void NotesEditWidget::addItemToModel() {
	if (_currentList != nullptr) {
		_currentList->insertRow(_currentList->rowCount());
	}
}
void NotesEditWidget::removeCurrentItemFromModel() {

	if (_currentList == nullptr) {
		return;
	}

	QQuickItem* list = ui->quickWidget->rootObject();

	if (list == nullptr) {
		return;
	}

	QVariant currentItem = list->property("currentIndex");

	if (currentItem.isValid()) {
		QModelIndex viewIndex = _proxy->index(currentItem.toInt(), 0);
		QModelIndex listIndex = _proxy->mapToSource(viewIndex);

		if (listIndex.isValid()) {
			_currentList->removeRow(listIndex.row());
		}
	}

}

} // namespace Sabrina
