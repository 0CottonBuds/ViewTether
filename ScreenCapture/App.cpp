#include "App.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QImage>
#include <QObject>
#include <QLayout>

App::App()
{
	screenDuplicator.Initialize();
	mainWidget = new Ui::MainWidget();
}

App::~App()
{
}

App::App(const App&)
{
}

void App::exec_ui(int argc, char **argv)
{
	QApplication app(argc, argv);
	QWidget* widget = new QWidget();
	mainWidget->setupUi(widget);
	QObject::connect(mainWidget->pushButton, SIGNAL(clicked()), this, SLOT(test()));

	widget->show();
	app.exec();
}

void App::test() {
	UCHAR* pPixelData = nullptr;
	UINT pixelDataSize = 0;
	screenDuplicator.getNextFrame(&pPixelData, pixelDataSize);

	QImage pScreenShot =  QImage(pPixelData, 1920, 1080, QImage::Format_RGBA8888);
	pScreenShot = pScreenShot.rgbSwapped();
	QLabel* imageLabel = new QLabel("");
	imageLabel->setPixmap(QPixmap::fromImage(pScreenShot));

	QHBoxLayout* layout = new QHBoxLayout(mainWidget->previewContainer);
	layout->addWidget(imageLabel);

	cout << "you pressed a button" << endl;
}
