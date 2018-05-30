#ifndef SABRINA_PROJECTNOTESDOCKWIDGET_H
#define SABRINA_PROJECTNOTESDOCKWIDGET_H

#include <QDockWidget>

namespace Sabrina {

namespace Ui {
class ProjectNotesDockWidget;
}

class MainWindow;
class EditableItemManager;

class ProjectNotesDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit ProjectNotesDockWidget(MainWindow *parent = 0);
	~ProjectNotesDockWidget();

private:

	void projectChanged(EditableItemManager* project);
	void projectCleared();


	MainWindow * _mw_parent;

	Ui::ProjectNotesDockWidget *ui;

	QMetaObject::Connection _projectDeletedConnection;
};


} // namespace Sabrina
#endif // SABRINA_PROJECTNOTESDOCKWIDGET_H
