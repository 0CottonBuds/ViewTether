#include "ScreenDupliactor.h"

using namespace std;

int main()
{
	HRESULT hr;
	ScreenDuplicator screenDuplicator = ScreenDuplicator();
	screenDuplicator.Initialize();

	UCHAR* pixelData = nullptr;
	UINT pixelDataSize = 0;
	hr = screenDuplicator.getNextFrame(&pixelData, pixelDataSize);
	hr = screenDuplicator.processUCharFrame(&pixelData, pixelDataSize);

	cout << "program ended successfully" << endl;
	return 0;
}