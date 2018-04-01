#include "projecttreedockwidget.h"
#include "ui_projecttreedockwidget.h"

#include <QMenu>
#include <QInputDialog>

#include "actions/editableitemtypespecializedaction.h"

#include "model/editableitemfactory.h"

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
}

ProjectTreeDockWidget::~ProjectTreeDockWidget()
{
	delete ui;
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

}

void ProjectTreeDockWidget::projectChanged(EditableItemManager* project) {

	if(_newItemFactoryWatcher) {
		disconnect(_newItemFactoryWatcher);
	}

	if (_itemCreationTrigger) {
		disconnect(_itemCreationTrigger);
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

	QModelIndexList smod = ui->treeView->selectionModel()->selectedIndexes();
	QStringList selectedItems;

	for (QModelIndex index : smod) {
		selectedItems << _internalModel->data(index, EditableItemFactoryManager::ItemRefRole).toString();
	}

	emit itemSuppressionTriggered(selectedItems);

}

} //namespace Sabrina
