#ifndef SABRINA_LABELITEMSDOCKWIDGET_H
#define SABRINA_LABELITEMSDOCKWIDGET_H

#include <QDockWidget>

namespace Sabrina {

class MainWindow;
class ProjectFilterByLabelProxyModel;
class EditableItemManager;

namespace Ui {
class LabelItemsDockWidget;
}

class LabelItemsDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit LabelItemsDockWidget(MainWindow *parent = 0);
	~LabelItemsDockWidget();

private:

	void projectChanged(EditableItemManager* project);

	Ui::LabelItemsDockWidget *ui;

	ProjectFilterByLabelProxyModel* _proxy;

	EditableItemManager* _currentProject;
};


} // namespace Sabrina
#endif // SABRINA_LABELITEMSDOCKWIDGET_H
