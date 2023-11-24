#ifndef CATHIA_APP_H
#define CATHIA_APP_H

/*
This file is part of the project Sabrina
Copyright (C) 2018-2023  Paragon <french.paragon@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "core_global.h"

#include <Aline/control/app.h>

#include <QString>

namespace Aline {
class EditorFactoryManager;
class EditableItemFactoryManager;
}

namespace Sabrina {

class MainWindow;
class EditableItemManager;
class JsonEditableItemManager;

class CATHIA_CORE_EXPORT App : public Aline::App
{
	Q_OBJECT

public:
	static void loadEditorsFactories();
	static void loadEditableFactories();

	static Aline::EditorFactoryManager* appEditorsFactoryManager();

	explicit App(int & argc, char** argv);
	~App();

	virtual bool start(QString appCode = "default");

	void openProject(QString const& projectFile);

protected:

	void openFileProject();
	void createFileProject();
	void closeProject();

	JsonEditableItemManager* configureJsonProject();

	void buildMainWindow();
	void addAppActionsToMainWindows(MainWindow* mw);
	void addAboutActionsToMainWindows(MainWindow* mw);

	void quitCathia();

	MainWindow* _mainWindow;
	EditableItemManager* _project;
};

} // namespace Cathia

#endif // APP_H
