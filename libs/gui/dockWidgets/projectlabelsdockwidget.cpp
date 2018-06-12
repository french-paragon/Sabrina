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

#include "projectlabelsdockwidget.h"
#include "ui_projectlabelsdockwidget.h"

#include "mainwindows.h"
#include "model/editableitemmanager.h"
#include "model/labels/labelstree.h"
#include "model/labels/labelselectionforitemproxymodel.h"

#include "delegates/labelsmodelitemdelegate.h"

namespace Sabrina {

ProjectLabelsDockWidget::ProjectLabelsDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	ui(new Ui::ProjectLabelsDockWidget),
	_mw_parent(parent)
{
	ui->setupUi(this);

	_proxy = new LabelSelectionForItemProxyModel(this);

	projectChanged(_mw_parent->currentProject());

	if (_mw_parent != nullptr) {
		connect(_mw_parent, &MainWindow::currentProjectChanged,
				this, &ProjectLabelsDockWidget::projectChanged);
	}

	connect(ui->addLabelButton, &QPushButton::clicked,
			this, &ProjectLabelsDockWidget::labelAdditionButtonClicked);

	connect(ui->deleteLabelButton, &QPushButton::clicked,
			this, &ProjectLabelsDockWidget::labelDeletionButtonClicked);

	ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);

	ui->treeView->setDragDropMode(QAbstractItemView::DragDrop);
	ui->treeView->setDragEnabled(true);
	ui->treeView->setAcceptDrops(true);
	ui->treeView->setDropIndicatorShown(true);

	ui->treeView->setItemDelegate(new LabelsModelItemDelegate(ui->treeView));

	ui->treeView->setModel(_proxy);

	connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
			this, &ProjectLabelsDockWidget::selectionChanged);
}

ProjectLabelsDockWidget::~ProjectLabelsDockWidget()
{
	delete ui;
}

void ProjectLabelsDockWidget::projectChanged(EditableItemManager* project) {

	_currentProject = project;

	if (_currentProject != nullptr) {
		_proxy->setSourceModel(project->labelsTree());
		setEnabled(true);
	} else {
		_proxy->setSourceModel((LabelsTree*) nullptr);
		setEnabled(false);
	}

}

void ProjectLabelsDockWidget::labelAdditionButtonClicked() {

	if (_currentProject != nullptr) {

		_currentProject->labelsTree()->insertRow(_currentProject->labelsTree()->rowCount());

	}

}
void ProjectLabelsDockWidget::labelDeletionButtonClicked() {

	QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

	if (selection.size() == 1) {
		QModelIndex index = selection.first();

		_currentProject->labelsTree()->removeRow(index.row(), index.parent());
	}

}

void ProjectLabelsDockWidget::selectionChanged() {

	QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

	if (selection.size() == 1) {
		QModelIndex index = selection.first();

		_currentProject->labelsTree()->setActiveLabel(_proxy->mapToSource(index));
	}

}

} // namespace Sabrina
