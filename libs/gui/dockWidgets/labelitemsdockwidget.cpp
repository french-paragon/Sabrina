#include "labelitemsdockwidget.h"
#include "ui_labelitemsdockwidget.h"

#include "mainwindows.h"
#include "model/labels/projectfilterbylabelproxymodel.h"

namespace Sabrina {

LabelItemsDockWidget::LabelItemsDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	ui(new Ui::LabelItemsDockWidget)
{
	ui->setupUi(this);

	_proxy = new ProjectFilterByLabelProxyModel(ui->treeView);

	projectChanged(parent->currentProject());

	if (parent != nullptr) {
		connect(parent, &MainWindow::currentProjectChanged,
				this, &LabelItemsDockWidget::projectChanged);
	}

	ui->treeView->setModel(_proxy);

}

LabelItemsDockWidget::~LabelItemsDockWidget()
{
	delete ui;
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

} // namespace Sabrina
