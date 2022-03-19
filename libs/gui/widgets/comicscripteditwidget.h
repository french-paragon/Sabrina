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

#include "./texteditwidget.h"
#include <QModelIndex>

namespace Sabrina {

class Comicscript;

class ComicscriptEditWidget : public TextEditWidget
{
	Q_OBJECT
public:

	explicit ComicscriptEditWidget(QWidget *parent = nullptr);
	virtual ~ComicscriptEditWidget();

	void setCurrentScript(Comicscript* script);
	void setCurrentScript(TextNode *root);

	void addPage();
	void addPanel();
	void addCaption();
	void addDialog();

	void highlightCurrentNode(bool highlight);
	void selectFullBlocks(bool selectFullBlocks);

};

} // namespace Sabrina

#endif // SABRINA_COMICSCRIPTEDITWIDGET_H
