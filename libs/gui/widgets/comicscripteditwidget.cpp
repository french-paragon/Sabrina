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
	QWidget(parent),
	_currentScript(nullptr),
	_baseIndex(),
	_baseIndexLine(0),
	_baseIndexHeightDelta(0),
	_endIndex(),
	_endIndexMargin(-1),
	_margins(25, 25, 25, 25)
{
	_cursor = new Cursor(this, 0, 0, 0);
	setFocusPolicy(Qt::StrongFocus);
}

ComicscriptEditWidget::~ComicscriptEditWidget() {
	delete _cursor;
}

Comicscript *ComicscriptEditWidget::getCurrentScript() const
{
	return _currentScript;
}

void ComicscriptEditWidget::setCurrentScript(Comicscript *script)
{
	if (script != _currentScript) {

		if (_currentScript != nullptr) {
			disconnect(_currentScript, &QObject::destroyed, this, &ComicscriptEditWidget::clearCurrentScript);
		}

		_currentScript = script;

		if (_currentScript != nullptr) {
			connect(_currentScript, &QObject::destroyed, this, &ComicscriptEditWidget::clearCurrentScript);
		}

		_cursor->reset();
		update();
	}
}

void ComicscriptEditWidget::clearCurrentScript() {
	if (_currentScript != nullptr) {
		disconnect(_currentScript, &QObject::destroyed, this, &ComicscriptEditWidget::clearCurrentScript);
		_currentScript = nullptr;
		update();
	}
}

bool ComicscriptEditWidget::hasScript() const {
	return _currentScript != nullptr;
}

int ComicscriptEditWidget::currentLineType() const {
	QModelIndex id = getIndexAtLine(_cursor->line());

	if (id == QModelIndex()) {
		return ComicscriptModel::ComicstripBlock::OTHER;
	}

	return id.data(ComicscriptModel::TypeRole).toInt();
}

void ComicscriptEditWidget::addPage() {

	if (_currentScript == nullptr) {
		return;
	}

	QModelIndex id = getIndexAtLine(_cursor->line());

	if (id == QModelIndex()) {
		_currentScript->getModel()->addPage(0);
		update();
		return;
	}

	while (id.parent() != QModelIndex()) {
		id = id.parent();
	}

	_currentScript->getModel()->addPage(id.row()+1);
	QModelIndex nid = _currentScript->getModel()->index(id.row()+1, 0);

	if (nid != QModelIndex()) {
		int l = indexLine(nid);
		_cursor->setLine(l);
		scrollToLine(l);
	}

	update();

}
void ComicscriptEditWidget::addPanel() {

	if (_currentScript == nullptr) {
		return;
	}

	QModelIndex id = getIndexAtLine(_cursor->line());

	if (id == QModelIndex()) {
		return;
	}

	int r = -1;
	while (id.data(ComicscriptModel::TypeRole).toInt() != ComicscriptModel::ComicstripBlock::PAGE) {
		r = id.row();
		id = id.parent();

		if (id == QModelIndex()) {
			return;
		}
	}

	r += 1;

	_currentScript->getModel()->addPanel(id, r);
	QModelIndex nid = _currentScript->getModel()->index(r, 0, id);

	if (nid != QModelIndex()) {
		int l = indexLine(nid);
		_cursor->setLine(l);
		scrollToLine(l);
	}

	update();

}
void ComicscriptEditWidget::addCaption() {

	if (_currentScript == nullptr) {
		return;
	}

	QModelIndex id = getIndexAtLine(_cursor->line());

	if (id == QModelIndex()) {
		return;
	}

	int r = -1;
	while (id.data(ComicscriptModel::TypeRole).toInt() != ComicscriptModel::ComicstripBlock::PANEL) {
		r = id.row();
		id = id.parent();

		if (id == QModelIndex()) {
			return;
		}
	}

	r += 1;

	_currentScript->getModel()->addCaption(id, r);
	QModelIndex nid = _currentScript->getModel()->index(r, 0, id);

	if (nid != QModelIndex()) {
		int l = indexLine(nid);
		_cursor->setLine(l);
		scrollToLine(l);
	}

	update();

}
void ComicscriptEditWidget::addDialog() {

	if (_currentScript == nullptr) {
		return;
	}

	QModelIndex id = getIndexAtLine(_cursor->line());

	if (id == QModelIndex()) {
		return;
	}

	int r = -1;
	while (id.data(ComicscriptModel::TypeRole).toInt() != ComicscriptModel::ComicstripBlock::PANEL) {
		r = id.row();
		id = id.parent();

		if (id == QModelIndex()) {
			return;
		}
	}

	r += 1;

	_currentScript->getModel()->addDialog(id, r);
	QModelIndex nid = _currentScript->getModel()->index(r, 0, id);

	if (nid != QModelIndex()) {
		int l = indexLine(nid);
		_cursor->setLine(l);
		scrollToLine(l);
	}

	update();
}

