#include "core/app.h"

int main(int argc, char** argv) {

	Cathia::App app(argc, argv);

	app.start();

	return app.exec();

}
