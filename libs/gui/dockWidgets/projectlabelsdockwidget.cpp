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
#include "aline/src/model/editableitemfactory.h"
#include "aline/src/model/labels/labelstree.h"
#include "aline/src/model/labels/labelselectionforitemproxymodel.h"

#include "actions/editableitemtypespecializedaction.h"

#include "delegates/labelsmodelitemdelegate.h"

#include <QMenu>

namespace Sabrina {

ProjectLabelsDockWidget::ProjectLabelsDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	ui(new Ui::ProjectLabelsDockWidget),
	_mw_parent(parent)
{
	ui->setupUi(this);

	_proxy = new Aline::LabelSelectionForItemProxyModel(this);

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

	ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->treeView, &QTreeView::customContextMenuRequested,
			this, &ProjectLabelsDockWidget::buildTreeContextMenu);

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
		_proxy->setSourceModel((Aline::LabelsTree*) nullptr);
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

void ProjectLabelsDockWidget::buildTreeContextMenu(QPoint const& pos) {

	QMenu menu;

	QModelIndex index = ui->treeView->indexAt(pos);

	QAction createSubAction(QIcon(":/icons/icons/new_simple.svg"), tr("nouveau sous-label"), &menu);
	auto callBackAdd = [this, &index] () {_proxy->insertRow(_proxy->rowCount(index), index); };
	connect(&createSubAction, &QAction::triggered, callBackAdd);

	QAction removeLabelAction(QIcon(":/icons/icons/delete_simple.svg"), tr("supprimer le label"), &menu);
	auto callBackRemove = [this, &index] () {_proxy->removeRow(index.row(), index.parent()); };
	connect(&removeLabelAction, &QAction::triggered, callBackRemove);

	menu.addAction(&createSubAction);
	menu.addAction(&removeLabelAction);
	menu.addSeparator();

	QMenu section(tr("ajouter un:"), &menu);

	Aline::EditableItemFactoryManager* f = _currentProject->factoryManager();

	for (QString typeRef : f->installedFactoriesKeys()) {

		QString typeName = f->itemTypeName(typeRef);

		EditableItemTypeSpecializedAction* action = new EditableItemTypeSpecializedAction(typeRef,
																						  QIcon(f->itemIconUrl(typeRef)),
																						  QString("%1").arg(typeName),
																						  &section);
		connect(action, &EditableItemTypeSpecializedAction::triggered,
				this, [&index, this] (QString type_id) {

					QString createdRef;
					bool done = _currentProject->createItem(type_id, type_id, &createdRef);

					if (done) {
						_proxy->setData(index, createdRef, Aline::LabelsTree::MarkLabelForItem);
					}

		});

		section.addAction(action);
	}

	menu.addMenu(&section);

	menu.exec(ui->treeView->viewport()->mapToGlobal(pos));
}

void ProjectLabelsDockWidget::selectionChanged() {

	QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

	if (selection.size() == 1) {
		QModelIndex index = selection.first();

		_currentProject->labelsTree()->setActiveLabel(_proxy->mapToSource(index));
	}

}

} // namespace Sabrina
