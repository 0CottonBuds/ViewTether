#include "App.h"

using namespace std;
using namespace Ui;

int main(int argc, char **argv)
{
	//HRESULT hr;
	//ScreenDuplicator screenDuplicator = ScreenDuplicator();
	//screenDuplicator.Initialize();

	//for (int i = 0; i < 5; ++i) {
	//	UCHAR* test = nullptr;
	//	UINT n = 0;
	//	screenDuplicator.getNextFrame(&test, n);
	//	delete test;
	//}

	//UCHAR* pixelData = nullptr;
	//UINT pixelDataSize = 0;
	//hr = screenDuplicator.getNextFrame(&pixelData, pixelDataSize);
	//hr = screenDuplicator.processUCharFrame(&pixelData, pixelDataSize);

	App app = App(argc, argv);

	return 0;
}

