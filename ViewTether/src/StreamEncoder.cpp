#include "StreamEncoder.h"
#include "qdebug.h"

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
		initializeD3D11();
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
		qDebug() << "Failed to create NV12 texture. HRESULT: " << hr;
		return hr;
	}

	std::cout << "NV12 texture created successfully!";
	return S_OK;
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

	// create ffmpeg hardware device context
	AVBufferRef* hw_device_ctx = nullptr;

	if (av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_QSV, "d3d11", nullptr, 0) < 0) {
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
	hw_frames_ctx->sw_format = AV_PIX_FMT_NV12;
	hw_frames_ctx->width = width;  
	hw_frames_ctx->height = height; 
	hw_frames_ctx->device_ref = av_buffer_ref(hw_device_ctx);
	hw_frames_ctx->initial_pool_size = 20;


	encoderContext->bit_rate = bitrate;
	encoderContext->width = width;
	encoderContext->height = height;

	encoderContext->time_base.num = 1;
	encoderContext->time_base.den = fps;
    encoderContext->framerate.den = 1;
	encoderContext->framerate.num = fps;

	encoderContext->pix_fmt = AV_PIX_FMT_QSV;

	//encoderContext->max_b_frames = 2;
	//encoderContext->gop_size = 20;

	encoderContext->hw_device_ctx = av_buffer_ref(hw_device_ctx);
	encoderContext->hw_frames_ctx = av_buffer_ref(hw_frames_ref);

	if (av_hwdevice_ctx_init(encoderContext->hw_device_ctx) < 0) {
		fprintf(stderr, "Failed to initialize hardware device context.\n");
		exit(-1);
	}
	if (av_hwframe_ctx_init(encoderContext->hw_frames_ctx) < 0) {
		fprintf(stderr, "Failed to initialize hardware frame context.\n");
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
}

void StreamEncoder::initializeD3D11()
{
	// Create a D3D11 device
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
		&d3d11Device,                // Output device
		&feature_level_out,           // Output feature level
		&d3d11DeviceContext         // Device context (can retrieve later)
	);
	if (FAILED(hr)) {
		fprintf(stderr, "Failed to create D3D11 device\n");
		exit(-1);
	}

	hr = d3d11Device->QueryInterface(IID_PPV_ARGS(&d3d11VideoDevice));
	if (FAILED(hr)) {
		exit(-1);
	}

	d3d11ContentDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
	d3d11ContentDesc.InputWidth = width;
	d3d11ContentDesc.InputHeight = height;
	d3d11ContentDesc.OutputWidth = width;
	d3d11ContentDesc.OutputHeight = height;
	d3d11ContentDesc.Usage = D3D11_VIDEO_USAGE_PLAYBACK_NORMAL;

	hr = d3d11VideoDevice->CreateVideoProcessorEnumerator(&d3d11ContentDesc, &d3d11VideoProcessorEnumerator);
	if (FAILED(hr)) {
		exit(-1);
	}

	hr = d3d11VideoDevice->CreateVideoProcessor(d3d11VideoProcessorEnumerator.Get(), 0, &d3d11VideoProcessor);
	if (FAILED(hr)) {
		exit(-1);
	}

	D3D11_TEXTURE2D_DESC inputDesc = {};
	inputDesc.Width = width;
	inputDesc.Height = height;
	inputDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	inputDesc.Usage = D3D11_USAGE_DEFAULT;
	inputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	inputDesc.MipLevels = 1;
	inputDesc.ArraySize = 1;
	inputDesc.SampleDesc.Count = 1;

	hr = d3d11Device->CreateTexture2D(&inputDesc, nullptr, &d3d11InputTexture);
	if (FAILED(hr)) {
		exit(-1);
	}

	D3D11_TEXTURE2D_DESC outputDesc = {};
	outputDesc.Width = width;
	outputDesc.Height = height;
	outputDesc.Format = DXGI_FORMAT_NV12;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	outputDesc.MipLevels = 1;
	outputDesc.ArraySize = 1;
	outputDesc.SampleDesc.Count = 1;

	hr = d3d11Device->CreateTexture2D(&outputDesc, nullptr, &d3d11OutputTexture);
	if (FAILED(hr)) {
		exit(-1);
	}

	hr = d3d11DeviceContext->QueryInterface(IID_PPV_ARGS(&d3d11VideoContext));
	if (FAILED(hr)) {
		exit(-1);
	}

	D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC inputViewDesc = {};
	inputViewDesc.ViewDimension = D3D11_VPIV_DIMENSION_TEXTURE2D;
	inputViewDesc.Texture2D.MipSlice = 0;

	hr = d3d11VideoDevice->CreateVideoProcessorInputView(d3d11InputTexture.Get(), d3d11VideoProcessorEnumerator.Get(), &inputViewDesc, &d3d11InputView);
	if (FAILED(hr)) {
		exit(-1);
	}

	D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC outputViewDesc = {};
	outputViewDesc.ViewDimension = D3D11_VPOV_DIMENSION_TEXTURE2D;

	hr = d3d11VideoDevice->CreateVideoProcessorOutputView(d3d11OutputTexture.Get(), d3d11VideoProcessorEnumerator.Get(), &outputViewDesc, &d3d11OutputView);
	if (FAILED(hr)) {
		exit(-1);
	}

	d3d11VideoContext->VideoProcessorSetOutputTargetRect(d3d11VideoProcessor.Get(), TRUE, nullptr);

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
		exit(1);
	}

	testDecoderSwsContext = sws_getContext(width, height, AV_PIX_FMT_NV12, width, height, AV_PIX_FMT_BGRA, NULL, NULL, NULL, NULL);
	if (!testDecoderSwsContext) {
		qDebug() << "Could not allocate SWS Context";
		exit(1);
	}
}

