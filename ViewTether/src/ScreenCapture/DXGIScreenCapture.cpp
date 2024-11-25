#include "ScreenCapture/DXGIScreenCapture.h"

DXGIScreenCapture::DXGIScreenCapture()
{
}

DXGIScreenCapture::~DXGIScreenCapture()
{
	releaseMemory();
}

HRESULT DXGIScreenCapture::Initialize()
{
	releaseMemory();
	HRESULT hr;
	if (FAILED(hr = initializeFactory()))
		return hr;
	if (FAILED(hr = initializeAdapters()))
		return hr;
	if (FAILED(hr = initializeAdapterDescription()))
		return hr;
	if (FAILED(hr = initualizeOutputs()))
		return hr;
	if (FAILED(hr = initializeDisplayInformationManager())) {
		return hr;
	}
	if (FAILED(hr = initializeD3D11Device()))
		return hr;
	if (FAILED(hr = changeDisplay()))
		return hr;
	emit initializationFinished();
	return S_OK;
}

HRESULT DXGIScreenCapture::getFrame()
{
	if (!isActive)
		return S_OK;

	if (pOutputDuplication == nullptr)
		return E_FAIL;

	HRESULT hr;
	IDXGIResource* pDesktopResource = nullptr;
	ID3D11Texture2D* pDesktopTexture = nullptr;
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	D3D11_TEXTURE2D_DESC desktopTextureDesc;

	// release frame incase previous frame is still there;
	pOutputDuplication->ReleaseFrame();

	frameCount++;

	// Sometimes ActuireNextFrame() fails so we try until we get a frame. 
	while (true) {
		hr = pOutputDuplication->AcquireNextFrame(0 ,&frameInfo, &pDesktopResource);
		if (FAILED(hr)) {
			emit frameReady(backFrame);
			return hr;
		}
		if (frameInfo.LastPresentTime.QuadPart == 0) {
			emit frameReady(backFrame);
			pDesktopResource->Release();
			hr = pOutputDuplication->ReleaseFrame();
			continue;
		}
		break;
	}

	// Get desktop texture 
	hr = pDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pDesktopTexture);
	if (FAILED(hr)) {
		std::cerr << "Failed to Query desktop texture" << std::endl;
		pOutputDuplication->ReleaseFrame();
	}
	pDesktopTexture->GetDesc(&desktopTextureDesc);


	// create an empty texture that has CPU read and write access.
	ID3D11Texture2D* pCPUTexture = nullptr;
	desktopTextureDesc.Usage = D3D11_USAGE_STAGING;
	desktopTextureDesc.BindFlags = 0;
	desktopTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	desktopTextureDesc.MiscFlags = 0;
	hr = pDevice->CreateTexture2D(&desktopTextureDesc, nullptr, &pCPUTexture);
	if (FAILED(hr)) {
		cerr << "Failed to create empty texture" << endl;
		releaseMemory();
		return hr;
	}

	// Copy texture into CPU read write abled texture
	pDeviceContext->CopyResource(pCPUTexture, pDesktopTexture);
	pDesktopTexture->Release();

	// Copy GPU Resource to CPU
	D3D11_MAPPED_SUBRESOURCE resource;
	UINT subresource = D3D11CalcSubresource(0, 0, 0);
	hr = pDeviceContext->Map(pCPUTexture, subresource, D3D11_MAP_READ_WRITE, NULL, &resource);
	if (FAILED(hr)) {
		cerr << "Failed to map desktop image to resource" << endl;
		return hr;
	}
	pDeviceContext->Unmap(pCPUTexture, subresource);
	pCPUTexture->Release();
	
	// Copy from texture to byte array buffer.
	BYTE* pBytePixelDataBuffer = new BYTE[resource.DepthPitch];
	memcpy(pBytePixelDataBuffer, resource.pData, resource.DepthPitch);

	shared_ptr<UCHAR> pPixelData = shared_ptr<UCHAR>(pBytePixelDataBuffer);

	backFrame.reset();
	backFrame = pPixelData;

	emit frameReady(pPixelData);


	return S_OK;
}

