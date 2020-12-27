#ifndef SABRINA_TEXTNODE_H
#define SABRINA_TEXTNODE_H

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

#include "./text_global.h"

#include <QObject>
#include <QTextLayout>


namespace Sabrina {

class TextNode;

class SABRINA_TEXT_EXPORT TextLine : public QObject
{
	Q_OBJECT
public:

	explicit TextLine(TextNode *parent = nullptr);

	QString getText() const;
	void setText(QString const& text);

	TextNode * nodeParent() const;
	TextLine* nextLine();
	TextLine* previousLine();

Q_SIGNALS:

	void lineEdited(TextLine* line);

protected:

	QString _text;
};

class SABRINA_TEXT_EXPORT TextNode : public QObject
{
	Q_OBJECT
public:
	explicit TextNode(QObject *parent = nullptr, int nbLines = 1);

	int styleId() const;
	void setStyleId(int style_id);

	int nbTextLines() const;
	void setNbTextLines(int nb_text_line);

	TextLine* lineAt(int id);
	const TextLine* lineAt(int id) const;

	inline bool isRootNode() const {
		return parentNode() == nullptr;
	}
	TextNode* parentNode() const;

	int height() const;

	//! \brief the position in the chain of nodes
	int nodePos() const;

	//! \brief the position in the list of children of the parent node
	int nodeIndex() const;

	//! \brief the starting line of the node in the document
	int nodeLine() const;

	//! \brief the numer of line to the last line of the last child of the node
	int maxLine() const;

	QList<TextNode*> childNodes();

	TextNode* nextNode();
	TextNode* previousNode();
	TextNode* lastNode() const;
	TextNode* rootNode();
	TextNode* subRootNode();
	TextNode* nodeAtLine(int line, int* nLine = nullptr);
	TextLine* getLineAtLine(int line);

	TextNode* insertNodeAbove(int styleCode = 0);
	TextNode* insertNodeAfter(int styleCode = 0);
	TextNode* insertNodeSubRoot(int styleCode = 0);
	TextNode* insertNodeBelow(int styleCode = 0, int pos = 0);

Q_SIGNALS:

	void nodeRemoved(TextNode* parent, int oldRow);
	void nodeAdded(TextNode* parent, int newRow);
	void nodeEdited(TextNode* node, TextLine* line);
	void nodeLineLayoutChanged(TextNode* node);

protected:

	void onLineEdited(TextLine* line);

	QList<TextNode*> _children;
	QList<TextLine*> _lines;

	int _style_id;

};

} // namespace Sabrina

#endif // SABRINA_TEXTNODE_H
