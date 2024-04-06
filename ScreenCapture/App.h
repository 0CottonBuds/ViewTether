#pragma once
#include "ScreenDupliactor.h"
#include "ui_MainWindow.h"
#include <QObject>
#include <QLabel>
#include <QThread>
#include <iostream>

class App: public QObject{
	Q_OBJECT;
public:
	App(int argc, char **argv);
	App(const App&);
	~App();

public slots:
	void previewSwitch();
	void test(UCHAR* pPixelData);

private:
	QThread screenDuplicatorThread;
	QScreenDuplicatorWorker* screenDuplicatorWorker = new QScreenDuplicatorWorker();
	QTimer* previewTimer;
	QLabel* frontFrame = nullptr;
	QLabel* backFrame = nullptr;
	Ui::MainWidget* mainWidget;
};