void ComicscriptEditWidget::paintEvent(QPaintEvent *event) {

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	//clear region
	painter.fillRect(event->rect().x(),
					 event->rect().y(),
					 event->rect().width(),
					 event->rect().height(),
					 QColor(255, 255, 255));

	if (_currentScript == nullptr) {
		return;
	}

	QModelIndex id = _baseIndex;
	int v_pos = _margins.top() - _baseIndexHeightDelta;
	int l = _baseIndexLine;

	ComicscriptModel* m = _currentScript->getModel();

	while(v_pos < height()) {
		Cursor* c = nullptr;
		int nLines = 1;

		if (id != QModelIndex()) {
			nLines = id.data(ComicscriptModel::NbLinesRole).toInt();
		}

		if (hasFocus()) {
			if (_cursor->line() >= l and _cursor->line() < l+nLines) {
				c = _cursor;
			}
		}

		v_pos += renderIndex(id, painter, _margins.left(), v_pos, l, c);
		l += nLines;
		_endIndex = id;
		id = m->getNextItem(id);

		if (id == QModelIndex()) {
			break;
		}
	}

	_endIndexMargin = v_pos - height();

}

QFont ComicscriptEditWidget::getFont(int blockType) {
	switch (blockType) {
	case ComicscriptModel::ComicstripBlock::OTHER:
		return QFont("Monospace", 18);
	default:
		return QFont("Monospace", 12);
	}
}


int ComicscriptEditWidget::getLineHeight(int blockType) {
	QFont f = getFont(blockType);
	QFontMetrics fm(f);
	return fm.height() + fm.leading();
}

QMargins ComicscriptEditWidget::getMargins(int blockType) {
	switch (blockType) {
	case ComicscriptModel::ComicstripBlock::OTHER:
		return QMargins(0,0,0,10);
	case ComicscriptModel::ComicstripBlock::PAGE:
		return QMargins(0, 30,0,5);
	case ComicscriptModel::ComicstripBlock::PANEL:
		return QMargins(15,10,0,5);
	case ComicscriptModel::ComicstripBlock::CAPTION:
		return QMargins(25,10,0,5);
	case ComicscriptModel::ComicstripBlock::DIALOG:
		return QMargins(35,10,0,5);
	default:
		return QMargins(0,0,0,5);
	}
}

