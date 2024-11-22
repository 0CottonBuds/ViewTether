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

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3d11.lib")

using namespace std;
class DXGIScreenCapture : public ScreenCapture {
	Q_OBJECT

private:
	shared_ptr<UCHAR> backFrame;

	IDXGIFactory2* pFactory = nullptr; 
	DXGI_ADAPTER_DESC1 adapterDesc;
	ID3D11Device* pDevice = nullptr;
	D3D_FEATURE_LEVEL featureLevel;
	IDXGIOutputDuplication* pOutputDuplication = nullptr;
	DXGI_OUTDUPL_DESC outputDuplicationDesc;
	ID3D11DeviceContext* pDeviceContext = nullptr;

public slots:
	HRESULT getFrame() override;
	HRESULT Initialize() override;

public:
	DXGIScreenCapture();
	~DXGIScreenCapture();

	DisplayInformationManager getInformationManager() override { return informationManager; }
	HRESULT changeDisplay(int providerIndex = 0, int displayIndex = 0) override;

private:
	DisplayInformationManager informationManager;
	vector <IDXGIAdapter1*> vAdapters; // available adapters 
	vector <DXGI_ADAPTER_DESC1> vAdapterDesc; // available adapter descriptions
	vector<vector<IDXGIOutput1*>> vvOutputs; // available outputs for each adapter; [adapter index][output index] 

	HRESULT HR;
	HRESULT initializeFactory(); 
	HRESULT initializeAdapters();
	HRESULT initializeAdapterDescription();
	HRESULT initualizeOutputs();
	HRESULT initializeDisplayInformationManager();
	HRESULT initializeD3D11Device();
	HRESULT releaseMemory();
};

