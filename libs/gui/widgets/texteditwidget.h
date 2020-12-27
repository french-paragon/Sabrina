#ifndef SABRINA_TEXTEDITWIDGET_H
#define SABRINA_TEXTEDITWIDGET_H

#include <QWidget>

#include "text/textnode.h"
#include "text/abstracttextstyle.h"
#include "text/textstylemanager.h"

namespace Sabrina {

class TextEditWidget : public QWidget
{
	Q_OBJECT
public:

	explicit TextEditWidget(QWidget *parent = nullptr);
	virtual ~TextEditWidget();

	void setStyleManager(TextStyleManager *styleManager);

	TextNode* getDocument() const;
	TextNode* getCurrentNode() const;
	void setCurrentScript(TextNode *root);
	void clearCurrentScript();

	bool hasScript() const;
	int currentLineStyleId() const;

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

	int nodeHeight(TextNode* n);
	AbstractTextNodeStyle* nodeStyle(TextNode* n);

	void scroll (int offset);
	void scrollToLine (int l);
	void scrollToLine (int l, int subL);
	Cursor::CursorState computeNewPosAfterJump(int nbPseudoLinesJump);

	void insertText(QString commited);
	void insertNextType(TextNode* n, Qt::KeyboardModifiers modifiers);

	Cursor* _cursor;

	TextStyleManager* _styleManager;
	TextNode* _currentScript;
	TextNode* _baseIndex;
	int _baseIndexLine;
	int _baseIndexHeightDelta;
	TextNode* _endIndex;
	int _endIndexMargin;

	QMargins _internalMargins;

};

} // namespace Sabrina

#endif // SABRINA_TEXTEDITWIDGET_H