int ComicscriptEditWidget::renderIndex(QModelIndex const& index,
									   QPainter & painter,
									   int x,
									   int y,
									   int l,
									   Cursor *cursor) {

	QString text = (index == QModelIndex()) ? _currentScript->getTitle() : index.data(ComicscriptModel::TextRole).toString();
	QTextLayout layout(text);

	int type = ComicscriptModel::ComicstripBlock::OTHER;
	if (index != QModelIndex()) type = index.data(ComicscriptModel::TypeRole).toInt();

	layout.setFont(getFont(type));

	int lineHeight = getLineHeight(type);
	int lineWidth = computeLineWidth();
	QMargins margins = getMargins(type);

	int height = margins.top();
	int hDelta = margins.top();

	if (type == ComicscriptModel::ComicstripBlock::PAGE or
		type == ComicscriptModel::ComicstripBlock::PANEL) {

		QString descr = index.data(ComicscriptModel::DescrRole).toString() + ": ";

		QFont f = getFont(type);
		f.setBold(true);

		QFontMetrics fm(f);
		int w = fm.horizontalAdvance(descr);

		painter.setFont(f);
		painter.drawText(QPointF(x + margins.left(), y + height + fm.ascent()), descr);

		margins.setLeft(margins.left() + w + 5);

	} else if (type == ComicscriptModel::ComicstripBlock::DIALOG) {

		QString descr = index.data(ComicscriptModel::DescrRole).toString() + ": ";
		QString dash (" \u2013 ");

		QFont f = getFont(type);
		bool b = f.bold();
		f.setBold(true);

		QFontMetrics fm(f);
		int w = fm.horizontalAdvance(dash) + 2;

		painter.setFont(f);
		painter.drawText(QPointF(x + margins.left() + w, y + height + fm.ascent()), descr);

		if (cursor != nullptr and index.data(ComicscriptModel::NbLinesRole).toInt() > 1 and cursor->line() == l) {
			int dx = fm.horizontalAdvance(descr, cursor->pos());
			painter.drawRect(x + margins.left() + w + dx, y + height, 1, fm.ascent() + fm.descent());
		}

		f.setBold(b);

		painter.drawText(QPointF(x + margins.left(), y + height + 2*fm.ascent() + 5), dash);

		hDelta += fm.ascent() + 5;
		height += fm.ascent() + 5;
		margins.setLeft(margins.left() + w);
	}

	layout.beginLayout();
	while (1) {
		QTextLine line = layout.createLine();
		if (!line.isValid())
			break;

		line.setLineWidth(lineWidth - margins.left() - margins.right());
		line.setPosition(QPointF(margins.left(), height));
		height += lineHeight;
	}
	layout.endLayout();

	QPoint pos(x, y);
	layout.draw(&painter, pos);

	if (cursor != nullptr) {
		if (index.data(ComicscriptModel::NbLinesRole).toInt() > 1 and cursor->line() == l+1) {
			layout.drawCursor(&painter, pos, cursor->pos());
		} else if (index == QModelIndex() or index.data(ComicscriptModel::NbLinesRole).toInt() == 1) {
			layout.drawCursor(&painter, pos, cursor->pos());
		}
	}

	hDelta += layout.boundingRect().height() + margins.bottom();
	return hDelta;

}

void ComicscriptEditWidget::keyPressEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Left) {
		_cursor->move(-1);
		scrollToLine(_cursor->line());
		update();
	} else if (event->key() == Qt::Key_Right) {
		_cursor->move(1);
		scrollToLine(_cursor->line());
		update();
	} else if (event->key() == Qt::Key_Up) {
		_cursor->jumpLines(-1);
		scrollToLine(_cursor->line());
		update();
	} else if (event->key() == Qt::Key_Down) {
		_cursor->jumpLines(1);
		scrollToLine(_cursor->line());
		update();
	} else if (event->key() == Qt::Key_Return or event->key() == Qt::Key_Enter) {
		int idL;
		QModelIndex id = getIndexAtLine(_cursor->line(), &idL);
		if (idL < 0) {
			return;
		}

		insertNextType(id, event->modifiers());
		_cursor->setLine(idL + id.data(ComicscriptModel::NbLinesRole).toInt());

		scrollToLine(_cursor->line());
		update();

	} else if(!event->text().isEmpty()) {

		int idLine;
		QModelIndex id = getIndexAtLine(_cursor->line(), &idLine);

		if (idLine < 0) {
			return;
		}

		QString line;

		if (id == QModelIndex()) {
			line = _currentScript->getTitle();
		} else {
			if (id.data(ComicscriptModel::TypeRole).toInt() == ComicscriptModel::ComicstripBlock::DIALOG and
				idLine == _cursor->line()) {
				line = id.data(ComicscriptModel::DescrRole).toString();
			} else {
				line = id.data(ComicscriptModel::TextRole).toString();
			}
		}

		int pLen = line.length();

		int p = _cursor->pos();

		if (event->text().contains(QChar('\b'))) {
			QStringList lst = event->text().split(QChar('\b'), Qt::KeepEmptyParts);
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
			line.insert(p, event->text());
		}

		int c_offset = line.length() - pLen;

		ComicscriptModel* m = _currentScript->getModel();

		if (id == QModelIndex()) {
			_currentScript->setTitle(line);
		} else {
			if (id.data(ComicscriptModel::TypeRole).toInt() == ComicscriptModel::ComicstripBlock::DIALOG and
				idLine == _cursor->line()) {
				m->setData(id, line, ComicscriptModel::DescrRole);
			} else {
				m->setData(id, line, ComicscriptModel::TextRole);
			}
		}

		_cursor->move(c_offset);
		update();

	} else {
		QWidget::keyPressEvent(event);
	}

}



