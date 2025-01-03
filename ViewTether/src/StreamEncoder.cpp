#include "StreamEncoder.h"
#include "qdebug.h"
#include "mfx/mfxvideo.h"
#include "libavutil/hwcontext_qsv.h"

void ffmpeg_debug_error(int err)
{
	char* errStr = new char;
	av_make_error_string(errStr, 255, err);
	qDebug() << "Error: " << err << ": " << errStr;
}

StreamEncoder::StreamEncoder(int height, int width, int fps, AVHWDeviceType hardwareAccelerationType)
{
	this->height = height;
	this->width = width;
	this->fps = fps;
	this->hardwareAccelerationType = hardwareAccelerationType;
}

void StreamEncoder::initialize()
{
	backPacket = av_packet_alloc();

	initializeTestDecoder();

	if (hardwareAccelerationType == AV_HWDEVICE_TYPE_QSV) {
		initializeHWEncoder();
	}
	else if (hardwareAccelerationType == AV_HWDEVICE_TYPE_NONE){
		initializeEncoder();
	}
	else{
		qDebug() << "Hardware acceleration type not supported";
		exit(1);
	}
}

void StreamEncoder::initializeHWEncoder()
{
	av_log_set_level(AV_LOG_DEBUG);
	av_log(NULL, AV_LOG_DEBUG, "Message\n");

	encoder = avcodec_find_encoder_by_name("hevc_qsv");
	if (!encoder) {
		std::cout << "Codec not found" << std::endl;
		exit(1);
	}
	 
	encoderContext = avcodec_alloc_context3(encoder);
	if (!encoderContext) {
		std::cout << "Could not allocate codec context" << std::endl;
		exit(1);
	}

	AVBufferRef* hw_device_ctx = nullptr;

	AVDictionary* options = nullptr;
	av_dict_set(&options, "child_device_type", "d3d11va", 0);
	if (av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_QSV, nullptr, options, 0) < 0) {
		fprintf(stderr, "Failed to create hardware device context.\n");
		exit(-1);
	}

	AVBufferRef* hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx);
	if (!hw_frames_ref) {
		fprintf(stderr, "Failed to create hardware frame context.\n");
		exit(-1);
	}

	AVHWFramesContext* hw_frames_ctx = (AVHWFramesContext*)hw_frames_ref->data;
	hw_frames_ctx->format = AV_PIX_FMT_QSV;  
	hw_frames_ctx->sw_format = AV_PIX_FMT_BGRA;
	hw_frames_ctx->width = width;  
	hw_frames_ctx->height = height; 
	hw_frames_ctx->device_ref = hw_device_ctx;
	hw_frames_ctx->device_ctx =(AVHWDeviceContext*) hw_device_ctx->data;
	//hw_frames_ctx->initial_pool_size = 20;


	encoderContext->bit_rate = bitrate;
	encoderContext->width = width;
	encoderContext->height = height;

	encoderContext->time_base.num = 1;
	encoderContext->time_base.den = fps;
    encoderContext->framerate.den = 1;
	encoderContext->framerate.num = fps;

	encoderContext->pix_fmt = AV_PIX_FMT_QSV;

	encoderContext->hw_device_ctx = hw_device_ctx;
	encoderContext->hw_frames_ctx = hw_frames_ref;

	if (av_hwframe_ctx_init(encoderContext->hw_frames_ctx) < 0) {
		fprintf(stderr, "Failed to initialize hardware frame context.\n");
		exit(-1);
	}

	int err = avcodec_open2(encoderContext, encoder, nullptr);
	if (err < 0) {
		std::cout << "Could not open codec" << std::endl;
		ffmpeg_debug_error(err);
		exit(1);
	}
}

void StreamEncoder::initializeTestDecoder()
{
	testDecoder = avcodec_find_decoder(AV_CODEC_ID_HEVC);
	if (!testDecoder) {
		qDebug() << "Codec not found";
		exit(1);
	}

	testDecoderContext = avcodec_alloc_context3(testDecoder);
	if (!testDecoderContext) {
		qDebug() << "Could not allocate codec context";
		exit(1);
	}

	testDecoderContext->height = height;
	testDecoderContext->width = width;
	testDecoderContext->time_base.num = 1;
	testDecoderContext->time_base.den = fps;
	testDecoderContext->framerate.num = fps;
	testDecoderContext->framerate.den = 1;
	testDecoderContext->pix_fmt = AV_PIX_FMT_NV12;

	//testDecoderContext->thread_count = 2;

	int err = avcodec_open2(testDecoderContext, testDecoder, nullptr);
	if (err < 0) {
		qDebug() << "Could not open codec";
		ffmpeg_debug_error(err);
		exit(1);
	}

	testDecoderSwsContext = sws_getContext(width, height, AV_PIX_FMT_NV12, width, height, AV_PIX_FMT_BGRA, NULL, NULL, NULL, NULL);
	if (!testDecoderSwsContext) {
		qDebug() << "Could not allocate SWS Context";
		exit(1);
	}
}

