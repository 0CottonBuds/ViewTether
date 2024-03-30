#include "App.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QObject>

App::App()
{
	screenDuplicator.Initialize();
	mainWidget = new Ui::MainWidget();
}

App::~App()
{
}

App::App(const App&)
{
}

void App::exec_ui(int argc, char **argv)
{
	QApplication app(argc, argv);
	QWidget* widget = new QWidget();
	mainWidget->setupUi(widget);
	QObject::connect(mainWidget->pushButton, SIGNAL(clicked()), this, SLOT(test()));

	widget->show();
	app.exec();
}

void App::test() {
	cout << "you pressed a button" << endl;
}