void ComicscriptEditWidget::insertNextType(QModelIndex const& id, int modifiers) {

	if (_currentScript == nullptr) {
		return;
	}

	if (id == QModelIndex()) {
		_currentScript->getModel()->addPage(0);
		return;
	}

	int l = _cursor->line();

	int pType = id.data(ComicscriptModel::TypeRole).toInt();

	switch (pType) {
	case ComicscriptModel::ComicstripBlock::OTHER:
		return; //should not happen, case already treated above.
	case ComicscriptModel::ComicstripBlock::PAGE:
		if (modifiers == Qt::ControlModifier) {
			_currentScript->getModel()->addPage(id.row()+1);
		} else if (modifiers == Qt::NoModifier) {
			_currentScript->getModel()->addPanel(id, 0);
		}
		break;
	case ComicscriptModel::ComicstripBlock::PANEL:
		if (modifiers == Qt::ShiftModifier) {
			_currentScript->getModel()->addPage(id.parent().row()+1);
		} else if (modifiers == Qt::ControlModifier) {
			_currentScript->getModel()->addPanel(id.parent(), id.row()+1);
		} else if (modifiers == Qt::NoModifier) {
			_currentScript->getModel()->addCaption(id, 0);
		}
		break;
	case ComicscriptModel::ComicstripBlock::DIALOG:
		if (l == indexLine(id)) {
			break;
		}
	case ComicscriptModel::ComicstripBlock::CAPTION:
		if (modifiers == Qt::ShiftModifier) {
			_currentScript->getModel()->addPanel(id.parent().parent(), id.parent().row()+1);
		} else if (modifiers == Qt::ControlModifier) {
			_currentScript->getModel()->addCaption(id.parent(), id.row()+1);
		} else if (modifiers == (Qt::ControlModifier | Qt::AltModifier)) {
			_currentScript->getModel()->addPage(id.parent().parent().row()+1);
		}  else if (modifiers == Qt::NoModifier) {
			_currentScript->getModel()->addDialog(id.parent(), id.row()+1);
		}
		break;
	}

}

int ComicscriptEditWidget::countLines() const {

	if (_currentScript == nullptr) {
		return 0;
	}

	QModelIndex id = QModelIndex();
	int l = 1;

	ComicscriptModel* m = _currentScript->getModel();

	while(1) {

		id = m->getNextItem(id);

		if (id == QModelIndex()) {
			break;
		}

		l += id.data(ComicscriptModel::NbLinesRole).toInt();
	}

	return l;
}

QModelIndex ComicscriptEditWidget::getIndexAtLine(int l, int *idLine) const {

	if (_currentScript == nullptr) {
		return QModelIndex();
	}

	if (l <= 0) {
		return QModelIndex();
	}

	QModelIndex id = QModelIndex();

	ComicscriptModel* m = _currentScript->getModel();

	int i;
	for(i = 0; i < l; i += id.data(ComicscriptModel::NbLinesRole).toInt()) {
		id = m->getNextItem(id);
		if (id == QModelIndex()) {
			if (idLine != nullptr) {
				*idLine = -1;
			}
			return id;
		}
	}

	if (idLine != nullptr) {
		*idLine = i - id.data(ComicscriptModel::NbLinesRole).toInt() + 1;
	}

	return id;

}

int ComicscriptEditWidget::indexLine(QModelIndex id) {

	if (_currentScript == nullptr) {
		return -1;
	}

	if (id == QModelIndex()) {
		return 0;
	}

	ComicscriptModel* m = _currentScript->getModel();
	int l = 1;

	QModelIndex p = id;

	while (1) {
		p = m->getPrevItem(p);

		if (p == QModelIndex()) {
			break;
		}

		l += p.data(ComicscriptModel::NbLinesRole).toInt();
	}

	return l;
}

