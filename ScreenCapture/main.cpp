#include "App.h"
#include "DisplayStreamServer.h"
#include "StreamCodec.h"

#pragma comment(lib, "strmiids.lib")

using namespace std;

int main(int argc, char **argv)
{
	StreamCodec sc(1080, 1980, 30);

	return 0;
}

