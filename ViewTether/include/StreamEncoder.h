#pragma once
#include <QObject>
#include "Windows.h"
#include <QImage>

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

#include <iostream>
#include <d3d11.h>
#include <d3d11_4.h>
#include <wrl/client.h>

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

// StreamEncoder is a class that encodes pixel data to AVPacket
class StreamEncoder : public QObject 
{
	Q_OBJECT
public:
	StreamEncoder(int height, int width, int fps, AVHWDeviceType hardwareAccelerationType = AV_HWDEVICE_TYPE_NONE);

public slots:
	void initialize();

	// encodes pixel data and emits encode finish when a packet is ready.
	// remember to free the frame on the reciever of packet 
	void encodeFrame(std::shared_ptr<UCHAR> pData);
	void encodeHWFrame(ComPtr<ID3D11Texture2D> desktopTexture);

signals:
	void encodeFinish(AVPacket* packet);
	void frameReady(std::shared_ptr<UCHAR> pData);

private:
	AVPacket* backPacket;

	const AVCodec* encoder;
	AVCodecContext* encoderContext;
	SwsContext *encoderSwsContext;

	const AVCodec* testDecoder;
	AVCodecContext* testDecoderContext;
	SwsContext *testDecoderSwsContext;


	int width;
	int height;
	int fps;
	int bitrate = 3000000;
	int pts = 0;

	// set to anything but none for hardware acceleration. As of now
	// only qsv is supported
	AVHWDeviceType hardwareAccelerationType = AV_HWDEVICE_TYPE_NONE;

	// These d3d11 variables are used for hardware acceleration
	// if hardware acceleration is not used all of this should 
	// be a null pointer

	ComPtr<ID3D11Device> d3d11Device;
	ComPtr<ID3D11DeviceContext> d3d11DeviceContext;

	// used for converting BGRA Frames to NV12

	ComPtr<ID3D11VideoDevice> d3d11VideoDevice;
	D3D11_VIDEO_PROCESSOR_CONTENT_DESC d3d11ContentDesc = {};
	ComPtr<ID3D11VideoProcessorEnumerator> d3d11VideoProcessorEnumerator;
	ComPtr<ID3D11VideoProcessor> d3d11VideoProcessor;
	ComPtr<ID3D11VideoContext> d3d11VideoContext;

	ComPtr<ID3D11Texture2D> d3d11InputTexture;
	ComPtr<ID3D11Texture2D> d3d11OutputTexture;
	ComPtr<ID3D11VideoProcessorInputView> d3d11InputView;
	ComPtr<ID3D11VideoProcessorOutputView> d3d11OutputView;

private:
	void initializeEncoder();
	void initializeHWEncoder();

	// initializes d3d11 device and context for hardware acceleration
	void initializeD3D11();

	// initializes the decoder used for testing the AVPackets
	void initializeTestDecoder();

	void convertBGRAtoNV12(ComPtr<ID3D11Texture2D> desktopTexture);
	AVFrame* convertFrameToBGRA(AVFrame* yuvFrame);

	// test if the d3d11 texture is correct uses the signal
	// frameReady to emit the frame data
	void testD3D11Texture(ComPtr<ID3D11Texture2D> Texture);

	// same as testD3D11Texture but for AVPackets this emulates
	// the configuration of the client application's decoder 
	void testPacket(AVPacket* packet);
	
	AVFrame* allocateFrame(std::shared_ptr<UCHAR> pData);

};

