#include "projectlabelsdockwidget.h"
#include "ui_projectlabelsdockwidget.h"

#include "mainwindows.h"
#include "model/editableitemmanager.h"
#include "model/labels/labelstree.h"

namespace Sabrina {

ProjectLabelsDockWidget::ProjectLabelsDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	ui(new Ui::ProjectLabelsDockWidget),
	_mw_parent(parent)
{
	ui->setupUi(this);

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

	ui->treeView->setDragDropMode(QAbstractItemView::DropOnly);
	ui->treeView->setAcceptDrops(true);
	ui->treeView->setDropIndicatorShown(true);
}

ProjectLabelsDockWidget::~ProjectLabelsDockWidget()
{
	delete ui;
}

void ProjectLabelsDockWidget::projectChanged(EditableItemManager* project) {

	_currentProject = project;

	if (_currentProject != nullptr) {
		ui->treeView->setModel(project->labelsTree());
		setEnabled(true);
	} else {
		ui->treeView->setModel(nullptr);
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

} // namespace Sabrina
