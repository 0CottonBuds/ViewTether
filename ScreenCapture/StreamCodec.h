#pragma once
#include <QObject>
#include "Windows.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

class StreamCodec : public QObject 
{
	Q_OBJECT
public:
	StreamCodec(int height, int width, int fps);

public slots:
	void encodeFrame(std::shared_ptr<UCHAR> pData);

private:
	const AVCodec* codec;
	AVCodecContext* context;
	int bytesPerPixel;
	int width;
	int height;
	int fps;
};

