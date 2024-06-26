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

enum CodecType {
	encode,
	decode
};

// class for encoding and decoding data on screen capture app.
// use encode frame to encode pixel data
// use decode frame to decode avpacket
// stream codec has 2 types 0 for encoding 1 for decoding
class StreamCodec : public QObject 
{
	Q_OBJECT
public:
	StreamCodec(int height, int width, int fps, CodecType type);

public slots:
	void encodeFrame(std::shared_ptr<UCHAR> pData);
	void decodePacket(AVPacket* packet);
	void run();

signals:
	void encodeFinish(AVPacket* packet);
	void decodeFinish(AVFrame* frame);

private:
	void initializeEncoder();
	void initializeEncoderSWS();
	void initializeDecoder();
	void initializedecoderSWS();

	AVPacket* allocatepacket(AVFrame* frame);
	AVFrame* allocateFrame(std::shared_ptr<UCHAR> pData);
	AVFrame* formatFrame(AVFrame* frame);

	const AVCodec* encoder;
	AVCodecContext* encoderContext;
	SwsContext *encoderSwsContext;

	const AVCodec* decoder;
	AVCodecContext* decoderContext;
	SwsContext *decoderSwsContext;  
	
	int bytesPerPixel;
	int width;
	int height;
	int fps;
	CodecType type;
	int pts = 0;
};

