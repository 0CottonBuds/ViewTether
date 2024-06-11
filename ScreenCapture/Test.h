#pragma once
#include <StreamCodec.h>
#include <ScreenDupliactor.h>
#include <qapplication.h>
#include <QObject>


class Test: public QObject{
	Q_OBJECT
private:
	ScreenDuplicator* screenDuplicator = new ScreenDuplicator();
	StreamCodec* streamCodec = new StreamCodec(1080, 1920, 60);

public:
	Test(int argc, char **argv) {
		QApplication app(argc, argv);
		connect(screenDuplicator, &ScreenDuplicator::frameReady, streamCodec, &StreamCodec::encodeFrame);
		screenDuplicator->getFrame();
		app.exec();
	}

};


