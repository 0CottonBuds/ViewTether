#pragma once
#include "ScreenDupliactor.h"
#include "ui_MainWindow.h"
#include <QObject>
#include <iostream>

class App: QObject{
	Q_OBJECT;
public:
	App();
	App(const App&);
	~App();
	void exec_ui(int argc, char** argv);

public slots:
	void test();

private:
	ScreenDuplicator screenDuplicator;
	Ui::MainWidget* mainWidget;
};