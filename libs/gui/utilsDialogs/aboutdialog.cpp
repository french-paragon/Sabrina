#include "aboutdialog.h"
#include "ui_aboutdialog.h"

namespace Cathia {

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
	delete ui;
}

} // namespace Cathia
