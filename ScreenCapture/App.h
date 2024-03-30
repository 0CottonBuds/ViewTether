#pragma once
#include "ScreenDupliactor.h"
#include "ui_MainWindow.h"

class App {
public:
	App();
	~App();
	void exec_ui(int argc, char** argv);

private:
	ScreenDuplicator screenDuplicator;
	Ui::MainWidget mainWidget;
};