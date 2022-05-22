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

	/*!
	 * \brief The SelectionMode enum represent a mode of selection
	 *
	 * For certain types of document, it makes little sense to allow the user to select parts of multiple lines/paragraphs or even select text across multiple lines/paragraphs.
	 * This enum describe the different selection modes that can be used by the widget.
	 */
	enum class SelectionMode {
		SingleLine, //! Selections are constrained to a single line
		SingleNode, //! Selections are constrained to a single node in the document
		FullMultiNodes, //! if the selection span more than a single node, then all the text is selected in each and every node
		FullMultiNodesWithChild, //! same as FullMultiNodes, but child nodes are considered part of parents nodes.
		FullLeveldMultiNodes, //! same as FullMultiNodesWithChild, but all the childs of the nodes with the smallest level in the interval are included.
		Text //! free selection across multiple lines and nodes.
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

	QString getTextInSelection() const;
	QString getHtmlInSelection() const;
	QJsonDocument getJsonInSelection() const;

Q_SIGNALS:

	void currentLineChanged(int line);

protected:

	void configureActions();

	void setSelectionMode(SelectionMode mode);
	SelectionMode currentSelectionMode() const;

	void setHighlightCurrent(bool highlight);
	bool highlightCurrent() const;

	int computeLineWidth() const;
	int computeLineStartingX() const;

	class Cursor {
	public:

		struct CursorPos {
			CursorPos() = default;
			CursorPos(int lineIndex, int posIndex) : line(lineIndex), pos(posIndex) {}
			CursorPos(TextNode::NodeCoordinate coord) : line(coord.lineIndex), pos(coord.linePos) {}
			int line;
			int pos;
		};

		struct DecomposedCursorPos {
			int line;
			int subline;
			int subpos;
		};

		struct CursorState {
			int line;
			int pos;
			int extend;
		};

		Cursor(TextEditWidget* widget, int line, int pos, int extend = 0);

		int line() const;
		int pos() const;
		int extend() const;

		TextNode::NodeCoordinate currentCoordinate() const;

		DecomposedCursorPos decomposePos(TextLine* currentLine);
		CursorPos composePos(TextLine* currentLine, DecomposedCursorPos decomposed);

		//! \brief If the current state is valid, return a state guaranteed to be equivalent to the cursor current state, but with a positive extend.
		CursorState getPositiveExtendState();

		//! \brief get the (virtual) state of the cursor after applying the constraints due to the selection mode and other selection extender.
		CursorState getExtendedSelectionState();

		void reset();
		void move(int offset);
		void jumpLines(int offset);
		void setLine(int line);
		void setPos(int pos);
		void setState(CursorPos state);
		void clearSelection();
		void setExtent(int extend);
		void constrainExtend();
		void limitExtendToLine();
		void limitExtendToNode();

		int charDistance(CursorPos target);

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
	void mouseMoveEvent(QMouseEvent *event) override;

	int nodeHeight(TextNode* n);
	int scroolableUpDistance(int maxScroll);
	AbstractTextNodeStyle* nodeStyle(TextNode* n);

	void setCursorAtPoint(QPoint const& p, int vMargin = 5);
	void setSelectionAtPoint(QPoint const& p, int vMargin = 5);
	TextNode::NodeCoordinate textCordAtPoint(QPoint const& point);
	TextNode* nodeAtHeight(int y, int * nodeH = nullptr);
	TextLine* lineAtPos(QPoint const& pos, int *cursorPos = nullptr);

	void scroll (int offset);
	void scrollToLine (int l);
	void scrollToLine (int l, int subL);
	Cursor::CursorPos computeNewPosAfterJump(int nbPseudoLinesJump);

	void insertText(QString commited);
	void insertNextType(TextNode* n, Qt::KeyboardModifiers modifiers);
	TextNode* insertNode(TextNode* n, int codeStyle, TextStyleManager::LevelJump level);
	TextNode* setNodeStyleId(TextNode* n, int codeStyle);
	void removeText();

	void copyTextToClipboard() const;
	void cutTextToClipboard();
	void pasteTxtFromClipboard();
	void pasteTextFromClipboard();
	void pasteTxt(QString const& txt);
	void pasteDoc(QByteArray const& docData);

	TextNode* configureNodeFromJson(TextNode* currentNode, QJsonObject const& obj, bool eraseStyle = true, QStringList lastLines = {});
	TextNode* setLinesInTextNode(TextNode* node, QStringList const& lines);

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

	QTextCharFormat _selectionFormat;
	QPoint _clickPos;

private:
	SelectionMode _selectionMode;
	bool _highlightCurrent;

};

} // namespace Sabrina

#endif // SABRINA_TEXTEDITWIDGET_H
