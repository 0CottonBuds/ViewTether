#include "App.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>

App::App()
{
	this->screenDuplicator.Initialize();
}

App::~App()
{
}

void App::exec_ui(int argc, char **argv)
{
	QApplication app(argc, argv);
	QWidget* widget = new QWidget();
	mainWidget.setupUi(widget);

	widget->show();
	app.exec();
}
