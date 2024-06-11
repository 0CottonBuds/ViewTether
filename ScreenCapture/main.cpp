#include "App.h"
#include "StreamCodec.h"
#include "ScreenDupliactor.h"
#include "Test.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#pragma comment(lib, "strmiids.lib")

using namespace std;
int main(int argc, char **argv)
{
	Test t = Test(argc, argv);
	return 0;
}

