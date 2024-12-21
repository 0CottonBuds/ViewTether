#include "App.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QImage>
#include <QObject>
#include <QLayout>
#include <QTimer>

#include "Helpers/MiscHelpers.h"
#include "ScreenCapture/ScreenCapture.h"

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
	screenCaptureThread.quit();
	screenCaptureThread.wait();
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
		screenCaptureWorker->setActive(false);
		previewTimer->stop();
		mainWidget->startButton->setText("Start Streaming");
		videoWidget->hide();


		mainWidget->adapterComboBox->setDisabled(false);
		mainWidget->outputComboBox->setDisabled(false);
	}
	else {
		screenCaptureWorker->setActive(true);
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

	screenCaptureWorker->changeDisplay(adapterIndex, outputIndex);
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
	screenCaptureWorker->moveToThread(&screenCaptureThread);
	connect(&screenCaptureThread, &QThread::started, screenCaptureWorker, &ScreenCapture::Initialize);
	connect(&screenCaptureThread, &QThread::finished, screenCaptureWorker, &QObject::deleteLater);

	streamEncoder->moveToThread(&displayStreamServerThread);
	connect(&displayStreamServerThread, &QThread::started, streamEncoder, &StreamEncoder::initialize);
	connect(&displayStreamServerThread, &QThread::finished, streamEncoder, &QObject::deleteLater);

	displayStreamServerWorker->moveToThread(&displayStreamServerThread);
	connect(&displayStreamServerThread, &QThread::started, displayStreamServerWorker, &DisplayStreamServer::initialize);
	connect(&displayStreamServerThread, &QThread::finished, displayStreamServerWorker, &QObject::deleteLater);

	displayStreamServerThread.start();
	screenCaptureThread.start();
}

void App::onFrameReady(shared_ptr<UCHAR> pixelData)
{
	QImage* notSwappedImage = new QImage(pixelData.get(), 1920, 1080, QImage::Format_RGBA8888);
	shared_ptr<QImage> image = shared_ptr<QImage>(new QImage(notSwappedImage->rgbSwapped()));
	delete notSwappedImage;
	videoWidget->updateImage(image);
}

void App::initializeMainEventLoop()
{
	connect(previewTimer, &QTimer::timeout, screenCaptureWorker, &ScreenCapture::getFrame);

	// choose if you want to use software or hardware encoding
	//connect(screenCaptureWorker, &ScreenCapture::frameReady, streamEncoder, &StreamEncoder::encodeFrame);
	connect(screenCaptureWorker, &ScreenCapture::hwframeReady, streamEncoder, &StreamEncoder::encodeHWFrame);

	// choose the preview source. used for testing if the pixel data is correct
	//connect(screenCaptureWorker, &ScreenCapture::frameReady, this, &App::onFrameReady);
	connect(streamEncoder, &StreamEncoder::frameReady, this, &App::onFrameReady);

	connect(streamEncoder, &StreamEncoder::encodeFinish, displayStreamServerWorker, &DisplayStreamServer::write);

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
	connect(driverHelper, &VirtualScreenDriverHelper::virtualScreenModified, screenCaptureWorker, &ScreenCapture::Initialize);
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
	connect(screenCaptureWorker, &ScreenCapture::initializationFinished, this, &App::initializeAdapterComboBox);
	connect(screenCaptureWorker, &ScreenCapture::initializationFinished, this, &App::initializeOutputComboBox);
	connect(mainWidget->adapterComboBox, &QComboBox::currentIndexChanged, this, &App::initializeOutputComboBox);
	connect(mainWidget->adapterComboBox, &QComboBox::currentIndexChanged, this, &App::setScreen);
	connect(mainWidget->outputComboBox, &QComboBox::currentIndexChanged, this, &App::setScreen);
}

void App::initializeAdapterComboBox()
{
	DisplayInformationManager displayInformationManager = screenCaptureWorker->getDisplayInformationManager();
	vector<DisplayProvider> displayProviders = displayInformationManager.getDisplayProviders();
	for (int i = 0; i < displayProviders.size(); i++) {
		DisplayProvider currProvider = displayProviders[i];
		mainWidget->adapterComboBox->addItem(currProvider.name.c_str());
	}
	mainWidget->adapterComboBox->setCurrentIndex(0);
}

void App::initializeOutputComboBox()
{
	mainWidget->outputComboBox->setCurrentIndex(0);
	int adapterIndex = mainWidget->adapterComboBox->currentIndex();
	DisplayInformationManager displayInformationManager = screenCaptureWorker->getDisplayInformationManager();
	DisplayProvider selectedDisplayProvider = displayInformationManager.getDisplayProvider(adapterIndex);

	mainWidget->outputComboBox->clear();

	for (int i = 0; i < selectedDisplayProvider.displayInformations.size(); i++) {
		mainWidget->outputComboBox->addItem(to_string(i).c_str());
	}
	mainWidget->outputComboBox->setCurrentIndex(0);
}


