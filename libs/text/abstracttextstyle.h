#ifndef SABRINA_ABSTRACTTEXTSTYLE_H
#define SABRINA_ABSTRACTTEXTSTYLE_H

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

#include <QObject>
#include <QMargins>
#include <QTextLayout>

#include "textnode.h"

#include "./text_global.h"

namespace Sabrina {

class TextLine;

class SABRINA_TEXT_EXPORT AbstractTextNodeStyle : public QObject
{
	Q_OBJECT
public:
	explicit AbstractTextNodeStyle(QObject *parent = nullptr);

	virtual int typeId() const = 0;
	virtual QString typeName() const = 0;

	virtual QString getPrefix(TextLine* line) const;
	virtual QString getSuffix(TextLine* line) const;

	virtual QFont getFont(TextLine* line) const = 0;
	virtual int getLineHeight(TextLine* line) const;
	virtual QMargins getLineMargins(TextLine* line) const;
	virtual int getTabulation(TextLine* line) const;

	const QTextLayout& lineLayout(TextLine* line) const;

	virtual QMargins getNodeMargins(TextNode* node) const;

	virtual int expectedNodeNbTextLines() const;

	virtual void layNodeOut(TextNode* node, int availableWidth) const;

	virtual void renderNode(TextNode* node,
							const QPointF &offset,
							int availableWidth,
							QPainter & painter,
							TextNode::NodeCoordinate selectionStart = {0,0},
							TextNode::NodeCoordinate selectionEnd = {0,0},
							int cursorLine = -1,
							int cursorPos = 0,
							const QTextCharFormat &selectionFormat = QTextCharFormat());


	int nodeHeight(TextNode* node, int availableWidth) const;
	int nodeHeightBetweenLines(TextNode* node, int availableWidth, int lineStart = 0, int lineEnd = -1) const;
	int nodeNbLayoutLines(TextNode* node, int availableWidth) const;
	int nodeNbLayoutLines(TextNode* node, int availableWidth, int availableHeight) const;

	void clearCache();

Q_SIGNALS:

	void updated();

protected:

	void renderLine(TextLine* line,
	                const QPointF &offset,
	                QPainter & painter,
	                int selectionStart = 0,
	                int selectionEnd = -1,
					int cursorStart = -1,
					const QTextCharFormat &selectionFormat = QTextCharFormat()) const;
	virtual void layOutLine(TextLine* line,
							const QPointF &offset,
							int availableWidth) const;

	struct LineLayoutCache{

		LineLayoutCache() :
			usedAvailableWidth(-1),
			layout(new QTextLayout)
		{

		}

		LineLayoutCache(LineLayoutCache const& other) :
			usedAvailableWidth(other.usedAvailableWidth),
			layout(other.layout)
		{

		}

		LineLayoutCache& operator=(LineLayoutCache const& other) {
			usedAvailableWidth = other.usedAvailableWidth;
			layout = other.layout;
			return *this;
		}

		int usedAvailableWidth;
		std::shared_ptr<QTextLayout> layout;
	};

	mutable QMap<TextLine*, LineLayoutCache> _cache;

};

} // namespace Sabrina

#endif // SABRINA_ABSTRACTTEXTSTYLE_H
