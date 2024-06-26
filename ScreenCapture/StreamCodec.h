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
// stream codec has 2 types. Use CodecType enum for types
class StreamCodec : public QObject 
{
	Q_OBJECT
public:
	StreamCodec(int height, int width, int fps, CodecType type);

public slots:
	
	// encodes pixel data and emits encode finish when a packet is ready.
	// remember to free the frame on the reciever of packet 
	void encodeFrame(std::shared_ptr<UCHAR> pData);

	// decodes avpacket and emits decode finish when a frame is ready.
	// remember to free the frame on the reciever of frame
	void decodePacket(AVPacket* packet);

	// initializes the stream codec and SWS
	void run();

signals:
	void encodeFinish(AVPacket* packet);
	void decodeFinish(AVFrame* frame);

private:
	//shared

	int bytesPerPixel;
	int width;
	int height;
	int fps;
	CodecType type;
	int pts = 0;

	//encoder
	const AVCodec* encoder;
	AVCodecContext* encoderContext;
	SwsContext *encoderSwsContext;

	void initializeEncoder();
	void initializeEncoderSWS();
	AVPacket* allocatepacket(AVFrame* frame);
	AVFrame* allocateFrame(std::shared_ptr<UCHAR> pData);
	AVFrame* convertFrameFromRGBAtoYUV(AVFrame* frame);

	//decoder
	const AVCodec* decoder;
	AVCodecContext* decoderContext;
	SwsContext *decoderSwsContext;  
	
	void initializeDecoder();
	void initializedecoderSWS();
	AVFrame* convertFrameFromYUVtoBGRA(AVFrame* frame);
};

