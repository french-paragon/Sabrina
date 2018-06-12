#ifndef MAINWINDOWSFACTORY_H
#define MAINWINDOWSFACTORY_H

/*
This file is part of the project Sabrina
Copyright (C) 2018  Paragon <french.paragon@gmail.com>

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

#include <QObject>
#include <QVector>

#include <functional>

#include "gui/gui_global.h"

namespace Sabrina {

class MainWindow;

class CATHIA_GUI_EXPORT MainWindowsFactory : public QObject
{
	Q_OBJECT
public:

	typedef std::function<void(MainWindow*)> Action;

	explicit MainWindowsFactory(QObject *parent = nullptr);

	static MainWindowsFactory GlobalMainWindowsFactory;

	MainWindow* factorizeMainWindows() const;

	void registerPreAction(Action const& a);
	void registerPostAction(Action const& a);

signals:

public slots:

protected:

	QVector<Action> _preActions;
	QVector<Action> _postActions;

};

} //namespace Sabrina

#endif // MAINWINDOWSFACTORY_H
