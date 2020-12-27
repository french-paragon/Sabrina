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

#include "comicscript.h"

#include <QFontMetrics>
#include <QPainter>

namespace Sabrina {

ComicScriptStyle::ComicScriptStyle(QObject* parent) :
	AbstractTextNodeStyle(parent)
{

}

QFont ComicScriptStyle::getFont(TextLine* line) const {

	Q_UNUSED(line);
	return QFont("Monospace", 12);
}


ComicScriptTitleStyle::ComicScriptTitleStyle(QObject* parent) :
	ComicScriptStyle(parent)
{

}

int ComicScriptTitleStyle::typeId() const {
	return MAIN;
}
QFont ComicScriptTitleStyle::getFont(TextLine* line) const {
	Q_UNUSED(line);
	return QFont("Monospace", 18);
}
QMargins ComicScriptTitleStyle::getNodeMargins(TextNode* node) const {

	if (node->isRootNode()) {
		return QMargins(0,0,0,5);
	}

	return QMargins(0,2*getLineHeight(node->lineAt(0)),0,5);
}

ComicScriptDescribedStyle::ComicScriptDescribedStyle(QObject* parent) :
	ComicScriptStyle(parent)
{

}

QMargins ComicScriptDescribedStyle::getLineMargins(TextLine* line) const {

	QFont f = getFont(line);
	f.setBold(true);

	QFontMetrics fm(f);
	QString descr = getDescr(line->nodeParent()) + ": ";

	return QMargins(fm.horizontalAdvance(descr), 0, 0, 0);
}

void ComicScriptDescribedStyle::renderNode(TextNode* node,
										   const QPointF &offset,
										   int availableWidth,
										   QPainter & painter,
										   int cursorLine,
										   int cursorPos) {

	QMargins m = getNodeMargins(node);

	QFont f = getFont(node->lineAt(0));
	f.setBold(true);

	QFontMetrics fm(f);
	QString descr = getDescr(node) + ": ";

	painter.setFont(f);
	painter.drawText(QPointF(offset.x() + m.left(), offset.y() + fm.ascent() + m.top()), descr);

	ComicScriptStyle::renderNode(node, offset, availableWidth, painter, cursorLine, cursorPos);

}

ComicScriptPageStyle::ComicScriptPageStyle(QObject* parent) :
	ComicScriptDescribedStyle(parent)
{

}

int ComicScriptPageStyle::typeId() const {
	return PAGE;
}

QMargins ComicScriptPageStyle::getNodeMargins(TextNode* node) const {

	return QMargins(0, 2*getLineHeight(node->lineAt(0)), 0, 0);
}
QString ComicScriptPageStyle::getDescr(TextNode* node) const {
	return tr("Page %1").arg(node->nodeIndex()+1);
}

ComicScriptPanelStyle::ComicScriptPanelStyle(QObject* parent) :
	ComicScriptDescribedStyle(parent)
{

}

int ComicScriptPanelStyle::typeId() const {
	return PANEL;
}

QMargins ComicScriptPanelStyle::getNodeMargins(TextNode* node) const {

	return QMargins(15, 1.5*getLineHeight(node->lineAt(0)), 0, 0);
}
QString ComicScriptPanelStyle::getDescr(TextNode* node) const {
	return tr("Panel %1").arg(node->nodeIndex()+1);
}

ComicScriptCaptionStyle::ComicScriptCaptionStyle(QObject* parent) :
	ComicScriptStyle(parent)
{

}

int ComicScriptCaptionStyle::typeId() const {
	return CAPTION;
}

QMargins ComicScriptCaptionStyle::getNodeMargins(TextNode* node) const {
	return QMargins(35, getLineHeight(node->lineAt(0))/2, 0, 0);
}


ComicScriptDialogStyle::ComicScriptDialogStyle(QObject* parent) :
	ComicScriptStyle(parent)
{

}

int ComicScriptDialogStyle::typeId() const {
	return DIALOG;
}

int ComicScriptDialogStyle::expectedNodeNbTextLines() const {
	return 2;
}

QString ComicScriptDialogStyle::getPrefix(TextLine* line) const {
	if (line->nodeParent()->lineAt(1) == line) {
		return QString("\u2013\u00A0");
	}

	return "";
}
QString ComicScriptDialogStyle::getSuffix(TextLine* line) const {
	if (line->nodeParent()->lineAt(0) == line) {
		return QString("\u202F:");
	}

	return "";
}

QFont ComicScriptDialogStyle::getFont(TextLine* line) const {

	QFont f = ComicScriptStyle::getFont(line);
	f.setBold(line->nodeParent()->lineAt(0) == line);
	return f;

}
int ComicScriptDialogStyle::getTabulation(TextLine* line) const {
	if (line->nodeParent()->lineAt(1) == line) {
		QFont f = getFont(line);

		QFontMetrics fm(f);

		return -fm.horizontalAdvance(getPrefix(line));
	}

	return 0;
}

QMargins ComicScriptDialogStyle::getNodeMargins(TextNode* node) const {
	return QMargins(35, getLineHeight(node->lineAt(0))/2, 0, 0);
}
QMargins ComicScriptDialogStyle::getLineMargins(TextLine* line) const {

	TextLine* d = line->nodeParent()->lineAt(1);

	QFont f = getFont(line);
	QFontMetrics fm(f);

	QMargins r(fm.horizontalAdvance(getPrefix(d)), (line != line->nodeParent()->lineAt(0)) ? getLineHeight(line)/3 : 0, 0, 0);
	return r;
}


ComicScriptTextStyleManager::ComicScriptTextStyleManager(QObject* parent) :
	TextStyleManager(parent)
{
	registerStyle(new ComicScriptTitleStyle(this));
	registerStyle(new ComicScriptPageStyle(this));
	registerStyle(new ComicScriptPanelStyle(this));
	registerStyle(new ComicScriptCaptionStyle(this));
	registerStyle(new ComicScriptDialogStyle(this));
}

QMap<Qt::KeyboardModifiers, TextStyleManager::NextNodeStyleAndPos>
ComicScriptTextStyleManager::getNextNodeStyleAndPos(int code) const {

	QMap<Qt::KeyboardModifiers, NextNodeStyleAndPos> m;

	m.insert(Qt::AltModifier|Qt::ControlModifier, {ComicScriptStyle::PAGE, LevelJump::UnderRoot});

	switch (code) {

	case ComicScriptStyle::MAIN:
		m.insert(Qt::NoModifier, {ComicScriptStyle::PAGE, LevelJump::Below});
		break;

	case ComicScriptStyle::PAGE:
		m.insert(Qt::NoModifier, {ComicScriptStyle::PANEL, LevelJump::Below});
		m.insert(Qt::ControlModifier, {ComicScriptStyle::PAGE, LevelJump::After});
		break;

	case ComicScriptStyle::PANEL:
		m.insert(Qt::NoModifier, {ComicScriptStyle::CAPTION, LevelJump::Below});
		m.insert(Qt::ControlModifier, {ComicScriptStyle::PANEL, LevelJump::After});
		m.insert(Qt::ShiftModifier, {ComicScriptStyle::PAGE, LevelJump::Above});
		break;

	case ComicScriptStyle::CAPTION:
		m.insert(Qt::NoModifier, {ComicScriptStyle::DIALOG, LevelJump::After});
		m.insert(Qt::ControlModifier, {ComicScriptStyle::CAPTION, LevelJump::After});
		m.insert(Qt::ShiftModifier, {ComicScriptStyle::PANEL, LevelJump::Above});
		break;

	case ComicScriptStyle::DIALOG:
		m.insert(Qt::NoModifier, {ComicScriptStyle::DIALOG, LevelJump::After});
		m.insert(Qt::ControlModifier, {ComicScriptStyle::CAPTION, LevelJump::After});
		m.insert(Qt::ShiftModifier, {ComicScriptStyle::PANEL, LevelJump::Above});
		break;
	}

	return m;

}

} // namespace Sabrina
