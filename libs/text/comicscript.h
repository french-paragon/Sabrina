#ifndef SABRINA_COMICSCRPIT_H
#define SABRINA_COMICSCRPIT_H

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

#include "./textnode.h"
#include "./abstracttextstyle.h"
#include "./textstylemanager.h"

namespace Sabrina {

class ComicScriptStyle : public AbstractTextNodeStyle
{
	Q_OBJECT
public:

	enum NodeStyleTypes {
		MAIN = 0,
		PAGE = 2,
		PANEL = 3,
		CAPTION = 4,
		DIALOG = 5
	};

	ComicScriptStyle(QObject* parent = nullptr);

	QFont getFont(TextLine* line) const override;
};

class ComicScriptTitleStyle : public ComicScriptStyle
{
	Q_OBJECT
public:

	ComicScriptTitleStyle(QObject* parent = nullptr);

	int typeId() const override;
	QFont getFont(TextLine* line) const override;
	QMargins getNodeMargins(TextNode* node) const override;
};

class ComicScriptDescribedStyle : public ComicScriptStyle
{
	Q_OBJECT
public:

	ComicScriptDescribedStyle(QObject* parent = nullptr);

	QMargins getLineMargins(TextLine* line) const override;

	virtual QString getDescr(TextNode* node) const = 0;

	void renderNode(TextNode* node,
					const QPointF &offset,
					int availableWidth,
					QPainter & painter,
					int cursorLine = -1,
					int cursorPos = 0) override;
};

class ComicScriptPageStyle : public ComicScriptDescribedStyle
{
	Q_OBJECT
public:

	ComicScriptPageStyle(QObject* parent = nullptr);

	int typeId() const override;

	QMargins getNodeMargins(TextNode* node) const override;
	QString getDescr(TextNode* node) const override;
};

class ComicScriptPanelStyle : public ComicScriptDescribedStyle
{
	Q_OBJECT
public:

	ComicScriptPanelStyle(QObject* parent = nullptr);

	int typeId() const override;

	QMargins getNodeMargins(TextNode* node) const override;
	QString getDescr(TextNode* node) const override;
};


class ComicScriptCaptionStyle : public ComicScriptStyle
{
	Q_OBJECT
public:

	ComicScriptCaptionStyle(QObject* parent = nullptr);

	int typeId() const override;

	QMargins getNodeMargins(TextNode* node) const override;
};


class ComicScriptDialogStyle : public ComicScriptStyle
{
	Q_OBJECT
public:

	ComicScriptDialogStyle(QObject* parent = nullptr);

	int typeId() const override;

	int expectedNodeNbTextLines() const override;

	QString getPrefix(TextLine* line) const override;
	QString getSuffix(TextLine* line) const override;

	QFont getFont(TextLine* line) const override;
	int getTabulation(TextLine* line) const override;

	QMargins getNodeMargins(TextNode* node) const override;
	QMargins getLineMargins(TextLine* line) const override;
};

class ComicScriptTextStyleManager : public TextStyleManager
{
	Q_OBJECT
public:

	ComicScriptTextStyleManager(QObject* parent = nullptr);

	QMap<Qt::KeyboardModifiers, NextNodeStyleAndPos> getNextNodeStyleAndPos(int code) const override;
};

} // namespace Sabrina

#endif // SABRINA_COMICSCRPIT_H
