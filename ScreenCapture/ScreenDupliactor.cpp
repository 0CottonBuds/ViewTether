#include "ScreenDupliactor.h"

HRESULT ScreenDuplicator::Initialize()
{

    return E_NOTIMPL;
}

HRESULT ScreenDuplicator::initializeAdapters()
{
    return E_NOTIMPL;
}

HRESULT ScreenDuplicator::initializeAdapterDescription()
{
    return E_NOTIMPL;
}

HRESULT ScreenDuplicator::initualizeOutputs()
{
    return E_NOTIMPL;
}

HRESULT ScreenDuplicator::releaseMemory()
{
	try {
		pFactory->Release();
		pAdapter1->Release();
		pOutput->Release();
		pOutput1->Release();

		for (IDXGIAdapter1* adapter : vAdapters) {
			adapter->Release();
		}

		for (vector<IDXGIOutput1*> tvOutputs : vvOutputs) {
			for (IDXGIOutput1* tpOutput : tvOutputs) {
				tpOutput->Release();
			}
		}
		return S_OK;
	}
	catch(exception e) {
		cerr << e.what() << endl;
		cerr << "Failed to Release pointers" << endl;
		return E_FAIL;
	}
}
