#include "texteditwidget.h"

#include <QPainter>

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
	_internalMargins(25, 25, 25, 25)
{
	_cursor = new Cursor(this, 0, 0, 0);
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_InputMethodEnabled, true);
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

	while(v_pos < height()) {
		Cursor* c = nullptr;

		if (hasFocus()) {
			if (_cursor->line() >= l and _cursor->line() < l+n->nbTextLines()) {
				c = _cursor;
			}
		}

		AbstractTextNodeStyle* s = nodeStyle(n);

		if (s == nullptr) {
			continue;
		}

		QPointF o(computeLineStartingX(), v_pos);

		if (c != nullptr) {
			s->renderNode(n, o, availableWidth, painter, c->line() - l, c->pos());
		} else {
			s->renderNode(n, o, availableWidth, painter);
		}

		v_pos += s->nodeHeight(n, availableWidth);
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
		Cursor::CursorState cs = computeNewPosAfterJump(-1);
		_cursor->setState(cs);
		scrollToLine(_cursor->line());
		update();
	} else if (event->key() == Qt::Key_Down) {
		Cursor::CursorState cs = computeNewPosAfterJump(1);
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

int TextEditWidget::nodeHeight(TextNode* n) {
	AbstractTextNodeStyle* s = nodeStyle(n);

	if (s == nullptr) {
		return 0;
	}

	return s->nodeHeight(n, computeLineWidth());

}
AbstractTextNodeStyle* TextEditWidget::nodeStyle(TextNode* n) {

	AbstractTextNodeStyle* s = _styleManager->getStyleByCode(n->styleId());

	if (s == nullptr) {
		s = _styleManager->getStyleByCode(_styleManager->getDefaultStyleCode());
	}

	return s;
}

void TextEditWidget::scroll (int offset) {

	if (_currentScript == nullptr) {
		return;
	}

	if (offset == 0) {
		return;
	}

	if (_baseIndex == _currentScript and offset < 0) { //can't scroll up
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

		int bIdMargin = nodeHeight(_baseIndex) - _baseIndexHeightDelta;

		while (p > 0) {

			if (_endIndexMargin < bIdMargin) { //need to update bottom index;

				p -= _endIndexMargin;
				bIdMargin -= _endIndexMargin;
				_baseIndexHeightDelta += _endIndexMargin;

				if (_endIndex->nextNode() == nullptr) { //reached the end
					_endIndexMargin = 0;

					if (p > 0) {
						int d = std::min(_internalMargins.bottom(), p);
						_endIndexMargin = -d;

						_baseIndexHeightDelta += d;
						bIdMargin -= d;

						while (bIdMargin < 0) {
							_baseIndex = _baseIndex->nextNode();
							_baseIndexHeightDelta = bIdMargin;
							bIdMargin += nodeHeight(_baseIndex);
						}
					}
					break;
				} else {
					_endIndex = _endIndex->nextNode();
					_endIndexMargin = nodeHeight(_endIndex);
				}
			} else { //need to update top index

				_endIndexMargin -= bIdMargin;

				_baseIndex = _baseIndex->nextNode();
				bIdMargin = nodeHeight(_baseIndex);
				_baseIndexHeightDelta = 0;
			}
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

TextEditWidget::Cursor::CursorState TextEditWidget::computeNewPosAfterJump(int nbPseudoLinesJump) {

	TextNode* n = getCurrentNode();
	AbstractTextNodeStyle* style = nodeStyle(n);

	if (style == nullptr) {
		return {0,0};
	}

	style->layNodeOut(n, computeLineWidth());

	int aLine = _cursor->line() - n->nodeLine();

	TextLine* tl = n->lineAt(aLine);

	Cursor::DecomposedCursorState s = _cursor->decomposePos(tl);

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

	TextLine* tLine = n->lineAt(_cursor->line() - idLine);
	QString line = tLine->getText();

	int pLen = line.length();

	int p = _cursor->pos();

	if (commited.contains(QChar('\b'))) {
		QStringList lst = commited.split(QChar('\b'), Qt::KeepEmptyParts);
		QString last = lst.takeLast();

		for (QString const& s : lst) {
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

TextEditWidget::Cursor::DecomposedCursorState TextEditWidget::Cursor::decomposePos(TextLine* currentLine) {

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

TextEditWidget::Cursor::CursorState TextEditWidget::Cursor::composePos(TextLine* currentLine, DecomposedCursorState decomposed) {

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

void TextEditWidget::Cursor::reset() {
	_line = 0;
	_pos = 0;
	_widget->currentLineChanged(0);
}
void TextEditWidget::Cursor::move(int offset) {
	int oldLine = _line;
	_pos = _pos + offset;
	constrainPos();

	if (oldLine != _line) {
		_widget->currentLineChanged(_line);
	}
}
void TextEditWidget::Cursor::jumpLines(int offset) {
	int oldLine = _line;
	_line += offset;
	constrainLine();
	constrainPosOnLine();

	if (oldLine != _line) {
		_widget->currentLineChanged(_line);
	}
}
void TextEditWidget::Cursor::setLine(int line) {
	int oldLine = _line;
	_line = line;
	constrainLine();
	constrainPosOnLine();

	if (oldLine != _line) {
		_widget->currentLineChanged(_line);
	}
}
void TextEditWidget::Cursor::setPos(int pos) {
	_pos = pos;
	constrainPosOnLine();
}
void TextEditWidget::Cursor::setState(CursorState state) {
	int oldLine = _line;
	_pos = state.pos;
	_line = state.line;
	constrainLine();
	constrainPosOnLine();

	if (state.line != oldLine) {
		_widget->currentLineChanged(_line);
	}
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
