#include "App.h"
#include "StreamCodec.h"
#include "ScreenDupliactor.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#pragma comment(lib, "strmiids.lib")

using namespace std;

static void save_gray_frame(unsigned char *buf, int wrap, int xsize, int ysize, char *filename)
{
    FILE *f;
    int i;
    errno_t err = fopen_s(&f ,filename,"w");
    // writing the minimal required header for a pgm file format
    // portable graymap format -> https://en.wikipedia.org/wiki/Netpbm_format#PGM_example
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

    // writing line by line
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

int main(int argc, char **argv)
{
	// format = container; initializing container ehre
	AVFormatContext *pFormatContext = avformat_alloc_context();
	// open file
	avformat_open_input(&pFormatContext, "./Big Buck Bunny.mp4", NULL, NULL);
	//printing the format and duration
	printf("Format %s, duration %lld us", pFormatContext->iformat->long_name, pFormatContext->duration);

	// getting the streams
	avformat_find_stream_info(pFormatContext,  NULL);

	// loop through all the streams; access the streams with pFormatContext->streams[i]
	for (int i = 0; i < (int)pFormatContext->nb_streams; i++) {
		//AVCodecParameters contains the properties of codec used by the stream
		AVCodecParameters *pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
		const AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

		// specific for video and audio
		if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
			printf("Video Codec: resolution %d x %d", pLocalCodecParameters->width, pLocalCodecParameters->height);
		} else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
			printf("Audio Codec: %d channels, sample rate %d", pLocalCodecParameters->ch_layout.nb_channels, pLocalCodecParameters->sample_rate);
		}
		// general
		printf("\tCodec %s ID %d bit_rate %lld", pLocalCodec->long_name, pLocalCodec->id, pLocalCodecParameters->bit_rate);

		// initialize codec context with local codec and codec parameterss
		AVCodecContext *pCodecContext = avcodec_alloc_context3(pLocalCodec);
		avcodec_parameters_to_context(pCodecContext, pLocalCodecParameters);

		// after initialization open codec with codec context
		avcodec_open2(pCodecContext, pLocalCodec, NULL);

		// allocate memory for packets and frame
		AVPacket* pPacket = av_packet_alloc();
		AVFrame* pFrame = av_frame_alloc();

		while (av_read_frame(pFormatContext, pPacket) >= 0) {
			// send packet to codec context and get frame from codec context
			avcodec_send_packet(pCodecContext, pPacket);
			avcodec_receive_frame(pCodecContext, pFrame); 

//			printf(
//				"Frame %c (%d) pts %d dts %d key_frame %d\n",
//				av_get_picture_type_char(pFrame->pict_type),
//				pCodecContext->frame_num,
//				(int)pFrame->pts,
//				(int)pFrame->pkt_dts,
//				(int)AV_FRAME_FLAG_KEY
//				);
			save_gray_frame(pFrame->data[0], pFrame->linesize[0], pFrame->width, pFrame->height, (char*)"test.pgm");
		}
	}
	return 0;
}

