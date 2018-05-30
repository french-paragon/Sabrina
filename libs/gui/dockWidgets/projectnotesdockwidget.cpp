#include "projectnotesdockwidget.h"
#include "ui_projectnotesdockwidget.h"

#include "mainwindows.h"
#include "model/editableitemmanager.h"

namespace Sabrina {

ProjectNotesDockWidget::ProjectNotesDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	_mw_parent(parent),
	ui(new Ui::ProjectNotesDockWidget)
{
	ui->setupUi(this);

	projectChanged(_mw_parent->currentProject());

	connect(_mw_parent, &MainWindow::currentProjectChanged,
			this, &ProjectNotesDockWidget::projectChanged);
}

ProjectNotesDockWidget::~ProjectNotesDockWidget()
{
	delete ui;
}


void ProjectNotesDockWidget::projectChanged(EditableItemManager* project) {

	if (_projectDeletedConnection) {
		disconnect(_projectDeletedConnection);
	}

	if (project != nullptr) {

		ui->notesWidgets->setCurrentNoteList(project->noteList());

		_projectDeletedConnection = connect(project, &QObject::destroyed,
											this, &ProjectNotesDockWidget::projectCleared);
	} else {
		ui->notesWidgets->setCurrentNoteList(nullptr);
	}

}

void ProjectNotesDockWidget::projectCleared() {

	if (_projectDeletedConnection) {
		disconnect(_projectDeletedConnection);
	}

	ui->notesWidgets->setCurrentNoteList(nullptr);

}

} // namespace Sabrina