// Converted texture is in d3d11OutputTexture variable
void StreamEncoder::convertBGRAtoNV12(ComPtr<ID3D11Texture2D> desktopTexture)
{
	d3d11DeviceContext->CopyResource(d3d11InputTexture.Get(), desktopTexture.Get());

	D3D11_VIDEO_PROCESSOR_STREAM stream = {};
	stream.Enable = TRUE;
	stream.pInputSurface = d3d11InputView.Get();

	HRESULT hr;

	// Perform the conversion
	hr = d3d11VideoContext->VideoProcessorBlt(d3d11VideoProcessor.Get(), d3d11OutputView.Get(), 0, 1, &stream);
	if (FAILED(hr)) {
		qDebug() << "Failed to perform video processor blit: 0x%x\n" << hr;
		exit(-1);
	}
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

void StreamEncoder::testD3D11Texture(ComPtr<ID3D11Texture2D> Texture)
{
	D3D11_TEXTURE2D_DESC desc;
	d3d11OutputTexture->GetDesc(&desc);

	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;

	ID3D11Texture2D* stagingTexture;
	HRESULT hr = d3d11Device->CreateTexture2D(&desc, nullptr, &stagingTexture);
	if (FAILED(hr)) {
		qDebug() << "Failed to create staging texture: 0x%x\n" << hr;
		exit(-1);
	}
	d3d11DeviceContext->CopyResource(stagingTexture, d3d11OutputTexture.Get());

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = d3d11DeviceContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);
	if (SUCCEEDED(hr)) {
		uint8_t* data = static_cast<uint8_t*>(mappedResource.pData);

		AVFrame* yuv_frame = av_frame_alloc();
		if (!yuv_frame) {
			std::cerr << "Could not allocate memory for YUV frame." << std::endl;
			return;
		}

		int y_size = width * height;
		int uv_size = (width / 2) * (height / 2);

		av_image_fill_arrays(yuv_frame->data, yuv_frame->linesize, data, AV_PIX_FMT_NV12, width, height, 1);

		AVFrame* rgba_frame = av_frame_alloc();
		if (!rgba_frame) {
			std::cerr << "Could not allocate memory for RGBA frame." << std::endl;
			return;
		}

		int rgba_size = width * height * 4;  
		uint8_t* rgba_data = (uint8_t*)av_malloc(rgba_size);
		av_image_fill_arrays(rgba_frame->data, rgba_frame->linesize, rgba_data, AV_PIX_FMT_RGBA, width, height, 1);

		SwsContext* sws_ctx = sws_getContext(width, height, AV_PIX_FMT_NV12, width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, nullptr, nullptr, nullptr);
		if (!sws_ctx) {
			std::cerr << "Could not initialize sws context." << std::endl;
			return;
		}

		sws_scale(sws_ctx, yuv_frame->data, yuv_frame->linesize, 0, height, rgba_frame->data, rgba_frame->linesize);

		emit frameReady(std::shared_ptr<UCHAR>(rgba_data, av_free));

		d3d11DeviceContext->Unmap(stagingTexture, 0);
		sws_freeContext(sws_ctx);
	}
	else {
		printf("Failed to map the staging texture: 0x%x\n", hr);
	}

	stagingTexture->Release();
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

		// this is commented our because we want to test if the packet is correct
		// and if we sent it to the server it handles the deletion of the packet
		// TODO: Make the packet a smart pointer

		//emit encodeFinish(packet);

		testPacket(packet);
	}

	av_frame_free(&frame);
}

