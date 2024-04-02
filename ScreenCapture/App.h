#pragma once
#include "ScreenDupliactor.h"
#include "ui_MainWindow.h"
#include <QObject>
#include <iostream>

class App: QObject{
	Q_OBJECT;
public:
	App(int argc, char **argv);
	App(const App&);
	~App();

public slots:
	void previewSwitch();
	void test();

private:
	QTimer* previewTimer;
	ScreenDuplicator screenDuplicator;
	Ui::MainWidget* mainWidget;
};