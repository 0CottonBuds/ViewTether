#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

class StreamCodec
{
public:
	StreamCodec(int height, int width, int fps);

private:
	bool setupffmpegContext();
	bool setupffmpegContextOptions(int height, int width, int fps);
	bool setupBytesPerPixel();

	const AVCodec* codec;
	AVCodecContext* context;
	int bytesPerPixel;
};

