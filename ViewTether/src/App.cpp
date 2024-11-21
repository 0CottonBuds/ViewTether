#include "App.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QImage>
#include <QObject>
#include <QLayout>
#include <QTimer>

#include "Helpers/miscHelpers.h"

App::App(int argc, char** argv)
{
	mainWidget = new Ui_MainWidget();
	QApplication app(argc, argv);
	QWidget* widget = new QWidget();
	mainWidget->setupUi(widget);

	initializePreviewTimer();
	initializeVideoWidget();
	initializeThreads();
	initializeMainEventLoop();
	initializeButtons();
	initializeConnectionInformation();
	initializeComboBoxes();

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

App::App(const App&)
{
}

void App::setFps()
{
	int fps = mainWidget->frameRateComboBox->currentText().toInt();
	while (previewTimer->interval() != fps) {
		previewTimer->setInterval(fps);
	}
}

void App::streamSwitch() 
{
	if (previewTimer->isActive()) {
		screenDuplicatorWorker->isActive = false;
		previewTimer->stop();
		mainWidget->startButton->setText("Start Streaming");
		videoWidget->hide();

		mainWidget->adapterComboBox->setDisabled(false);
		mainWidget->outputComboBox->setDisabled(false);
	}
	else {
		screenDuplicatorWorker->isActive = true;
		previewTimer->start();
		mainWidget->startButton->setText("Stop Streaming");
		videoWidget->show();

		mainWidget->adapterComboBox->setDisabled(true);
		mainWidget->outputComboBox->setDisabled(true);
	}
}

void App::setScreen()
{
	int adapterIndex = mainWidget->adapterComboBox->currentIndex();
	int outputIndex = mainWidget->outputComboBox->currentIndex();
	if (adapterIndex < 0 || outputIndex < 0) {
		return;
	}

	screenDuplicatorWorker->changeDisplay(adapterIndex, outputIndex);
}

void App::initializePreviewTimer()
{
	previewTimer = new QTimer();
	previewTimer->setInterval(1000 / 60);
	previewTimer->stop();
}

void App::initializeVideoWidget()
{
	videoWidget = new VideoWidget(mainWidget->previewContainer);
	videoWidget->show();
	QHBoxLayout* layout = new QHBoxLayout(mainWidget->previewContainer);
	layout->addWidget(videoWidget);
}

void App::initializeThreads()
{
	screenDuplicatorWorker->moveToThread(&screenDuplicatorThread);
	connect(&screenDuplicatorThread, &QThread::started, screenDuplicatorWorker, &DXGIScreenDuplicator::Initialize);
	connect(&screenDuplicatorThread, &QThread::finished, screenDuplicatorWorker, &QObject::deleteLater);

	streamEncoder->moveToThread(&displayStreamServerThread);
	connect(&displayStreamServerThread, &QThread::started, streamEncoder, &StreamCodec::run);
	connect(&displayStreamServerThread, &QThread::finished, streamEncoder, &QObject::deleteLater);

	displayStreamServerWorker->moveToThread(&displayStreamServerThread);
	connect(&displayStreamServerThread, &QThread::started, displayStreamServerWorker, &DisplayStreamServer::run);
	connect(&displayStreamServerThread, &QThread::finished, displayStreamServerWorker, &QObject::deleteLater);

	displayStreamServerThread.start();
	screenDuplicatorThread.start();
}

void App::initializeMainEventLoop()
{
	connect(previewTimer, &QTimer::timeout, screenDuplicatorWorker, &DXGIScreenDuplicator::getFrame);
	connect(screenDuplicatorWorker, &DXGIScreenDuplicator::frameReady, streamEncoder, &StreamCodec::encodeFrame);
	connect(screenDuplicatorWorker, &DXGIScreenDuplicator::imageReady, videoWidget, &VideoWidget::updateImage);
	connect(streamEncoder, &StreamCodec::encodeFinish, displayStreamServerWorker, &DisplayStreamServer::sendDataToClient);
}

void App::initializeButtons()
{
	connect(mainWidget->startButton, &QPushButton::clicked, this, &App::streamSwitch);

	connect(mainWidget->settingsButton, &QPushButton::clicked, this, [this] {mainWidget->appRouter->setCurrentWidget(mainWidget->settingsPage); });
	connect(mainWidget->backToStreaming, &QPushButton::clicked, this, [this] {mainWidget->appRouter->setCurrentWidget(mainWidget->streamingPage); });

	connect(mainWidget->aboutSettingsButton, &QPushButton::clicked, this, [this] {mainWidget->settingsRouter->setCurrentWidget(mainWidget->aboutSettings); });
	connect(mainWidget->virtualMonitorSettingsButton, &QPushButton::clicked, this, [this] {mainWidget->settingsRouter->setCurrentWidget(mainWidget->virtualMonitorSettings); });

	connect(mainWidget->githubRepoButton, &QPushButton::clicked, this, [] {openLink(L"https://github.com/0CottonBuds/Screen-Capture"); });
	connect(mainWidget->clientGithubRepoButton, &QPushButton::clicked, this, [] {openLink(L"https://github.com/0CottonBuds/Screen-Capture-Android-Client"); });
	connect(mainWidget->linkedinButton, &QPushButton::clicked, this, [] {openLink(L"https://www.linkedin.com/in/emil-john-llanes-187214231/"); });
	connect(mainWidget->githubButton, &QPushButton::clicked, this, [] {openLink(L"https://github.com/0CottonBuds"); });
	connect(mainWidget->facebookButton, &QPushButton::clicked, this, [] {openLink(L"https://www.facebook.com/0CottonBuds"); });

	// driver buttons
	connect(mainWidget->amyuniInstallButton, &QPushButton::clicked, this, [this] {driverHelper->installAmyuni(); });
	connect(mainWidget->amyuniUninstallButton, &QPushButton::clicked, this, [this] {driverHelper->uninstallAmyuni(); });
	connect(mainWidget->amyuniAddMonitorButton, &QPushButton::clicked, this, [this] {driverHelper->addVirtualScreen(); });
	connect(mainWidget->AmyuniRemoveMonitorButton, &QPushButton::clicked, this, [this] {driverHelper->removeVirtualScreen(); });
	connect(driverHelper, &VirtualScreenDriverHelper::virtualScreenModified, screenDuplicatorWorker, &DXGIScreenDuplicator::Initialize);
}

void App::initializeConnectionInformation()
{
	connect(displayStreamServerWorker, &DisplayStreamServer::initializationFinished, this, [this] {mainWidget->ip_label->setText("IP: " + displayStreamServerWorker->getServerIp()); });
	connect(displayStreamServerWorker, &DisplayStreamServer::initializationFinished, this, [this] {mainWidget->port_label->setText("PORT: " + displayStreamServerWorker->getServerPort()); });
	connect(displayStreamServerWorker, &DisplayStreamServer::connected, this, [this] {mainWidget->connected_status_label->setText("Current Status: Connected"); });
	connect(displayStreamServerWorker, &DisplayStreamServer::disconnected, this, [this] {mainWidget->connected_status_label->setText("Current Status: Not Connected"); });
}

void App::initializeComboBoxes()
{
	connect(mainWidget->frameRateComboBox, &QComboBox::currentIndexChanged, this, &App::setFps);
	connect(screenDuplicatorWorker, &DXGIScreenDuplicator::initializationFinished, this, &App::initializeAdapterComboBox);
	connect(screenDuplicatorWorker, &DXGIScreenDuplicator::initializationFinished, this, &App::initializeOutputComboBox);
	connect(mainWidget->adapterComboBox, &QComboBox::currentIndexChanged, this, &App::initializeOutputComboBox);
	connect(mainWidget->adapterComboBox, &QComboBox::currentIndexChanged, this, &App::setScreen);
	connect(mainWidget->outputComboBox, &QComboBox::currentIndexChanged, this, &App::setScreen);
}

void App::initializeAdapterComboBox()
{
	vector<DXGI_ADAPTER_DESC1> adapters = screenDuplicatorWorker->getAdapters();
	vector<vector<IDXGIOutput1*>> outputs = screenDuplicatorWorker->getDisplays();
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
	vector<vector<IDXGIOutput1*>> outputs = screenDuplicatorWorker->getDisplays();
	vector<IDXGIOutput1*> currOutputs = outputs[adapterIndex];

	mainWidget->outputComboBox->clear();

	for (int i = 0; i < currOutputs.size(); i++) {
		mainWidget->outputComboBox->addItem(to_string(i).c_str());
	}
	mainWidget->outputComboBox->setCurrentIndex(0);
}


