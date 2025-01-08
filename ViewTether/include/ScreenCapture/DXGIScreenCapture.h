#pragma once

#include <iostream>
#include <vector>

#include <QObject>
#include <QImage>
#include <QMutexLocker>

#include "ScreenCapture/DisplayInformation.h"
#include"ScreenCapture/ScreenCapture.h"

#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3d11.lib")
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

using namespace std;
class DXGIScreenCapture : public ScreenCapture {
	Q_OBJECT

public:
	DXGIScreenCapture();
	~DXGIScreenCapture();

	DisplayInformationManager getDisplayInformationManager() override { return informationManager; }
	HRESULT changeDisplay(int providerIndex = 0, int displayIndex = 0) override;

public slots:
	HRESULT getFrame() override;
	HRESULT Initialize() override;

private:
	shared_ptr<uint8_t> m_backFrame;
	DisplayInformationManager informationManager;

	DXGI_ADAPTER_DESC1 m_adapterDesc;
	D3D_FEATURE_LEVEL m_D3DFeatureLevel;	
	ComPtr<ID3D11Device> m_device = nullptr;
	ComPtr<IDXGIFactory2> m_factory = nullptr; 

	DXGI_OUTDUPL_DESC m_outputDuplicationDesc;
	ComPtr<IDXGIOutputDuplication> m_outputDuplication = nullptr;

	ComPtr<ID3D11DeviceContext> m_DeviceContext = nullptr;

	vector <IDXGIAdapter1*> m_adapters; // available providers 
	vector <DXGI_ADAPTER_DESC1> m_adapterDescs; // available provider descriptions
	vector<vector<IDXGIOutput1*>> m_outputs; // available displays for each provider; [provider index][display index] 

private:
	HRESULT initializeFactory(); 
	HRESULT initializeAdapters();
	HRESULT initializeAdapterDescription();
	HRESULT initualizeOutputs();
	HRESULT initializeDisplayInformationManager();
	HRESULT initializeD3D11Device();
	HRESULT destroyD3D11();
};

