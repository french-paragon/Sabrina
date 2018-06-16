#include "core/app.h"

/*
This file is part of the project Sabrina
Copyright (C) 2018  Paragon <french.paragon@gmail.com>

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

#include <QCommandLineParser>

int main(int argc, char** argv) {

	Sabrina::App app(argc, argv);

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("project", QCoreApplication::translate("main", "project to open."));

	parser.process(app);

	const QStringList args = parser.positionalArguments();

	if(!app.start()) {
		return 1;
	}

	if (args.size() >= 1) {
		app.openProject(args.first());
	}

	return app.exec();

}
