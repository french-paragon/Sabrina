#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "utils/app_info.h"

namespace Sabrina {

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);

	ui->label_author->setText(tr("%1 est un programme crée par %2").arg(APP_NAME).arg(ORG_NAME));
	ui->label_compilation->setText(tr("<span style=\" color:#a1a1a1;\">Compilé le %1</span>").arg(compileDateFull()));
	ui->label_version->setText(tr("<span style=\" color:#a1a1a1;\">Version %1</span>").arg(appTag()));

	setModal(true);

}

AboutDialog::~AboutDialog()
{
	delete ui;
}

} // namespace Cathia
