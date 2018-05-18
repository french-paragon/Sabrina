#ifndef CATHIA_MAINWINDOWS_H
#define CATHIA_MAINWINDOWS_H

#include "gui_global.h"

#include "aline/src/mainwindow.h"
#include "model/editableitemmanager.h"

namespace Sabrina {

class CATHIA_GUI_EXPORT MainWindow : public Aline::MainWindow
{
	Q_OBJECT
public:

	static const QString GEOMETRY_CONFIG_KEY;
	static const QString STATE_CONFIG_KEY;

	static const QString MENU_FILE_NAME;
	static const QString MENU_DISPLAY_NAME;

	explicit MainWindow(QWidget* parent = nullptr);

	EditableItemManager *currentProject() const;
	void setCurrentProject(EditableItemManager *currentProject);

	QMenu* findMenuByName(QString const& name, bool createIfNotExist = false);

	void addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockwidget);
	void addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockwidget, Qt::Orientation orientation);

signals:

	void currentProjectChanged(EditableItemManager*);

public slots:

	void displayAboutWindows();

	void editItem(QString const& itemRef);
	void saveAll();

protected:

	EditableItemManager* _currentProject;

	QMap<QString, Aline::Editor*> _openedEditors;

	QMenu* _submenuDock;

};

} // namespace Cathia

#endif // MAINWINDOWS_H
