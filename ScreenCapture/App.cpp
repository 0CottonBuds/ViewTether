#include "App.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QImage>
#include <QObject>
#include <QLayout>
#include <QTimer>

App::App(int argc, char **argv)
{
	screenDuplicator.Initialize();
	mainWidget = new Ui::MainWidget();
	QApplication app(argc, argv);
	QWidget* widget = new QWidget();
	mainWidget->setupUi(widget);

	// extra QT initialization
	QHBoxLayout* layout = new QHBoxLayout(mainWidget->previewContainer);
	previewTimer = new QTimer();
	previewTimer->setInterval(33);

	//connects
	QObject::connect(mainWidget->pushButton, SIGNAL(clicked()), this, SLOT(previewSwitch()));
	QObject::connect(previewTimer, SIGNAL(timeout()), this, SLOT(test()));

	previewTimer->stop();

	widget->show();
	app.exec();

}

App::~App()
{
}

App::App(const App&)
{
}

void App::previewSwitch() 
{
	if (previewTimer->isActive()) {
		previewTimer->stop();
		mainWidget->pushButton->setText("Start Preview");
	}
	else {
		previewTimer->start();
		mainWidget->pushButton->setText("Stop Preview");
	}
}
	
	
void App::test() {
	UCHAR* pPixelData = nullptr;
	UINT pixelDataSize = 0;
	screenDuplicator.getFrame(&pPixelData, pixelDataSize);

	QImage *pScreenShot =  new QImage(pPixelData, 1920, 1080, QImage::Format_RGBA8888);
	pScreenShot = new QImage(pScreenShot->rgbSwapped());

	QPixmap *pix = new QPixmap(QPixmap::fromImage(*pScreenShot));
	delete pScreenShot;
	delete backFrame;
	backFrame = new QLabel("");
	backFrame->setPixmap(pix->scaled(1920, 1080, Qt::KeepAspectRatio));
	delete pix;

	backFrame->setScaledContents(true);
	backFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

	QLayout* layout = mainWidget->previewContainer->layout();
	delete layout->takeAt(0);
	layout->addWidget(backFrame);

	layout = nullptr;
	delete pPixelData;
}
