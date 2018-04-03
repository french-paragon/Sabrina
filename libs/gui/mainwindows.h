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
	explicit MainWindow(QWidget* parent = nullptr);

	EditableItemManager *currentProject() const;
	void setCurrentProject(EditableItemManager *currentProject);

signals:

	void currentProjectChanged(EditableItemManager*);

public slots:

	void displayAboutWindows();

	void editItem(QString const& itemRef);
	void saveAll();

protected:

	EditableItemManager* _currentProject;

	QMap<QString, Aline::Editor*> _openedEditors;

};

} // namespace Cathia

#endif // MAINWINDOWS_H
