#pragma once
#include "ScreenDupliactor.h"
#include "ui_MainWindow.h"
#include <QObject>
#include <QLabel>
#include <QThread>
#include <iostream>
#include <QVideoWidget>
#include <QPainter>

class VideoWidget : public QWidget {
public:
    VideoWidget(QWidget *parent = nullptr) : QWidget(parent) {
    }

    void updateImage(const QImage *image) {
        currentImage = *image;
        update(); // Schedule a repaint
    }

protected:
    void paintEvent(QPaintEvent *event) override {
		QPainter painter = QPainter(this);
        painter.drawImage(rect(), currentImage, currentImage.rect());
    }

private:
    QImage currentImage;
};

class App: public QObject{
	Q_OBJECT;
public:
	App(int argc, char **argv);
	App(const App&);
	~App();

public slots:
	void previewSwitch();
	void test(UCHAR* pPixelData);
	void test1(QImage* img);

private:
	QThread screenDuplicatorThread;
	QScreenDuplicatorWorker* screenDuplicatorWorker = new QScreenDuplicatorWorker();
	QTimer* previewTimer;
	QLabel* frontFrame = nullptr;
	QLabel* backFrame = nullptr;
	Ui::MainWidget* mainWidget;

	//test
	VideoWidget *videoWidget;
};
