#include "App.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QImage>
#include <QObject>
#include <QLayout>

App::App(int argc, char **argv)
{
	screenDuplicator.Initialize();
	mainWidget = new Ui::MainWidget();
	QApplication app(argc, argv);
	QWidget* widget = new QWidget();
	mainWidget->setupUi(widget);
	QHBoxLayout* layout = new QHBoxLayout(mainWidget->previewContainer);
	QObject::connect(mainWidget->pushButton, SIGNAL(clicked()), this, SLOT(test()));

	widget->show();
	app.exec();

}

App::~App()
{
}

App::App(const App&)
{
}
	
void App::test() {
	UCHAR* pPixelData = nullptr;
	UINT pixelDataSize = 0;
	screenDuplicator.getNextFrame(&pPixelData, pixelDataSize);

	QImage pScreenShot =  QImage(pPixelData, 1920, 1080, QImage::Format_RGBA8888);
	pScreenShot = pScreenShot.rgbSwapped();
	QLabel* imageLabel = new QLabel("");
	QPixmap pix = QPixmap::fromImage(pScreenShot);
	imageLabel->setPixmap(pix.scaled(1920, 1080, Qt::KeepAspectRatio));
	imageLabel->setScaledContents(true);
	imageLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

	QLayout* layout = mainWidget->previewContainer->layout();
	delete layout->takeAt(0);
	layout->addWidget(imageLabel);

	imageLabel = nullptr;

	cout << "you pressed a button" << endl;
}