int ComicscriptEditWidget::indexHeight(QModelIndex index) {

	QString text = (index == QModelIndex()) ? _currentScript->getTitle() : index.data(ComicscriptModel::TextRole).toString();
	QTextLayout layout(text);

	int type = ComicscriptModel::ComicstripBlock::OTHER;
	if (index != QModelIndex()) type = index.data(ComicscriptModel::TypeRole).toInt();

	layout.setFont(getFont(type));

	int lineHeight = getLineHeight(type);
	int lineWidth = computeLineWidth();
	QMargins margins = getMargins(type);

	int hDelta = margins.top();

	if (type == ComicscriptModel::ComicstripBlock::PAGE or
		type == ComicscriptModel::ComicstripBlock::PANEL) {

		QString descr = index.data(ComicscriptModel::DescrRole).toString() + ": ";

		QFont f = getFont(type);
		f.setBold(true);

		QFontMetrics fm(f);
		int w = fm.horizontalAdvance(descr);

		margins.setLeft(margins.left() + w + 5);

	} else if (type == ComicscriptModel::ComicstripBlock::DIALOG) {

		QString descr = index.data(ComicscriptModel::DescrRole).toString() + ": ";
		QString dash (" \u2013 ");

		QFont f = getFont(type);
		f.setBold(true);

		QFontMetrics fm(f);
		int w = fm.horizontalAdvance(dash) + 2;

		hDelta += fm.ascent() + 5;
		margins.setLeft(margins.left() + w);
	}

	layout.beginLayout();
	while (1) {
		QTextLine line = layout.createLine();
		if (!line.isValid())
			break;

		line.setLineWidth(lineWidth - margins.left() - margins.right());
		hDelta += lineHeight;
	}
	layout.endLayout();

	hDelta += margins.bottom();
	return hDelta;

}

int ComicscriptEditWidget::lineLen(int l) {

	if (_currentScript == nullptr) {
		return -1;
	}

	if (l < 0) {
		return -1;
	}

	int idL;
	QModelIndex id = getIndexAtLine(l, &idL);

	if (idL < 0) {
		return -1;
	}

	if (id == QModelIndex()) {
		return _currentScript->getTitle().length();
	} else if (id.data(ComicscriptModel::TypeRole).toInt() == ComicscriptModel::ComicstripBlock::DIALOG and
		idL == l ) {
		return id.data(ComicscriptModel::DescrRole).toString().length();
	} else {
		return id.data(ComicscriptModel::TextRole).toString().length();
	}
}

void ComicscriptEditWidget::scroll (int offset) {

	if (_currentScript == nullptr) {
		return;
	}

	if (offset == 0) {
		return;
	}

	ComicscriptModel* m = _currentScript->getModel();

	if (_baseIndex == QModelIndex() and offset < 0) { //can't scroll up
		return;
	}

	while (_endIndexMargin < 0 and m->getNextItem(_endIndex) != QModelIndex()) {//new indices have been added
		_endIndex = m->getNextItem(_endIndex);
		_endIndexMargin += indexHeight(_endIndex);
	}

	if (_endIndexMargin < 0 and offset > 0) { //can't scroll down
		return;
	}

	int p = offset;

	if (p < 0) { //scroll up

		p = -p;
		while (p > _baseIndexHeightDelta) {
			if (_baseIndex == QModelIndex()) {
				break;
			}
			p -= _baseIndexHeightDelta;
			_baseIndex = m->getPrevItem(_baseIndex);
			_baseIndexHeightDelta = indexHeight(_baseIndex);
		}
		_baseIndexHeightDelta = std::max(_baseIndexHeightDelta - p, 0);
	} else { //scroll down

		int bIdMargin = indexHeight(_baseIndex) - _baseIndexHeightDelta;

		while (p > 0) {

			if (_endIndexMargin < bIdMargin) { //need to update bottom index;

				p -= _endIndexMargin;
				bIdMargin -= _endIndexMargin;
				_baseIndexHeightDelta += _endIndexMargin;

				if (m->getNextItem(_endIndex) == QModelIndex()) { //reached the end
					_endIndexMargin = 0;
					break;
				} else {
					_endIndex = m->getNextItem(_endIndex);
					_endIndexMargin = indexHeight(_endIndex);
				}
			} else { //need to update top index

				_endIndexMargin -= bIdMargin;

				_baseIndex = m->getNextItem(_baseIndex);
				bIdMargin = indexHeight(_baseIndex);
				_baseIndexHeightDelta = 0;
			}
		}
	}

	_baseIndexLine = indexLine(_baseIndex);
}

