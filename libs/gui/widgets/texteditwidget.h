#ifndef SABRINA_TEXTEDITWIDGET_H
#define SABRINA_TEXTEDITWIDGET_H

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

#include "text/textnode.h"
#include "text/abstracttextstyle.h"
#include "text/textstylemanager.h"

namespace Sabrina {

class TextEditWidget : public QWidget
{
	Q_OBJECT
public:

	/*!
	 * \brief The NodeSupprBehavior enum list possible behavior when suppr is triggered
	 *
	 * Possible behavior includes MergeContent (The remaining content of a block when the node jump is suppressed is added to the previous block,
	 * multiple lines blocks would follow the same rule, thus the empty lines would not be deleted)
	 * or KeepNonEmptyBlocks (A block cannot be removed as long as it is not empty).
	 */
	enum class NodeSupprBehavior {
		MergeContent,
		KeepNonEmptyBlocks
	};

	explicit TextEditWidget(QWidget *parent = nullptr);
	virtual ~TextEditWidget();

	void setStyleManager(TextStyleManager *styleManager);

	TextNode* getDocument() const;
	TextNode* getCurrentNode() const;
	void setCurrentScript(TextNode *root);
	void clearCurrentScript();

	bool hasScript() const;
	int currentLineStyleId() const;

	NodeSupprBehavior getNodeSupprBehavior() const;

Q_SIGNALS:

	void currentLineChanged(int line);

protected:

	int computeLineWidth() const;
	int computeLineStartingX() const;

	class Cursor {
	public:

		struct CursorState {
			int line;
			int pos;
		};

		struct DecomposedCursorState {
			int line;
			int subline;
			int subpos;
		};

		Cursor(TextEditWidget* widget, int line, int pos, int extend = 0);

		int line() const;
		int pos() const;
		int extend() const;

		DecomposedCursorState decomposePos(TextLine* currentLine);
		CursorState composePos(TextLine* currentLine, DecomposedCursorState decomposed);

		void reset();
		void move(int offset);
		void jumpLines(int offset);
		void setLine(int line);
		void setPos(int pos);
		void setState(CursorState state);

	private:

		void constrainLine();
		void constrainPos();
		void constrainPosOnLine();

		TextEditWidget* _widget;

		int _line;
		int _pos;
		int _extend;
	};

	void paintEvent(QPaintEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void inputMethodEvent(QInputMethodEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;

	int nodeHeight(TextNode* n);
	int scroolableUpDistance(int maxScroll);
	AbstractTextNodeStyle* nodeStyle(TextNode* n);

	void setCursorAtPoint(QPoint const& p, int vMargin = 5);
	TextNode* nodeAtHeight(int y, int * nodeH = nullptr);
	TextLine* lineAtPos(QPoint const& pos, int *cursorPos = nullptr);

	void scroll (int offset);
	void scrollToLine (int l);
	void scrollToLine (int l, int subL);
	Cursor::CursorState computeNewPosAfterJump(int nbPseudoLinesJump);

	void insertText(QString commited);
	void insertNextType(TextNode* n, Qt::KeyboardModifiers modifiers);
	void removeText();

	Cursor* _cursor;

	TextStyleManager* _styleManager;
	TextNode* _currentScript;
	TextNode* _baseIndex;
	int _baseIndexLine;
	int _baseIndexHeightDelta;
	TextNode* _endIndex;
	int _endIndexMargin;

	QMargins _internalMargins;
	NodeSupprBehavior _nodeSupprBehavior;

};

} // namespace Sabrina

#endif // SABRINA_TEXTEDITWIDGET_H
