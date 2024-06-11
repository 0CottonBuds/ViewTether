#include "StreamCodec.h"
#include "qdebug.h"
StreamCodec::StreamCodec(int height, int width, int fps)
{
	this->height = height;
	this->width = width;
	this->fps = fps;
	initializeCodec();
}

void StreamCodec::initializeCodec()
{
	codec = avcodec_find_encoder(AV_CODEC_ID_HEVC);
	if (!codec) {
		qDebug() << "Codec not found";
		exit(1);
	}
	 
	context = avcodec_alloc_context3(codec);
	if (!context) {
		qDebug() << "Could not allocate codec context";
		exit(1);
	}

	context->bit_rate = 400000;
	context->width = 1920;
	context->height = 1080;
	context->time_base.num = 1;
	context->time_base.den = fps;
	context->framerate.num = fps;
	context->framerate.den = 1;
	context->pix_fmt = AV_PIX_FMT_YUV420P;

	context->max_b_frames = 3;
	context->refs = 3;

	av_opt_set(context->priv_data, "preset", "ultrafast", 0);
	av_opt_set(context->priv_data, "crf", "35", 0);
	av_opt_set(context->priv_data, "tune", "zerolatency", 0);

	auto desc = av_pix_fmt_desc_get(AV_PIX_FMT_BGRA);
	if (!desc){
		qDebug() << "Can't get descriptor for pixel format";
		exit(1);
	}
	bytesPerPixel = av_get_bits_per_pixel(desc) / 8;
	if(av_get_bits_per_pixel(desc) % 8 != 0){
		qDebug() << "Unhandled bits per pixel, bad in pix fmt";
		exit(1);
	}

	int err = avcodec_open2(context, codec, nullptr);
	if (err < 0) {
		qDebug() << "Could not open codec";
		exit(1);
	}
}

void StreamCodec::initializeSWS()
{
	swsContext = sws_getContext(width, height, AV_PIX_FMT_BGRA, width, height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
	if (!swsContext) {
		qDebug() << "Could not allocate SWS Context";
		exit(1);
	}
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

	av_frame_free(&frame);
	qDebug() << "Finished allocating frame";
}


