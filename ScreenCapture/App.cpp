#include "App.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QImage>
#include <QObject>
#include <QLayout>
#include <QTimer>

App::App(int argc, char **argv)
{
	mainWidget = new Ui::MainWidget();
	QApplication app(argc, argv);
	QWidget* widget = new QWidget();
	mainWidget->setupUi(widget);

	// extra QT initialization
	QHBoxLayout* layout = new QHBoxLayout(mainWidget->previewContainer);
	previewTimer = new QTimer();
	previewTimer->setInterval(1000/60);
	previewTimer->stop();

	// testing
	videoWidget = new VideoWidget(mainWidget->previewContainer);
	videoWidget->show();

	layout->addWidget(videoWidget);

	//Thread
	screenDuplicatorWorker->moveToThread(&screenDuplicatorThread);
	QObject::connect(&screenDuplicatorThread, &QThread::finished, screenDuplicatorWorker, &QObject::deleteLater);
	QObject::connect(previewTimer, &QTimer::timeout, screenDuplicatorWorker, &QScreenDuplicatorWorker::getFrame);
	QObject::connect(screenDuplicatorWorker, &QScreenDuplicatorWorker::imageReady, this, &App::test1);
	QObject::connect(mainWidget->pushButton, SIGNAL(clicked()), this, SLOT(previewSwitch()));
	screenDuplicatorThread.start();

	widget->show();
	app.exec();

}

App::~App()
{
	screenDuplicatorThread.quit();
	screenDuplicatorThread.wait();
}

App::App(const App&)
{
}

void App::previewSwitch() 
{
	if (previewTimer->isActive()) {
		while (previewTimer->isActive()) {
			previewTimer->stop();
			mainWidget->pushButton->setText("Start Preview");
		}
		videoWidget->hide();
	}
	else {
		previewTimer->start();
		mainWidget->pushButton->setText("Stop Preview");
		videoWidget->show();
	}
}

void App::test1(QImage *img)
{
	videoWidget->updateImage(img);
	delete img;
}