HRESULT DXGIScreenCapture::initializeFactory()
{	
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&pFactory);
	if (FAILED(hr)) {
		cerr << "Failed to initialize DXGI Factory 1" << endl;
		releaseMemory();
		return hr;
	}
	return S_OK;
}

HRESULT DXGIScreenCapture::initializeAdapters()
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

	pFactory->Release();
	return S_OK;
}

HRESULT DXGIScreenCapture::initializeAdapterDescription()
{
	HRESULT hr;
	for (int i = 0; i < vAdapters.size(); i++) {
		hr = vAdapters[i]->GetDesc1(&adapterDesc);
		vAdapterDesc.push_back(adapterDesc);
		if (FAILED(hr)) {
			cerr << "Failed to get adapter " << i << " pdesktopTextureDescription" << endl;
			releaseMemory();
			return hr;
		}
	}
	return S_OK;
}

HRESULT DXGIScreenCapture::initualizeOutputs()
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
		if(tpOutput != nullptr)
			tpOutput->Release();
		if(tpOutput1 != nullptr)
			tpOutput1->Release();
		vvOutputs.push_back(tvOutputs);
	}

	return S_OK;
}

HRESULT DXGIScreenCapture::initializeDisplayInformationManager()
{
	for (int i = 0; i < vAdapterDesc.size(); i++) {
		DisplayProvider currProvider;

		DXGI_ADAPTER_DESC1 currAdapter = vAdapterDesc[i];
		wchar_t* desc = currAdapter.Description;
		wstring wstrDesc(desc);
		string strDesc(wstrDesc.begin(), wstrDesc.end());

		currProvider.name = strDesc.c_str();
		currProvider.desc = strDesc.c_str();
		vector<IDXGIOutput1*> currOutputs = vvOutputs[i];
		for (int j = 0; i < currOutputs.size(); i++) {
			DisplayInformation currDisplayProvider;

			currDisplayProvider.name = to_string(j);
			currDisplayProvider.desc = to_string(j);
			currProvider.displayInformations.push_back(currDisplayProvider);

		}

		informationManager.addDisplayProvers(currProvider);
	}

	return S_OK;

}

HRESULT DXGIScreenCapture::initializeD3D11Device()
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

HRESULT DXGIScreenCapture::changeDisplay(int adapterIndex, int outputIndex)
{
	if (pOutputDuplication != nullptr) {
		pOutputDuplication->ReleaseFrame();
		pOutputDuplication->Release();
	}
	
	HRESULT hr;
	hr = vvOutputs[adapterIndex][outputIndex]->DuplicateOutput(pDevice, &pOutputDuplication);
	if (FAILED(hr)) {
		cerr << "Failed to initialize Output Duplication" << endl;
		return hr;
	}

	try {
		pOutputDuplication->GetDesc(&outputDuplicationDesc);
	}
	catch (exception e) {
		cerr << "Failed to get Output Duplication Description" << endl;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT DXGIScreenCapture::releaseMemory()
{
	try {
		if (pFactory != nullptr) {
			pFactory->Release();
			pFactory = nullptr;
		}
		if (pDevice != nullptr) {
			pDevice->Release();
			pDevice = nullptr;
		}
		if (pDeviceContext != nullptr) {
			pDeviceContext->Release();
			pDeviceContext = nullptr;
		}
		if (pOutputDuplication != nullptr) {
			pOutputDuplication->Release();
			pOutputDuplication = nullptr;
		}

		vvOutputs.clear();
		vAdapterDesc.clear();
		vAdapters.clear();

		return S_OK;
	}
	catch(exception e) {
		cerr << e.what() << endl;
		cerr << "Failed to Release pointers" << endl;
		return E_FAIL;
	}
}