void StreamEncoder::encodeHWFrame(ComPtr<ID3D11Texture2D> desktopTexture)
{
	AVFrame* frame = av_frame_alloc();
	//frame->width = width;
	//frame->height = height;
	//frame->format = AV_PIX_FMT_QSV;
	frame->hw_frames_ctx = encoderContext->hw_frames_ctx;

	if (av_hwframe_get_buffer(encoderContext->hw_frames_ctx, frame, 0) < 0) {
		fprintf(stderr, "Failed to allocate HW frame\n");
		exit(-1);
	}

	convertBGRAtoNV12(desktopTexture);

	ID3D11Texture2D* hw_texture = (ID3D11Texture2D*)frame->data[3];

	HRESULT hr;

	D3D11_TEXTURE2D_DESC outputDesc = {};
	outputDesc.Width = width;
	outputDesc.Height = height;
	outputDesc.Format = DXGI_FORMAT_NV12;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	outputDesc.MipLevels = 1;
	outputDesc.ArraySize = 1;
	outputDesc.SampleDesc.Count = 1;

	d3d11DeviceContext->Flush();
	hr = d3d11Device->CreateTexture2D(&outputDesc, nullptr, &hw_texture);
	if (FAILED(hr)) {
		qDebug() << "Failed to create NV12 texture";
		exit(-1);
	}

	d3d11DeviceContext->CopyResource(hw_texture, d3d11OutputTexture.Get());
	d3d11DeviceContext->Flush(); // Ensure synchronization

	int err = avcodec_send_frame(encoderContext, frame);
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

		// this is commented our because we want to test if the packet is correct
		// and if we sent it to the server it handles the deletion of the packet
		// TODO: Make the packet a smart pointer
		//emit encodeFinish(packet);

		testPacket(packet);
	}

	av_frame_free(&frame);
}

void StreamEncoder::testPacket(AVPacket* packet)
{
	int err = 0;

	err = avcodec_send_packet(testDecoderContext, packet);
	if (err < 0) {
		qDebug() << "Error sending packet to codec";
		char* errStr = new char;
		av_make_error_string(errStr, 255, err);
		qDebug() << errStr;
		exit(1);
	}

	AVFrame* frame;
	int response = 0;
	while (response >= 0) {
		frame = av_frame_alloc();
		response = avcodec_receive_frame(testDecoderContext, frame);

		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			break;
		}
		else if (response < 0) {
			qDebug() << "Error recieving frame from codec";
			char* errStr = new char;
			av_make_error_string(errStr, 255, err);
			qDebug() << errStr;
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
