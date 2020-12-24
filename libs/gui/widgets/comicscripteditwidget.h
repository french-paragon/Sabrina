#ifndef SABRINA_COMICSCRIPTEDITWIDGET_H
#define SABRINA_COMICSCRIPTEDITWIDGET_H

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

#include <QWidget>
#include <QModelIndex>

namespace Sabrina {

class Comicscript;

class ComicscriptEditWidget : public QWidget
{
	Q_OBJECT
public:

	explicit ComicscriptEditWidget(QWidget *parent = nullptr);
	virtual ~ComicscriptEditWidget();

	Comicscript *getCurrentScript() const;
	void setCurrentScript(Comicscript *script);
	void clearCurrentScript();

	bool hasScript() const;
	int currentLineType() const;

	void addPage();
	void addPanel();
	void addCaption();
	void addDialog();

signals:

	void currentLineChanged(int line);

protected:

	int computeLineWidth() const;
	int computeLineStartingX() const;


	class Cursor {
	public:
		Cursor(ComicscriptEditWidget* widget, int line, int pos, int extend = 0);

		int line() const;
		int pos() const;
		int extend() const;

		void reset();
		void move(int offset);
		void jumpLines(int offset);
		void setLine(int line);

	private:

		void constrainLine();
		void constrainPos();
		void constrainPosOnLine();

		ComicscriptEditWidget* _widget;

		int _line;
		int _pos;
		int _extend;
	};

	void paintEvent(QPaintEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void inputMethodEvent(QInputMethodEvent *event) override;

	QModelIndex getIndexAtLine(int l, int * idLine = nullptr) const;
	int indexLine(QModelIndex id);
	int indexHeight(QModelIndex index);
	int lineLen(int l);
	int countLines() const;

	void scroll (int offset);
	void scrollToLine (int l);

	QFont getFont(int blockType);
	int getLineHeight(int blockType);
	QMargins getMargins(int blockType);
	int renderIndex(QModelIndex const& index,
					QPainter & painter,
					int x,
					int y,
					int l,
					Cursor* cursor);

	void insertText(QString commited);
	void insertNextType(const QModelIndex &id, int modifiers);

	Cursor* _cursor;

	Comicscript* _currentScript;
	QModelIndex _baseIndex;
	int _baseIndexLine;
	int _baseIndexHeightDelta;
	QModelIndex _endIndex;
	int _endIndexMargin;

	QMargins _margins;

};

} // namespace Sabrina

#endif // SABRINA_COMICSCRIPTEDITWIDGET_H
