#ifndef CATHIA_ABOUTDIALOG_H
#define CATHIA_ABOUTDIALOG_H

#include <QDialog>

#include "gui/gui_global.h"

namespace Sabrina {

namespace Ui {
class AboutDialog;
}

class CATHIA_GUI_EXPORT AboutDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AboutDialog(QWidget *parent = 0);
	~AboutDialog();

private:
	Ui::AboutDialog *ui;
};


} // namespace Cathia
#endif // CATHIA_ABOUTDIALOG_H
