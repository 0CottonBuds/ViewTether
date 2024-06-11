#pragma once
#include <QObject>
#include "Windows.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#pragma comment(lib, "strmiids.lib")

class StreamCodec : public QObject 
{
	Q_OBJECT
public:
	StreamCodec(int height, int width, int fps);
	void initializeCodec();

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

