#ifndef CATHIA_APP_H
#define CATHIA_APP_H

#include "core_global.h"

#include "aline/src/control/app.h"
#include <QString>

namespace Sabrina {

class MainWindow;
class EditableItemManager;

class CATHIA_CORE_EXPORT App : public Aline::App
{
	Q_OBJECT

public:
	explicit App(int & argc, char** argv);
	~App();

	virtual bool start(QString appCode = "default");

	void openProject(QString const& projectFile);

protected:

	void openFileProject();
	void createFileProject();
	void closeProject();

	void buildMainWindow();
	void addAppActionsToMainWindows(MainWindow* mw);
	void addAboutActionsToMainWindows(MainWindow* mw);
	void loadEditorsFactories();
	void loadEditableFactories();

	void quitCathia();

	MainWindow* _mainWindow;
	EditableItemManager* _project;
};

} // namespace Cathia

#endif // APP_H
