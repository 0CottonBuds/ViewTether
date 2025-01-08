#pragma once
#include <QObject>
#include <iostream>

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
	#include <libavutil/hwcontext.h>
	#include <libavutil/error.h>
	#include <libavutil/opt.h>
	#include <libavutil/avconfig.h>
	#include <libswscale/swscale.h>
	#include <libavutil/imgutils.h>
}

class StreamEncoder : public QObject 
{
	Q_OBJECT
public:
	StreamEncoder(int height, int width, int fps, AVHWDeviceType hardwareAccelerationType = AV_HWDEVICE_TYPE_NONE);

public slots:
	void initialize();

	// encodes pixel data and emits encode finish when a packet is ready.
	// remember to free the frame on the reciever of packet 
	void encodeFrame(std::shared_ptr<uint8_t> pixelData);
	void encodeHWFrame(std::shared_ptr<uint8_t> pixelData);

signals:
	void encodeFinish(AVPacket* packet);
	void frameReady(std::shared_ptr<uint8_t> pData);

private:
	AVPacket* m_backPacket;

	const AVCodec* m_encoder;
	AVCodecContext* m_encoderContext;
	SwsContext* m_encoderSwsContext;

	const AVCodec* m_testDecoder;
	AVCodecContext* m_testDecoderContext;
	SwsContext *m_testDecoderSwsContext;

	int m_width;
	int m_height;
	int m_fps;
	int m_bitrate = 3000000;
	int m_pts = 0;

	// set to anything but none for hardware acceleration. As of now
	// only qsv is supported
	AVHWDeviceType m_hardwareAccelerationType = AV_HWDEVICE_TYPE_NONE;

private:
	void initializeEncoder();
	void initializeHWEncoder();
	void initializeTestDecoder();

	AVFrame* allocateFrame(std::shared_ptr<uint8_t> pixelData);
	AVFrame* convertYUVFrameToBGRA(AVFrame* yuvFrame);

	// emits frameReady signal with the frame data
	// for testing avpackets. the initializeTestDecoder
	// must be called before using tihs.
	void testPacket(AVPacket* packet);
	
};

