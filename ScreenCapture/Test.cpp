#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3d11.lib")

using namespace std;

int main() {
	IDXGIFactory2* pFactory = nullptr;
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&pFactory);
	if (FAILED(hr)) {
		cerr << "Creating factory failed" << endl;
		return 1;
	}

	
	UINT i = 0; 
	IDXGIAdapter1 * pAdapter; 
	vector <IDXGIAdapter1*> vAdapters;
	while(hr = pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) 
	{ 
		vAdapters.push_back(pAdapter); 
		++i; 
	}
	if (FAILED(hr)) {
		cerr << "enumarating adapters failed" << endl;
		return 1;
	}

	DXGI_ADAPTER_DESC1 adapterDesc;
	vector <DXGI_ADAPTER_DESC1> vAdapterDesc;
	for (int i = 0; i < vAdapters.size(); i++) {
		hr = vAdapters[i]->GetDesc1(&adapterDesc);
		vAdapterDesc.push_back(adapterDesc);
		if (FAILED(hr)) {
			cerr << "Failed to get adapter " << i << " description" << endl;
			break;
		}
	}
	if (FAILED(hr)) {
		for (int i = 0; i < vAdapters.size(); i++) {
			vAdapters[i]->Release();
		}
		pFactory->Release();
	}

	cout << "program ended successfuly" << endl;
	return 0;
}