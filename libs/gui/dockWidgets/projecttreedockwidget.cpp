#include "projecttreedockwidget.h"
#include "ui_projecttreedockwidget.h"

#include <QMenu>

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

void ProjectTreeDockWidget::projectChanged(EditableItemManager* project) {

	if(_newItemFactoryWatcher) {
		disconnect(_newItemFactoryWatcher);
	}

	reselectProject(project);
	rebuildMenu(project);

	_newItemFactoryWatcher = connect(project->factoryManager(), &EditableItemFactoryManager::rowsInserted,
									 this, &ProjectTreeDockWidget::rebuildMenuWithoutProject);

	if (project == nullptr) {
		setEnabled(false);
	} else {
		setEnabled(true);
	}
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
				this, &ProjectTreeDockWidget::itemCreationTriggered);

		_newItemMenu->addAction(action);

	}

	ui->toolButton_new->setMenu(_newItemMenu);

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
