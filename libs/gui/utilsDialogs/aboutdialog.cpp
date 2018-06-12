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
