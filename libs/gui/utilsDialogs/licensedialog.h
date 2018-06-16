#ifndef SABRINA_LICENSEDIALOG_H
#define SABRINA_LICENSEDIALOG_H

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

#include <QDialog>

#include "../gui_global.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace Sabrina {

namespace Ui {
class LicenseDialog;
}

class CATHIA_GUI_EXPORT LicenseDialog : public QDialog
{
	Q_OBJECT

public:

	static bool triggerAcceptLicense(QWidget* parent = nullptr);
	static const QString LICENSE_ACCEPTED_KEY;

	explicit LicenseDialog(QWidget *parent = 0);
	~LicenseDialog();

private:

	void startDownloadLicense();
	void licenseDownloaded(QNetworkReply* pReply);

	QNetworkAccessManager* _netWorkManager;


	Ui::LicenseDialog *ui;
};


} // namespace Sabrina
#endif // SABRINA_LICENSEDIALOG_H
