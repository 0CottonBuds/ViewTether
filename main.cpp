#include <iostream>
//#include "App.h"


extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
	#include <libavutil/hwcontext.h>
	#include <libavutil/error.h>
	#include <libavutil/opt.h>
	#include <libavutil/avconfig.h>
}

#include <iostream>
#include <d3d11.h>
#include <d3d11_4.h>

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "d3d11.lib")

#include <d3d11.h>
#include <iostream>
#include <vector>

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


int main(int argc, char** argv){

	const AVCodec* encoder = avcodec_find_encoder_by_name("hevc_qsv");
	if (!encoder) {
		std::cout << "Codec not found" << std::endl;
		exit(1);
	}
	 
	AVCodecContext* encoderContext = avcodec_alloc_context3(encoder);
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
		return -1;
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
		return -1;
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

	while (true) {
		if (avcodec_send_frame(encoderContext, frame) < 0) {
			fprintf(stderr, "Error sending frame to encoder\n");
			return -1;
		}

		AVPacket pkt;
		av_init_packet(&pkt);

		while (avcodec_receive_packet(encoderContext, &pkt) == 0) {
			// Write encoded packet to file or stream
			av_packet_unref(&pkt);
		}
	}

	std::cout << "Success" << std::endl;

	//App app(argc, argv);
	return 0;
}



