#ifndef SABRINA_PROJECTLABELSDOCKWIDGET_H
#define SABRINA_PROJECTLABELSDOCKWIDGET_H

#include <QDockWidget>

#include "gui/gui_global.h"

namespace Sabrina {

namespace Ui {
class ProjectLabelsDockWidget;
}

class MainWindow;
class EditableItemManager;
class LabelSelectionForItemProxyModel;

class CATHIA_GUI_EXPORT ProjectLabelsDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit ProjectLabelsDockWidget(MainWindow *parent = 0);
	~ProjectLabelsDockWidget();

private:

	void projectChanged(EditableItemManager* project);

	void labelAdditionButtonClicked();
	void labelDeletionButtonClicked();

	void selectionChanged();

	Ui::ProjectLabelsDockWidget *ui;

	EditableItemManager* _currentProject;
	LabelSelectionForItemProxyModel* _proxy;

	MainWindow* _mw_parent;
};


} // namespace Sabrina
#endif // SABRINA_PROJECTLABELSDOCKWIDGET_H
