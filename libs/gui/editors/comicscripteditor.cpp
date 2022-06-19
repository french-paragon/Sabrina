/*
This file is part of the project Sabrina
Copyright (C) 2020  Paragon <french.paragon@gmail.com>

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

#include "comicscripteditor.h"
#include "ui_comicscripteditor.h"

#include "model/editableItems/comicscript.h"
#include "text/comicscript.h"
#include "text/exportfunctions.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include <QSettings>
#define COMIC_EDITOR_HIGHLIGHT_SETTING "comic_editor_highlight_activetext"
#define COMIC_EDITOR_SELECTBYBLOCK_SETTING "comic_editor_select_restricted_to_blocks"
#define COMIC_EDITOR_EXPORT_SETTING "comic_editor_export_path"

namespace Sabrina {

const QString ComicscriptEditor::COMICSCRIPT_EDITOR_TYPE_ID = "sabrina_comic_script_editor";

ComicscriptEditor::ComicscriptEditor(QWidget *parent) :
	Aline::EditableItemEditor(parent),
	ui(new Ui::ComicscriptEditor),
	_currentScript(nullptr)
{
	ui->setupUi(this);

	connect(ui->editWidget, &ComicscriptEditWidget::currentLineChanged, this, &ComicscriptEditor::checkAddButtonsActivation);

	connect(ui->addPageButton, &QPushButton::pressed, ui->editWidget, &ComicscriptEditWidget::addPage);
	connect(ui->addPanelButton, &QPushButton::pressed, ui->editWidget, &ComicscriptEditWidget::addPanel);
	connect(ui->addCaptionButton, &QPushButton::pressed, ui->editWidget, &ComicscriptEditWidget::addCaption);
	connect(ui->addDialogButton, &QPushButton::pressed, ui->editWidget, &ComicscriptEditWidget::addDialog);

	connect(ui->exportPdfButton, &QPushButton::pressed, this, &ComicscriptEditor::exportPdf);

	connect(ui->nameEdit, &QLineEdit::textChanged, this, &ComicscriptEditor::onNameChanged);
	connect(ui->synopsisEdit, &QTextEdit::textChanged, this, &ComicscriptEditor::onSynopsisChanged);

	connect(ui->highlightActiveTextPartButton, &QPushButton::toggled, this, &ComicscriptEditor::onHighlightTextButtonToggled);

	QSettings settings;

	if (settings.contains(COMIC_EDITOR_HIGHLIGHT_SETTING)) {
		ui->highlightActiveTextPartButton->setChecked(settings.value(COMIC_EDITOR_HIGHLIGHT_SETTING).toBool());
		onHighlightTextButtonToggled(); //depending on the state of the button the signal might have not propagated to the corresponding widgets if the function is not called explicitely.
	} else {
		ui->highlightActiveTextPartButton->setChecked(true);
	}

	bool selectByBlock = false;
	if (settings.contains(COMIC_EDITOR_SELECTBYBLOCK_SETTING)) {
		selectByBlock = settings.value(COMIC_EDITOR_HIGHLIGHT_SETTING).toBool();
	}

	ui->editWidget->selectFullBlocks(selectByBlock);
	if (selectByBlock) {
		ui->blockSelectButton->setChecked(true);
	} else {
		ui->freeSelectButton->setChecked(true);
	}

	connect(ui->selectionMode, &QButtonGroup::idToggled, this, &ComicscriptEditor::onSelectionModeToggled);
}

ComicscriptEditor::~ComicscriptEditor()
{
	delete ui;
}

QString ComicscriptEditor::getTypeId() const {
	return COMICSCRIPT_EDITOR_TYPE_ID;
}
QString ComicscriptEditor::getTypeName() const {
	return tr("Comic scripts editor");
}

/*!
 * \brief editableType allow to get the types the editor might support
 * \return the type id string, or empty string if none is supported.
 */
QStringList ComicscriptEditor::editableTypes() const {
	return {Comicscript::COMICSTRIP_TYPE_ID};
}

bool ComicscriptEditor::effectivelySetEditedItem(Aline::EditableItem* item) {
	Comicscript* script = qobject_cast<Comicscript*>(item);

	if (script == nullptr) {
		return false;
	}

	if (_currentScript != nullptr) {
		disconnect(_currentScript, &Comicscript::objectNameChanged, this, &ComicscriptEditor::onScriptNameChanged);
		disconnect(_currentScript, &Comicscript::synopsisChanged, this, &ComicscriptEditor::onScriptSynopsisChanged);
	}

	_currentScript = nullptr;

	ui->nameEdit->setText(script->objectName());
	ui->synopsisEdit->setText(script->synopsis());

	_currentScript = script;

	connect(_currentScript, &Comicscript::objectNameChanged, this, &ComicscriptEditor::onScriptNameChanged);
	connect(_currentScript, &Comicscript::synopsisChanged, this, &ComicscriptEditor::onScriptSynopsisChanged);

	ui->editWidget->setCurrentScript(script);

	return true;
}

