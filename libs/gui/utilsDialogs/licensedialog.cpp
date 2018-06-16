#include "licensedialog.h"
#include "ui_licensedialog.h"

#include "utils/app_info.h"

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

#include <QMetaEnum>

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>

namespace Sabrina {

const QString LicenseDialog::LICENSE_ACCEPTED_KEY = "license_accepted";

bool LicenseDialog::triggerAcceptLicense(QWidget* parent) {

	QSettings settings;
	QVariant vaccepted = settings.value(LICENSE_ACCEPTED_KEY, QVariant(false));

	if (vaccepted.toBool()) {
		return true;
	}

	LicenseDialog d(parent);

	d.ui->explainLabel->setVisible(true);
	d.ui->explainLabel->setText(tr("Il semble que vous n'ayiez pas encore pris connaissance et accepté la license de %1.").arg(APP_NAME));

	d.ui->acceptLicenseCheckBox->setVisible(true);
	d.ui->acceptLicenseCheckBox->setChecked(false);

	d.exec();

	if (d.ui->acceptLicenseCheckBox->isChecked()) {

		settings.setValue(LICENSE_ACCEPTED_KEY, QVariant(true));

		return true;
	}

	settings.setValue(LICENSE_ACCEPTED_KEY, QVariant(false));

	return false;


}

LicenseDialog::LicenseDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LicenseDialog),
	_netWorkManager(nullptr)
{
	ui->setupUi(this);

	QFile license(QCoreApplication::applicationDirPath() + "/LICENSE");

	if (license.open(QIODevice::ReadOnly|QIODevice::Text)) {

		QByteArray datas = license.readAll();

		ui->plainTextEdit->setPlainText(datas);

		ui->getLicenseButton->setVisible(false);

	} else {

		QString error = "A copy of the gpl license has not been provided with the current copy of ";
		error += APP_NAME;
		error += " this shouldn't be the case, you can get one at https://www.gnu.org/licenses/gpl.txt (save it in the app root directory as LICENSE) or click get license.";

		ui->plainTextEdit->setPlainText(error);

		connect(ui->getLicenseButton, &QPushButton::clicked,
				this, &LicenseDialog::startDownloadLicense);

	}
	ui->explainLabel->setVisible(false);
	ui->acceptLicenseCheckBox->setVisible(false);

	ui->plainTextEdit->setReadOnly(true);
}

LicenseDialog::~LicenseDialog()
{
	delete ui;
}

void LicenseDialog::startDownloadLicense() {

	if (_netWorkManager == nullptr) {
		_netWorkManager = new QNetworkAccessManager(this);
		connect(_netWorkManager, &QNetworkAccessManager::finished,
				this, &LicenseDialog::licenseDownloaded);
	}

	if (_netWorkManager->networkAccessible() != QNetworkAccessManager::Accessible) {
		ui->plainTextEdit->setPlainText("Unable to get license from network, network is not accessible.");
		return;
	}

	QNetworkRequest req(QUrl("https://www.gnu.org/licenses/gpl.txt"));

	_netWorkManager->get(req);

	ui->getLicenseButton->setEnabled(false);

}

void LicenseDialog::licenseDownloaded(QNetworkReply* pReply) {

	if (pReply->error() != QNetworkReply::NoError) {

		QMetaEnum m_enum = QMetaEnum::fromType<QNetworkReply::NetworkError>();

		QString error = "Unable to get license from network: ";
		error += m_enum.valueToKey(pReply->error());

		ui->plainTextEdit->setPlainText(error);

		ui->getLicenseButton->setEnabled(true);

		return;

	}

	QByteArray datas = pReply->readAll();

	ui->plainTextEdit->setPlainText(datas);

	ui->getLicenseButton->setVisible(false);

	QFile license(QCoreApplication::applicationDirPath() + "/LICENSE");

	if (license.open(QIODevice::WriteOnly)) {
		license.write(datas);
	}

}

} // namespace Sabrina
