#pragma once

#include "UIManager.h"
#include "VIdeoWidget.h"
#include "StreamEncoder.h"
#include "ScreenCapture/ScreenCapture.h"
#include "StreamServer/TCPStreamServer.h"
#include "ScreenCapture/DXGIScreenCapture.h"
#include "Helpers/VirtualScreenDriverHelper.h"

#include <QtWidgets/QApplication>
#include <QGuiApplication>
#include <QThread>



class App : public QObject {
	Q_OBJECT;
public:
	App(int argc, char** argv);
	App(const App&);
	~App();

private:
	// TODO: currently this is taken by looking at primary screen, need to change this to use DisplayInformationManager
	int m_primaryScreenHeight;
	int m_primaryScreenWidth;

	UIManager* m_uiManager;

	QThread m_screenCaptureThread;
	DXGIScreenCapture* m_screenCapture;


	// TODO: this share threads figure out if we should create a thread for streamEncoder
	QThread m_displayStreamServerThread;
	DisplayStreamServer* m_displayStreamServer;
	StreamEncoder* streamEncoder;

	VirtualScreenDriverHelper* driverHelper;

	void initializeThreads();
	void initializeMainEventLoop();

};
