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

public slots:
	void encodeFrame(std::shared_ptr<UCHAR> pData);

signals:
	void encodeFinish(AVPacket* packet);

private:
	void initializeSWS();
	void initializeCodec();

	AVPacket* allocatepacket(AVFrame* frame);
	AVFrame* allocateFrame(std::shared_ptr<UCHAR> pData);
	AVFrame* formatFrame(AVFrame* frame);

	const AVCodec* codec;
	AVCodecContext* context;
	SwsContext *swsContext;
	int bytesPerPixel;
	int width;
	int height;
	int fps;
	int pts = 0;
};

