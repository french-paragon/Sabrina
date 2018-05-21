#include "projecttreedockwidget.h"
#include "ui_projecttreedockwidget.h"

#include <QMenu>
#include <QInputDialog>

#include "actions/editableitemtypespecializedaction.h"

#include "model/editableitemfactory.h"

#include <aline/src/editorfactorymanager.h>

namespace Sabrina {

ProjectTreeDockWidget::ProjectTreeDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	ui(new Ui::ProjectTreeDockWidget),
	_mw_parent(parent),
	_newItemMenu(nullptr)
{
	_internalModel = new QSortFilterProxyModel(this);

	ui->setupUi(this);

	ui->treeView->setModel(_internalModel);

	projectChanged(_mw_parent->currentProject());

	connect(_mw_parent, &MainWindow::currentProjectChanged,
			this, &ProjectTreeDockWidget::projectChanged);

	connect(ui->treeView, &QTreeView::doubleClicked,
			this, &ProjectTreeDockWidget::receiveDoubleClick);

	connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
			this, &ProjectTreeDockWidget::selectionChanged);

	connect(ui->pushButtonSuppr, &QPushButton::clicked,
			this, &ProjectTreeDockWidget::supprButtonClicked);

	ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->treeView->setDragDropMode(QAbstractItemView::DragOnly);
	ui->treeView->setDragEnabled(true);
	ui->treeView->setDropIndicatorShown(true);

	ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->treeView, &QTreeView::customContextMenuRequested,
			this, &ProjectTreeDockWidget::buildTreeContextMenu);
}

ProjectTreeDockWidget::~ProjectTreeDockWidget()
{
	delete ui;
}

void ProjectTreeDockWidget::selectionChanged() {

	QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

	if (selection.size() > 0) {

		QString ref = ui->treeView->model()->data(selection.first(), EditableItemManager::ItemRefRole).toString();

		_mw_parent->currentProject()->setActiveItem(ref);
	}
}

void ProjectTreeDockWidget::onItemCreationTriggered(QString itemTypeRef) {


	QString parent_ref = "";

	QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

	if (selection.size() > 0) {

		QVariant info = ui->treeView->model()->data(selection.first(), EditableItemManager::ItemAcceptChildrenRole);

		if (info.userType() == QMetaType::Bool) { //a bool has been set.
			if (info.toBool() == true) {
				parent_ref = ui->treeView->model()->data(selection.first(), EditableItemManager::ItemRefRole).toString();
			}
		}

	}

	bool ok;

	QString ref = QInputDialog::getText(_mw_parent,
										tr("Saisir une référence"),
										tr("référence"),
										QLineEdit::Normal,
										itemTypeRef,
										&ok);

	if (!ok) {
		return;
	}

	emit itemCreationTriggered(itemTypeRef, ref, parent_ref);

	if (selection.size() > 0) {
		ui->treeView->expand(selection.first());
	}

}

void ProjectTreeDockWidget::projectChanged(EditableItemManager* project) {

	if(_newItemFactoryWatcher) {
		disconnect(_newItemFactoryWatcher);
	}

	if (_itemCreationTrigger) {
		disconnect(_itemCreationTrigger);
	}

	if (_itemSuppresionTrigger) {
		disconnect(_itemSuppresionTrigger);
	}

	if (project == nullptr) {
		setEnabled(false);
		return;
	} else {
		setEnabled(true);
	}

	reselectProject(project);
	rebuildMenu(project);

	_newItemFactoryWatcher = connect(project->factoryManager(), &EditableItemFactoryManager::rowsInserted,
									 this, &ProjectTreeDockWidget::rebuildMenuWithoutProject);

	_itemCreationTrigger = connect(this, &ProjectTreeDockWidget::itemCreationTriggered,
								   project, &EditableItemManager::createItem);

	_itemSuppresionTrigger = connect(this, &ProjectTreeDockWidget::itemSuppressionTriggered,
									 project, &EditableItemManager::clearItems);
}

void ProjectTreeDockWidget::reselectProject(EditableItemManager *project) {

	_internalModel->setSourceModel(project);

}

