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

#include "abstracttextstyle.h"

#include "textnode.h"

#include <QFontMetrics>

#include <cmath>

namespace Sabrina {

AbstractTextNodeStyle::AbstractTextNodeStyle(QObject *parent) : QObject(parent)
{

}

QString AbstractTextNodeStyle::getPrefix(TextLine* line) const {
	Q_UNUSED(line);
	return "";
}
QString AbstractTextNodeStyle::getSuffix(TextLine* line) const {
	Q_UNUSED(line);
	return "";
}

int AbstractTextNodeStyle::getLineHeight(TextLine* line) const {
	QFont f = getFont(line);
	QFontMetrics fm(f);

	return fm.ascent() + fm.descent();
}

QMargins AbstractTextNodeStyle::getLineMargins(TextLine* line) const {
	Q_UNUSED(line);
	return QMargins(0,0,0,0);
}
int AbstractTextNodeStyle::getTabulation(TextLine* line) const {
	Q_UNUSED(line);
	return 0;
}

const QTextLayout& AbstractTextNodeStyle::lineLayout(TextLine* line) const {

	return *(_cache[line].layout.get());

}

QMargins AbstractTextNodeStyle::getNodeMargins(TextNode* node) const {
	return QMargins(0, getLineHeight(node->lineAt(0)), 0, 0);
}

int AbstractTextNodeStyle::expectedNodeNbTextLines() const {
	return 1;
}

void AbstractTextNodeStyle::layNodeOut(TextNode* node, int availableWidth) const {

	if (node->nbTextLines() != expectedNodeNbTextLines()) {
		node->setNbTextLines(expectedNodeNbTextLines());
	}

	QMargins m = getNodeMargins(node);

	int h = m.top();
	int x = m.left();

	int act_width = availableWidth - m.left() - m.right();

	for (int i = 0; i < node->nbTextLines(); i++) {
		TextLine* l = node->lineAt(i);

		layOutLine(l, QPointF(x, h), act_width);

		h += lineLayout(l).boundingRect().height() + getLineMargins(l).bottom();
	}
}

void AbstractTextNodeStyle::renderNode(TextNode* node,
									   const QPointF &offset,
									   int availableWidth,
									   QPainter & painter,
									   TextNode::NodeCoordinate selectionStart,
									   TextNode::NodeCoordinate selectionEnd,
									   int cursorLine,
									   int cursorPos,
									   QTextCharFormat const& selectionFormat) {

	layNodeOut(node, availableWidth);

	int sLineIndex = (selectionStart.lineIndex < 0) ?  node->nbTextLines() + selectionStart.lineIndex : selectionStart.lineIndex;
	int eLineIndex = (selectionEnd.lineIndex < 0) ?  node->nbTextLines() + selectionEnd.lineIndex : selectionEnd.lineIndex;

	bool selectionStarted = false;

	for (int i = 0; i < node->nbTextLines(); i++) {

		int lSelStart = 0;
		int lSelEnd = (selectionStarted) ? -1 : 0;

		if (sLineIndex == i) {
			lSelStart = selectionStart.linePos;
			selectionStarted = true;
			lSelEnd = -1;
		}
		if (eLineIndex == i) {
			lSelEnd = selectionEnd.linePos;
			selectionStarted = false;
		}

		if (cursorLine == i) {
			renderLine(node->lineAt(i), offset, painter, lSelStart, lSelEnd, cursorPos, selectionFormat);
		} else {
			renderLine(node->lineAt(i), offset, painter, lSelStart, lSelEnd, -1, selectionFormat);
		}
	}

}


int AbstractTextNodeStyle::nodeHeight(TextNode* node, int availableWidth) const {

	layNodeOut(node, availableWidth);

	int mH = 0;

	for (int i = 0; i < node->nbTextLines(); i++) {
		const QTextLayout& layout = lineLayout(node->lineAt(i));
		int dH = static_cast<int>(std::ceil(layout.boundingRect().height() + layout.boundingRect().y()));
		dH += getLineMargins(node->lineAt(i)).bottom();

		if (dH > mH) {
			mH = dH;
		}
	}

	return mH + getNodeMargins(node).bottom();
}


int AbstractTextNodeStyle::nodeHeightBetweenLines(TextNode* node, int availableWidth, int pLineStart, int pLineEnd) const {

	layNodeOut(node, availableWidth);

	int n = 0;
	qreal sH = 0;
	qreal eH = 0;

	int lineStart = pLineStart;

	if (lineStart < 0) {
		lineStart = nodeNbLayoutLines(node, availableWidth) + lineStart;
	}

	int lineEnd = pLineEnd;

	if (lineEnd < 0) {
		lineEnd = nodeNbLayoutLines(node, availableWidth) + lineEnd;
	}

	for (int i = 0; i < node->nbTextLines(); i++) {
		const QTextLayout& layout = lineLayout(node->lineAt(i));

		for (int i = 0; i < layout.lineCount(); i++) {
			QTextLine l = layout.lineAt(i);

			if (n == lineStart) {
				sH = l.rect().top();
			}

			eH = l.rect().bottom();
			if (n == lineEnd) {
				break;
			}

			n++;
		}
	}

	return eH - sH;

}

int AbstractTextNodeStyle::nodeNbLayoutLines(TextNode* node, int availableWidth) const {

	layNodeOut(node, availableWidth);

	int n = 0;

	for (int i = 0; i < node->nbTextLines(); i++) {
		const QTextLayout& layout = lineLayout(node->lineAt(i));
		n += layout.lineCount();
	}

	return n;
}
int AbstractTextNodeStyle::nodeNbLayoutLines(TextNode* node, int availableWidth, int availableHeight) const {

	layNodeOut(node, availableWidth);

	int n = 0;
	int mH = 0;

	for (int i = 0; i < node->nbTextLines(); i++) {
		const QTextLayout& layout = lineLayout(node->lineAt(i));

		for (int i = 0; i < layout.lineCount(); i++) {
			QTextLine l = layout.lineAt(i);

			mH = l.rect().bottom();

			if (mH >= availableHeight) {
				return n;
			}

			n++;
		}
	}

	return n;

}

void AbstractTextNodeStyle::clearCache() {
	_cache.clear();
}

void AbstractTextNodeStyle::renderLine(TextLine* line,
				const QPointF &offset,
				QPainter & painter,
				int selectionStart,
				int selectionEnd,
				int cursorStart,
				const QTextCharFormat &selectionFormat) const {

	QVector<QTextLayout::FormatRange> selections;

	int sStart = selectionStart < 0 ? 0 : selectionStart;
	int sEnd = selectionEnd < 0 ? line->getText().length() + selectionEnd + 1 : selectionEnd;
	int l = sEnd - sStart;

	if (l > 0) {
		QTextLayout::FormatRange range;
		range.start = sStart + getPrefix(line).size();
		range.length = l;
		range.format = selectionFormat;
		selections.push_back(range);
	}

	_cache[line].layout->draw(&painter, offset, selections);

	if (cursorStart >= 0) {

		int p = getPrefix(line).length();
		int l = line->getText().length();

		int position = cursorStart;
		if (l < position) {
			position = l;
		}
		position += p;

		_cache[line].layout->drawCursor(&painter, offset, position);
	}

}

void AbstractTextNodeStyle::layOutLine(TextLine* line,
						const QPointF &offset,
						int availableWidth) const {

	QTextLayout & layout = *(_cache[line].layout.get());

	if (_cache[line].usedAvailableWidth == availableWidth and
		layout.text() == getPrefix(line) + line->getText() + getSuffix(line)) {
		return;
	}

	QMargins m = getLineMargins(line);

	layout.setText(getPrefix(line) + line->getText() + getSuffix(line));
	layout.setFont(getFont(line));

	float height = offset.y() + m.top();
	float lineHeight = getLineHeight(line);

	int actualWidth = availableWidth - m.left() - m.right();

	layout.beginLayout();
	QTextLine textline = layout.createLine();

	textline.setLineWidth(actualWidth - getTabulation(line));
	textline.setPosition(QPointF(offset.x() + getTabulation(line) + m.left(), height));
	height += lineHeight;

	while (1) {
		textline = layout.createLine();
		if (!textline.isValid())
			break;

		textline.setLineWidth(availableWidth);
		textline.setPosition(QPointF(offset.x() + m.left(), height));
		height += lineHeight;
	}
	layout.endLayout();

}

} // namespace Sabrina
