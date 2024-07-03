#include "App.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QImage>
#include <QObject>
#include <QLayout>
#include <QTimer>

App::App(int argc, char** argv)
{
	mainWidget = new Ui::MainWidget();
	QApplication app(argc, argv);
	QWidget* widget = new QWidget();
	mainWidget->setupUi(widget);

	// extra QT initialization
	QHBoxLayout* layout = new QHBoxLayout(mainWidget->previewContainer);

	displayStreamServerWorker = new DisplayStreamServer();
	mainWidget->ip_label->setText("IP: " + displayStreamServerWorker->getServerIp());
	mainWidget->port_label->setText("PORT: " + displayStreamServerWorker->getServerPort());

	previewTimer = new QTimer();
	previewTimer->setInterval(1000 / 60);
	previewTimer->stop();

	// preview video widget
	videoWidget = new VideoWidget(mainWidget->previewContainer);
	videoWidget->show();
	layout->addWidget(videoWidget);

	//Threads
	screenDuplicatorWorker->moveToThread(&screenDuplicatorThread);
	connect(&screenDuplicatorThread, &QThread::started, screenDuplicatorWorker, &ScreenDuplicator::Initialize);
	connect(&screenDuplicatorThread, &QThread::finished, screenDuplicatorWorker, &QObject::deleteLater);

	streamEncoder->moveToThread(&displayStreamServerThread);
	connect(&displayStreamServerThread, &QThread::started, streamEncoder, &StreamCodec::run);
	connect(&displayStreamServerThread, &QThread::finished, streamEncoder, &QObject::deleteLater);

	displayStreamServerWorker->moveToThread(&displayStreamServerThread);
	connect(&displayStreamServerThread, &QThread::started, displayStreamServerWorker, &DisplayStreamServer::run);
	connect(&displayStreamServerThread, &QThread::finished, displayStreamServerWorker, &QObject::deleteLater);

	displayStreamServerThread.start();
	screenDuplicatorThread.start();

	connect(previewTimer, &QTimer::timeout, screenDuplicatorWorker, &ScreenDuplicator::getFrame);

	connect(screenDuplicatorWorker, &ScreenDuplicator::frameReady, streamEncoder, &StreamCodec::encodeFrame);
	connect(streamEncoder, &StreamCodec::encodeFinish, displayStreamServerWorker, &DisplayStreamServer::sendDataToClient);

	connect(screenDuplicatorWorker, &ScreenDuplicator::imageReady, this, &App::updateFrame);

	connect(mainWidget->pushButton, SIGNAL(clicked()), this, SLOT(previewSwitch()));
	connect(mainWidget->comboBox, &QComboBox::currentIndexChanged, this, &App::setFps);

	connect(displayStreamServerWorker, &DisplayStreamServer::connected, this, [this] {mainWidget->connected_status_label->setText("Current Status: Connected"); });
	connect(displayStreamServerWorker, &DisplayStreamServer::disconnected, this, [this] {mainWidget->connected_status_label->setText("Current Status: Not Connected"); });

	widget->show();
	app.exec();
}

App::~App()
{
	screenDuplicatorThread.quit();
	screenDuplicatorThread.wait();
	displayStreamServerThread.quit();
	displayStreamServerThread.wait();
}

void App::setFps()
{
	int fps = mainWidget->comboBox->currentText().toInt();
	while (previewTimer->interval() != fps) {
		previewTimer->setInterval(fps);
	}
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

void App::updateFrame(shared_ptr<QImage> img)
{
	videoWidget->updateImage(img.get());
}
