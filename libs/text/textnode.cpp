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

#include "textnode.h"

#include <cmath>

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

int TextNode::nCharsBetweenNodes(const TextNode* start, const TextNode* end) {

	const TextNode* n = start;
	const TextNode* l = end;

	if (n == nullptr) {
		return 0;
	}

	if (n == l) {
		return 0;
	}

	int c = 0;

	do {
		for (int i = 0; i < n->nbTextLines(); i++) {
			c += n->lineAt(i)->getText().size();
		}
		n = n->nextNode();
	} while (n != l and n != nullptr);

	return c;
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

int TextNode::nCharsBefore() const {

	const TextNode* n = rootNode();
	const TextNode* l = this;

	return nCharsBetweenNodes(n,l);
}

int TextNode::nCharsAfter() const {

	const TextNode* n = this->nextNode();
	const TextNode* l = nullptr;

	return nCharsBetweenNodes(n,l);
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

QList<TextNode*> TextNode::childNodes() {
	return _children;
}
QList<TextLine*> TextNode::lines() {
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
