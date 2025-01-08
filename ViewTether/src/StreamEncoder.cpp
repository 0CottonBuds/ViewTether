#include "StreamEncoder.h"
#include "qdebug.h"
#include "mfx/mfxvideo.h"
#include "libavutil/hwcontext_qsv.h"

static void ffmpeg_debug_error(int err)
{
	char* errStr = new char;
	av_make_error_string(errStr, 255, err);
	qDebug() << "Error: " << err << ": " << errStr;
}

StreamEncoder::StreamEncoder(int height, int width, int fps, AVHWDeviceType hardwareAccelerationType)
{
	this->m_height = height;
	this->m_width = width;
	this->m_fps = fps;
	this->m_hardwareAccelerationType = hardwareAccelerationType;
}

void StreamEncoder::initialize()
{
	m_backPacket = av_packet_alloc();
	if (!m_backPacket) {
		qDebug() << "Could not allocate memory for packet";
		exit(1);
	}

	initializeTestDecoder();

	if (m_hardwareAccelerationType == AV_HWDEVICE_TYPE_QSV) {
		initializeHWEncoder();
	}
	else if (m_hardwareAccelerationType == AV_HWDEVICE_TYPE_NONE){
		initializeEncoder();
	}
	else{
		qDebug() << "Hardware acceleration type not supported";
		exit(1);
	}
}

