﻿/*
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

#include "textnode.h"

#include <cmath>

#include <QJsonArray>
#include <QJsonValue>

namespace Sabrina {

TextLine::TextLine(TextNode *parent ) :
	QObject(parent),
	_text("")
{

}


QString TextLine::getText() const {
	return _text;
}
void TextLine::setText(QString const& text) {
	if (_text != text) {
		_text = text;
		void lineEdited(TextLine* line);
	}
}

TextNode * TextLine::nodeParent() const {
	return qobject_cast<TextNode*>(parent());
}

TextLine* TextLine::nextLine() {

	TextNode* n = nodeParent();

	for (int i = 0; i < n->nbTextLines(); i++) {
		TextLine* l = n->lineAt(i);

		if (l == this) {
			if (i < n->nbTextLines()-1) {
				return n->lineAt(i+1);
			} else {
				TextNode* next = n->nextNode();

				if (next != nullptr) {
					return next->lineAt(0);
				} else {
					return nullptr;
				}
			}
		}
	}

	return nullptr;
}

TextLine* TextLine::previousLine() {

	TextNode* n = nodeParent();

	for (int i = 0; i < n->nbTextLines(); i++) {
		TextLine* l = n->lineAt(i);

		if (l == this) {
			if (i > 0) {
				return n->lineAt(i-1);
			} else {
				TextNode* prev = n->previousNode();

				if (prev != nullptr) {
					return prev->lineAt(prev->nbTextLines()-1);
				} else {
					return nullptr;
				}
			}
		}
	}

	return nullptr;

}
TextLine* TextLine::lineAfterOffset(int initialPos, int offset, int & newPos, int* unusedOffset)
{
	int unused = offset;

	TextLine* l = this;
	int pos = initialPos;

	while (unused != 0) {

		TextLine* tmp;

		if (unused < 0) {

			tmp = l->previousLine();

			if (unused + pos >= 0) {

				pos += unused;
				unused = 0;

			} else {

				unused += pos;

				if (tmp != nullptr) {
					unused += 1;
					l = tmp;
					pos = l->getText().length();
				}
			}

		} else {

			tmp = l->nextLine();

			int ll = l->getText().length();

			if (unused + pos - ll <= 0) {

				pos += unused;
				unused = 0;

			} else {

				unused -= ll - pos;

				if (tmp != nullptr) {
					unused -= 1;
					l = tmp;
					pos = 0;
				}
			}

		}

		if (tmp == nullptr) {
			break;
		}

	}

	newPos = pos;

	if (unusedOffset != nullptr) {
		*unusedOffset = unused;
	}

	return l;

}

int TextLine::lineLineNumber() const {

	TextNode* n = nodeParent();

	int l = n->nodeLine();
	l += n->lines().indexOf(const_cast<TextLine*>(this));

	return l;
}
int TextLine::lineNodeIndexNumber() const {
	TextNode* n = nodeParent();

	int l = n->lines().indexOf(const_cast<TextLine*>(this));

	return l;
}

int TextLine::nCharsBefore() const {
	TextNode* n = nodeParent();

	int c = n->nCharsBefore();
	int l = n->lines().indexOf(const_cast<TextLine*>(this));

	for (int i = 0; i < l; i++) {
		c += n->lines().at(i)->getText().size();
	}

	return c;

}

int TextLine::nCharsBeforeInNode() const {
	TextNode* n = nodeParent();

	int c = 0;
	int l = n->lines().indexOf(const_cast<TextLine*>(this));

	for (int i = 0; i < l; i++) {
		c += n->lines().at(i)->getText().size();
	}

	return c;
}

int TextLine::nChars() const {
	return getText().size();
}

int TextLine::nCharsAfter() const {

	TextNode* n = nodeParent();

	int c = n->nCharsAfter();
	int l = n->lines().indexOf(const_cast<TextLine*>(this));

	for (int i = l+1; i < n->lines().size(); i++) {
		c += n->lines()[i]->getText().size();
	}

	return c;
}

int TextLine::nCharsAfterInNode() const{

	TextNode* n = nodeParent();

	int c = 0;
	int l = n->lines().indexOf(const_cast<TextLine*>(this));

	for (int i = l+1; i < n->lines().size(); i++) {
		c += n->lines()[i]->getText().size();
	}

	return c;
}

int TextNode::nCharsBetweenNodes(const TextNode* start, const TextNode* end) {

	const TextNode* n = start;
	const TextNode* l = end;

	if (n == nullptr) {
		return 0;
	}

	if (n == l) {
		return 0;
	}

	int c = -1;

	do {
		for (int i = 0; i < n->nbTextLines(); i++) {
			c += n->lineAt(i)->getText().size() + 1;
		}
		n = n->nextNode();
	} while (n != l and n != nullptr);

	return c;
}

TextNode::DocumentInterval TextNode::intervalWithFlatParentsLevel(TextNode* n1, TextNode* n2) {

	if (n1->rootNode() != n2->rootNode()) {
		return {{-1,-1},{-1,-1}};
	}

	int lvl1 = n1->nodeLevel();
	int lvl2 = n2->nodeLevel();

	TextNode* node1 = n1;
	TextNode* node2 = n2;

	if (lvl1 > lvl2) {
		node1 = node1->nodeAbove(lvl1-lvl2);
	} else if (lvl2 > lvl1) {
		node2 = node2->nodeAbove(lvl2-lvl1);
	}

	if (node1 == nullptr or node2 == nullptr) {
		return {{-1,-1},{-1,-1}};
	}

	while (node1->parentNode() != node2->parentNode()) {

		node1 = node1->parentNode();
		node2 = node2->parentNode();

		if (node1 == nullptr or node2 == nullptr) {
			return {{-1,-1},{-1,-1}};
		}
	}

	int id1 = node1->nodeIndex();
	int id2 = node2->nodeIndex();

	if (id1 > id2) {
		TextNode* tmp = node1;
		node1 = node2;
		node2 = tmp;
	}

	node2 = node2->lastNode();

	NodeCoordinate coord1 = {node1->nodeLine(), 0};
	NodeCoordinate coord2 = {node2->nodeLine() + node2->nbTextLines() -1, node2->lineAt(node2->nbTextLines() -1)->nChars()};

	return {coord1, coord2};
}

TextNode::TextNode(QObject *parent, int nbLines) :
	QObject(parent),
	_lines(),
	_style_id(0)
{
	_lines.push_back(new TextLine(this));
	connect(_lines[0], &TextLine::lineEdited, this, &TextNode::onLineEdited);
	setNbTextLines(nbLines);
}

TextLine* TextNode::lineAt(int id) {
	return _lines.value(id, nullptr);
}

const TextLine* TextNode::lineAt(int id) const {
	TextNode* n = const_cast<TextNode*>(this);
	return const_cast<const TextLine*>(n->lineAt(id));
}

TextNode* TextNode::parentNode() const {
	return qobject_cast<TextNode*>(parent());
}

int TextNode::nodePos() const {
	TextNode* n = const_cast<TextNode*>(this);

	n = n->rootNode();

	int p = 0;

	while (n != this) {
		n = n->nextNode();
		if (n == nullptr) {
			return -1;
		}
		p++;
	}

	return p;
}

int TextNode::nodeIndex() const {

	TextNode* p = parentNode();

	if (p == nullptr) {
		return -1;
	}

	return p->_children.indexOf(const_cast<TextNode*>(this));

}

int TextNode::nodeLine() const {
	TextNode* n = const_cast<TextNode*>(this);

	n = n->rootNode();

	int l = 0;

	while (n != this) {
		l += n->nbTextLines();
		n = n->nextNode();
		if (n == nullptr) {
			return -1;
		}
	}

	return l;
}

int TextNode::nodeLevel() const {
	int level = 0;

	TextNode* n = const_cast<TextNode*>(this);


	while (!n->isRootNode()) {
		n = n->parentNode();
		level++;
	}

	return level;
}

int TextNode::maxLine() const {

	TextNode* n = lastNode();
	TextNode* t = const_cast<TextNode*>(this);

	int l = t->nbTextLines();

	while (t != n) {
		t = t->nextNode();

		if (t == nullptr) {
			return -1;
		}

		l += t->nbTextLines();
	}

	return l;

}
int TextNode::nChars() const {

	const TextNode* n = this;
	const TextNode* l = lastNode()->nextNode();

	return nCharsBetweenNodes(n,l);
}

int TextNode::nCharsInNode() const {

	const TextNode* n = this;
	const TextNode* l = n->nextNode();

	return nCharsBetweenNodes(n,l);
}

int TextNode::nCharsBefore() const {

	const TextNode* n = rootNode();
	const TextNode* l = this;

	int numChars = nCharsBetweenNodes(n,l);

	if (numChars > 0) {
		return numChars + 1; //acount for the line break before the start of the node
	}

	return 0;
}

int TextNode::nCharsAfter() const {

	const TextNode* n = this->nextNode();
	const TextNode* l = nullptr;

	int numChars = nCharsBetweenNodes(n,l);

	if (numChars > 0) {
		return numChars + 1; //acount for the line break after the end of the node
	}

	return 0;
}

bool TextNode::clearFromDoc(bool deleteLater) {

	TextNode* p = parentNode();

	if (p == nullptr) {
		return false;
	}

	int i = p->_children.indexOf(this);
	p->_children.removeAt(i);
	Q_EMIT nodeRemoved(p, i);

	if (deleteLater) {
		this->deleteLater();
	}

	return true;
}

int TextNode::nbChildren() const {
	return _children.size();
}

const QList<TextNode *> & TextNode::childNodes() {
	return _children;
}

QList<const TextNode*> TextNode::childNodes() const {
	QList<const TextNode*> children;
	children.reserve(_children.size());

	for (TextNode* n : _children) {
		children.append(n);
	}

	return children;
}
const QList<TextLine*> & TextNode::lines() {
	return _lines;
}

TextNode* TextNode::nextNode() {
	if (_children.size() > 0) {
		return _children[0];
	}

	TextNode* p = this;
	TextNode* t = this;

	while (1) {
		p = p->parentNode();

		if (p == nullptr) {
			return nullptr;
		}

		QList<TextNode*> c = p->childNodes();
		int i = c.indexOf(t);

		if (i+1 < c.size()) {
			return c.at(i+1);
		}

		t = p;
	}
}

const TextNode* TextNode::nextNode() const {
	return const_cast<TextNode*>(this)->nextNode();
}

TextNode* TextNode::previousNode() {

	TextNode* p = parentNode();

	if (p == nullptr) {
		return nullptr;
	}

	QList<TextNode*> c = p->childNodes();
	int i = c.indexOf(this);

	if (i == 0) {
		return p;
	} else {
		return c.at(i-1)->lastNode();
	}

}

TextNode* TextNode::lastNode() const{

	TextNode* p = const_cast<TextNode*>(this);

	while (p->_children.size() > 0) {
		p = p->_children.last();
	}

	return p;
}

TextNode* TextNode::rootNode() {

	TextNode* p = this;

	while (p->parentNode() != nullptr) {
		p = p->parentNode();
	}

	return p;
}
const TextNode* TextNode::rootNode() const {
	return const_cast<TextNode*>(this)->rootNode();
}
TextNode* TextNode::nodeAbove(int lvl) {

	if (lvl < 0) {
		return nullptr;
	}

	TextNode* n = this;

	for (int i = 0; i < lvl; i++) {
		n = n->parentNode();
		if (n == nullptr) {
			return n;
		}
	}

	return n;
}
const TextNode* TextNode::nodeAbove(int lvl) const {
	return const_cast<TextNode*>(this)->nodeAbove(lvl);
}

TextNode* TextNode::subRootNode() {

	TextNode* p = this;

	if (p->parentNode() == nullptr) {
		return nullptr;
	}

	while (p->parentNode()->parentNode() != nullptr) {
		p = p->parentNode();
	}

	return p;
}

TextNode* TextNode::nodeAtLine(int line, int *nLine) {
	TextNode* n = const_cast<TextNode*>(this);

	n = n->rootNode();

	int l = 0;

	while (l + n->nbTextLines() <= line) {
		l += n->nbTextLines();
		n = n->nextNode();
		if (n == nullptr) {
			return nullptr;
		}
	}

	if (nLine != nullptr) {
		*nLine = l;
	}

	return n;
}
TextLine* TextNode::getLineAtLine(int line) {

	int l;
	TextNode* n = nodeAtLine(line, &l);
	if (n == nullptr) {
		return nullptr;
	}
	return n->lineAt(line - l);

}

TextNode::NodeCoordinate TextNode::coordinateAtLineStart(NodeCoordinate coord) const {
	return {coord.lineIndex, 0};
}
TextNode::NodeCoordinate TextNode::coordinateAtLineEnd(NodeCoordinate coord) const {
	TextLine* tl = const_cast<TextNode*>(this)->getLineAtLine(coord.lineIndex);

	if (tl != nullptr) {
		return {coord.lineIndex, tl->getText().size()};
	}

	return {-1,-1};
}

TextNode::NodeCoordinate TextNode::coordinateAtNodeStart(NodeCoordinate coord) const {
	int finalLine;
	TextNode* n = const_cast<TextNode*>(this)->nodeAtLine(coord.lineIndex, &finalLine);

	if (n != nullptr) {
		return {finalLine, 0};
	}

	return {-1,-1};
}
TextNode::NodeCoordinate TextNode::coordinateAtNodeEnd(NodeCoordinate coord) const {
	int nodeStartLine;
	TextNode* n = const_cast<TextNode*>(this)->nodeAtLine(coord.lineIndex, &nodeStartLine);

	if (n != nullptr) {
		TextLine* ftl = n->lineAt(n->nbTextLines() - 1);
		return {nodeStartLine + n->nbTextLines() - 1, (ftl != nullptr) ? ftl->getText().size() : 0};
	}

	return {-1,-1};

}
TextNode::NodeCoordinate TextNode::coordinateAfterNodeChildrens(NodeCoordinate coord) const {
	int nodeStartLine;
	TextNode* n = const_cast<TextNode*>(this)->nodeAtLine(coord.lineIndex, &nodeStartLine);

	if (n != nullptr) {

		auto childs = n->childNodes();

		TextNode* final = n;

		if (!childs.isEmpty()) {

			for(int i = 0; i < childs.size()-1; i++) {
				nodeStartLine += childs.at(i)->nbTextLines();
			}

			final = childs.last();

		}
		TextLine* ftl = final->lineAt(final->nbTextLines() - 1);
		return {nodeStartLine + final->nbTextLines() - 1, (ftl != nullptr) ? ftl->getText().size() : 0};
	}

	return {-1,-1};
}

TextNode::NodeCoordinate TextNode::getCoordinateAfterOffset(TextNode::NodeCoordinate coord,
															int offset) const {
	TextLine* tl = const_cast<TextNode*>(this)->getLineAtLine(coord.lineIndex);

	if (tl == nullptr) {
		return {-1, -1};
	}

	int len = tl->getText().length();

	if (len < 0) {
		return {-1,-1};
	}

	TextNode::NodeCoordinate newCoord(coord.lineIndex, coord.linePos+offset);

	if (newCoord.linePos < 0) {
		while (newCoord.linePos < 0) {
			if (newCoord.lineIndex > 0) {
				newCoord.lineIndex--;

				tl =  tl->previousLine();

				if (tl == nullptr) {
					return {-1,-1};
				}

				len = tl->getText().length();
				if (len < 0) {
					return {-1,-1};
				}

				newCoord.linePos += len+1;
			} else {
				return {-1,-1};
			}
		}
	}

	if (newCoord.linePos > len) {

		int nLines  = rootNode()->maxLine();

		while(newCoord.linePos > len) {
			if (newCoord.lineIndex < nLines-1) {
				newCoord.linePos -= len+1;
				newCoord.lineIndex++;

				tl =  tl->nextLine();

				if (tl == nullptr) {
					return {-1,-1};
				}

				len = tl->getText().length();
				if (len < 0) {
					return {-1,-1};
				}
			} else {
				return {-1,-1};
			}
		}
	}

	return newCoord;
}
int TextNode::offsetBetweenCoordinates(NodeCoordinate start, NodeCoordinate end) const {

	if (end.lineIndex == start.lineIndex) {
		return end.linePos - start.linePos;
	}

	int d = end.lineIndex - start.lineIndex;
	int dir = (d < 0) ? -1 : 1;

	TextLine* sl = const_cast<TextNode*>(this)->rootNode()->getLineAtLine(start.lineIndex);

	int dist = 0;

	if (dir < 0) {
		dist = start.linePos;
	} else {
		dist = sl->nChars() - start.linePos;
	}

	for (int i = d-dir; i != 0; i -= dir) {
		if (dir < 0) {
			sl = sl->previousLine();
		} else {
			sl = sl->nextLine();
		}
		dist += sl->nChars();
	}

	if (dir < 0) {
		sl = sl->previousLine();
	} else {
		sl = sl->nextLine();
	}

	if (dir > 0) {
		dist += end.linePos;
	} else {
		dist += sl->nChars() - end.linePos;
	}

	return dir*dist + end.lineIndex - start.lineIndex;

}

bool TextNode::isBetweenNode(const TextNode &sNode, const TextNode &eNode, bool withBoundaries) {

	if (withBoundaries and (this == &sNode or this == &eNode)) {
		return true;
	}

	TextNode* n = const_cast<TextNode*>(&sNode);
	n = n->nextNode();

	while (n != &eNode and n != nullptr) {

		if (this == n) {
			return true;
		}

		n = n->nextNode();
	}

	return false;
}

TextNode* TextNode::insertNodeAbove(int styleCode) {

	if (parentNode() == nullptr) {
		return nullptr;
	}
	return parentNode()->insertNodeAfter(styleCode);
}

TextNode* TextNode::insertNodeAfter(int styleCode) {

	TextNode* p = parentNode();

	if (p == nullptr) {
		return nullptr;
	}

	return p->insertNodeBelow(styleCode, nodeIndex()+1);

}


TextNode* TextNode::insertNodeBefore(int styleCode) {

	TextNode* p = parentNode();

	if (p == nullptr) {
		return nullptr;
	}

	return p->insertNodeBelow(styleCode, nodeIndex());
}

TextNode* TextNode::insertNodeSubRoot(int styleCode) {

	TextNode* sr = subRootNode();

	if (sr == nullptr) {
		return nullptr;
	}

	return sr->parentNode()->insertNodeBelow(styleCode, sr->nodeIndex()+1);

}

TextNode* TextNode::insertNodeBelow(int styleCode, int pos) {

	TextNode* n = new TextNode(this);
	n->setStyleId(styleCode);

	int n_pos;

	if (pos < 0) {
		n_pos = _children.size() + 1 + pos;
	} else {
		n_pos = pos;
	}
	_children.insert(n_pos, n);

	connect(n, &TextNode::nodeAdded, this, &TextNode::nodeAdded);
	connect(n, &TextNode::nodeRemoved, this, &TextNode::nodeRemoved);
	connect(n, &TextNode::nodeEdited, this, &TextNode::nodeEdited);

	Q_EMIT nodeAdded(this, n_pos);

	return n;
}

TextNode* TextNode::moveNode(TextNode* newParent, int newPos) {

	if (newParent == nullptr) {
		return nullptr;
	}

	if (newParent->rootNode() != rootNode()) {
		return nullptr;
	}

	if (newParent->isBetweenNode(*this, *lastNode(), true)) { //cannot move the node to one of its descendants.
		return nullptr;
	}

	TextNode* oldParent = parentNode();

	if (oldParent != nullptr) {
		oldParent->_children.removeAt(nodeIndex());

		disconnect(this, &TextNode::nodeAdded, oldParent, &TextNode::nodeAdded);
		disconnect(this, &TextNode::nodeRemoved, oldParent, &TextNode::nodeRemoved);
		disconnect(this, &TextNode::nodeEdited, oldParent, &TextNode::nodeEdited);
	}

	newParent->_children.insert(newPos, this);
	setParent(newParent);

	connect(this, &TextNode::nodeAdded, newParent, &TextNode::nodeAdded);
	connect(this, &TextNode::nodeRemoved, newParent, &TextNode::nodeRemoved);
	connect(this, &TextNode::nodeEdited, newParent, &TextNode::nodeEdited);

	Q_EMIT nodeMoved(this, oldParent);

	return this;

}

QString TextNode::getHtmlRepresentation(NodeCoordinate start,
										NodeCoordinate end,
										QMap<int, QString> const& styleNameMap) const {

	const TextNode* root = rootNode();
	const TextNode* current = root;
	const TextLine* currentLine = current->lineAt(0);

	int nodeStartLine = 0;
	int currentTxtLine = 0;
	int currentLineId = 0;
	int currentLinePos = 0;

	if (start.isValid()) {
		current = const_cast<TextNode*>(root)->nodeAtLine(start.lineIndex, &nodeStartLine);
		currentTxtLine = start.lineIndex;
		currentLineId = start.lineIndex - nodeStartLine;
		currentLine = current->lineAt(currentLineId);
		currentLinePos = start.linePos;
	}

	QString out;
	int currentPlevel = 0;

	if (currentLineId == 0 and currentLinePos == 0) {
		if (!(end.lineIndex >= currentTxtLine and end.lineIndex < currentTxtLine + current->nbTextLines()) or
				(end.lineIndex == currentTxtLine + current->nbTextLines() - 1 and end.linePos == current->lineAt(current->nbTextLines() - 1)->nChars())) {
			out += "<p";

			int nodeStyleId = current->styleId();

			if (styleNameMap.contains(nodeStyleId)) {
				out += " class=\"";
				out += styleNameMap.value(nodeStyleId) + "\"";
			}
			out += " styleId=\"";
			out += QString::number(nodeStyleId);
			out += "\"";

			out += ">";
			currentPlevel++;
		}
	}

	int endLinePos = currentLine->nChars();

	if (currentTxtLine == end.lineIndex) {
		endLinePos = end.lineIndex;
	}

	out += currentLine->getText().midRef(currentLinePos, endLinePos - currentLinePos);
	out += "<br>";

	while (currentLine != nullptr and currentTxtLine != end.lineIndex) {
		currentLine = const_cast<TextLine*>(currentLine)->nextLine();
		currentTxtLine++;

		if (currentLine->nodeParent() != current) {

			int delta = currentLine->nodeParent()->nodeLevel() - current->nodeLevel();

			if (delta <= 0) {
				int jumps = std::max(1 - delta, currentPlevel);

				for (int i = 0; i < jumps; i++) {
					out += "</p>";
				}

				currentPlevel -= jumps;
			}

			current = currentLine->nodeParent();

			out += "<p";

			int nodeStyleId = current->styleId();

			if (styleNameMap.contains(nodeStyleId)) {
				out += " class=\"";
				out += styleNameMap.value(nodeStyleId) + "\"";
			}
			out += " styleId=\"";
			out += QString::number(nodeStyleId);
			out += "\"";

			out += ">";
			currentPlevel++;

		}

		endLinePos = (currentTxtLine == end.linePos) ? end.linePos : -1;

		out += currentLine->getText().midRef(0, endLinePos);
		out += "<br>";
	}

	for (int i = 0; i < currentPlevel; i++) {
		out += "</p>";
	}

	return out;

}

const QString TextNode::TextNodeJsonRepresentationInfos::LINES_KEY = "lines";
const QString TextNode::TextNodeJsonRepresentationInfos::STYLE_ID_KEY = "style_id";
const QString TextNode::TextNodeJsonRepresentationInfos::STYLE_NAME_KEY = "style_name";
const QString TextNode::TextNodeJsonRepresentationInfos::CHILDREN_KEY = "childs";
const QString TextNode::TextNodeJsonRepresentationInfos::JUMPS_KEY = "jumps";

const QString TextNode::TextNodeJsonRepresentationInfos::NODES_KEY = "nodes";

const QString TextNode::TextNodeMimeTypeInfos::DocumentData = "application/sabrina-text-document";

QJsonObject TextNode::fullJsonRepresentation(QMap<int, QString> const& styleNameMap,
											 TextNode const* limitNode) const {
	bool hitLimit = false;
	return fullJsonRepresentation(styleNameMap, limitNode, &hitLimit);
}

QJsonObject TextNode::fullJsonRepresentation(QMap<int, QString> const& styleNameMap,
											 TextNode const* limitNode,
											 bool * hitLimit) const {

	bool hitLimitBck;
	bool* hitLimitPtr;

	if (hitLimit != nullptr) {
		hitLimitPtr = hitLimit;
	} else {
		hitLimitPtr = &hitLimitBck;
	}

	QJsonObject out;

	out.insert(TextNodeJsonRepresentationInfos::STYLE_ID_KEY, styleId());

	if (styleNameMap.contains(styleId())) {
		out.insert(TextNodeJsonRepresentationInfos::STYLE_NAME_KEY, styleNameMap.value(styleId()));
	}

	QJsonArray lines;

	for (TextLine* l : _lines) {
		lines.append(l->getText());
	}

	out.insert(TextNodeJsonRepresentationInfos::LINES_KEY, lines);

	QJsonArray children;

	for (TextNode* n : _children) {
		if (n == limitNode) {
			*hitLimitPtr = true;
			break;
		}
		children.append(n->fullJsonRepresentation(styleNameMap, limitNode, hitLimitPtr));

		if (*hitLimitPtr) {
			break;
		}
	}

	out.insert(TextNodeJsonRepresentationInfos::CHILDREN_KEY, lines);

	return out;
}

QJsonObject TextNode::rangeJsonRepresentation(NodeCoordinate start,

											  NodeCoordinate end,
											  QMap<int, QString> const& styleNameMap) const {


	const TextNode* root = rootNode();
	const TextNode* curentNode = root;

	int nodeStartLine = 0;
	int startLineNumber = 0;
	int currentLineNumber = 0;
	int startLinePos = 0;

	if (start.isValid()) {
		startLineNumber = start.lineIndex;
		curentNode = const_cast<TextNode*>(root)->nodeAtLine(start.lineIndex, &nodeStartLine);
		startLinePos = start.linePos;
		currentLineNumber = nodeStartLine;
	}

	int endLineNumber = root->maxLine();
	int endLinePos = -1;

	if (end.isValid()) {
		endLineNumber = end.lineIndex;
		endLinePos = end.linePos;
	}

	QJsonObject out;
	QJsonArray nodesArray;
	QVector<QJsonObject> nodes = {QJsonObject()}; // root node
	int depth = 0;

	do {
		nodes[depth].insert(TextNodeJsonRepresentationInfos::STYLE_ID_KEY, curentNode->styleId());

		if (styleNameMap.contains(styleId())) {
			nodes[depth].insert(TextNodeJsonRepresentationInfos::STYLE_NAME_KEY, styleNameMap.value(curentNode->styleId()));
		}

		QJsonArray lines;

		for (TextLine* l : curentNode->_lines) {

			if (currentLineNumber < startLineNumber) {
				currentLineNumber++;
				continue;
			}

			QString txt = l->getText();

			int startPos = 0;
			int len = -1;

			if (currentLineNumber == startLineNumber) {
				startPos = startLinePos;
			}

			if (currentLineNumber == endLineNumber) {
				len = endLinePos - startPos;
				if (len < 0) {
					len = 0;
				}
			}

			lines.append(txt.mid(startPos, len));
			currentLineNumber++;

			if (currentLineNumber > end.lineIndex) {
				break;
			}
		}

		nodes[depth].insert(TextNodeJsonRepresentationInfos::LINES_KEY, lines);

		if (currentLineNumber > end.lineIndex) {
			break;
		}

		const TextNode* next = curentNode->nextNode();

		if (next == nullptr) {
			break;
		}

		if (curentNode->nbChildren() > 0) { //we move to a child
			QJsonObject next_node;
			nodes.append(next_node);
			depth++;
		} else if (curentNode->parentNode() == next->parentNode()) { //we keep the same parent

			if (depth == 0) {
				nodesArray.append(nodes[depth]);
			} else {
				if (nodes[depth-1].contains(TextNodeJsonRepresentationInfos::CHILDREN_KEY)) {
					QJsonArray nds = nodes[depth-1].value(TextNodeJsonRepresentationInfos::CHILDREN_KEY).toArray();
					nds.append(nodes[depth]);
					nodes[depth-1].insert(TextNodeJsonRepresentationInfos::CHILDREN_KEY, nds);
				} else {
					QJsonArray nds;
					nds.append(nodes[depth]);
					nodes[depth-1].insert(TextNodeJsonRepresentationInfos::CHILDREN_KEY, nds);
				}
			}
			nodes[depth] = QJsonObject();

		} else { //we climb to a different parent

			const TextNode* p = curentNode;
			int jumps = 0;

			while (p != next->parentNode() and jumps <= depth) {
				p = p->parentNode();
				jumps++;
			}

			for (int i = 0; i < jumps; i++) {

				if (depth == 0) {
					nodesArray.append(nodes[depth]);
				} else {
					if (nodes[depth-1].contains(TextNodeJsonRepresentationInfos::CHILDREN_KEY)) {
						QJsonArray nds = nodes[depth-1].value(TextNodeJsonRepresentationInfos::CHILDREN_KEY).toArray();
						nds.append(nodes[depth]);
						nodes[depth-1].insert(TextNodeJsonRepresentationInfos::CHILDREN_KEY, nds);
					} else {
						QJsonArray nds;
						nds.append(nodes[depth]);
						nodes[depth-1].insert(TextNodeJsonRepresentationInfos::CHILDREN_KEY, nds);
					}
				}
				nodes.pop_back();
				depth--;
			}
			nodes.push_back(QJsonObject());
			depth++;

			if (depth == 0) {
				nodes[depth].insert(TextNode::TextNodeJsonRepresentationInfos::JUMPS_KEY, jumps);
			}
		}

		curentNode = next;

	} while (depth >= 0);


	int jumps = depth+1;
	for (int i = 0; i < jumps; i++) {

		if (depth == 0) {
			nodesArray.append(nodes[depth]);
		} else {
			if (nodes[depth-1].contains(TextNodeJsonRepresentationInfos::CHILDREN_KEY)) {
				QJsonArray nds = nodes[depth-1].value(TextNodeJsonRepresentationInfos::CHILDREN_KEY).toArray();
				nds.append(nodes[depth]);
				nodes[depth-1].insert(TextNodeJsonRepresentationInfos::CHILDREN_KEY, nds);
			} else {
				QJsonArray nds;
				nds.append(nodes[depth]);
				nodes[depth-1].insert(TextNodeJsonRepresentationInfos::CHILDREN_KEY, nds);
			}
		}
		nodes.pop_back();
		depth--;
	}

	out.insert(TextNodeJsonRepresentationInfos::NODES_KEY, nodesArray);

	return out;

}

void TextNode::onLineEdited(TextLine* line) {
	int pos = _lines.indexOf(line);

	if (pos >= 0) {
		Q_EMIT nodeEdited(this, line);
	}
}


int TextNode::nbTextLines() const
{
	return _lines.size();
}

void TextNode::setNbTextLines(int nb_text_line)
{

	if (nb_text_line < 1) {
		return;
	}

	if (_lines.size() > nb_text_line) {

		while (_lines.size() != nb_text_line) {
			TextLine* l = _lines.takeLast();
			l->deleteLater();
		}

	} else if (_lines.size() < nb_text_line) {

		_lines.reserve(nb_text_line);
		while (_lines.size() != nb_text_line) {
			TextLine* l = new TextLine(this);
			_lines.push_back(l);
			connect(l, &TextLine::lineEdited, this, &TextNode::onLineEdited);
		}
	}

	Q_EMIT nodeLineLayoutChanged(this);
}

int TextNode::styleId() const
{
    return _style_id;
}

void TextNode::setStyleId(int style_id)
{
	_style_id = style_id;
}

} // namespace Sabrina
