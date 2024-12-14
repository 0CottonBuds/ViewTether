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


// Function to create a test NV12 texture
HRESULT CreateNV12Texture(ID3D11Device* device, ID3D11Texture2D** texture) {
	if (!device || !texture) return E_POINTER;

	// Define the texture description
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = 1920;                         // Texture width
	textureDesc.Height = 1080;                        // Texture height
	textureDesc.MipLevels = 1;                        // Single mip level
	textureDesc.ArraySize = 1;                        // Single array slice
	textureDesc.Format = DXGI_FORMAT_NV12;            // NV12 format
	textureDesc.SampleDesc.Count = 1;                 // No multisampling
	textureDesc.Usage = D3D11_USAGE_DEFAULT;          // Default usage
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind as a shader resource

	// Calculate the size of the Y plane and UV plane
	const size_t yPlaneSize = 1920 * 1080;            // Width * Height
	const size_t uvPlaneSize = (1920 / 2) * (1080 / 2) * 2; // (Width/2 * Height/2) * 2 bytes per pixel (UV interleaved)

	// Create test data for NV12 (all white)
	std::vector<unsigned char> nv12Data(yPlaneSize + uvPlaneSize, 0);
	std::fill(nv12Data.begin(), nv12Data.begin() + yPlaneSize, 255); // Y plane: white (255)
	std::fill(nv12Data.begin() + yPlaneSize, nv12Data.end(), 128);   // UV plane: neutral chroma (128, 128)

	// Define the subresource data
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = nv12Data.data();
	initData.SysMemPitch = 1920;                     // Pitch for Y plane
	initData.SysMemSlicePitch = 0;                   // Only one slice in 2D textures

	// Create the NV12 texture
	HRESULT hr = device->CreateTexture2D(&textureDesc, &initData, texture);
	if (FAILED(hr)) {
		std::cerr << "Failed to create NV12 texture. HRESULT: " << hr << std::endl;
		return hr;
	}

	std::cout << "NV12 texture created successfully!" << std::endl;
	return S_OK;
}




