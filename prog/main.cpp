#include "core/app.h"

#include <QCommandLineParser>

int main(int argc, char** argv) {

	Sabrina::App app(argc, argv);

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("project", QCoreApplication::translate("main", "project to open."));

	parser.process(app);

	const QStringList args = parser.positionalArguments();

	app.start();

	if (args.size() >= 1) {
		app.openProject(args.first());
	}

	return app.exec();

}
