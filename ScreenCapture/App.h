#pragma once
#include <iostream>
#include <QObject>
#include <QLabel>
#include <QThread>

#include "ui_MainWindow.h"
#include "VIdeoWidget.h"
#include "ScreenDupliactor.h"
#include "DisplayStreamServer.h"

class App : public QObject {
	Q_OBJECT;
public:
	App(int argc, char** argv);
	App(const App&);
	~App();

public slots:
	void previewSwitch();
	void test1(QImage* img);

private:
	QThread screenDuplicatorThread;
	QScreenDuplicatorWorker* screenDuplicatorWorker = new QScreenDuplicatorWorker();
	QThread displayStreamServerThread;
	DisplayStreamServer displayStreamServerWorker = DisplayStreamServer();

	Ui::MainWidget* mainWidget;
	QTimer* previewTimer;
	VideoWidget* videoWidget;
};