void ComicscriptEditor::onHighlightTextButtonToggled() {
	ui->editWidget->highlightCurrentNode(ui->highlightActiveTextPartButton->isChecked());

	QSettings settings;

	settings.setValue(COMIC_EDITOR_HIGHLIGHT_SETTING, ui->highlightActiveTextPartButton->isChecked());
}
void ComicscriptEditor::onSelectionModeToggled() {
	if(ui->freeSelectButton->isChecked()) {
		ui->editWidget->selectFullBlocks(false);
	} else if (ui->blockSelectButton->isChecked()) {
		ui->editWidget->selectFullBlocks(true);
	}
}

void ComicscriptEditor::checkAddButtonsActivation() {

	int code = ui->editWidget->currentLineStyleId();

	bool enablePages = ui->editWidget->hasScript();
	bool enablePanels = ui->editWidget->hasScript() and (code != ComicScriptStyle::MAIN);
	bool enableCaptions = ui->editWidget->hasScript() and (code >= ComicScriptStyle::PANEL);
	bool enableDialogs = ui->editWidget->hasScript() and (code >= ComicScriptStyle::PANEL);

	ui->addPageButton->setEnabled(enablePages);
	ui->addPanelButton->setEnabled(enablePanels);
	ui->addCaptionButton->setEnabled(enableCaptions);
	ui->addDialogButton->setEnabled(enableDialogs);

	ui->addPageButton->setVisible(enablePages);
	ui->addPanelButton->setVisible(enablePanels);
	ui->addCaptionButton->setVisible(enableCaptions);
	ui->addDialogButton->setVisible(enableDialogs);
}

void ComicscriptEditor::onNameChanged() {
	if (_currentScript != nullptr) {
		_currentScript->setObjectName(ui->nameEdit->text());
	}
}
void ComicscriptEditor::onSynopsisChanged() {
	if (_currentScript != nullptr) {
		QString txt = ui->synopsisEdit->toPlainText();
		_currentScript->setSynopsis(txt);
	}
}

void ComicscriptEditor::onScriptNameChanged() {
	if (_currentScript == nullptr) {
		return;
	}

	QString txt = _currentScript->objectName();

	if (txt != ui->nameEdit->text()) {
		ui->nameEdit->setText(txt);
	}
}
void ComicscriptEditor::onScriptSynopsisChanged() {
	if (_currentScript == nullptr) {
		return;
	}

	QString txt = _currentScript->synopsis();

	if (txt != ui->synopsisEdit->toPlainText()) {
		ui->synopsisEdit->setText(txt);
	}
}

void ComicscriptEditor::exportPdf() {

	QString dir;

	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);

	if (dirs.size() > 0) {
		dir = dirs.first();
	}

	QSettings settings;

	if (settings.contains(COMIC_EDITOR_EXPORT_SETTING)) {
		QString exportDir = settings.value(COMIC_EDITOR_EXPORT_SETTING).toString();

		QFileInfo info(exportDir);

		if (info.exists() and info.isDir()) {
			dir = exportDir;
		}
	}

	bool ok = true;
	QString file;
	QDir outDir;

	do {
		ok = true;

		file = QFileDialog::getSaveFileName(this, tr("Save script to pdf"), dir, "pdf files (*.pdf)");

		if (file.isEmpty()) {
			return;
		}

		QFileInfo outFileInfo(file);

		outDir = outFileInfo.dir();

		if (!outDir.exists()) {
			QMessageBox::warning(this, tr("Imposible to save file"), tr("Target directory do not exist !"));
			return;
		}

		if (!file.endsWith(".pdf")) {
			QMessageBox::warning(this, tr("Imposible to save file"), tr("The file is not a pdf !"));
			return;
		}

	} while (!ok);

	ComicScriptTextStyleManager comicstyle;

	QPageLayout layout;
	layout.setUnits(QPageLayout::Unit::Point);
	layout.setOrientation(QPageLayout::Orientation::Portrait);
	layout.setPageSize(QPageSize(QPageSize::A4), QMargins(40.0, 40.0, 40.0, 40.0));

	ok = savePdf(ui->editWidget->getCurrentNode(), &comicstyle, layout, file);
	settings.setValue(COMIC_EDITOR_EXPORT_SETTING, outDir.path());

	if (!ok) {
		QMessageBox::warning(this, tr("Imposible to save file"), tr("Unknown error !"));
		return;
	}

}

ComicscriptEditor::ComicscriptEditorFactory::ComicscriptEditorFactory(QObject* parent) :
	Aline::EditorFactory(parent)
{

}

Aline::Editor* ComicscriptEditor::ComicscriptEditorFactory::createItem(QWidget* parent) const {
	return new ComicscriptEditor(parent);
}

} //namespace Sabrina
