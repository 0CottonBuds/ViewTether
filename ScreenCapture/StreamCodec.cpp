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
	setupffmpegContext();
	setupffmpegContextOptions(height, width, fps);
	setupBytesPerPixel();
}

void StreamCodec::run()
{
	//open codec
	if (avcodec_open2(context, codec, nullptr) < 0) {
		qDebug() << "Could not open codec";
		exit(-1);
	}
	
	// SWS context
	SwsContext* swsContext = sws_getContext(
	  width, height,
	  AV_PIX_FMT_BGRA,
	  width, height,
	  AV_PIX_FMT_YUV420P,
	  0, nullptr, nullptr, nullptr);

	if (!swsContext){
	  qDebug() << "Could not allocate sws context";
	  exit(-1);
	}

	//frame and packet init
	AVPacket* packet = av_packet_alloc();

	AVFrame *frame;
	frame = av_frame_alloc();
	if (!frame){
	  qDebug() << "Could not allocate video frame";
	  exit(-1);
	}

	frame->format = context->pix_fmt;
	frame->height = context->height;
	frame->width  = context->width;

	if(av_frame_get_buffer(frame, 0) < 0){
	  qDebug() << "Can't allocate the video frame data";
	  exit(-1);
	}

	qDebug() << "Finished";
}

bool StreamCodec::setupffmpegContext()
{
	codec = avcodec_find_encoder(AV_CODEC_ID_HEVC);
	if (!codec) {
		qDebug() << "Codec with specified id not found";
		return false;
	}
	context = avcodec_alloc_context3(codec);
	if (!context) {
		qDebug() << "Can't allocate video codec context";
		return false;
	}
}

bool StreamCodec::setupffmpegContextOptions(int height, int width, int fps)
{
	try {
		context->height = height;
		context->width = width;

		/// Frames per second
		context->time_base.num = 1;
		context->time_base.den = fps;
		context->framerate.num = fps;
		context->framerate.den = 1;

		context->pix_fmt = AV_PIX_FMT_YUV420P;
		context->gop_size = fps * 2;
		context->max_b_frames = 3;

		context->refs = 3;

		/// Compression efficiency (slower -> better quality + higher cpu%)
		/// [ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow]
		av_opt_set(context->priv_data, "preset", "ultrafast", 0);

		/// Compression rate (lower -> higher compression) compress to lower size, makes decoded image more noisy
		/// Range: [0; 51], sane range: [18; 26]. I used 35 as good compression/quality compromise. This option also critical for realtime encoding
		av_opt_set(context->priv_data, "crf", "35", 0);

		/// Change settings based upon the specifics of input
		/// [psnr, ssim, grain, zerolatency, fastdecode, animation]
		/// This option is most critical for realtime encoding, because it removes delay between 1th input frame and 1th output packet.
		av_opt_set(context->priv_data, "tune", "zerolatency", 0);
		return true;
	}
	catch (...) {
		qDebug() << "Error setting up ffmpeg context options. exiting.";
		exit(-1);
	}
}

bool StreamCodec::setupBytesPerPixel()
{
	auto desc = av_pix_fmt_desc_get(AV_PIX_FMT_RGB24);
	if (!desc){
	  qDebug() << "Can't get descriptor for pixel format";
	  return false;
	}

	bytesPerPixel = av_get_bits_per_pixel(desc) / 8;
	if(!(bytesPerPixel==3 && !(av_get_bits_per_pixel(desc) % 8))){
	  qDebug() << "Unhandled bits per pixel, bad in pix fmt";
	  return false;
	}
}




