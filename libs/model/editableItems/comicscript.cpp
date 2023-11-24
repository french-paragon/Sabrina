/*
This file is part of the project Sabrina
Copyright (C) 2020-2023  Paragon <french.paragon@gmail.com>

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

#include "comicscript.h"

#include <Aline/control/app.h>
#include <Aline/utils/jsonutils.h>

namespace Sabrina {

const QString Comicscript::COMICSTRIP_TYPE_ID = "sabrina_comic_script";
const QString Comicscript::COMICSTRIP_TEXT_ID = "ComicScriptText";


const QString Comicscript::TEXT_ID = "txt";
const QString Comicscript::CHARACTER_ID = "character";
const QString Comicscript::CHILDREN_ID = "children";

Comicscript::Comicscript(QString ref, Aline::EditableItemManager *parent) :
	EditableItem(ref, parent)
{
	_document = new TextNode(this);
	_document->setStyleId(ComicScriptStyle::MAIN);
	_document->lineAt(0)->setText(tr("Title"));

	connect(this, &Comicscript::titleChanged, this, &Comicscript::newUnsavedChanges);

	connect(_document, &TextNode::nodeAdded, this, &Comicscript::newUnsavedChanges);
	connect(_document, &TextNode::nodeRemoved, this, &Comicscript::newUnsavedChanges);
	connect(_document, &TextNode::nodeEdited, this, &Comicscript::newUnsavedChanges);
	connect(_document, &TextNode::nodeLineLayoutChanged, this, &Comicscript::newUnsavedChanges);
}

QString Comicscript::getTypeId() const {
	return COMICSTRIP_TYPE_ID;
}
QString Comicscript::getTypeName() const {
	return tr("Comic script");
}

QString Comicscript::iconInternalUrl() const {
	return ":/icons/icons/editable_item_comic.svg";
}

QString Comicscript::getTitle() const
{
	return _document->lineAt(0)->getText();
}

void Comicscript::setTitle(const QString &title)
{
	if (title != getTitle()) {
		_document->lineAt(0)->setText(title);
		emit titleChanged(title);
	}
}

QString Comicscript::synopsis() const
{
	return _synopsis;
}

void Comicscript::setSynopsis(const QString &synopsis)
{
	if (_synopsis != synopsis) {
		_synopsis = synopsis;
		Q_EMIT synopsisChanged(synopsis);
	}
}


TextNode* Comicscript::document() {
	return _document;
}

void Comicscript::treatDeletedRef(QString deletedRef) {
	Q_UNUSED(deletedRef);
}
void Comicscript::treatChangedRef(QString oldRef, QString newRef) {
	Q_UNUSED(oldRef);
	Q_UNUSED(newRef);
}


Comicscript::ComicstripFactory::ComicstripFactory(QObject *parent) :
	Aline::EditableItemFactory(parent)
{

}
Aline::EditableItem* Comicscript::ComicstripFactory::createItem(QString ref, Aline::EditableItemManager* parent) const {
	return new Comicscript(ref, parent);
}

void extractPageFromJson(TextNode* doc,  QJsonObject const& obj) {

	TextNode* page = doc->insertNodeBelow(ComicScriptStyle::PAGE, -1);

	if (obj.contains(Comicscript::TEXT_ID)) {
		page->lineAt(0)->setText(obj.value(Comicscript::TEXT_ID).toString());
	}

	if (obj.contains(Comicscript::CHILDREN_ID)) {

		QJsonValue v = obj.value(Comicscript::CHILDREN_ID);

		if (v.isArray()) {

			const QJsonArray panels = v.toArray();

			for (QJsonValue const& p : panels) {
				QJsonObject panel = p.toObject();

				TextNode* panelNode = page->insertNodeBelow(ComicScriptStyle::PANEL, -1);

				if (panel.contains(Comicscript::TEXT_ID)) {
					panelNode->lineAt(0)->setText(panel.value(Comicscript::TEXT_ID).toString());
				}


				if (panel.contains(Comicscript::CHILDREN_ID)) {

					v = panel.value(Comicscript::CHILDREN_ID);

					if (v.isArray()) {

						const QJsonArray blocks = v.toArray();

						for (QJsonValue const& b : blocks) {
							QJsonObject block = b.toObject();

							int nodeType = (block.contains(Comicscript::CHARACTER_ID)) ? ComicScriptStyle::DIALOG : ComicScriptStyle::CAPTION;
							TextNode* blockNode = panelNode->insertNodeBelow(nodeType, -1);

							if (nodeType == ComicScriptStyle::DIALOG) {
								blockNode->setNbTextLines(2);
								blockNode->lineAt(0)->setText(block.value(Comicscript::CHARACTER_ID).toString());

								if (block.contains(Comicscript::TEXT_ID)) {
									blockNode->lineAt(1)->setText(block.value(Comicscript::TEXT_ID).toString());
								}
							} else {
								if (block.contains(Comicscript::TEXT_ID)) {
									blockNode->lineAt(0)->setText(block.value(Comicscript::TEXT_ID).toString());
								}
							}
						}
					}
				}
			}

		}
	}

}

void Comicscript::extractComicScriptFromJson(Aline::EditableItem* script, QJsonObject const& obj, bool blockChangeTracks) {

	Aline::JsonUtils::extractItemData(script,
									  obj,
									  Aline::App::getAppEditableItemFactoryManager(),
									  {COMICSTRIP_TEXT_ID},
									  blockChangeTracks);

	if (script->getTypeId() == Comicscript::COMICSTRIP_TYPE_ID) {
		script->blockChangeDetection(blockChangeTracks);

		Comicscript* comicscript = qobject_cast<Comicscript*>(script);
		TextNode* doc = comicscript->document();

		if (obj.contains(COMICSTRIP_TEXT_ID)) {
			QJsonValue v = obj.value(COMICSTRIP_TEXT_ID);
			if (v.isArray()) {
				const QJsonArray pages = v.toArray();

				for (QJsonValue const& v : pages) {
					extractPageFromJson(doc, v.toObject());
				}
			}
		}

		script->blockChangeDetection(false);
	}

}

QJsonObject encapsulatePage(TextNode* page) {

	QJsonObject p;

	if (!page->lineAt(0)->getText().isEmpty()) {
		p.insert(Comicscript::TEXT_ID, page->lineAt(0)->getText());
	}

	if (page->children().size() > 0) {
		QJsonArray panels;

		for (TextNode* panel : page->childNodes()) {
			QJsonObject pan;

			if (!panel->lineAt(0)->getText().isEmpty()) {
				pan.insert(Comicscript::TEXT_ID, panel->lineAt(0)->getText());
			}

			if (panel->children().size() > 0) {

				QJsonArray blocks;

				for (TextNode* block : panel->childNodes()) {
					QJsonObject blk;

					int nodeType = block->styleId();

					if (nodeType == ComicScriptStyle::DIALOG) {
						blk.insert(Comicscript::CHARACTER_ID, block->lineAt(0)->getText());
						blk.insert(Comicscript::TEXT_ID, block->lineAt(1)->getText());
					} else {
						blk.insert(Comicscript::TEXT_ID, block->lineAt(0)->getText());
					}

					blocks.push_back(blk);
				}

				pan.insert(Comicscript::CHILDREN_ID, blocks);
			}

			panels.push_back(pan);
		}

		p.insert(Comicscript::CHILDREN_ID, panels);
	}

	return p;

}

QJsonObject Comicscript::encapsulateComicScriptToJson(Aline::EditableItem* script) {

	QJsonObject obj = Aline::JsonUtils::encapsulateItemToJson(script);

	Comicscript* comicscript = qobject_cast<Comicscript*>(script);
	TextNode* doc = comicscript->document();

	if (comicscript != nullptr) {
		QJsonArray pages;

		for (TextNode* page : doc->childNodes()) {
			pages.push_back(encapsulatePage(page));
		}

		obj.insert(COMICSTRIP_TEXT_ID, QJsonValue(pages));
	}

	return obj;

}


} // namespace Sabrina
