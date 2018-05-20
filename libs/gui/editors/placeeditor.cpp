#include "placeeditor.h"
#include "ui_placeeditor.h"

#include "model/editableItems/place.h"

namespace Sabrina {

const QString PlaceEditor::PLACE_EDITOR_TYPE_ID = "sabrina_jdr_place_editor";

PlaceEditor::PlaceEditor(QWidget *parent) :
	Aline::EditableItemEditor(parent),
	ui(new Ui::PlaceEditor),
	_current_place(nullptr)
{
	ui->setupUi(this);

	connect(ui->textEdit_background, &QTextEdit::textChanged, this, &PlaceEditor::onBackgroundTextChanged);
}

PlaceEditor::~PlaceEditor()
{
	delete ui;
}

QString PlaceEditor::getTypeId() const {
	return PLACE_EDITOR_TYPE_ID;
}
QString PlaceEditor::getTypeName() const {
	return tr("Éditeur de lieux");
}

QStringList PlaceEditor::editableTypes() const {
	return {Place::PLACE_TYPE_ID};
}

bool PlaceEditor::effectivelySetEditedItem(Aline::EditableItem* item) {

	Place* place = qobject_cast<Place*>(item);

	if (place == nullptr) {
		return false;
	}

	if (_current_place != nullptr) {

		disconnect(ui->lineEdit_name, &QLineEdit::textEdited, _current_place, &Place::setObjectName);

		if (_nameWatchConnection) {
			disconnect(_nameWatchConnection);
		}
	}

	ui->lineEdit_name->setText(place->objectName());
	ui->textEdit_background->setPlainText(place->placeDescription());

	connect(ui->lineEdit_name, &QLineEdit::textEdited, place, &Place::setObjectName);

	_nameWatchConnection = connect(place, &Place::objectNameChanged, [this] (QString const& text) {if (text != ui->lineEdit_name->text()) ui->lineEdit_name->setText(text);});

	_current_place = place;

	return true;

}

void PlaceEditor::onBackgroundTextChanged() {

	if (_current_place != nullptr) {
		_current_place->setplaceDescription(ui->textEdit_background->document()->toPlainText());
	}

}

PlaceEditor::PlaceEditorFactory::PlaceEditorFactory(QObject* parent) :
	Aline::EditorFactory(parent)
{

}
Aline::Editor* PlaceEditor::PlaceEditorFactory::createItem(QWidget* parent) const {
	return new PlaceEditor(parent);
}

} // namespace Sabrina
