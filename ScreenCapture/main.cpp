#include "ScreenDupliactor.h"
#include <QApplication> 
#include <qwidget.h>
#include <ui_MainWindow.h>
#include <QtWidgets/QPushButton>

using namespace std;
using namespace Ui;

int main(int argc, char **argv)
{
	HRESULT hr;
	ScreenDuplicator screenDuplicator = ScreenDuplicator();
	screenDuplicator.Initialize();

	UCHAR* pixelData = nullptr;
	UINT pixelDataSize = 0;
	hr = screenDuplicator.getNextFrame(&pixelData, pixelDataSize);
	hr = screenDuplicator.processUCharFrame(&pixelData, pixelDataSize);

	QApplication app (argc, argv);
	QWidget window;
	
	Ui::MainWidget main = MainWidget();
	main.setupUi(&window);


	window.show();

	return app.exec();
}

