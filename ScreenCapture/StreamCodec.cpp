#include "StreamCodec.h"
#include "qdebug.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

StreamCodec::StreamCodec(int height, int width, int fps)
{
	this->height = height;
	this->width = width;
	this->fps = fps;

}

void StreamCodec::encodeFrame(std::shared_ptr<UCHAR> pData)
{
	AVFrame* frame = av_frame_alloc();
	if (!frame) {
		qDebug() << "Could not allocate memory for frame";
		exit(1);
	}

	frame->format = AV_PIX_FMT_BGRA;
	frame->width = width;
	frame->height = height;

	if (av_frame_get_buffer(frame, 0) < 0) {
		qDebug() << "Failed to get frame buffer";
		exit(1);
	}

	if (av_frame_make_writable(frame) < 0) {
		qDebug() << "Failed to make frame writable";
		exit(1);
	}

	frame->data[0] = pData.get();
	frame->linesize[0] = width * 4;


	qDebug() << frame->data[0];

	av_frame_free(&frame);
	qDebug() << "Finished allocating frame";
}


