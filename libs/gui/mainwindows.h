#ifndef CATHIA_MAINWINDOWS_H
#define CATHIA_MAINWINDOWS_H

#include "gui_global.h"

#include "aline/src/mainwindow.h"

namespace Cathia {

class CATHIA_GUI_EXPORT MainWindow : public Aline::MainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);

signals:

public slots:

	void displayAboutWindows();

};

} // namespace Cathia

#endif // MAINWINDOWS_H
