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
	displayStreamServerWorker = new DisplayStreamServer();
	previewTimer = new QTimer();
	previewTimer->setInterval(1000/60);
	previewTimer->stop();

	// preview video widget
	videoWidget = new VideoWidget(mainWidget->previewContainer);
	videoWidget->show();


	layout->addWidget(videoWidget);

	// Screen Duplicator Thread
	screenDuplicatorWorker->moveToThread(&screenDuplicatorThread);
	connect(&screenDuplicatorThread, &QThread::finished, screenDuplicatorWorker, &QObject::deleteLater);
	connect(previewTimer, &QTimer::timeout, screenDuplicatorWorker, &QScreenDuplicatorWorker::getFrame);
	connect(screenDuplicatorWorker, &QScreenDuplicatorWorker::imageReady, this, &App::test1);
	screenDuplicatorThread.start();

	connect(screenDuplicatorWorker, &QScreenDuplicatorWorker::frameReady, displayStreamServerWorker, &DisplayStreamServer::sendDataToClient);

	// other connects
	connect(mainWidget->pushButton, SIGNAL(clicked()), this, SLOT(previewSwitch()));

	connect(displayStreamServerWorker, &DisplayStreamServer::connected, this, [this] {mainWidget->connected_status_label->setText("Current Status: Connected"); });
	connect(displayStreamServerWorker, &DisplayStreamServer::disconnected, this, [this] {mainWidget->connected_status_label->setText("Current Status: Not Connected"); });

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