void StreamEncoder::initializeEncoder()
{
	encoder = avcodec_find_encoder_by_name("hevc_qsv");
	if (!encoder) {
		std::cout << "Codec not found" << std::endl;
		exit(1);
	}
	 
	encoderContext = avcodec_alloc_context3(encoder);
	if (!encoderContext) {
		std::cout << "Could not allocate codec context" << std::endl;
		exit(1);
	}

	encoderContext->bit_rate = bitrate;
	encoderContext->width = width;
	encoderContext->height = height;

	encoderContext->time_base.num = 1;
	encoderContext->time_base.den = fps;
    encoderContext->framerate.den = 1;
	encoderContext->framerate.num = fps;

	encoderContext->pix_fmt = AV_PIX_FMT_NV12;

	av_opt_set(encoderContext->priv_data, "preset", "veryfast", 0);
	av_opt_set(encoderContext->priv_data, "crf", "26", 0);
	av_opt_set(encoderContext->priv_data, "tune", "zerolatency", 0);
	av_opt_set(encoderContext->priv_data, "forced_idr", "1", 0);

	int err = avcodec_open2(encoderContext, encoder, nullptr);
	if (err < 0) {
		std::cout << "Could not open codec" << std::endl;
		ffmpeg_debug_error(err);
		exit(1);
	}

	// non hardware accelerated encoder needs sws to convert pixel data to yuv420p
	encoderSwsContext = sws_getContext(width, height, AV_PIX_FMT_BGRA, width, height, AV_PIX_FMT_NV12, NULL, NULL, NULL, NULL);
	if (!encoderSwsContext) {
		qDebug() << "Could not allocate SWS Context";
		exit(1);
	}
}

void StreamEncoder::encodeFrame(std::shared_ptr<UCHAR> pData)
{
	int err = 0;

	AVFrame* frame = allocateFrame(pData);
	if (!frame) {
		qDebug() << "Could not allocate frame";
		exit(1);
	}

	err = avcodec_send_frame(encoderContext, frame);
	if (err < 0) {
		qDebug() << "Error sending frame to codec";
		ffmpeg_debug_error(err);
		exit(1);
	}
	av_frame_free(&frame);

	while (true) {
		AVPacket* packet = av_packet_alloc();
		if (!packet) {
			qDebug() << "Could not allocate memory for packet";
			exit(1);
		}
		
		err = avcodec_receive_packet(encoderContext, packet);
		if (err == AVERROR_EOF || err == AVERROR(EAGAIN) ) {
			av_packet_unref(packet);
			av_packet_free(&packet);
			break;
		}
		if (err < 0) {
			qDebug() << "Error recieving to codec";
			ffmpeg_debug_error(err);
			exit(1);
		}

		// TODO: Make the packet a smart pointer
		emit encodeFinish(packet);
	}
}

void StreamEncoder::encodeHWFrame(std::shared_ptr<UCHAR> pData)
{
	AVFrame* hw_frame = av_frame_alloc();
	if (!hw_frame) {
		qDebug() << "Could not allocate frame";
		exit(1);
	}
	hw_frame->hw_frames_ctx = encoderContext->hw_frames_ctx;

	int err = 0;
	if (err = av_hwframe_get_buffer(encoderContext->hw_frames_ctx, hw_frame, 0) < 0) {
		fprintf(stderr, "Failed to allocate HW frame\n");
		ffmpeg_debug_error(err);
		exit(-1);
	}

	AVFrame* sw_frame = av_frame_alloc();
	if (!sw_frame) {
		qDebug() << "Could not allocate software frame";
		exit(1);
	}
	sw_frame->format = AV_PIX_FMT_BGRA;
	sw_frame->height = height;
	sw_frame->width = width;

	err = av_image_fill_arrays(sw_frame->data, sw_frame->linesize, pData.get(), AV_PIX_FMT_BGRA, width, height, 1);
	if (err < 0) {
		fprintf(stderr, "Failed to transfer data to hardware frame\n");
		ffmpeg_debug_error(err);
		exit(-1);
	}

	err = av_hwframe_transfer_data(hw_frame, sw_frame, 0);
	if (err < 0) {
		fprintf(stderr, "Failed to transfer data to hardware frame\n");
		ffmpeg_debug_error(err);
		exit(-1);
	}

	av_frame_free(&sw_frame);

	err = avcodec_send_frame(encoderContext, hw_frame);
	if (err < 0) {
		qDebug() << "Error sending frame to codec";
		ffmpeg_debug_error(err);
		exit(1);
	}

	av_frame_free(&hw_frame);

	while (true) {
		AVPacket* packet = av_packet_alloc();
		if (!packet) {
			qDebug() << "Could not allocate memory for packet.";
			qDebug() << "Packet is null.";
			exit(1);
		}
		
		err = avcodec_receive_packet(encoderContext, packet);
		if (err == AVERROR_EOF || err == AVERROR(EAGAIN) ) { // packet not ready
			av_packet_unref(packet);
			av_packet_free(&packet);
			break;
		}
		if (err < 0) {
			qDebug() << "Error recieving to codec";
			ffmpeg_debug_error(err);
			exit(1);
		}

		emit encodeFinish(packet); 
	}
}

