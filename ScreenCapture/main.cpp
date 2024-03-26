#include "ScreenDupliactor.h"

using namespace std;

int main()
{
	ScreenDuplicator screenDuplicator = ScreenDuplicator();
	screenDuplicator.Initialize();

	UCHAR* pixelData = nullptr;
	HRESULT hr = screenDuplicator.getNextFrame(pixelData);

	cout << "program ended successfully" << endl;
	return 0;
}