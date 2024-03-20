#include "ScreenDupliactor.h"

using namespace std;

int main()
{
	ScreenDuplicator screenDuplicator = ScreenDuplicator();
	screenDuplicator.Initialize();

	HRESULT hr = screenDuplicator.getNextFrame();

	cout << "program ended successfully" << endl;
	return 0;
}