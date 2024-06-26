#include "StreamCodec.h"
#include "qdebug.h"


StreamCodec::StreamCodec(int height, int width, int fps, CodecType type)
{
	this->height = height;
	this->width = width;
	this->fps = fps;
	this->type = type;
}


void StreamCodec::run()
{
	if (type == CodecType::encode) {
		initializeEncoder();
		initializeEncoderSWS();
	}
	else {
		initializeDecoder();
		initializedecoderSWS();
	}
}



void StreamCodec::initializeEncoder()
{
	encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!encoder) {
		qDebug() << "Codec not found";
		exit(1);
	}
	 
	encoderContext = avcodec_alloc_context3(encoder);
	if (!encoderContext) {
		qDebug() << "Could not allocate codec context";
		exit(1);
	}

	encoderContext->height = height;
	encoderContext->width = width;
	encoderContext->time_base.num = 1;
	encoderContext->time_base.den = fps;
	encoderContext->framerate.num = fps;
    encoderContext->framerate.den = 1;
	encoderContext->pix_fmt = AV_PIX_FMT_YUV420P;

	encoderContext->gop_size = 0;

	av_opt_set(encoderContext->priv_data, "preset", "ultrafast", 0);
	av_opt_set(encoderContext->priv_data, "crf", "35", 0);
	av_opt_set(encoderContext->priv_data, "tune", "zerolatency", 0);

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

	int err = avcodec_open2(encoderContext, encoder, nullptr);
	if (err < 0) {
		qDebug() << "Could not open codec";
		exit(1);
	}
}

void StreamCodec::initializeEncoderSWS()
{
	encoderSwsContext = sws_getContext(width, height, AV_PIX_FMT_BGRA, width, height, AV_PIX_FMT_YUV420P, NULL, NULL, NULL, NULL);
	if (!encoderSwsContext) {
		qDebug() << "Could not allocate SWS Context";
		exit(1);
	}
}

void StreamCodec::encodeFrame(std::shared_ptr<UCHAR> pData)
{
	if (type != CodecType::encode) {
		qDebug() << "Error: not on encode mode";
		exit(1);
	}

	int err = 0;
	AVFrame* frame1 = allocateFrame(pData);
	AVFrame* frame = convertFrameFromRGBAtoYUV(frame1);

	err = avcodec_send_frame(encoderContext, frame);
	if (err < 0) {
		qDebug() << "Error sending frame to codec";
		char* errStr = new char;
		av_make_error_string(errStr, 255, err);
		qDebug() << errStr;
		av_frame_free(&frame);
		exit(1);
	}

	while (true) {
		AVPacket* packet = allocatepacket(frame);
		err = avcodec_receive_packet(encoderContext, packet);
		if (err == AVERROR_EOF || err == AVERROR(EAGAIN) ) {
			av_packet_unref(packet);
			av_packet_free(&packet);
			break;
		}
		if (err < 0) {
			qDebug() << "Error recieving to codec";
			char* errStr = new char;
			av_make_error_string(errStr, 255, err);
			qDebug() << errStr;
			av_frame_free(&frame);
			av_frame_free(&frame1);
			av_packet_free(&packet);
			exit(1);
		}
		emit encodeFinish(packet);
	}

	av_frame_free(&frame);
	av_frame_free(&frame1);
}

AVPacket* StreamCodec::allocatepacket(AVFrame* frame)
{
	AVPacket* packet = av_packet_alloc();
	if (!packet) {
		qDebug() << "Could not allocate memory for packet";
		av_frame_free(&frame);
		exit(1);
	}
	return packet;
}

AVFrame* StreamCodec::allocateFrame(std::shared_ptr<UCHAR> pData)
{
	AVFrame* frame = av_frame_alloc();
	if (!frame) {
		qDebug() << "Could not allocate memory for frame";
		exit(1);
	}

	frame->format = AV_PIX_FMT_BGRA;
	frame->width = width;
	frame->height = height;
	frame->pts = pts;

	if (av_frame_get_buffer(frame, 0) < 0) {
		qDebug() << "Failed to get frame buffer";
		exit(1);
	}

	if (av_frame_make_writable(frame) < 0) {
		qDebug() << "Failed to make frame writable";
		exit(1);
	}

	frame->data[0] = pData.get();

	return frame;
}

