#include <iostream>
#include "App.h"

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
    #include <libavutil/hwcontext.h>
	#include <libavutil/error.h>
	//#include <libavutil/hwcontext_qsv.h>
}

using namespace std;
int main(int argc, char** argv){

	// Initialize the libavcodec library
	av_log_set_level(AV_LOG_DEBUG);
	avformat_network_init();

	// Get the first registered hardware device type

	AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;

    std::cout << "Available hardware acceleration types:\n";
    while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
        std::cout << av_hwdevice_get_type_name(type) << std::endl;
    }

        // Get the first registered encoder
    const AVCodec* codec = nullptr;
    void* opaque = nullptr;

    while ((codec = av_codec_iterate(&opaque))) {
        // Check if the codec is an encoder
        if (av_codec_is_encoder(codec)) {
            std::cout << "Encoder: " << codec->name;
            if (codec->long_name) {
                std::cout << " (" << codec->long_name << ")";
            }
            std::cout << std::endl;
        }
    }

    AVBufferRef* hw_device_ctx = nullptr;

    AVHWDeviceType hwa = av_hwdevice_find_type_by_name("qsv");

    if (!hwa) {
        std::cerr << "Failed to find QSV HWA Device" << std::endl;
        return -1;
    }

    int err;
    if (err = av_hwdevice_ctx_create(&hw_device_ctx, hwa, nullptr, nullptr, 0) < 0) {
        std::cerr << "Failed to create QSV device context.\n";
        AVERROR(err);
        return err;
    }

    std::cout << "QSV device initialized successfully.\n";
    av_buffer_unref(&hw_device_ctx);  // Clean up after use

	//App app(argc, argv);
	return 0;
}



