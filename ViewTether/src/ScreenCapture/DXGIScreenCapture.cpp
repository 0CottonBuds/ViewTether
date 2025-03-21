#include "ScreenCapture/DXGIScreenCapture.h"

DXGIScreenCapture::DXGIScreenCapture()
{
}

DXGIScreenCapture::~DXGIScreenCapture()
{
	destroyD3D11();
}

HRESULT DXGIScreenCapture::Initialize()
{
	setupFrameTimer();

	destroyD3D11();
	HRESULT hr;
	if (FAILED(hr = initializeFactory()))
		return hr;
	if (FAILED(hr = initializeAdapters()))
		return hr;
	if (FAILED(hr = initializeAdapterDescription()))
		return hr;
	if (FAILED(hr = initualizeOutputs()))
		return hr;
	if (FAILED(hr = initializeDisplayInformationManager())) 
		return hr;
	if (FAILED(hr = initializeD3D11Device()))
		return hr;
	if (FAILED(hr = changeScreen()))
		return hr;
	emit initializationFinished();
	emit displayInformationReady(informationManager);
	return S_OK;
}

HRESULT DXGIScreenCapture::getFrame()
{
	if (!isActive)
		return S_OK;

	if (m_outputDuplication == nullptr)
		return E_FAIL;

	HRESULT hr;
	ComPtr<IDXGIResource> desktopTextureResource = nullptr;
	ComPtr<ID3D11Texture2D> desktopTexture = nullptr;
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	D3D11_TEXTURE2D_DESC desktopTextureDesc;

	// Sometimes ActuireNextFrame() fails so we try until we get a frame. 
	while (true) {
		hr = m_outputDuplication->AcquireNextFrame(0 ,&frameInfo, &desktopTextureResource);

		if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
			emit frameReady(m_backFrame);
			return hr;
		}
		if (hr == DXGI_ERROR_ACCESS_LOST) {
			cerr << "Access Lost" << endl;
			return hr;
		}
		else if (frameInfo.LastPresentTime.QuadPart == 0) {
			emit frameReady(m_backFrame);
			continue;
		}
		break;
	}

	hr = desktopTextureResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&desktopTexture);
	if (FAILED(hr)) {
		std::cerr << "Failed to Query desktop texture" << std::endl;
		return hr;
	}
	desktopTexture->GetDesc(&desktopTextureDesc);

	// Create a CPU read write abled texture
	// using the desktop texture description but with extra flags
	ID3D11Texture2D* stagingTexture = nullptr;
	desktopTextureDesc.Usage = D3D11_USAGE_STAGING;
	desktopTextureDesc.BindFlags = 0;
	desktopTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	desktopTextureDesc.MiscFlags = 0;
	hr = m_device->CreateTexture2D(&desktopTextureDesc, nullptr, &stagingTexture);
	if (FAILED(hr)) {
		cerr << "Failed to create empty texture" << endl;
		return hr;
	}

	m_DeviceContext->CopyResource(stagingTexture, desktopTexture.Get());

	// Copy GPU Resource to CPU
	D3D11_MAPPED_SUBRESOURCE cpuResource;	
	hr = m_DeviceContext->Map(stagingTexture, 0, D3D11_MAP_READ, NULL, &cpuResource);
	if (FAILED(hr)) {
		cerr << "Failed to map desktop image to resource" << endl;
		return hr;
	}
	stagingTexture->Release();
	
	uint8_t* pixelDataBuffer = new uint8_t[cpuResource.DepthPitch];
	memcpy(pixelDataBuffer, cpuResource.pData, cpuResource.DepthPitch);
	shared_ptr<uint8_t> pixelData = shared_ptr<uint8_t>(pixelDataBuffer);

	m_backFrame.reset();
	m_backFrame = pixelData;

	frameCount++;
	emit frameReady(pixelData);

	m_outputDuplication->ReleaseFrame();
	return S_OK;
}

HRESULT DXGIScreenCapture::initializeFactory()
{	
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&m_factory);
	if (FAILED(hr)) {
		cerr << "Failed to initialize DXGI Factory 1" << endl;
		destroyD3D11();
		return hr;
	}
	return S_OK;
}

