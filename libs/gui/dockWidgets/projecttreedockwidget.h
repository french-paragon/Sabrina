#ifndef PROJECTTREEDOCKWIDGET_H
#define PROJECTTREEDOCKWIDGET_H

#include "gui/gui_global.h"
#include <QDockWidget>

#include "gui/mainwindows.h"
#include "model/editableitemmanager.h"
#include <QSortFilterProxyModel>

class QMenu;

namespace Sabrina {

namespace Ui {
class ProjectTreeDockWidget;
}

class CATHIA_GUI_EXPORT ProjectTreeDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit ProjectTreeDockWidget(MainWindow *parent = 0);
	~ProjectTreeDockWidget();

signals:

	void itemCreationTriggered(QString itemTypeRef);
	void itemDoubleClicked(QString itemRef);
	void itemSuppressionTriggered(QStringList itemRef);

private:

	void projectChanged(EditableItemManager* project);

	void reselectProject(EditableItemManager* project);
	void rebuildMenuWithoutProject();
	void rebuildMenu(EditableItemManager *project);

	void receiveDoubleClick(const QModelIndex &index);
	void supprButtonClicked();

	MainWindow * _mw_parent;

	QMenu* _newItemMenu;
	QSortFilterProxyModel* _internalModel;

	Ui::ProjectTreeDockWidget *ui;

	QMetaObject::Connection _newItemFactoryWatcher;
};

} //namespace Sabrina

#endif // PROJECTTREEDOCKWIDGET_H
