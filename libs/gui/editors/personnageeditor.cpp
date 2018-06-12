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

#include "personnageeditor.h"
#include "ui_personnageeditor.h"

#include "model/editableItems/personnage.h"

namespace Sabrina {

const QString PersonnageEditor::PERSONNAGE_EDITOR_TYPE_ID = "sabrina_jdr_personnage_editor";

PersonnageEditor::PersonnageEditor(QWidget *parent) :
	Aline::EditableItemEditor(parent),
	ui(new Ui::PersonnageEditor),
	_currentPerso(nullptr),
	_has_intermediate_connections(false)
{
	ui->setupUi(this);
}

PersonnageEditor::~PersonnageEditor()
{
	delete ui;
}

QString PersonnageEditor::getTypeId() const {
	return PERSONNAGE_EDITOR_TYPE_ID;
}

QString PersonnageEditor::getTypeName() const {

	return QString(tr("Éditeur de personnages"));

}

QStringList PersonnageEditor::PersonnageEditor::editableTypes() const {
	return { Personnage::PERSONNAGE_TYPE_ID };
}

bool PersonnageEditor::effectivelySetEditedItem(Aline::EditableItem* item) {

	if (!_has_intermediate_connections) {
		connect(ui->textEdit_background, &QTextEdit::textChanged, this, &PersonnageEditor::onBackgroundTextChanged);
		_has_intermediate_connections = true;
	}

	Personnage* perso = qobject_cast<Personnage*>(item);

	if (perso == nullptr) {
		return false;
	}

	if (_currentPerso != nullptr) {

		disconnect(ui->lineEdit_name, &QLineEdit::textEdited, _currentPerso, &Personnage::setObjectName);
		disconnect(ui->lineEdit_race, &QLineEdit::textEdited, _currentPerso, &Personnage::setPersoRace);
		disconnect(ui->spinBox_age, QOverload<int>::of(&QSpinBox::valueChanged), _currentPerso, &Personnage::setAge);

		if (_nameWatchConnection) {
			disconnect(_nameWatchConnection);
		}
		if (_raceWatchConnection) {
			disconnect(_raceWatchConnection);
		}
		if (_ageWatchConnection) {
			disconnect(_ageWatchConnection);
		}

	}

	ui->lineEdit_name->setText(perso->objectName());
	ui->lineEdit_race->setText(perso->persoRace());
	ui->spinBox_age->setValue(perso->age());

	ui->textEdit_background->setPlainText(perso->background());

	connect(ui->lineEdit_name, &QLineEdit::textEdited, perso, &Personnage::setObjectName);
	connect(ui->lineEdit_race, &QLineEdit::textEdited, perso, &Personnage::setPersoRace);
	connect(ui->spinBox_age, QOverload<int>::of(&QSpinBox::valueChanged), perso, &Personnage::setAge);

	_nameWatchConnection = connect(perso, &Personnage::objectNameChanged, [this] (QString const& text) {if (text != ui->lineEdit_name->text()) ui->lineEdit_name->setText(text);});
	_raceWatchConnection = connect(perso, &Personnage::persoRaceChanged, [this] (QString const& text) {if (text != ui->lineEdit_race->text()) ui->lineEdit_race->setText(text);});
	_ageWatchConnection = connect(perso, &Personnage::persoAgeChanged, [this] (int value) {if (value != ui->spinBox_age->value()) ui->spinBox_age->setValue(value);});

	_currentPerso = perso;

	return true;
}

void PersonnageEditor::onBackgroundTextChanged() {

	if (_currentPerso != nullptr) {
		_currentPerso->setBackground(ui->textEdit_background->document()->toPlainText());
	}

}

PersonnageEditor::PersonnageEditorFactory::PersonnageEditorFactory(QObject* parent) :
	Aline::EditorFactory(parent)
{

}

Aline::Editor* PersonnageEditor::PersonnageEditorFactory::createItem(QWidget* parent) const {
	return new PersonnageEditor(parent);
}

} // namespace Sabrina
