#ifndef SABRINA_LABELITEMSDOCKWIDGET_H
#define SABRINA_LABELITEMSDOCKWIDGET_H

#include <QDockWidget>

#include "gui/gui_global.h"

namespace Sabrina {

class MainWindow;
class ProjectFilterByLabelProxyModel;
class EditableItemManager;

namespace Ui {
class LabelItemsDockWidget;
}

class CATHIA_GUI_EXPORT LabelItemsDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit LabelItemsDockWidget(MainWindow *parent = 0);
	~LabelItemsDockWidget();

signals:

	void itemDoubleClicked(QString itemRef);

private:

	void selectionChanged();

	void projectChanged(EditableItemManager* project);

	void receiveDoubleClick(const QModelIndex &index);

	MainWindow * _mw_parent;

	Ui::LabelItemsDockWidget *ui;

	ProjectFilterByLabelProxyModel* _proxy;

	EditableItemManager* _currentProject;
};


} // namespace Sabrina
#endif // SABRINA_LABELITEMSDOCKWIDGET_H