HRESULT DXGIScreenCapture::initializeAdapters()
{
	HRESULT hr;
	UINT i = 0;
	IDXGIAdapter1* tpAdapter;
	while (hr = m_factory->EnumAdapters1(i, &tpAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		m_adapters.push_back(tpAdapter);
		++i;
	}
	if (FAILED(hr)) {
		cerr << "enumarating adapters failed" << endl;
		destroyD3D11();
		return hr;
	}

	m_factory->Release();
	return S_OK;
}

HRESULT DXGIScreenCapture::initializeAdapterDescription()
{
	HRESULT hr;
	for (int i = 0; i < m_adapters.size(); i++) {
		hr = m_adapters[i]->GetDesc1(&m_adapterDesc);
		m_adapterDescs.push_back(m_adapterDesc);
		if (FAILED(hr)) {
			cerr << "Failed to get adapter " << i << " pdesktopTextureDescription" << endl;
			destroyD3D11();
			return hr;
		}
	}
	return S_OK;
}

HRESULT DXGIScreenCapture::initualizeOutputs()
{
	HRESULT hr;
	for (int i = 0; i < m_adapters.size(); i++) {
		UINT j = 0;
		IDXGIOutput* tpOutput = nullptr;
		IDXGIOutput1* tpOutput1 = nullptr;
		vector<IDXGIOutput1*> tvOutputs;
		while (hr = m_adapters[i]->EnumOutputs(j, &tpOutput) != DXGI_ERROR_NOT_FOUND) {
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
		m_outputs.push_back(tvOutputs);
	}

	return S_OK;
}

HRESULT DXGIScreenCapture::initializeDisplayInformationManager()
{
	for (int i = 0; i < m_adapterDescs.size(); i++) {
		DisplayProvider currProvider;

		DXGI_ADAPTER_DESC1 currAdapter = m_adapterDescs[i];
		wchar_t* desc = currAdapter.Description;
		wstring wstrDesc(desc);
		string strDesc(wstrDesc.begin(), wstrDesc.end());

		currProvider.name = strDesc.c_str();
		currProvider.desc = strDesc.c_str();
		vector<IDXGIOutput1*> currOutputs = m_outputs[i];
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

	hr = D3D11CreateDevice(m_adapters[0], D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, D3DFeatureLevel, 6, D3D11_SDK_VERSION, &m_device, &m_D3DFeatureLevel, &m_DeviceContext);
	if (FAILED(hr)) {
		cerr << "failed to initialize D3D device" << endl;
		destroyD3D11();
		return hr;
	}
	return S_OK;
}

HRESULT DXGIScreenCapture::changeScreen(int adapterIndex, int outputIndex)
{
	if (adapterIndex < 0 || outputIndex < 0) {
		return E_FAIL;
	}

	if (m_outputDuplication!= nullptr) {
		m_outputDuplication->ReleaseFrame();
		m_outputDuplication->Release();
	}
	
	HRESULT hr;
	hr = m_outputs[adapterIndex][outputIndex]->DuplicateOutput(m_device.Get(), &m_outputDuplication);
	if (FAILED(hr)) {
		cerr << "Failed to initialize Output Duplication" << endl;
		return hr;
	}

	try {
		m_outputDuplication->GetDesc(&m_outputDuplicationDesc);
	}
	catch (exception e) {
		cerr << "Failed to get Output Duplication Description" << endl;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT DXGIScreenCapture::destroyD3D11()
{
	try {
		if (m_factory != nullptr) {
			m_factory->Release();
			m_factory = nullptr;
		}
		if (m_device != nullptr) {
			m_device->Release();
			m_device = nullptr;
		}
		if (m_DeviceContext != nullptr) {
			m_DeviceContext->Release();
			m_DeviceContext = nullptr;
		}
		if (m_outputDuplication != nullptr) {
			m_outputDuplication->Release();
			m_outputDuplication = nullptr;
		}

		m_outputs.clear();
		m_adapterDescs.clear();
		m_adapters.clear();

		return S_OK;
	}
	catch(exception e) {
		cerr << e.what() << endl;
		cerr << "Failed to Release pointers" << endl;
		return E_FAIL;
	}
}

