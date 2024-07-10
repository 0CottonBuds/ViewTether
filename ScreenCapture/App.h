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

class App : public QObject {
	Q_OBJECT;
public:
	App(int argc, char** argv);
	App(const App&);
	~App();
	void setFps();

public slots:
	void previewSwitch();
	void updateFrame(shared_ptr<QImage> img);

private:
	QThread screenDuplicatorThread;
	ScreenDuplicator* screenDuplicatorWorker = new ScreenDuplicator();
	QThread displayStreamServerThread;
	DisplayStreamServer* displayStreamServerWorker = new DisplayStreamServer();

	StreamCodec* streamEncoder = new StreamCodec(1080, 1920, 60, CodecType::encode);

	Ui::MainWidget* mainWidget;
	QTimer* previewTimer;
	VideoWidget* videoWidget;

	void initializeAdapterComboBox();
	void initializeOutputComboBox();
	void setScreen();
};