void ComicscriptEditWidget::scrollToLine (int l) {

	if (_currentScript == nullptr) {
		return;
	}

	if (l < 0) {
		return;
	}

	int endIndexLine = indexLine(_endIndex);

	if (l > _baseIndexLine and l < endIndexLine) {
		return;
	}

	QModelIndex target = getIndexAtLine(l);

	if (l > 0 and target == QModelIndex()) {
		return;
	}

	ComicscriptModel* m = _currentScript->getModel();
	int dh = 0;

	QModelIndex pos;

	if (l <= _baseIndexLine) { //needs to scroll up
		pos = _baseIndex;
		dh -= _baseIndexHeightDelta;

		while (pos != target and pos != QModelIndex()) {
			pos = m->getPrevItem(pos);
			dh -= indexHeight(pos);
		}
	} else if (l >= endIndexLine) { //needs to scroll forward

		pos = _endIndex;
		dh += std::abs(_endIndexMargin);

		while (pos != target and pos != QModelIndex()) {
			pos = m->getNextItem(pos);
			dh += indexHeight(pos);
		}

		if (pos != QModelIndex()) {
			pos = m->getNextItem(pos);
			dh += indexHeight(pos);
		}
	}

	if (dh == 0) {
		return;
	}

	scroll(dh);

}

ComicscriptEditWidget::Cursor::Cursor(ComicscriptEditWidget *widget,
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

int ComicscriptEditWidget::Cursor::line() const {
	return _line;
}
int ComicscriptEditWidget::Cursor::pos() const {
	return _pos;
}
int ComicscriptEditWidget::Cursor::extend() const {
	return _extend;
}

void ComicscriptEditWidget::Cursor::reset() {
	_line = 0;
	_pos = 0;
	_widget->currentLineChanged(0);
}

void ComicscriptEditWidget::Cursor::move(int offset) {

	int oldLine = _line;
	_pos = _pos + offset;
	constrainPos();

	if (oldLine != _line) {
		_widget->currentLineChanged(_line);
	}

}
void ComicscriptEditWidget::Cursor::jumpLines(int offset) {
	int oldLine = _line;
	_line += offset;
	constrainLine();
	constrainPosOnLine();

	if (oldLine != _line) {
		_widget->currentLineChanged(_line);
	}
}
void ComicscriptEditWidget::Cursor::setLine(int line) {
	int oldLine = _line;
	_line = line;
	constrainLine();
	constrainPosOnLine();

	if (oldLine != _line) {
		_widget->currentLineChanged(_line);
	}
}

void ComicscriptEditWidget::Cursor::constrainLine() {

	if (_widget->_currentScript == nullptr) {
		return;
	}

	if (_line < 0) {
		_line = 0;
	}

	int maxLine = _widget->countLines();
	if (_line >= maxLine) {
		_line = maxLine-1;
	}

}
void ComicscriptEditWidget::Cursor::constrainPos() {

	if (_widget->_currentScript == nullptr) {
		return;
	}

	int len = _widget->lineLen(_line);

	if (len < 0) {
		_pos = 0;
		return;
	}

	if (_pos < 0) {
		while (_pos < 0) {
			if (_line > 0) {
				_line--;

				len = _widget->lineLen(_line);
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

		int nLines = _widget->countLines();

		while(_pos > len) {
			if (_line < nLines-1) {
				_pos -= len+1;
				_line++;

				len = _widget->lineLen(_line);
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

void ComicscriptEditWidget::Cursor::constrainPosOnLine() {

	if (_widget->_currentScript == nullptr) {
		return;
	}

	int len = _widget->lineLen(_line);

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

int ComicscriptEditWidget::computeLineWidth() const {
	return width() - _margins.left() - _margins.right();
}
int ComicscriptEditWidget::computeLineStartingX() const {
	return _margins.left();
}

} // namespace Sabrina