void StreamEncoder::encodeFrame(std::shared_ptr<uint8_t> pixelData)
{
	int err = 0;

	AVFrame* frame = allocateFrame(pixelData);
	if (!frame) {
		qDebug() << "Could not allocate frame";
		exit(1);
	}

	err = avcodec_send_frame(m_encoderContext, frame);
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
		
		err = avcodec_receive_packet(m_encoderContext, packet);
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

void StreamEncoder::encodeHWFrame(std::shared_ptr<uint8_t> pixelData)
{
	AVFrame* hwFrame = av_frame_alloc();
	if (!hwFrame) {
		qDebug() << "Could not allocate frame";
		exit(1);
	}
	hwFrame->hw_frames_ctx = m_encoderContext->hw_frames_ctx;

	int err = 0;
	if (err = av_hwframe_get_buffer(m_encoderContext->hw_frames_ctx, hwFrame, 0) < 0) {
		fprintf(stderr, "Failed to allocate HW frame\n");
		ffmpeg_debug_error(err);
		exit(-1);
	}

	AVFrame* swFrame = av_frame_alloc();
	if (!swFrame) {
		qDebug() << "Could not allocate software frame";
		exit(1);
	}
	swFrame->format = AV_PIX_FMT_BGRA;
	swFrame->m_height = m_height;
	swFrame->m_width = m_width;

	err = av_frame_get_buffer(swFrame, 0);
	if (err < 0) {
		qDebug() << "Failed to get frame buffer";
		ffmpeg_debug_error(err);
		exit(1);
	}

	err = av_frame_make_writable(swFrame);
	if (err < 0) {
		qDebug() << "Failed to make frame writable";
		ffmpeg_debug_error(err);
		exit(1);
	}

	memcpy(swFrame->data[0], pixelData.get(), m_width * m_height * 4);


	err = av_hwframe_transfer_data(hwFrame, swFrame, 0);
	if (err < 0) {
		fprintf(stderr, "Failed to transfer data to hardware frame\n");
		ffmpeg_debug_error(err);
		exit(-1);
	}

	av_frame_free(&swFrame);

	err = avcodec_send_frame(m_encoderContext, hwFrame);
	if (err < 0) {
		qDebug() << "Error sending frame to codec";
		ffmpeg_debug_error(err);
		exit(1);
	}

	av_frame_free(&hwFrame);

	while (true) {
		AVPacket* packet = av_packet_alloc();
		if (!packet) {
			qDebug() << "Could not allocate memory for packet.";
			qDebug() << "Packet is null.";
			exit(1);
		}
		
		err = avcodec_receive_packet(m_encoderContext, packet);
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
		//testPacket(packet);
	}
}

void StreamEncoder::initializeHWEncoder()
{
	//av_log_set_level(AV_LOG_DEBUG);
	//av_log(NULL, AV_LOG_DEBUG, "Message\n");

	m_encoder = avcodec_find_encoder_by_name("hevc_qsv");
	if (!m_encoder) {
		std::cout << "Codec not found" << std::endl;
		exit(1);
	}
	 
	m_encoderContext = avcodec_alloc_context3(m_encoder);
	if (!m_encoderContext) {
		std::cout << "Could not allocate codec context" << std::endl;
		exit(1);
	}

	AVBufferRef* hwDeviceContext = nullptr;

	AVDictionary* hwDeviceContextOptions = nullptr;
	av_dict_set(&hwDeviceContextOptions, "child_device_type", "d3d11va", 0);
	if (av_hwdevice_ctx_create(&hwDeviceContext, AV_HWDEVICE_TYPE_QSV, nullptr, hwDeviceContextOptions, 0) < 0) {
		fprintf(stderr, "Failed to create hardware device context.\n");
		exit(-1);
	}

	AVBufferRef* hwFramesRef = av_hwframe_ctx_alloc(hwDeviceContext);
	if (!hwFramesRef) {
		fprintf(stderr, "Failed to create hardware frame context.\n");
		exit(-1);
	}

	AVHWFramesContext* hwFramesContext = (AVHWFramesContext*)hwFramesRef->data;
	hwFramesContext->format = AV_PIX_FMT_QSV;  
	hwFramesContext->sw_format = AV_PIX_FMT_BGRA;
	hwFramesContext->m_width = m_width;  
	hwFramesContext->m_height = m_height; 
	hwFramesContext->device_ref = hwDeviceContext;
	hwFramesContext->device_ctx =(AVHWDeviceContext*) hwDeviceContext->data;


	m_encoderContext->bit_rate = m_bitrate;
	m_encoderContext->m_width = m_width;
	m_encoderContext->m_height = m_height;

	m_encoderContext->time_base.num = 1;
	m_encoderContext->time_base.den = m_fps;
    m_encoderContext->framerate.den = 1;
	m_encoderContext->framerate.num = m_fps;

	m_encoderContext->pix_fmt = AV_PIX_FMT_QSV;

	m_encoderContext->hw_device_ctx = hwDeviceContext;
	m_encoderContext->hw_frames_ctx = hwFramesRef;

	if (av_hwframe_ctx_init(m_encoderContext->hw_frames_ctx) < 0) {
		fprintf(stderr, "Failed to initialize hardware frame context.\n");
		exit(-1);
	}

	int err = avcodec_open2(m_encoderContext, m_encoder, nullptr);
	if (err < 0) {
		std::cout << "Could not open codec" << std::endl;
		ffmpeg_debug_error(err);
		exit(1);
	}
}

void StreamEncoder::initializeTestDecoder()
{
	m_testDecoder = avcodec_find_decoder_by_name("hevc");
	if (!m_testDecoder) {
		qDebug() << "Codec not found";
		exit(1);
	}

	m_testDecoderContext = avcodec_alloc_context3(m_testDecoder);
	if (!m_testDecoderContext) {
		qDebug() << "Could not allocate codec context";
		exit(1);
	}

	m_testDecoderContext->m_height = m_height;
	m_testDecoderContext->m_width = m_width;
	m_testDecoderContext->bit_rate = m_bitrate;

	m_testDecoderContext->time_base.num = 1;
	m_testDecoderContext->time_base.den = m_fps;
	m_testDecoderContext->framerate.num = m_fps;
	m_testDecoderContext->framerate.den = 1;
	m_testDecoderContext->pix_fmt = AV_PIX_FMT_YUV420P;

	//testDecoderContext->thread_count = 2;

	int err = avcodec_open2(m_testDecoderContext, m_testDecoder, nullptr);
	if (err < 0) {
		qDebug() << "Could not open codec";
		ffmpeg_debug_error(err);
		exit(1);
	}

	m_testDecoderSwsContext = sws_getContext(m_width, m_height, AV_PIX_FMT_YUV420P, m_width, m_height, AV_PIX_FMT_BGRA, NULL, NULL, NULL, NULL);
	if (!m_testDecoderSwsContext) {
		qDebug() << "Could not allocate SWS Context";
		exit(1);
	}
}

void StreamEncoder::initializeEncoder()
{
	m_encoder = avcodec_find_encoder_by_name("hevc_qsv");
	if (!m_encoder) {
		std::cout << "Codec not found" << std::endl;
		exit(1);
	}
	 
	m_encoderContext = avcodec_alloc_context3(m_encoder);
	if (!m_encoderContext) {
		std::cout << "Could not allocate codec context" << std::endl;
		exit(1);
	}

	m_encoderContext->bit_rate = m_bitrate;
	m_encoderContext->m_width = m_width;
	m_encoderContext->m_height = m_height;

	m_encoderContext->time_base.num = 1;
	m_encoderContext->time_base.den = m_fps;
    m_encoderContext->framerate.den = 1;
	m_encoderContext->framerate.num = m_fps;

	m_encoderContext->pix_fmt = AV_PIX_FMT_NV12;

	av_opt_set(m_encoderContext->priv_data, "preset", "veryfast", 0);
	av_opt_set(m_encoderContext->priv_data, "crf", "26", 0);
	av_opt_set(m_encoderContext->priv_data, "tune", "zerolatency", 0);
	av_opt_set(m_encoderContext->priv_data, "forced_idr", "1", 0);

	int err = avcodec_open2(m_encoderContext, m_encoder, nullptr);
	if (err < 0) {
		std::cout << "Could not open codec" << std::endl;
		ffmpeg_debug_error(err);
		exit(1);
	}

	// non hardware accelerated encoder needs sws to convert pixel data to yuv420p
	m_encoderSwsContext = sws_getContext(m_width, m_height, AV_PIX_FMT_BGRA, m_width, m_height, AV_PIX_FMT_NV12, NULL, NULL, NULL, NULL);
	if (!m_encoderSwsContext) {
		qDebug() << "Could not allocate SWS Context";
		exit(1);
	}
}

AVFrame* StreamEncoder::allocateFrame(std::shared_ptr<uint8_t> pixelData)
{
	uint8_t* data[8];
	int linesize[8];

	data[0] = pixelData.get();
	linesize[0] = m_width * 4;

	AVFrame* yuvFrame = av_frame_alloc();
	if (!yuvFrame) {
		qDebug() << "Unable to allocate memory for yuv frame";
		exit(1);
	}

	yuvFrame->format = m_encoderContext->pix_fmt;
	yuvFrame->m_width = m_width;
	yuvFrame->m_height = m_height;
	yuvFrame->m_pts = m_pts;
	//yuvFrame->pict_type = AV_PICTURE_TYPE_I;

	m_pts += 1;
	
	
	if (av_frame_get_buffer(yuvFrame, 0) < 0) {
		qDebug() << "Failed to get frame buffer";
		exit(1);
	}
	
	if (av_frame_make_writable(yuvFrame) < 0) {
		qDebug() << "Failed to make frame writable";
		exit(1);
	}

	int err = sws_scale(m_encoderSwsContext, (const uint8_t* const*)data, linesize, 0, m_height, (uint8_t* const*)yuvFrame->data, yuvFrame->linesize);
	if (err < 0) {
		qDebug() << "Could not format frame to yuv420p";
		exit(1);
	}
	
	return yuvFrame;
}

AVFrame* StreamEncoder::convertYUVFrameToBGRA(AVFrame* yuvFrame)
{
	AVFrame* bgraFrame = av_frame_alloc();
	if (!bgraFrame) {
		qDebug() << "Unable to allocate memory for yuv frame";
		av_frame_free(&yuvFrame);
		exit(1);
	}

	bgraFrame->format = AV_PIX_FMT_BGRA;
	bgraFrame->m_width = m_width;
	bgraFrame->m_height = m_height;
	bgraFrame->m_pts = m_pts;

	if (av_frame_get_buffer(bgraFrame, 0) < 0) {
		qDebug() << "Failed to get frame buffer";
		exit(1);
	}

	if (av_frame_make_writable(bgraFrame) < 0) {
		qDebug() << "Failed to make frame writable";
		exit(1);
	}

	int err = sws_scale(m_testDecoderSwsContext, (const uint8_t* const*)yuvFrame->data, yuvFrame->linesize, 0, m_height, (uint8_t* const*)bgraFrame->data, bgraFrame->linesize);
	if (err < 0) {
		qDebug() << "Could not format frame to bgra";
		exit(1);
	}
	return bgraFrame;
}

void StreamEncoder::testPacket(AVPacket* packet)
{
	int err = 0;

	err = avcodec_send_packet(m_testDecoderContext, packet);
	if (err < 0) {
		qDebug() << "Error sending packet to codec";
		ffmpeg_debug_error(err);
		exit(1);
	}
	av_packet_free(&packet);

	AVFrame* frame;
	err = 0;
	while (err >= 0) {
		frame = av_frame_alloc();
		err = avcodec_receive_frame(m_testDecoderContext, frame);

		if (err == AVERROR(EAGAIN) || err == AVERROR_EOF) {
			break;
		}
		else if (err < 0) {
			qDebug() << "Error recieving frame from codec";
			ffmpeg_debug_error(err);
			exit(1);
		}

		AVFrame* bgraFrame = convertYUVFrameToBGRA(frame);

		emit frameReady(std::shared_ptr<uint8_t>(bgraFrame->data[0], av_free));
		
		av_frame_unref(frame);
		av_frame_free(&frame);
	}

	av_frame_free(&frame);
	av_packet_free(&packet);
}
