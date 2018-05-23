#ifndef MAINWINDOWSFACTORY_H
#define MAINWINDOWSFACTORY_H

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
