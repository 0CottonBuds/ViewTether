#pragma once

#include <iostream>
#include <vector>

#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11.h>

#include <QObject>
#include <QImage>
#include <QMutexLocker>

#include "ScreenCapture/DisplayInformation.h"
#include"ScreenCapture/ScreenCapture.h"

#include <wrl/client.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3d11.lib")

using Microsoft::WRL::ComPtr;

using namespace std;
class DXGIScreenCapture : public ScreenCapture {
	Q_OBJECT

private:
	HRESULT HR;
	shared_ptr<UCHAR> backFrame;

	IDXGIFactory2* pFactory = nullptr; 
	DXGI_ADAPTER_DESC1 adapterDesc;
	ID3D11Device* pDevice = nullptr;
	D3D_FEATURE_LEVEL featureLevel;
	IDXGIOutputDuplication* pOutputDuplication = nullptr;
	DXGI_OUTDUPL_DESC outputDuplicationDesc;
	ID3D11DeviceContext* pDeviceContext = nullptr;

	DisplayInformationManager informationManager;
	vector <IDXGIAdapter1*> vAdapters; // available providers 
	vector <DXGI_ADAPTER_DESC1> vAdapterDesc; // available provider descriptions
	vector<vector<IDXGIOutput1*>> vvOutputs; // available displays for each provider; [provider index][display index] 

public slots:
	HRESULT getFrame() override;
	HRESULT Initialize() override;

public:
	DXGIScreenCapture();
	~DXGIScreenCapture();

	DisplayInformationManager getDisplayInformationManager() override { return informationManager; }
	HRESULT changeDisplay(int providerIndex = 0, int displayIndex = 0) override;


private:
	HRESULT initializeFactory(); 
	HRESULT initializeAdapters();
	HRESULT initializeAdapterDescription();
	HRESULT initualizeOutputs();
	HRESULT initializeDisplayInformationManager();
	HRESULT initializeD3D11Device();
	HRESULT releaseMemory();
};

