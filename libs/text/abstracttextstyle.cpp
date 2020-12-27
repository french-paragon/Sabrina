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
									   int cursorLine,
									   int cursorPos) {

	layNodeOut(node, availableWidth);

	for (int i = 0; i < node->nbTextLines(); i++) {
		if (cursorLine == i) {
			renderLine(node->lineAt(i), offset, painter, cursorPos);
		} else {
			renderLine(node->lineAt(i), offset, painter);
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

void AbstractTextNodeStyle::renderLine(TextLine* line,
				const QPointF &offset,
				QPainter & painter,
				int cursorStart) const {

	_cache[line].layout->draw(&painter, offset);

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
