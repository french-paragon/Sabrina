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

#include "text/comicscript.h"

#include "gui/widgets/texteditwidget.h"

#include <QApplication>

using namespace Sabrina;

int main(int argc, char ** argv) {

	QApplication app(argc, argv);

	TextEditWidget w;

	ComicScriptTextStyleManager st;
	w.setStyleManager(&st);

	TextNode n;
	n.setStyleId(ComicScriptStyle::MAIN);
	n.lineAt(0)->setText("Test");

	w.setCurrentScript(&n);

	w.show();

	return app.exec();

}