AVFrame* StreamEncoder::allocateFrame(std::shared_ptr<UCHAR> pData)
{
	uint8_t* data[8];
	int linesize[8];

	data[0] = pData.get();
	linesize[0] = 7680;

	AVFrame* yuvFrame = av_frame_alloc();
	if (!yuvFrame) {
		qDebug() << "Unable to allocate memory for yuv frame";
		exit(1);
	}

	yuvFrame->format = encoderContext->pix_fmt;
	yuvFrame->width = width;
	yuvFrame->height = height;
	yuvFrame->pts = pts;
	//yuvFrame->pict_type = AV_PICTURE_TYPE_I;

	pts += 1;
	
	
	if (av_frame_get_buffer(yuvFrame, 0) < 0) {
		qDebug() << "Failed to get frame buffer";
		exit(1);
	}
	
	if (av_frame_make_writable(yuvFrame) < 0) {
		qDebug() << "Failed to make frame writable";
		exit(1);
	}

	int err = sws_scale(encoderSwsContext, (const uint8_t* const*)data, linesize, 0, height, (uint8_t* const*)yuvFrame->data, yuvFrame->linesize);
	if (err < 0) {
		qDebug() << "Could not format frame to yuv420p";
		exit(1);
	}
	
	return yuvFrame;
}

AVFrame* StreamEncoder::convertFrameToBGRA(AVFrame* yuvFrame)
{
	AVFrame* bgraFrame = av_frame_alloc();
	if (!bgraFrame) {
		qDebug() << "Unable to allocate memory for yuv frame";
		av_frame_free(&yuvFrame);
		exit(1);
	}

	bgraFrame->format = AV_PIX_FMT_BGRA;
	bgraFrame->width = width;
	bgraFrame->height = height;
	bgraFrame->pts = pts;

	if (av_frame_get_buffer(bgraFrame, 0) < 0) {
		qDebug() << "Failed to get frame buffer";
		exit(1);
	}

	if (av_frame_make_writable(bgraFrame) < 0) {
		qDebug() << "Failed to make frame writable";
		exit(1);
	}

	int err = sws_scale(testDecoderSwsContext, (const uint8_t* const*)yuvFrame->data, yuvFrame->linesize, 0, height, (uint8_t* const*)bgraFrame->data, bgraFrame->linesize);
	if (err < 0) {
		qDebug() << "Could not format frame to bgra";
		exit(1);
	}
	return bgraFrame;
}

void StreamEncoder::testPacket(AVPacket* packet)
{
	int err = 0;

	err = avcodec_send_packet(testDecoderContext, packet);
	if (err < 0) {
		qDebug() << "Error sending packet to codec";
		ffmpeg_debug_error(err);
		exit(1);
	}

	AVFrame* frame;
	err = 0;
	while (err >= 0) {
		frame = av_frame_alloc();
		err = avcodec_receive_frame(testDecoderContext, frame);

		if (err == AVERROR(EAGAIN) || err == AVERROR_EOF) {
			break;
		}
		else if (err < 0) {
			qDebug() << "Error recieving frame from codec";
			ffmpeg_debug_error(err);
			exit(1);
		}

		AVFrame* bgraFrame = convertFrameToBGRA(frame);

		emit frameReady(std::shared_ptr<UCHAR>(bgraFrame->data[0], av_free));
		
		av_frame_unref(frame);
		av_frame_free(&frame);
	}

	av_frame_free(&frame);
	av_packet_free(&packet);
}
