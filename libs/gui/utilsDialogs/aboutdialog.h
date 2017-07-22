#ifndef CATHIA_ABOUTDIALOG_H
#define CATHIA_ABOUTDIALOG_H

#include <QDialog>

namespace Cathia {

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
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
