#include "ScreenDupliactor.h"

ScreenDuplicator::ScreenDuplicator()
{
}

ScreenDuplicator::~ScreenDuplicator()
{
	releaseMemory();
}

HRESULT ScreenDuplicator::Initialize()
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
	if (FAILED(hr = initializeD3D11Device()))
		return hr;
	if (FAILED(hr = initializeOutputDuplication()))
		return hr;
	emit finishInitialization();
	return S_OK;
}

HRESULT ScreenDuplicator::getFrame()
{
	if (pOutputDuplication == nullptr)
		return E_FAIL;

	HRESULT hr;
	IDXGIResource* pResource = nullptr;
	ID3D11Texture2D* pDesktopTexture = nullptr;
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	D3D11_TEXTURE2D_DESC desktopTextureDesc;

	// release frame incase previous frame is still there;
	pOutputDuplication->ReleaseFrame();

	// Sometimes ActuireNextFrame() fails so we try until we get a frame. 
	while (true) {
		hr = pOutputDuplication->AcquireNextFrame(500 ,&frameInfo, &pResource);
		if (FAILED(hr)) {
			cerr << "Failed to get next frame" << endl;
			return hr;
		}
		if (frameInfo.LastPresentTime.QuadPart == 0) {
			pResource->Release();
			hr = pOutputDuplication->ReleaseFrame();
			continue;
		}
		break;
	}

	hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pDesktopTexture);
	if (FAILED(hr)) {
		std::cerr << "Failed to Query texture interface from desktop resource" << std::endl;
		pOutputDuplication->ReleaseFrame();
	}
	pDesktopTexture->GetDesc(&desktopTextureDesc);
	pDesktopTexture->Release();

	// QueryInterface for ID3D11Texture2D
	ID3D11Texture2D* pAcquiredDesktopImage = nullptr;
	hr = pResource->QueryInterface(IID_PPV_ARGS(&pAcquiredDesktopImage));
	if (FAILED(hr)) {
		cerr << "Failed to Query interface for desktop texture" << endl;
		return hr;
	}
	pResource->Release();
	
	// create an empty texture that has CPU read and write access.
	ID3D11Texture2D* pDestImage = nullptr;
	desktopTextureDesc.Usage = D3D11_USAGE_STAGING;
	desktopTextureDesc.BindFlags = 0;
	desktopTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	desktopTextureDesc.MiscFlags = 0;
	hr = pDevice->CreateTexture2D(&desktopTextureDesc, nullptr, &pDestImage);
	if (FAILED(hr)) {
		cerr << "Failed to create empty texture" << endl;
		releaseMemory();
		return hr;
	}

	// Copy image into CPU read write abled texture
	pDeviceContext->CopyResource(pDestImage,pAcquiredDesktopImage);
	pAcquiredDesktopImage->Release();

	// Copy GPU Resource to CPU
	D3D11_MAPPED_SUBRESOURCE resource;
	UINT subresource = D3D11CalcSubresource(0, 0, 0);
	hr = pDeviceContext->Map(pDestImage, subresource, D3D11_MAP_READ_WRITE, NULL, &resource);
	if (FAILED(hr)) {
		cerr << "Failed to map desktop image to resource" << endl;
		return hr;
	}
	pDeviceContext->Unmap(pDestImage, subresource);
	pDestImage->Release();
	
	// Copy from texture to bitmap buffer.
	BYTE* pBytePixelDataBuffer = new BYTE[resource.DepthPitch];

	BYTE* pSourcePos = reinterpret_cast<BYTE*>(resource.pData);
	BYTE* pDestinationPos = pBytePixelDataBuffer;

	for (size_t h = 0; h < desktopTextureDesc.Height; ++h)
	{
		memcpy_s(pDestinationPos, resource.RowPitch, pSourcePos, resource.RowPitch);
		pSourcePos += resource.RowPitch;
		pDestinationPos += resource.RowPitch;
	}

	shared_ptr<UCHAR> pPixelData = shared_ptr<UCHAR>(pBytePixelDataBuffer);
	//out_pixelDataSize = resource.DepthPitch;

	QImage *img =  new QImage(pPixelData.get(), 1920, 1080, QImage::Format_RGBA8888);
	shared_ptr<QImage> rgbSwappedImg = shared_ptr<QImage>(new QImage(img->rgbSwapped()));
	delete img;

	emit frameReady(pPixelData);
	emit imageReady(rgbSwappedImg);

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

	pFactory->Release();
	return S_OK;
}

HRESULT ScreenDuplicator::initializeAdapterDescription()
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
		if(tpOutput != nullptr)
			tpOutput->Release();
		if(tpOutput1 != nullptr)
			tpOutput1->Release();
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

HRESULT ScreenDuplicator::initializeOutputDuplication(int adapterIndex, int outputIndex)
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

HRESULT ScreenDuplicator::releaseMemory()
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

vector<DXGI_ADAPTER_DESC1> ScreenDuplicator::getAdapters()
{
    return vAdapterDesc;
}

vector<vector<IDXGIOutput1*>> ScreenDuplicator::getOutputs()
{
    return vvOutputs;
}


