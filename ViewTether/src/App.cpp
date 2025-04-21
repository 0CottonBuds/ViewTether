#include "App.h"

void GetScreenResolution(int& height, int& width)
{
	SetProcessDPIAware();

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	width = desktop.right;
	height = desktop.bottom;
}

App::App(int argc, char** argv)
{
	QApplication app(argc, argv);
	GetScreenResolution(m_primaryScreenHeight, m_primaryScreenWidth);

	m_screenCapture = new DXGIScreenCapture();	
	m_displayStreamServer = new DisplayStreamServer();
	driverHelper = new VirtualScreenDriverHelper();
	streamEncoder = new StreamEncoder(m_primaryScreenHeight, m_primaryScreenWidth, 16, AV_HWDEVICE_TYPE_QSV);
	m_uiManager = new UIManager();

	initializeThreads();
	initializeMainEventLoop();

	app.exec();
}

App::~App()
{
	m_screenCaptureThread.quit();
	m_screenCaptureThread.wait();
	m_displayStreamServerThread.quit();
	m_displayStreamServerThread.wait();
}

App::App(const App&)
{
	delete m_uiManager;
	delete m_screenCapture;
	delete m_displayStreamServer;
	delete streamEncoder;
}



void App::initializeThreads()
{
	m_screenCapture->moveToThread(&m_screenCaptureThread);
	connect(&m_screenCaptureThread, &QThread::started, m_screenCapture, &ScreenCapture::Initialize);
	connect(&m_screenCaptureThread, &QThread::finished, m_screenCapture, &QObject::deleteLater);

	streamEncoder->moveToThread(&m_displayStreamServerThread);
	connect(&m_displayStreamServerThread, &QThread::started, streamEncoder, &StreamEncoder::initialize);
	connect(&m_displayStreamServerThread, &QThread::finished, streamEncoder, &QObject::deleteLater);

	m_displayStreamServer->moveToThread(&m_displayStreamServerThread);
	connect(&m_displayStreamServerThread, &QThread::started, m_displayStreamServer, &DisplayStreamServer::initialize);
	connect(&m_displayStreamServerThread, &QThread::finished, m_displayStreamServer, &QObject::deleteLater);

	m_screenCaptureThread.start();
	m_displayStreamServerThread.start();
}

void App::initializeMainEventLoop(){
	// choose if you want to use software or hardware encoding
	//connect(screenCaptureWorker, &ScreenCapture::frameReady, streamEncoder, &StreamEncoder::encodeFrame);
	connect(m_screenCapture, &ScreenCapture::frameReady, streamEncoder, &StreamEncoder::encodeHWFrame);
	//connect(screenCaptureWorker, &ScreenCapture::hwframeReady, streamEncoder, &StreamEncoder::encodeHWFrame);

	// choose the preview source. used for testing if the pixel data is correct
	//connect(streamEncoder, &StreamEncoder::frameReady, m_uiManager, &UIManager::setVideoFrame);
	connect(m_screenCapture, &ScreenCapture::frameReady, m_uiManager, &UIManager::setVideoFrame);

	connect(streamEncoder, &StreamEncoder::encodeFinish, m_displayStreamServer, &DisplayStreamServer::write);
	connect(m_screenCapture, &ScreenCapture::displayInformationReady, m_uiManager, &UIManager::setDisplayInformation);

	connect(m_displayStreamServer, &DisplayStreamServer::addressReady, m_uiManager, &UIManager::setAddress);
	connect(m_displayStreamServer, &DisplayStreamServer::connected, this, [this] {m_uiManager->setConnectionStatus(true);});
	connect(m_displayStreamServer, &DisplayStreamServer::disconnected, this, [this] {m_uiManager->setConnectionStatus(false);});

	connect(m_uiManager, &UIManager::activeStatusChanged, m_screenCapture, &ScreenCapture::setActive);
	connect(driverHelper, &VirtualScreenDriverHelper::virtualScreenModified, m_uiManager, &UIManager::initialize);

	connect(m_uiManager, &UIManager::screenChanged, m_screenCapture, &ScreenCapture::changeScreen);
	connect(m_uiManager, &UIManager::driverInstallClicked, this, [this] {driverHelper->installAmyuni(); });
	connect(m_uiManager, &UIManager::driverUninstallClicked, this, [this] {driverHelper->uninstallAmyuni(); });
	connect(m_uiManager, &UIManager::addVirtualMonitorClicked,this, [this] {driverHelper->addVirtualScreen(); });
	connect(m_uiManager, &UIManager::removeVirtualMonitorClicked, this, [this] {driverHelper->removeVirtualScreen(); });
}
