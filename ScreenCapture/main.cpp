#include "App.h"

using namespace std;
using namespace Ui;

int main(int argc, char **argv)
{
	//HRESULT hr;
	//ScreenDuplicator screenDuplicator = ScreenDuplicator();
	//screenDuplicator.Initialize();

	//UCHAR* pixelData = nullptr;
	//UINT pixelDataSize = 0;
	//hr = screenDuplicator.getNextFrame(&pixelData, pixelDataSize);
	//hr = screenDuplicator.processUCharFrame(&pixelData, pixelDataSize);

	App app = App();

	app.exec_ui(argc, argv);
	return 0;
}