void StreamCodec::initializeEncoder()
{
	backPacket = av_packet_alloc();

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

	// Create a D3D11 device
	ID3D11Device* d3d11_device = nullptr;
	ID3D11DeviceContext* d3d11_device_context = nullptr;
	D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL feature_level_out;
	HRESULT hr = D3D11CreateDevice(
		nullptr,                      // Adapter (use default)
		D3D_DRIVER_TYPE_HARDWARE,     // Driver type
		nullptr,                      // No software rasterizer
		D3D11_CREATE_DEVICE_BGRA_SUPPORT, // Flags (required for video interop)
		feature_levels,               // Feature levels
		1,                            // Number of feature levels
		D3D11_SDK_VERSION,            // SDK version
		&d3d11_device,                // Output device
		&feature_level_out,           // Output feature level
		&d3d11_device_context         // Device context (can retrieve later)
	);
	if (FAILED(hr)) {
		fprintf(stderr, "Failed to create D3D11 device\n");
		exit(-1);
	}

	// create ffmpeg hardware device context
	AVBufferRef* hw_device_ctx = nullptr;

	if (av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_QSV, "d3d11", nullptr, 0) < 0) {
		fprintf(stderr, "Failed to create QSV hardware device context.\n");
		exit(-1);
	}

	AVBufferRef* hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx);
	if (!hw_frames_ref) {
		fprintf(stderr, "Failed to create QSV hardware frame context.\n");
		exit(-1);
	}

	AVHWFramesContext* hw_frames_ctx = (AVHWFramesContext*)hw_frames_ref->data;
	hw_frames_ctx->format = AV_PIX_FMT_QSV;  // Use the correct pixel format for your hardware device
	hw_frames_ctx->sw_format = AV_PIX_FMT_NV12;
	hw_frames_ctx->width = 1920;  // Frame width
	hw_frames_ctx->height = 1080; // Frame height
	hw_frames_ctx->device_ref = av_buffer_ref(hw_device_ctx);
	hw_frames_ctx->initial_pool_size = 20;


	encoderContext->bit_rate = 3000000;
	encoderContext->width = 1920;
	encoderContext->height = 1080;
	encoderContext->time_base.num = 1;
	encoderContext->time_base.den = 60;
	encoderContext->framerate.num = 60;
    encoderContext->framerate.den = 1;
	encoderContext->pix_fmt = AV_PIX_FMT_QSV;
	//encoderContext->max_b_frames = 2;
	//encoderContext->gop_size = 20;

	encoderContext->hw_device_ctx = av_buffer_ref(hw_device_ctx);
	encoderContext->hw_frames_ctx = av_buffer_ref(hw_frames_ref);

	if (av_hwdevice_ctx_init(encoderContext->hw_device_ctx) < 0) {
		fprintf(stderr, "Failed to initialize D3D12VA hardware device context.\n");
		exit(-1);
	}
	if (av_hwframe_ctx_init(encoderContext->hw_frames_ctx) < 0) {
		fprintf(stderr, "Failed to initialize D3D12VA hardware frame context.\n");
		exit(-1);
	}

	av_opt_set(encoderContext->priv_data, "preset", "veryfast", 0);
	av_opt_set(encoderContext->priv_data, "crf", "26", 0);
	av_opt_set(encoderContext->priv_data, "tune", "zerolatency", 0);
	av_opt_set(encoderContext->priv_data, "forced_idr", "1", 0);

	int err = avcodec_open2(encoderContext, encoder, nullptr);
	if (err < 0) {
		std::cout << "Could not open codec" << std::endl;
		exit(1);
	}

	// Allocate a hardware-backed AVFrame
	AVFrame* frame = av_frame_alloc();
	frame->width = 1920;
	frame->height = 1080;
	frame->format = AV_PIX_FMT_QSV; // QSV format
	frame->hw_frames_ctx = av_buffer_ref(hw_frames_ref);

	if (av_hwframe_get_buffer(hw_frames_ref, frame, 0) < 0) {
		fprintf(stderr, "Failed to allocate HW frame\n");
		exit(-1);
	}
	
	// Create the NV12 texture
	ID3D11Texture2D* source_texture = nullptr;
	hr = CreateNV12Texture(d3d11_device, &source_texture);
	if (SUCCEEDED(hr)) {
		std::cout << "NV12 texture successfully created!" << std::endl;
	}

	// Copy your D3D11 texture into the QSV frame
	ID3D11Texture2D* hw_texture = (ID3D11Texture2D*)frame->data[0];
	d3d11_device_context->CopyResource(hw_texture, source_texture);
	d3d11_device_context->Flush(); // Ensure synchronization

	if (avcodec_send_frame(encoderContext, frame) < 0) {
		fprintf(stderr, "Error sending frame to encoder\n");
		exit(-1);
	}

	AVPacket pkt;
	av_init_packet(&pkt);

	while (avcodec_receive_packet(encoderContext, &pkt) == 0) {
		// Write encoded packet to file or stream
		av_packet_unref(&pkt);
	}

	std::cout << "Success" << std::endl;

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
	//AVFrame* frame = allocateFrame(pData);

	AVFrame* frame = av_frame_alloc();
	//frame->width = 1920;
	//frame->height = 1080;
	frame->format = AV_PIX_FMT_NV12;
	//frame->hw_frames_ctx = av_buffer_ref(encoderContext->hw_frames_ctx);

	// Obtain a hardware frame and map it to the GPU texture
	if (err = av_hwframe_get_buffer(encoderContext->hw_frames_ctx, frame, 0) < 0) {
		fprintf(stderr, "Failed to allocate hardware frame\n");
		return;
	}

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
		AVPacket* packet = av_packet_alloc();
		if (!packet) {
			qDebug() << "Could not allocate memory for packet";
			av_frame_free(&frame);
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
			char* errStr = new char;
			av_make_error_string(errStr, 255, err);
			qDebug() << errStr;
			av_frame_free(&frame);
			av_packet_free(&packet);
			exit(1);
		}

		emit encodeFinish(packet);
		frameCount++;
	}

	av_frame_free(&frame);
}

AVFrame* StreamCodec::allocateFrame(std::shared_ptr<UCHAR> pData)
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


