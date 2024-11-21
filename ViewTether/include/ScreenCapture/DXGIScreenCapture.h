#pragma once
#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <iostream>
#include <vector>
#include <QObject>
#include <QImage>
#include <QMutexLocker>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3d11.lib")

using namespace std;
class DXGIScreenDuplicator : public QObject {
	Q_OBJECT
public slots:
	HRESULT getFrame();
	HRESULT Initialize();

signals:
	void frameReady(shared_ptr<UCHAR> pPixelData);
	void imageReady(shared_ptr<QImage> img);
	void initializationFinished();

public:
	DXGIScreenDuplicator();
	~DXGIScreenDuplicator();
	vector<DXGI_ADAPTER_DESC1> getAdapters(); 	
	vector<vector<IDXGIOutput1*>> getDisplays();
	HRESULT changeDisplay(int adapterIndex = 0, int displayIndex = 0);

	bool isActive = false;

private:
	shared_ptr<UCHAR> backFrame;

	IDXGIFactory2* pFactory = nullptr; 
	DXGI_ADAPTER_DESC1 adapterDesc;
	ID3D11Device* pDevice = nullptr;
	D3D_FEATURE_LEVEL featureLevel;
	IDXGIOutputDuplication* pOutputDuplication = nullptr;
	DXGI_OUTDUPL_DESC outputDuplicationDesc;
	ID3D11DeviceContext* pDeviceContext = nullptr;

	vector <IDXGIAdapter1*> vAdapters; // available adapters 
	vector <DXGI_ADAPTER_DESC1> vAdapterDesc; // available adapter descriptions
	vector<vector<IDXGIOutput1*>> vvOutputs; // available outputs for each adapter; [adapter index][output index] 

	HRESULT HR;
	HRESULT initializeFactory(); 
	HRESULT initializeAdapters();
	HRESULT initializeAdapterDescription();
	HRESULT initualizeOutputs();
	HRESULT initializeD3D11Device();
	HRESULT releaseMemory();
};

