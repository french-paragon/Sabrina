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

#include "comicscripteditwidget.h"

#include "model/editableItems/comicscript.h"

#include <QPainter>
#include <QPaintEvent>
#include <QTextLayout>

namespace Sabrina {

ComicscriptEditWidget::ComicscriptEditWidget(QWidget *parent) :
	TextEditWidget(parent)
{
	ComicScriptTextStyleManager* st = new ComicScriptTextStyleManager(this);
	setStyleManager(st);
}

ComicscriptEditWidget::~ComicscriptEditWidget() {

}

void ComicscriptEditWidget::setCurrentScript(Comicscript* script) {
	setCurrentScript(script->document());
}

void ComicscriptEditWidget::setCurrentScript(TextNode *root) {

	if (root->styleId() == ComicScriptStyle::MAIN) {
		TextEditWidget::setCurrentScript(root);
	}

}

void ComicscriptEditWidget::addPage() {

	TextNode* n = getCurrentNode();

	if (n->isRootNode()) {
		n->insertNodeBelow(ComicScriptStyle::PAGE);
	} else if (n->styleId() == ComicScriptStyle::PAGE) {
		n->insertNodeAfter(ComicScriptStyle::PAGE);
	} else {
		n->insertNodeSubRoot(ComicScriptStyle::PAGE);
	}
}

void ComicscriptEditWidget::addPanel() {

	TextNode* n = getCurrentNode();

	if (n->styleId() == ComicScriptStyle::PAGE) {
		n->insertNodeBelow(ComicScriptStyle::PANEL);
	} else if (n->styleId() == ComicScriptStyle::PANEL) {
		n->insertNodeAfter(ComicScriptStyle::PANEL);
	} else if (n->styleId() > ComicScriptStyle::PANEL) {
		n->insertNodeAbove(ComicScriptStyle::PANEL);
	}
}

void ComicscriptEditWidget::addCaption() {

	TextNode* n = getCurrentNode();

	if (n->styleId() == ComicScriptStyle::PANEL) {
		n->insertNodeBelow(ComicScriptStyle::CAPTION);
	} else if (n->styleId() > ComicScriptStyle::PANEL) {
		n->insertNodeAfter(ComicScriptStyle::CAPTION);
	}

}

void ComicscriptEditWidget::addDialog() {

	TextNode* n = getCurrentNode();

	if (n->styleId() == ComicScriptStyle::PANEL) {
		n->insertNodeBelow(ComicScriptStyle::DIALOG);
	} else if (n->styleId() > ComicScriptStyle::PANEL) {
		n->insertNodeAfter(ComicScriptStyle::DIALOG);
	}
}

void ComicscriptEditWidget::highlightCurrentNode(bool highlight) {
	TextEditWidget::setHighlightCurrent(highlight);
}

void ComicscriptEditWidget::selectFullBlocks(bool selectFullBlocks) {
	TextEditWidget::SelectionMode selMode = TextEditWidget::SelectionMode::Text;
	if (selectFullBlocks) {
		selMode = TextEditWidget::SelectionMode::FullLeveldMultiNodes;
	}
	TextEditWidget::setSelectionMode(selMode);
}

} // namespace Sabrina