AVFrame* StreamCodec::convertFrameFromRGBAtoYUV(AVFrame* frame)
{
	AVFrame* yuvFrame = av_frame_alloc();
	if (!yuvFrame) {
		qDebug() << "Unable to allocate memory for yuv frame";
		av_frame_free(&frame);
		exit(1);
	}

	yuvFrame->format = encoderContext->pix_fmt;
	yuvFrame->width = width;
	yuvFrame->height = height;
	yuvFrame->pts = pts;
	pts += 1;
	
	if (av_frame_get_buffer(yuvFrame, 0) < 0) {
		qDebug() << "Failed to get frame buffer";
		exit(1);
	}

	if (av_frame_make_writable(yuvFrame) < 0) {
		qDebug() << "Failed to make frame writable";
		exit(1);
	}

	int err = sws_scale(encoderSwsContext, (const uint8_t* const*)frame->data, frame->linesize, 0, height, (uint8_t* const*)yuvFrame->data, yuvFrame->linesize);
	if (err < 0) {
		qDebug() << "Could not format frame to yuv420p";
		exit(1);
	}
	return yuvFrame;
}



void StreamCodec::initializeDecoder()
{
	decoder = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!decoder) {
		qDebug() << "Codec not found";
		exit(1);
	}
	 
	decoderContext = avcodec_alloc_context3(decoder);
	if (!decoderContext) {
		qDebug() << "Could not allocate codec context";
		exit(1);
	}

	decoderContext->height = height;
	decoderContext->width = width;
	decoderContext->time_base.num = 1;
	decoderContext->time_base.den = fps;
	decoderContext->framerate.num = fps;
    decoderContext->framerate.den = 1;
	decoderContext->pix_fmt = AV_PIX_FMT_YUV420P;


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

	CoUninitialize();
	int err = avcodec_open2(decoderContext, decoder, nullptr);
	if (err < 0) {
		qDebug() << "Could not open codec";
		exit(1);
	}
}

void StreamCodec::initializedecoderSWS()
{
	decoderSwsContext = sws_getContext(width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_BGRA, NULL, NULL, NULL, NULL);
	if (!decoderSwsContext) {
		qDebug() << "Could not allocate SWS Context";
		exit(1);
	}
}

void StreamCodec::decodePacket(AVPacket* packet)
{
	if (type != CodecType::decode) {
		qDebug() << "Error: Not on decode mode";
		exit(1);
	}
	int err = 0;

	err = avcodec_send_packet(decoderContext, packet);
	if (err < 0) {
		qDebug() << "Error sending packet to codec";
		char* errStr = new char;
		av_make_error_string(errStr, 255, err);
		qDebug() << errStr;
		av_packet_free(&packet);
		exit(1);
	}

	AVFrame* frame = av_frame_alloc();
	int response = 0;
	while (response >= 0) {
		response = avcodec_receive_frame(decoderContext, frame);

		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			break;
		}
		else if (response < 0) {
			qDebug() << "Error recieving frame from codec";
			char* errStr = new char;
			av_make_error_string(errStr, 255, err);
			qDebug() << errStr;
			av_packet_free(&packet);
			av_frame_free(&frame);
			exit(1);
		}

		AVFrame *bgraFrame = convertFrameFromYUVtoBGRA(frame);

		emit decodeFinish(bgraFrame);

		av_frame_unref(frame);
		av_frame_free(&frame);
	}

	av_frame_free(&frame);
	av_packet_free(&packet);
}

AVFrame* StreamCodec::convertFrameFromYUVtoBGRA(AVFrame* frame)
{
	AVFrame *bgraFrame = av_frame_alloc();
	if (!bgraFrame) {
		qDebug() << "Unable to allocate memory for yuv frame";
		av_frame_free(&frame);
		exit(1);
	}

	bgraFrame->format = AV_PIX_FMT_BGRA;
	bgraFrame->width = width;
	bgraFrame->height = height;
	bgraFrame->pts = pts;
	pts += 1;
	
	if (av_frame_get_buffer(bgraFrame, 0) < 0) {
		qDebug() << "Failed to get frame buffer";
		exit(1);
	}

	if (av_frame_make_writable(bgraFrame) < 0) {
		qDebug() << "Failed to make frame writable";
		exit(1);
	}

	int err = sws_scale(decoderSwsContext, (const uint8_t* const*)frame->data, frame->linesize, 0, height, (uint8_t* const*)bgraFrame->data, bgraFrame->linesize);
	if (err < 0) {
		qDebug() << "Could not format frame to bgra";
		exit(1);
	}	
	return bgraFrame;
}


