#pragma once
#include <iostream>
#include <QObject>
#include <QLabel>
#include <QThread>

#include "ui_MainWindow.h"
#include "VIdeoWidget.h"
#include "ScreenDupliactor.h"
#include "DisplayStreamServer.h"
#include "StreamCodec.h"
#include "DriverHelper.h"

class App : public QObject {
	Q_OBJECT;
public:
	App(int argc, char** argv);
	App(const App&);
	~App();
	void setFps();

public slots:
	// switches the screen on or off
	void streamSwitch();
	// updates video widget

private:
	QThread screenDuplicatorThread;
	ScreenDuplicator* screenDuplicatorWorker = new ScreenDuplicator();
	QThread displayStreamServerThread;
	DisplayStreamServer* displayStreamServerWorker = new DisplayStreamServer();
	StreamCodec* streamEncoder = new StreamCodec(1080, 1920, 60, CodecType::encode);
	DriverHelper* driverHelper = new DriverHelper();

	Ui_MainWidget* mainWidget;
	VideoWidget* videoWidget;
	QTimer* previewTimer;

	// sets what screen to be duplicated based on adapter and output combo boxes.
	void setScreen();

	void initializePreviewTimer(); // initializes a QTimer used for timing the fps of streaming. By default this is set to 60 fps
	void initializeVideoWidget();
	void initializeThreads();

	// Handles event loop of..
	// 1: getting the frame encoding and streaming.  
	// 2: getting the image and updating video widget
	void initializeMainEventLoop();

	void initializeButtons();
	void initializeConnectionInformation();
	void initializeComboBoxes();
	void populateAdapterComboBox();
	void populateOutputComboBox();
};
