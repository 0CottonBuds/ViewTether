#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <iostream>
#pragma comment(lib, "dxgi.lib")

int main()
{
	// Step 1: Create DXGI Factory
	IDXGIFactory* pFactory = nullptr;
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pFactory));
	if (FAILED(hr)) {
		// Handle error
	}

		// Step 2: Enumerate Adapters (Optional)
	IDXGIAdapter* pAdapter = nullptr;
	hr = pFactory->EnumAdapters(0, &pAdapter); // You may need to loop through adapters to choose the desired one
	if (FAILED(hr)) {
		// Handle error
	}

	// Step 3: Get DXGI Output
	IDXGIOutput* pOutput = nullptr;
	hr = pAdapter->EnumOutputs(0, &pOutput); // You may need to loop through outputs to choose the desired one
	if (FAILED(hr)) {
		// Handle error
	}

	IDXGIOutput1* pOutput1 = nullptr;
	hr = pOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&pOutput1)); // You may need to loop through outputs to choose the desired one
	if (FAILED(hr)) {
		// Handle error
	}

	// Step 4: Create Output Duplication
	IDXGIOutputDuplication* pDuplication = nullptr;
	hr = pOutput1->DuplicateOutput(pAdapter, &pDuplication);
	if (FAILED(hr)) {
		// Handle error
	}
		
}