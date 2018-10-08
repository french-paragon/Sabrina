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

#include "labelitemsdockwidget.h"
#include "ui_labelitemsdockwidget.h"

#include "mainwindows.h"
#include "aline/src/model/labels/projectfilterbylabelproxymodel.h"

namespace Sabrina {

LabelItemsDockWidget::LabelItemsDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	ui(new Ui::LabelItemsDockWidget),
	_mw_parent(parent)
{
	ui->setupUi(this);

	_proxy = new Aline::ProjectFilterByLabelProxyModel(ui->treeView);

	projectChanged(parent->currentProject());

	if (parent != nullptr) {
		connect(parent, &MainWindow::currentProjectChanged,
				this, &LabelItemsDockWidget::projectChanged);
	}

	connect(ui->treeView, &QTreeView::doubleClicked,
			this, &LabelItemsDockWidget::receiveDoubleClick);

	ui->treeView->setModel(_proxy);

	ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->treeView->setDragDropMode(QAbstractItemView::DragOnly);
	ui->treeView->setDragEnabled(true);
	ui->treeView->setDropIndicatorShown(true);

	connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
			this, &LabelItemsDockWidget::selectionChanged);

}

LabelItemsDockWidget::~LabelItemsDockWidget()
{
	delete ui;
}

void LabelItemsDockWidget::selectionChanged() {

	QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

	if (selection.size() > 0) {

		QString ref = ui->treeView->model()->data(selection.first(), EditableItemManager::ItemRefRole).toString();

		_mw_parent->currentProject()->setActiveItem(ref);
	}

}

void LabelItemsDockWidget::projectChanged(Aline::EditableItemManager* project) {

	_currentProject = project;

	if (_currentProject != nullptr) {

		_proxy->connectLabelTree(project->labelsTree());
		_proxy->setSourceModel(project);
		setEnabled(true);

	} else {

		_proxy->connectLabelTree(nullptr);
		_proxy->setSourceModel(nullptr);
		setEnabled(false);

	}

}

void LabelItemsDockWidget::receiveDoubleClick(const QModelIndex &index) {

	QVariant data = index.data(EditableItemManager::ItemRefRole);

	if (data.isValid()) {

		emit itemDoubleClicked(data.toString());

	}

}

} // namespace Sabrina
