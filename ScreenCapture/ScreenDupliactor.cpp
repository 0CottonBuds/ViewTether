#include "ScreenDupliactor.h"

HRESULT ScreenDuplicator::Initialize()
{
	HRESULT hr;
	if (FAILED(hr = initializeFactory()))
		return hr;
	if (FAILED(hr = initializeAdapters()))
		return hr;
	if (FAILED(hr = initializeAdapterDescription()))
		return hr;
	if (FAILED(hr = initualizeOutputs()))
		return hr;
	if (FAILED(hr = initializeD3D11Device()))
		return hr;
	if (FAILED(hr = initializeOutputDuplication()))
		return hr;
    return S_OK;
}

HRESULT ScreenDuplicator::initializeFactory()
{ 
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&pFactory);
	if (FAILED(hr)) {
		cerr << "Failed to initialize DXGI Factory 1" << endl;
		releaseMemory();
		return hr;
	}
	return S_OK;
}

HRESULT ScreenDuplicator::initializeAdapters()
{
	HRESULT hr;
	UINT i = 0;
	IDXGIAdapter1* tpAdapter;
	while (hr = pFactory->EnumAdapters1(i, &tpAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		vAdapters.push_back(tpAdapter);
		++i;
	}
	if (FAILED(hr)) {
		cerr << "enumarating adapters failed" << endl;
		releaseMemory();
		return hr;
	}
    return S_OK;
}

HRESULT ScreenDuplicator::initializeAdapterDescription()
{
	HRESULT hr;
	for (int i = 0; i < vAdapters.size(); i++) {
		hr = vAdapters[i]->GetDesc1(&adapterDesc);
		vAdapterDesc.push_back(adapterDesc);
		if (FAILED(hr)) {
			cerr << "Failed to get adapter " << i << " description" << endl;
			releaseMemory();
			return hr;
		}
	}
    return S_OK;
}

HRESULT ScreenDuplicator::initualizeOutputs()
{
	HRESULT hr;
	for (int i = 0; i < vAdapters.size(); i++) {
		UINT j = 0;
		IDXGIOutput* tpOutput = nullptr;
		IDXGIOutput1* tpOutput1 = nullptr;
		vector<IDXGIOutput1*> tvOutputs;
		while (hr = vAdapters[i]->EnumOutputs(j, &tpOutput) != DXGI_ERROR_NOT_FOUND) {
			hr = tpOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)& tpOutput1);
			if (FAILED(hr)) {
				cerr << "Failed to get " << i << "," << j << " output" << endl;
				return hr;
			}
			tvOutputs.push_back(tpOutput1);
			++j;
		}
		vvOutputs.push_back(tvOutputs);
	}

    return S_OK;
}

HRESULT ScreenDuplicator::initializeD3D11Device()
{
	HRESULT hr;

	D3D_FEATURE_LEVEL D3DFeatureLevel[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	hr = D3D11CreateDevice(vAdapters[0], D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, D3DFeatureLevel, 6, D3D11_SDK_VERSION, &pDevice, &featureLevel, &pDeviceContext);
	if (FAILED(hr)) {
		cerr << "failed to initialize D3D device" << endl;
		releaseMemory();
		return hr;
	}
	return S_OK;
}

HRESULT ScreenDuplicator::initializeOutputDuplication()
{
	HRESULT hr;
	hr = vvOutputs[0][0]->DuplicateOutput(pDevice, &pOutputDuplication);
	if (FAILED(hr)) {
		cerr << "Failed to initialize Output Duplication" << endl;
		return hr;
	}
	return S_OK;
}

HRESULT ScreenDuplicator::releaseMemory()
{
	try {
		if(pFactory != nullptr)
			pFactory->Release();
		if(pAdapter1 != nullptr)
			pAdapter1->Release();
		if(pOutput != nullptr)
			pOutput->Release();
		if(pOutput1 != nullptr)
			pOutput1->Release();
		if(pDevice != nullptr)
			pDevice->Release();
		if(pDeviceContext != nullptr)
			pDeviceContext->Release();

		for (IDXGIAdapter1* tAdapter : vAdapters) {
			if(tAdapter != nullptr)
				tAdapter->Release();
		}

		for (vector<IDXGIOutput1*> tvOutputs : vvOutputs) {
			for (IDXGIOutput1* tpOutput : tvOutputs) {
				if(tpOutput != nullptr)
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
