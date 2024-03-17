#pragma once
#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3d11.lib")

using namespace std;

class ScreenDuplicator {
public:


	HRESULT Initialize();

private:
	IDXGIFactory2* pFactory = nullptr; 
	IDXGIAdapter1* pAdapter1 = nullptr;
	DXGI_ADAPTER_DESC1 adapterDesc;
	IDXGIOutput* pOutput = nullptr;
	IDXGIOutput1* pOutput1 = nullptr;
	vector <IDXGIAdapter1*> vAdapters; // available adapters 
	vector <DXGI_ADAPTER_DESC1> vAdapterDesc; // available adapter descriptions
	vector<vector<IDXGIOutput1*>> vvOutputs; // available outputs for each adapter; [adapter index][output index] 

	HRESULT initializeAdapters();
	HRESULT initializeAdapterDescription();
	HRESULT initualizeOutputs();
	HRESULT releaseMemory();
	HRESULT HR;
};
