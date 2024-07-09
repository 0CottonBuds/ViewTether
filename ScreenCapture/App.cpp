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

	// preview timer setup
	previewTimer = new QTimer();
	previewTimer->setInterval(1000 / 60);
	previewTimer->stop();
	connect(previewTimer, &QTimer::timeout, screenDuplicatorWorker, &ScreenDuplicator::getFrame);
	// end preview timer setup

	// preview video widget initialization
	videoWidget = new VideoWidget(mainWidget->previewContainer);
	videoWidget->show();
	layout->addWidget(videoWidget);
	// END preview video widget initialization

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
	// END Threads

	// main event loop
	connect(screenDuplicatorWorker, &ScreenDuplicator::frameReady, streamEncoder, &StreamCodec::encodeFrame);
	connect(streamEncoder, &StreamCodec::encodeFinish, displayStreamServerWorker, &DisplayStreamServer::sendDataToClient);
	connect(screenDuplicatorWorker, &ScreenDuplicator::imageReady, this, &App::updateFrame);
	// end main event loop

	// preview switch 
	connect(mainWidget->startButton, &QPushButton::clicked, this, &App::previewSwitch);
	connect(mainWidget->frameRateComboBox, &QComboBox::currentIndexChanged, this, &App::setFps);
	// end preview switch

	// connection status connects
	connect(displayStreamServerWorker, &DisplayStreamServer::connected, this, [this] {mainWidget->connected_status_label->setText("Current Status: Connected"); });
	connect(displayStreamServerWorker, &DisplayStreamServer::disconnected, this, [this] {mainWidget->connected_status_label->setText("Current Status: Not Connected"); });
	// end connection status connects

	connect(screenDuplicatorWorker, &ScreenDuplicator::finishInitialization, this, &App::initializeAdapterComboBox);
	connect(screenDuplicatorWorker, &ScreenDuplicator::finishInitialization, this, &App::initializeOutputComboBox);
	connect(mainWidget->adapterComboBox, &QComboBox::currentIndexChanged, this, &App::initializeOutputComboBox);
	connect(mainWidget->adapterComboBox, &QComboBox::currentIndexChanged, this, &App::setScreen);
	connect(mainWidget->outputComboBox, &QComboBox::currentIndexChanged, this, &App::setScreen);

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
	int fps = mainWidget->frameRateComboBox->currentText().toInt();
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
			mainWidget->startButton->setText("Start Streaming");
		}
		mainWidget->adapterComboBox->setDisabled(false);
		mainWidget->outputComboBox->setDisabled(false);
		videoWidget->hide();
	}
	else {
		previewTimer->start();
		mainWidget->startButton->setText("Stop Streaming");
		videoWidget->show();
		mainWidget->adapterComboBox->setDisabled(true);
		mainWidget->outputComboBox->setDisabled(true);
	}
}

void App::updateFrame(shared_ptr<QImage> img)
{
	videoWidget->updateImage(img.get());
}

void App::initializeAdapterComboBox()
{
	vector<DXGI_ADAPTER_DESC1> adapters = screenDuplicatorWorker->getAdapters();
	vector<vector<IDXGIOutput1*>> outputs = screenDuplicatorWorker->getOutputs();
	for (int i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC1 currAdapter = adapters[i];
		wchar_t* desc = currAdapter.Description;
		wstring wstrDesc(desc);
		string strDesc(wstrDesc.begin(), wstrDesc.end());
		mainWidget->adapterComboBox->addItem(strDesc.c_str());
	}
	mainWidget->adapterComboBox->setCurrentIndex(0);
}

void App::initializeOutputComboBox()
{
	int adapterIndex = mainWidget->adapterComboBox->currentIndex();
	vector<vector<IDXGIOutput1*>> outputs = screenDuplicatorWorker->getOutputs();
	vector<IDXGIOutput1*> currOutputs = outputs[adapterIndex];

	mainWidget->outputComboBox->clear();

	for (int i = 0; i < currOutputs.size(); i++) {
		mainWidget->outputComboBox->addItem(to_string(i).c_str());
	}
	mainWidget->outputComboBox->setCurrentIndex(0);
}

void App::setScreen()
{
	int adapterIndex = mainWidget->adapterComboBox->currentIndex();
	int outputIndex = mainWidget->outputComboBox->currentIndex();
	if (adapterIndex < 0 || outputIndex < 0) {
		return;
	}

	screenDuplicatorWorker->initializeOutputDuplication(adapterIndex, outputIndex);
}

