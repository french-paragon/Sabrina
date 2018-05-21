#include "labelitemsdockwidget.h"
#include "ui_labelitemsdockwidget.h"

#include "mainwindows.h"
#include "model/labels/projectfilterbylabelproxymodel.h"

namespace Sabrina {

LabelItemsDockWidget::LabelItemsDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	ui(new Ui::LabelItemsDockWidget),
	_mw_parent(parent)
{
	ui->setupUi(this);

	_proxy = new ProjectFilterByLabelProxyModel(ui->treeView);

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

void LabelItemsDockWidget::projectChanged(EditableItemManager* project) {

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
