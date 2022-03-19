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

#include "texteditwidget.h"

#include <QPainter>
#include <cmath>

#include "utils/envvars.h"

#include <QDebug>

namespace Sabrina {

TextEditWidget::TextEditWidget(QWidget *parent) :
	QWidget(parent),
	_styleManager(nullptr),
	_currentScript(nullptr),
	_baseIndex(nullptr),
	_baseIndexLine(0),
	_baseIndexHeightDelta(0),
	_endIndex(nullptr),
	_endIndexMargin(-1),
	_internalMargins(25, 25, 25, 25),
	_nodeSupprBehavior(NodeSupprBehavior::MergeContent),
	_selectionMode(SelectionMode::Text),
	_highlightCurrent(true)
{
	_cursor = new Cursor(this, 0, 0, 0);
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_InputMethodEnabled, true);

	_selectionFormat.setBackground(QBrush(QColor(123, 169, 220)));
}

TextEditWidget::~TextEditWidget() {
	delete _cursor;
}

void TextEditWidget::setStyleManager(TextStyleManager *styleManager)
{
	if (_styleManager != nullptr) {
		disconnect(_styleManager, &TextStyleManager::styleUpdated, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
		disconnect(_styleManager, &TextStyleManager::styleRemoved, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
	}

	_styleManager = styleManager;

	if (_styleManager != nullptr) {
		connect(_styleManager, &TextStyleManager::styleUpdated, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
		connect(_styleManager, &TextStyleManager::styleRemoved, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
	}

	update();
}

TextNode* TextEditWidget::getDocument() const {
	return _currentScript;
}
TextNode* TextEditWidget::getCurrentNode() const {
	return _currentScript->nodeAtLine(_cursor->line());
}
void TextEditWidget::setCurrentScript(TextNode *root) {

	if (root != _currentScript) {

		if (_currentScript != nullptr) {
			disconnect(_currentScript, &QObject::destroyed, this, &TextEditWidget::clearCurrentScript);

			disconnect(_currentScript, &TextNode::nodeAdded, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
			disconnect(_currentScript, &TextNode::nodeRemoved, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
			disconnect(_currentScript, &TextNode::nodeEdited, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
			disconnect(_currentScript, &TextNode::nodeLineLayoutChanged, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
		}

		_currentScript = root;

		if (_currentScript != nullptr) {
			connect(_currentScript, &QObject::destroyed, this, &TextEditWidget::clearCurrentScript);

			connect(_currentScript, &TextNode::nodeAdded, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
			connect(_currentScript, &TextNode::nodeRemoved, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
			connect(_currentScript, &TextNode::nodeEdited, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
			connect(_currentScript, &TextNode::nodeLineLayoutChanged, this, static_cast<void(TextEditWidget::*)()>(&TextEditWidget::update));
		}

		_baseIndex = _currentScript;
		_cursor->reset();
		update();
	}

}
void TextEditWidget::clearCurrentScript() {

	if (_currentScript != nullptr) {
		disconnect(_currentScript, &QObject::destroyed, this, &TextEditWidget::clearCurrentScript);
		_currentScript = nullptr;
		update();
	}
}

bool TextEditWidget::hasScript() const {
	return _currentScript != nullptr;
}
int TextEditWidget::currentLineStyleId() const {
	return getCurrentNode()->styleId();
}

void TextEditWidget::setSelectionMode(SelectionMode mode) {
	_selectionMode = mode;

	_cursor->constrainExtend();

	if (_cursor->extend() != 0) {
		update();
	}
}
TextEditWidget::SelectionMode TextEditWidget::currentSelectionMode() const {
	return _selectionMode;
}

void TextEditWidget::setHighlightCurrent(bool highlight) {
	if (highlight != _highlightCurrent) {
		_highlightCurrent = highlight;
		update();
	}
}
bool TextEditWidget::highlightCurrent() const {
	return _highlightCurrent;
}

int TextEditWidget::computeLineWidth() const {
	return width() - _internalMargins.left() - _internalMargins.right();
}
int TextEditWidget::computeLineStartingX() const {
	return _internalMargins.left();
}

void TextEditWidget::paintEvent(QPaintEvent *event) {

	QPainter painter(this);
	painter.setPen(QColor(Qt::black));
	painter.setRenderHint(QPainter::Antialiasing);

	//clear region
	painter.fillRect(event->rect().x(),
					 event->rect().y(),
					 event->rect().width(),
					 event->rect().height(),
					 QColor(255, 255, 255));

	if (_styleManager == nullptr or _currentScript == nullptr) {
		return;
	}

	TextNode* n = _baseIndex;

	if (n == nullptr) {
		return;
	}

	int v_pos = _internalMargins.top() - _baseIndexHeightDelta;
	int l = _baseIndexLine;

	int availableWidth = computeLineWidth();

	int selStart = 0;
	int selEnd = -1;

	Cursor::CursorState selectionState = _cursor->getExtendedSelectionState();

	TextLine* startSelectionLine = nullptr;
	TextLine* endSelectionLine = nullptr;

	if (selectionState.extend > 0) {
		startSelectionLine = _currentScript->getLineAtLine(selectionState.line);
		selStart = selectionState.pos;
		endSelectionLine = startSelectionLine->lineAfterOffset(selectionState.pos,selectionState.extend, selEnd);
	} else if (selectionState.extend < 0) {
		endSelectionLine = _currentScript->getLineAtLine(selectionState.line);
		selEnd = selectionState.pos;
		startSelectionLine = endSelectionLine->lineAfterOffset(selectionState.pos,selectionState.extend, selStart);
	}

	bool selection_running = false;

	if (startSelectionLine != nullptr and endSelectionLine != nullptr) {

		selection_running = n->isBetweenNode(*startSelectionLine->nodeParent(),
											 *endSelectionLine->nodeParent(),
											 true);

	}
	bool currentNodeInSelection = selection_running;

	while(v_pos < height()) {
		Cursor::CursorState* c = nullptr;

		if (hasFocus()) {
			if (selectionState.line >= l and selectionState.line < l+n->nbTextLines()) {
				c = &selectionState;
			}
		}

		TextNode::NodeCoordinate sStart = TextNode::NodeCoordinate(0,0);

		TextNode::NodeCoordinate sEnd = TextNode::NodeCoordinate(0,0);

		if (startSelectionLine != nullptr and endSelectionLine != nullptr) {

			if (startSelectionLine->nodeParent() == n) {
				selection_running = true;
				currentNodeInSelection = true;
				sStart = TextNode::NodeCoordinate(startSelectionLine->lineNodeIndexNumber(), selStart);
			}

			if (endSelectionLine->nodeParent() == n) {
				selection_running = false;
				sEnd = TextNode::NodeCoordinate(endSelectionLine->lineNodeIndexNumber(), selEnd);
			} else if (selection_running) {
				sEnd = TextNode::NodeCoordinate(-1,-1);
			}

		}

		AbstractTextNodeStyle* s = nodeStyle(n);

		if (s == nullptr) {
			continue;
		}

		QPointF o(computeLineStartingX(), v_pos);

		int nodeHeight = s->nodeHeight(n, availableWidth);

		if (highlightCurrent()) {
			if (currentNodeInSelection or n == _currentScript->getLineAtLine(_cursor->line())->nodeParent()) {
				painter.fillRect(0,
								 v_pos,
								 width(),
								 nodeHeight,
								 QColor(230, 240, 255));
			}
		}

		if (c != nullptr) {
			s->renderNode(n, o, availableWidth, painter, sStart, sEnd, c->line - l, c->pos, _selectionFormat);
		} else {
			s->renderNode(n, o, availableWidth, painter, sStart, sEnd, -1, 0, _selectionFormat);
		}
		currentNodeInSelection = selection_running;

		v_pos += nodeHeight;
		l += n->nbTextLines();
		_endIndex = n;
		n = n->nextNode();

		if (n == nullptr) {
			break;
		}
	}

	_endIndexMargin = v_pos - height();

}
void TextEditWidget::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Left) {
		_cursor->move(-1);
		scrollToLine(_cursor->line());
		update();
	} else if (event->key() == Qt::Key_Right) {
		_cursor->move(1);
		scrollToLine(_cursor->line());
		update();
	} else if (event->key() == Qt::Key_Up) {
		Cursor::CursorPos cs = computeNewPosAfterJump(-1);
		_cursor->setState(cs);
		scrollToLine(_cursor->line());
		update();
	} else if (event->key() == Qt::Key_Down) {
		Cursor::CursorPos cs = computeNewPosAfterJump(1);
		_cursor->setState(cs);
		scrollToLine(_cursor->line());
		update();
	} else if (event->key() == Qt::Key_Return or event->key() == Qt::Key_Enter) {
		int idL;
		TextNode* n = _currentScript->nodeAtLine(_cursor->line(), &idL);
		if (n == nullptr) {
			return;
		}

		insertNextType(n, event->modifiers());
		_cursor->setLine(idL + n->nbTextLines());

		scrollToLine(_cursor->line());
		update();

	} else if (event->key() == Qt::Key_Backspace) {

		removeText();
		update();

	} else if(!event->text().isEmpty()) {

		insertText(event->text());
		update();

	} else {
		QWidget::keyPressEvent(event);
	}

}
void TextEditWidget::inputMethodEvent(QInputMethodEvent *event) {

	QString s = event->commitString();

	if (!s.isEmpty()) {
		insertText(s);
		update();
	}

}

void TextEditWidget::wheelEvent(QWheelEvent *event) {

	QPoint numPixels = event->pixelDelta();
	QPoint numDegrees = event->angleDelta() / 8;

	if (!numPixels.isNull() and getEnvVar("XDG_SESSION_TYPE").toLower() != "x11") {
		scroll(-numPixels.y());
	} else if (!numDegrees.isNull()) {
		scroll(-numDegrees.y());
	}
	update();
}

void TextEditWidget::mousePressEvent(QMouseEvent *event) {

	if (event->buttons() == Qt::LeftButton) {
		_clickPos = event->pos();
		setCursorAtPoint(_clickPos);
		update();
		event->accept();
	}

}
void TextEditWidget::mouseMoveEvent(QMouseEvent *event) {

	if (event->buttons() == Qt::LeftButton) {
		QPoint p = event->pos();
		setCursorAtPoint(p);
		setSelectionAtPoint(_clickPos);
		update();
		event->accept();
	}
}

int TextEditWidget::nodeHeight(TextNode* n) {
	AbstractTextNodeStyle* s = nodeStyle(n);

	if (s == nullptr) {
		return 0;
	}

	return s->nodeHeight(n, computeLineWidth());

}
int TextEditWidget::scroolableUpDistance(int maxScroll) {

	if (_endIndexMargin < 0) {
		return 0;
	}

	int dist = _endIndexMargin;

	TextNode* n = _endIndex->nextNode();

	while (n != nullptr) {
		int h = nodeHeight(n);

		if (dist + h > maxScroll) {
			return maxScroll;
		}

		dist += h;
	}

	return dist + _internalMargins.bottom();

}
AbstractTextNodeStyle* TextEditWidget::nodeStyle(TextNode* n) {

	AbstractTextNodeStyle* s = _styleManager->getStyleByCode(n->styleId());

	if (s == nullptr) {
		s = _styleManager->getStyleByCode(_styleManager->getDefaultStyleCode());
	}

	return s;
}

void TextEditWidget::setCursorAtPoint(QPoint const& p, int vMargin) {

	QPoint tp = p;

	tp.ry() -= vMargin + _internalMargins.top();
	if (tp.ry() < 0) {
		tp.ry() = 0;
	}

	tp.rx() -= _internalMargins.left();

	TextNode::NodeCoordinate pos = textCordAtPoint(tp);
	if (pos.isValid()) {
		_cursor->setState(pos);
	}
}
void TextEditWidget::setSelectionAtPoint(QPoint const& p, int vMargin) {

	QPoint tp = p;

	tp.ry() -= vMargin + _internalMargins.top();
	if (tp.ry() < 0) {
		tp.ry() = 0;
	}

	tp.rx() -= _internalMargins.left();

	TextNode::NodeCoordinate pos = textCordAtPoint(tp);
	if (pos.isValid()) {
		int dist = _currentScript->offsetBetweenCoordinates(_cursor->currentCoordinate(), pos);
		_cursor->setExtent(dist);
	}

}
TextNode::NodeCoordinate TextEditWidget::textCordAtPoint(QPoint const& point) {

	int pos;
	TextLine* tl = lineAtPos(point, &pos);

	return {tl->lineLineNumber(), pos};
}

TextNode* TextEditWidget::nodeAtHeight(int y, int * nodeH) {

	if (_styleManager == nullptr) {
		return nullptr;
	}

	if (y < 0 or y > height()) {
		return nullptr;
	}

	TextNode* n = _baseIndex;

	int pos = -_baseIndexHeightDelta;

	int h = nodeHeight(n);

	if (pos + h >= y) {
		if (nodeH != nullptr) {
			*nodeH = pos;
		}
		return n;
	}

	pos += h;

	while (n != _endIndex) {

		n = n->nextNode();

		h = nodeHeight(n);

		if (pos + h >= y) {
			if (nodeH != nullptr) {
				*nodeH = pos;
			}
			return n;
		}

		pos += h;
	}

	if (nodeH != nullptr) {
		*nodeH = pos-h;
	}

	return _endIndex;
}

TextLine* TextEditWidget::lineAtPos(QPoint const& pos, int* cursorPos) {

	if (_styleManager == nullptr) {
		return nullptr;
	}

	int dy = 0;
	TextNode* n = nodeAtHeight(pos.y(), &dy);

	QPoint p = pos;
	p.ry() -= dy;

	if (n == nullptr) {
		return nullptr;
	}

	AbstractTextNodeStyle* s = nodeStyle(n);

	if (s == nullptr) {
		return nullptr;
	}

	float vd = std::numeric_limits<float>::infinity();
	float hd = std::numeric_limits<float>::infinity();

	TextLine* l = nullptr;

	for (TextLine* ln : n->lines()) {

		QTextLayout const& lyt = s->lineLayout(ln);

		QRectF r = lyt.boundingRect();

		if (r.contains(p)) {
			vd = 0;
			hd = 0;
			l = ln;
			continue;
		}

		float tmp_vd = (p.y() < r.y()) ? r.y() - p.y() : p.y() - r.y() - r.height();
		if (tmp_vd < 0) tmp_vd = 0;
		float tmp_hd = (p.x() < r.x()) ? r.x() - p.x() : p.x() - r.x() - r.width();
		if (tmp_hd < 0) tmp_hd = 0;

		if (tmp_vd < vd) {
			vd = tmp_vd;
			hd = tmp_hd;
			l = ln;
		} else if (tmp_vd == vd) {
			if (tmp_hd < hd) {
				vd = tmp_vd;
				hd = tmp_hd;
				l = ln;
			}
		}

	}

	if (cursorPos != nullptr) {

		QTextLayout const& lyt = s->lineLayout(l);

		float vd = std::numeric_limits<float>::infinity();
		int pos = -1;

		if (p.y() > lyt.boundingRect().y() + lyt.boundingRect().height()) {
			pos = l->getText().length();
		} else if (p.y() + s->getLineHeight(l) < lyt.boundingRect().y()) {
			pos = 0;
		} else {
			for (int i = 0; i < lyt.lineCount(); i++) {
				QTextLine l = lyt.lineAt(i);

				float tmp_vd = std::fabs(l.y() - p.y());

				if (tmp_vd < vd) {
					vd = tmp_vd;
					pos = l.xToCursor(p.x());
				}
			}

			pos -= s->getPrefix(l).length();
		}

		if (pos < 0) {
			pos = 0;
		}

		if (pos > l->getText().length()) {
			pos = l->getText().length();
		}

		*cursorPos = pos;

	}

	return l;

}

void TextEditWidget::scroll (int offset) {

	if (_currentScript == nullptr) {
		return;
	}

	if (offset == 0) {
		return;
	}

	if (_baseIndex == _currentScript and _baseIndexHeightDelta == 0 and offset < 0) { //can't scroll up
		return;
	}

	while (_endIndexMargin < 0 and _endIndex->nextNode() != nullptr) {//new indices have been added
		_endIndex = _endIndex->nextNode();
		_endIndexMargin += nodeHeight(_endIndex);
	}

	if (_endIndexMargin < 0 and offset > 0) { //can't scroll down
		return;
	}

	int p = offset;

	if (p < 0) { //scroll up

		p = -p;
		while (p > _baseIndexHeightDelta) {
			if (_baseIndex == _currentScript) {
				break;
			}
			p -= _baseIndexHeightDelta;
			_baseIndex = _baseIndex->previousNode();
			_baseIndexHeightDelta = nodeHeight(_baseIndex);
		}
		_baseIndexHeightDelta = std::max(_baseIndexHeightDelta - p, 0);
	} else { //scroll down

		p = scroolableUpDistance(p);

		int bIdMargin = nodeHeight(_baseIndex) - _baseIndexHeightDelta;

		while (p > 0) {

			if (p < bIdMargin) {
				_baseIndexHeightDelta += p;
				break;
			}

			p -= bIdMargin;

			_baseIndex = _baseIndex->nextNode();
			_baseIndexHeightDelta = 0;
			bIdMargin = nodeHeight(_baseIndex);
		}
	}

	_baseIndexLine = _baseIndex->nodeLine();

}

void TextEditWidget::scrollToLine (int l) {

	if (_currentScript == nullptr) {
		return;
	}

	if (l < 0) {
		return;
	}

	while (_endIndexMargin < 0 and _endIndex->nextNode() != nullptr) {//new indices have been added
		_endIndex = _endIndex->nextNode();
		_endIndexMargin += nodeHeight(_endIndex);
	}

	int endIndexLine = _endIndex->nodeLine();

	if (l > _baseIndexLine and l < endIndexLine) {
		return;
	}

	TextNode* target = _currentScript->nodeAtLine(l);

	if (l > 0 and target == nullptr) {
		return;
	}

	int dh = 0;

	TextNode* pos;

	if (l <= _baseIndexLine) { //needs to scroll up
		pos = _baseIndex;
		dh -= _baseIndexHeightDelta;

		while (pos != target and pos != nullptr) {
			pos = pos->previousNode();
			dh -= nodeHeight(pos);
		}
	} else if (l >= endIndexLine) { //needs to scroll forward

		pos = _endIndex;
		dh += std::abs(_endIndexMargin);

		while (pos != target and pos != nullptr) {
			pos = pos->nextNode();
			dh += nodeHeight(pos);
		}

		dh += _internalMargins.bottom();
		/*if (pos->nextNode() != nullptr) {
			pos = pos->nextNode();
			dh += nodeHeight(pos);
		}*/
	}

	if (dh == 0) {
		return;
	}

	scroll(dh);

}

TextEditWidget::Cursor::CursorPos TextEditWidget::computeNewPosAfterJump(int nbPseudoLinesJump) {

	TextNode* n = getCurrentNode();
	AbstractTextNodeStyle* style = nodeStyle(n);

	if (style == nullptr) {
		return {0,0};
	}

	style->layNodeOut(n, computeLineWidth());

	int aLine = _cursor->line() - n->nodeLine();

	TextLine* tl = n->lineAt(aLine);

	Cursor::DecomposedCursorPos s = _cursor->decomposePos(tl);

	int nJs = nbPseudoLinesJump;

	if (nJs == 0) {
		return {_cursor->line(), _cursor->pos()};
	}

	while (nJs != 0) {

		if (nJs < 0) { //go back

			if (s.subline < -nJs) {

				aLine--;
				if (aLine < 0) {
					n = n->previousNode();
					if (n == nullptr) {
						return {0,0};
					}

					style = nodeStyle(n);
					if (style == nullptr) {
						return {0,0};
					}

					aLine = n->nbTextLines()-1;

					style->layNodeOut(n, computeLineWidth());
				}

				tl = n->lineAt(aLine);

				nJs += s.subline + 1;

				s.line--;
				s.subline = style->lineLayout(tl).lineCount()-1;

			} else {
				s.subline += nJs;
				break;
			}

		} else {

			int margin = style->lineLayout(tl).lineCount() - s.subline - 1;

			if (margin < nJs) {

				aLine++;
				if (aLine >= n->nbTextLines()) {
					TextNode* tmp = n->nextNode();
					aLine = 0;

					if (tmp == nullptr) {
						return {n->nodeLine() + n->nbTextLines()-1, n->lineAt(n->nbTextLines()-1)->getText().length()};
					}

					n = tmp;

					style = nodeStyle(n);
					if (style == nullptr) {
						return {0,0};
					}

					style->layNodeOut(n, computeLineWidth());
				}

				tl = n->lineAt(aLine);

				nJs -= margin + 1;
				s.line++;
				s.subline = 0;

			} else {
				s.subline += nJs;
				break;
			}
		}

	}

	return _cursor->composePos(tl, s);

}

void TextEditWidget::insertText(QString commited) {

	int idLine;
	TextNode* n = _currentScript->nodeAtLine(_cursor->line(), &idLine);

	if (n == nullptr) {
		return;
	}

	if (_cursor->extend() != 0) {
		removeText();
	}

	TextLine* tLine = n->lineAt(_cursor->line() - idLine);
	QString line = tLine->getText();

	int pLen = line.length();

	int p = _cursor->pos();

	if (commited.contains(QChar('\b'))) {
		QStringList lst = commited.split(QChar('\b'), Qt::KeepEmptyParts);
		QString last = lst.takeLast();

		for (QString const& s : qAsConst(lst)) {
			line.insert(p, s);

			p += s.size();

			if (p > 0) {
				line.remove(p-1,1);
				p -= 1;
			}
		}

		line.insert(p, last);

	} else {
		line.insert(p, commited);
	}

	int c_offset = line.length() - pLen;

	int pH = nodeHeight(n);

	tLine->setText(line);

	int dH = nodeHeight(n) - pH;

	if ((n == _endIndex or n->nextNode() == _endIndex)
			and dH > 0) {
		scroll(dH);
	}
	_cursor->move(c_offset);

}

void TextEditWidget::insertNextType(TextNode* n, Qt::KeyboardModifiers modifiers) {
	if (_currentScript == nullptr) {
		return;
	}

	if (_styleManager == nullptr) {
		return;
	}

	auto jumpInfo = _styleManager->getNextNodeStyleAndPos(n->styleId());

	if (!jumpInfo.contains(modifiers)) {
		return;
	}

	TextNode* inserted = nullptr;

	switch (jumpInfo[modifiers].levelJump) {
	case TextStyleManager::LevelJump::Below:
		inserted = n->insertNodeBelow(jumpInfo[modifiers].code);
		break;
	case TextStyleManager::LevelJump::After:
		inserted = n->insertNodeAfter(jumpInfo[modifiers].code);
		break;
	case TextStyleManager::LevelJump::Above:
		inserted = n->insertNodeAbove(jumpInfo[modifiers].code);
		break;
	case TextStyleManager::LevelJump::UnderRoot:
		inserted = n->insertNodeSubRoot(jumpInfo[modifiers].code);
		break;
	}

	int exptexdLines = _styleManager->getStyleByCode(jumpInfo[modifiers].code)->expectedNodeNbTextLines();

	if (inserted->nbTextLines() != exptexdLines) {
		inserted->setNbTextLines(exptexdLines);
	}

}
void TextEditWidget::removeText() {

	if (_currentScript == nullptr) {
		return;
	}

	TextLine* sl = _currentScript->getLineAtLine(_cursor->line());
	int offset = (_cursor->extend() != 0) ? _cursor->extend() : -1;
	int sPos = _cursor->pos();
	int ePos;

	TextLine* tl = sl->lineAfterOffset(sPos, offset, ePos);

	if (offset < 0) {
		TextLine* lTmp = sl;
		sl = tl;
		tl = lTmp;

		int pTmp = sPos;
		sPos = ePos;
		ePos = pTmp;
	}

	TextLine* l = sl;

	TextNode* sN = sl->nodeParent();
	TextNode* tN = tl->nodeParent();

	TextNode* n = l->nodeParent();

	bool nodeCanBeEmptied = false;
	QList<TextNode*> emptiedNodes;

	while (l != tl) {

		TextLine* tmp = l->nextLine();

		if (l != sl and l != tl and (n == sN or n == tN)) {
			l->setText(""); //empty the lines of non removable blocks;
		}

		l = tmp;
		if (l->nodeParent() != n) { //went to a new node

			if (nodeCanBeEmptied) {
				emptiedNodes.push_back(n);
			}

			n = l->nodeParent();
			nodeCanBeEmptied = true;
		}
	}

	QString front = sl->getText().mid(0, sPos);

	QString e = tl->getText();
	QString back = "";

	if (ePos == e.length()) {
		if (sl != tl) {
			tl->setText("");

			if (sN != tN and tN->lines().last() == tl) {
				emptiedNodes.push_back(tN);
			}
		}
	} else if ((tN->lines().last() != tl or _nodeSupprBehavior == NodeSupprBehavior::KeepNonEmptyBlocks)
			   and sl != tl) {

		tl->setText(e.mid(ePos));

	} else {

		back = e.mid(ePos);
		tl->setText("");

		if (tN != sN) {
			emptiedNodes.push_back(tN);
		}
	}

	sl->setText(front + back);

	_cursor->setLine(sl->lineLineNumber());
	_cursor->setPos(sPos);
	for (TextNode* n : emptiedNodes) {
		n->clearFromDoc();
	}

}

TextEditWidget::NodeSupprBehavior TextEditWidget::getNodeSupprBehavior() const
{
    return _nodeSupprBehavior;
}

TextEditWidget::Cursor::Cursor(TextEditWidget* widget,
                               int line,
                               int pos,
                               int extend) :
	_widget(widget),
	_line(line),
	_pos(pos),
	_extend(extend)
{
	constrainLine();
	constrainPos();
}

int TextEditWidget::Cursor::line() const {
	return _line;
}
int TextEditWidget::Cursor::pos() const {
	return _pos;
}
int TextEditWidget::Cursor::extend() const {
	return _extend;
}

TextNode::NodeCoordinate TextEditWidget::Cursor::currentCoordinate() const {
	return {_line, _pos};
}

TextEditWidget::Cursor::DecomposedCursorPos TextEditWidget::Cursor::decomposePos(TextLine* currentLine) {

	if (currentLine == nullptr) {
		return {0,0,0};
	}

	if (_widget->_styleManager == nullptr) {
		return {0,0,0};
	}

	TextNode* p = currentLine->nodeParent();

	if (p == nullptr) {
		return {0,0,0};
	}

	AbstractTextNodeStyle* s = _widget->nodeStyle(p);

	if (s == nullptr) {
		return {0,0,0};
	}

	s->layNodeOut(p, _widget->computeLineWidth());

	QTextLine subLine = s->lineLayout(currentLine).lineForTextPosition(_pos + s->getPrefix(currentLine).length());

	if (!subLine.isValid()) {
		return {_line, 0, 0};
	}

	return {_line, subLine.lineNumber(), _pos - subLine.textStart() + ((subLine.textStart() > 0) ? s->getPrefix(currentLine).length() : 0) };
}

TextEditWidget::Cursor::CursorPos TextEditWidget::Cursor::composePos(TextLine* currentLine, DecomposedCursorPos decomposed) {

	if (currentLine == nullptr) {
		return {0,0};
	}

	if (_widget->_styleManager == nullptr) {
		return {0,0};
	}

	TextNode* p = currentLine->nodeParent();

	if (p == nullptr) {
		return {0,0};
	}

	AbstractTextNodeStyle* s = _widget->nodeStyle(p);

	if (s == nullptr) {
		return {0,0};
	}

	s->layNodeOut(p, _widget->computeLineWidth());

	QTextLine l = s->lineLayout(currentLine).lineAt(decomposed.subline);
	int compPos = l.textStart() - ((l.textStart() > 0) ? s->getPrefix(currentLine).length() : 0) + decomposed.subpos;

	return {decomposed.line, compPos};
}


TextEditWidget::Cursor::CursorState TextEditWidget::Cursor::getPositiveExtendState() {

	if (_extend >= 0) {
		return {_line, _pos, _extend};
	}

	TextNode::NodeCoordinate newCord = _widget->_currentScript->getCoordinateAfterOffset(
				TextNode::NodeCoordinate(_line, _pos),
				_extend);

	if (newCord.isValid()) {
		return {newCord.lineIndex, newCord.linePos, -_extend};
	}

	TextLine* tl = _widget->_currentScript->getLineAtLine(_line);

	if (tl != nullptr) {
		return {0,0,tl->nCharsBefore()+_pos};
	}

	return {0,0,0};

}

TextEditWidget::Cursor::CursorState TextEditWidget::Cursor::getExtendedSelectionState() {

	SelectionMode sMode = _widget->_selectionMode;

	CursorState state = getPositiveExtendState();

	if (_extend != 0 ) {
		if (sMode == SelectionMode::FullMultiNodes or
			sMode == SelectionMode::FullMultiNodesWithChild or
			sMode == SelectionMode::FullLeveldMultiNodes) {

			TextLine* tl = _widget->_currentScript->getLineAtLine(state.line);

			int linePos = tl->lineNodeIndexNumber();

			if (state.pos + state.extend <= tl->getText().length()) {
				return state;
			}

			state.extend += tl->nCharsBeforeInNode() + state.pos;
			state.line -= linePos;
			state.pos = 0;

			TextNode::NodeCoordinate source(state.line, state.pos);
			TextNode::NodeCoordinate offseted = _widget->_currentScript->getCoordinateAfterOffset(source, state.extend);

			TextLine* fl = _widget->_currentScript->getLineAtLine(offseted.lineIndex);
			state.extend += fl->nCharsAfterInNode() + fl->getText().length() - offseted.linePos;

			if (sMode == SelectionMode::FullMultiNodesWithChild) {
				TextNode* n = fl->nodeParent();

				if (n != nullptr) {
					state.extend += n->childNodes().size();
					for (TextNode* child : n->childNodes()) {
						state.extend += child->nChars();
					}
				}
			}

			if (sMode == SelectionMode::FullLeveldMultiNodes) {
				auto interval = TextNode::intervalWithFlatParentsLevel(tl->nodeParent(), fl->nodeParent());

				state.line = interval.first.lineIndex;
				state.extend = _widget->_currentScript->offsetBetweenCoordinates(interval.first, interval.second);
			}

		}
	}

	return state;

}

void TextEditWidget::Cursor::reset() {
	_line = 0;
	_pos = 0;
	Q_EMIT _widget->currentLineChanged(0);
}
void TextEditWidget::Cursor::move(int offset) {
	int oldLine = _line;
	_pos = _pos + offset;
	constrainPos();
	clearSelection();

	if (oldLine != _line) {
		Q_EMIT _widget->currentLineChanged(_line);
	}
}
void TextEditWidget::Cursor::jumpLines(int offset) {
	int oldLine = _line;
	_line += offset;
	constrainLine();
	constrainPosOnLine();
	clearSelection();

	if (oldLine != _line) {
		Q_EMIT _widget->currentLineChanged(_line);
	}
}
void TextEditWidget::Cursor::setLine(int line) {
	int oldLine = _line;
	_line = line;
	constrainLine();
	constrainPosOnLine();
	clearSelection();

	if (oldLine != _line) {
		Q_EMIT _widget->currentLineChanged(_line);
	}
}
void TextEditWidget::Cursor::setPos(int pos) {
	_pos = pos;
	constrainPosOnLine();
}
void TextEditWidget::Cursor::setState(CursorPos state) {
	int oldLine = _line;
	_pos = state.pos;
	_line = state.line;
	constrainLine();
	constrainPosOnLine();
	clearSelection();

	if (state.line != oldLine) {
		Q_EMIT _widget->currentLineChanged(_line);
	}
}

void TextEditWidget::Cursor::clearSelection() {
	_extend = 0;
}
void TextEditWidget::Cursor::setExtent(int extend) {

	if (_widget->_currentScript == nullptr) {
		return;
	}

	TextLine* tl = _widget->_currentScript->getLineAtLine(_line);

	if (tl == nullptr) {
		return;
	}

	int textPos = tl->nCharsBefore() + _pos + _line;
	int remainingText = tl->nChars() - _pos + tl->nCharsAfter() + _widget->_currentScript->maxLine() - _line - 1;

	if (extend < 0) {
		_extend = std::max(extend, -textPos);
	} else {
		_extend = std::min(extend, remainingText);
	}

	constrainExtend();

}

void TextEditWidget::Cursor::constrainExtend() {
	if (_widget->currentSelectionMode() == SelectionMode::SingleLine) {
		limitExtendToLine();
	} else if (_widget->currentSelectionMode() == SelectionMode::SingleNode) {
		limitExtendToNode();
	}
}

void TextEditWidget::Cursor::limitExtendToLine() {

	if (_widget->_currentScript == nullptr) {
		return;
	}

	TextLine* tl = _widget->_currentScript->getLineAtLine(_line);

	int lLen = tl->nChars();

	if (_extend < 0) {
		_extend = std::max(_extend, -_pos);
	} else {
		_extend = std::min(_extend, lLen - _pos);
	}

}
void TextEditWidget::Cursor::limitExtendToNode() {

	if (_widget->_currentScript == nullptr) {
		return;
	}

	TextLine* tl = _widget->_currentScript->getLineAtLine(_line);
	TextNode* tn = tl->nodeParent();

	int lLen = tn->nCharsInNode();

	int p = tl->nCharsBeforeInNode() + _pos;

	if (_extend < 0) {
		_extend = std::max(_extend, - p);
	} else {
		_extend = std::min(_extend, lLen - p);
	}

}

int TextEditWidget::Cursor::charDistance(CursorPos target) {

	if (target.line == _line) {
		return target.pos - _pos;
	}

	int d = target.line - _line;
	int dir = (d < 0) ? -1 : 1;

	TextLine* sl = _widget->getCurrentNode()->getLineAtLine(_line);

	int dist = 0;

	if (dir < 0) {
		dist = _pos;
	} else {
		dist = sl->nChars() - _pos;
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
		dist += target.pos;
	} else {
		dist += sl->nChars() - target.pos;
	}

	return dir*dist;

}

void TextEditWidget::Cursor::constrainLine() {
	if (_widget->_currentScript == nullptr) {
		return;
	}

	if (_line < 0) {
		_line = 0;
	}

	int maxLine = _widget->_currentScript->maxLine();
	if (_line >= maxLine) {
		_line = maxLine-1;
	}
}
void TextEditWidget::Cursor::constrainPos() {

	if (_widget->_currentScript == nullptr) {
		return;
	}

	TextLine* tl = _widget->_currentScript->getLineAtLine(_line);

	if (tl == nullptr) {
		return;
	}

	int len = tl->getText().length();

	if (len < 0) {
		_pos = 0;
		return;
	}

	if (_pos < 0) {
		while (_pos < 0) {
			if (_line > 0) {
				_line--;

				tl =  tl->previousLine();

				if (tl == nullptr) {
					_pos = 0;
					return;
				}

				len = tl->getText().length();
				if (len < 0) {
					_pos = 0;
					return;
				}

				_pos += len+1;
			} else {
				_pos = 0;
			}
		}
	}

	if (_pos > len) {

		int nLines  = _widget->_currentScript->maxLine();

		while(_pos > len) {
			if (_line < nLines-1) {
				_pos -= len+1;
				_line++;

				tl =  tl->nextLine();

				if (tl == nullptr) {
					_pos = 0;
					return;
				}

				len = tl->getText().length();
				if (len < 0) {
					_pos = 0;
					return;
				}
			} else {
				_pos = len;
			}
		}
	}

}
void TextEditWidget::Cursor::constrainPosOnLine() {
	if (_widget->_currentScript == nullptr) {
		return;
	}

	TextLine* tl = _widget->_currentScript->getLineAtLine(_line);

	if (tl == nullptr) {
		return;
	}

	int len = tl->getText().length();

	if (len < 0) {
		_pos = 0;
		return;
	}


	if (_pos < 0) {
		_pos = 0;
	}

	if (_pos > len) {
		_pos = len;
	}
}

} // namespace Sabrina
