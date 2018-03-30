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

		disconnect(_currentPerso, &Personnage::objectNameChanged, ui->lineEdit_name, &QLineEdit::setText);
		disconnect(_currentPerso, &Personnage::persoRaceChanged, ui->lineEdit_race, &QLineEdit::setText);
		disconnect(_currentPerso, &Personnage::persoAgeChanged, ui->spinBox_age, &QSpinBox::setValue);

		disconnect(_currentPerso, &Personnage::persoBackgroundChanged, ui->textEdit_background, &QTextEdit::setHtml);

	}

	connect(ui->lineEdit_name, &QLineEdit::textEdited, perso, &Personnage::setObjectName);
	connect(ui->lineEdit_race, &QLineEdit::textEdited, perso, &Personnage::setPersoRace);
	connect(ui->spinBox_age, QOverload<int>::of(&QSpinBox::valueChanged), perso, &Personnage::setAge);

	connect(perso, &Personnage::objectNameChanged, ui->lineEdit_name, &QLineEdit::setText);
	connect(perso, &Personnage::persoRaceChanged, ui->lineEdit_race, &QLineEdit::setText);
	connect(perso, &Personnage::persoAgeChanged, ui->spinBox_age, &QSpinBox::setValue);

	connect(perso, &Personnage::persoBackgroundChanged, ui->textEdit_background, &QTextEdit::setHtml);

	_currentPerso = perso;
}

void PersonnageEditor::onBackgroundTextChanged() {

	if (_currentPerso != nullptr) {
		_currentPerso->setBackground(ui->textEdit_background->document()->toHtml());
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