void ProjectTreeDockWidget::rebuildMenuWithoutProject() {
	rebuildMenu(_mw_parent->currentProject());
}

void ProjectTreeDockWidget::rebuildMenu(EditableItemManager* project) {

	if (_newItemMenu != nullptr) {
		_newItemMenu->deleteLater();
	}

	if (project == nullptr) {
		return;
	}

	_newItemMenu = new QMenu(this);

	EditableItemFactoryManager* f = project->factoryManager();

	for (int i = 0; i < f->rowCount(); i++) {

		EditableItemTypeSpecializedAction* action = new EditableItemTypeSpecializedAction(f->data(f->index(i),
																								  EditableItemFactoryManager::ItemRefRole).toString(),
																						  f->data(f->index(i),
																								  Qt::DisplayRole).toString(),
																						  _newItemMenu);

		connect(action, &EditableItemTypeSpecializedAction::triggered,
				this, &ProjectTreeDockWidget::onItemCreationTriggered);

		_newItemMenu->addAction(action);

	}

	ui->toolButton_new->setMenu(_newItemMenu);
	ui->toolButton_new->setPopupMode(QToolButton::MenuButtonPopup);

}

void ProjectTreeDockWidget::receiveDoubleClick(const QModelIndex &index) {

	QString ref = _internalModel->data(index, EditableItemManager::ItemRefRole).toString();

	emit itemDoubleClicked(ref);

}

void ProjectTreeDockWidget::supprButtonClicked() {

	//TODO: options to have a warning about deletion.

	QModelIndexList smod = ui->treeView->selectionModel()->selectedIndexes();
	QStringList selectedItems;

	for (QModelIndex index : smod) {
		selectedItems << _internalModel->data(index, EditableItemFactoryManager::ItemRefRole).toString();
	}

	emit itemSuppressionTriggered(selectedItems);

}

void ProjectTreeDockWidget::buildTreeContextMenu(QPoint const& pos) {

	QModelIndex index = ui->treeView->indexAt(pos);

	if (index.isValid()) {

		QMenu menu;

		QVariant data  = index.data(EditableItemManager::ItemTypeRefRole);
		QString itemTypeRef = data.toString();

		if (Aline::EditorFactoryManager::GlobalEditorFactoryManager.hasFactoryInstalledForItem(itemTypeRef)) {
			QAction* editAction = menu.addAction(tr("éditer"));

			connect(editAction, &QAction::triggered, [this, &index] () {
				emit itemDoubleClicked(index.data(EditableItemManager::ItemRefRole).toString());
			});

			menu.addSeparator();
		}

		if (_internalModel->flags(index) & Qt::ItemIsEditable) {

			QAction* renameAction = menu.addAction(tr("renommer"));

			connect(renameAction, &QAction::triggered, [&index, this] () {ui->treeView->edit(index);});

			menu.addSeparator();

		}

		data = index.data(EditableItemManager::ItemAcceptChildrenRole);

		if (data.toBool()) {

			QMenu* section = menu.addMenu(tr("ajouter un:"));

			EditableItemFactoryManager* f = _mw_parent->currentProject()->factoryManager();

			for (int i = 0; i < f->rowCount(); i++) {

				EditableItemTypeSpecializedAction* action = new EditableItemTypeSpecializedAction(f->data(f->index(i),
																										  EditableItemFactoryManager::ItemRefRole).toString(),
																								  f->data(f->index(i),
																										  Qt::DisplayRole).toString(),
																								  _newItemMenu);

				connect(action, &EditableItemTypeSpecializedAction::triggered,
						this, [&index, this] (QString type_id) {

					QString parentRef = index.data(EditableItemManager::ItemRefRole).toString();
					emit itemCreationTriggered(type_id, type_id, parentRef);

				});

				section->addAction(action);

			}

			menu.addSeparator();

		}

		QAction* actionRemove = menu.addAction(tr("supprimer"));

		connect(actionRemove, &QAction::triggered, [&index, this] () {
			itemSuppressionTriggered({index.data(EditableItemManager::ItemRefRole).toString()});
		});

		menu.exec(ui->treeView->viewport()->mapToGlobal(pos));

	}

}

} //namespace Sabrina
