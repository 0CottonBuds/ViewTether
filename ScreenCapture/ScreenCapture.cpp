#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <iostream>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3d11.lib")

IDXGIFactory1* pFactory = nullptr;
IDXGIAdapter1* pAdapter = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
IDXGIOutput* pOutput = nullptr;
IDXGIOutput1* pOutput1 = nullptr;

// Function to initialize DXGI and set up components for window duplication
HRESULT InitializeDXGI(HWND hWnd, IDXGIOutputDuplication** ppOutputDuplication) {
    HRESULT hr = S_OK;

    // Create DXGI Factory
    hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&pFactory);
    if (FAILED(hr)) {
        std::cerr << "Failed to create DXGI factory!" << std::endl;
        return hr;
    }

    // Get primary adapter
    hr = pFactory->EnumAdapters1(0, &pAdapter);
    if (FAILED(hr)) {
        std::cerr << "Failed to get primary adapter!" << std::endl;
        pFactory->Release();
        return hr;
    }

    // Get DXGI device
    hr = D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &pDevice, NULL, NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to create DXGI device!" << std::endl;
        pAdapter->Release();
        pFactory->Release();
        return hr;
    }

    // Get DXGI device context
    pDevice->GetImmediateContext(&pContext);

    // Get output
    hr = pAdapter->EnumOutputs(0, &pOutput);
    if (FAILED(hr)) {
        std::cerr << "Failed to get output!" << std::endl;
        pDevice->Release();
        pAdapter->Release();
        pFactory->Release();
        return hr;
    }

    hr = pOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&pOutput1);

    // Create duplication interface
    hr = pOutput1->DuplicateOutput(pDevice, ppOutputDuplication);
    if (FAILED(hr)) {
        std::cerr << "Failed to create output duplication!" << std::endl;
        pOutput->Release();
        pDevice->Release();
        pAdapter->Release();
        pFactory->Release();
        return hr;
    }

    // Clean up
    pOutput->Release();
    pDevice->Release();
    pAdapter->Release();
    pFactory->Release();

    return hr;
}

int main() {

    // Create window
    HWND hWnd = CreateWindow(L"STATIC", L"DXGI Window Duplication", WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, nullptr, nullptr, nullptr, nullptr);
    if (hWnd == nullptr) {
        std::cerr << "Failed to create window!" << std::endl;
        return 1;
    }

    // Initialize DXGI
    IDXGIOutputDuplication* pOutputDuplication = nullptr;
    HRESULT hr = InitializeDXGI(hWnd, &pOutputDuplication);
    if (FAILED(hr)) {
        DestroyWindow(hWnd);
        return 1;
    }

    // Get Frame
    UINT timeOutInMilliseconds = 100;
    IDXGIResource* pDesktopResoure = nullptr;
    DXGI_OUTDUPL_FRAME_INFO pFrameInfo;
    hr = pOutputDuplication->AcquireNextFrame(timeOutInMilliseconds, &pFrameInfo, &pDesktopResoure);
    if (FAILED(hr)) {
        std::cerr << "Failed to get Frame Data" << std::endl;
        pDesktopResoure->Release();
        pOutputDuplication->Release();
        DestroyWindow(hWnd);
        return 1;
    }

    ID3D11Texture2D* pDesktopTexture = nullptr;
    D3D11_TEXTURE2D_DESC desktopTextureDesc;

    hr = pDesktopResoure->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pDesktopTexture);
    if (FAILED(hr)) {
        std::cerr << "Failed to Query texture interface from desktop resource" << std::endl;
        pOutputDuplication->ReleaseFrame();
    }
    pDesktopTexture->GetDesc(&desktopTextureDesc);


    DXGI_MAPPED_RECT mappedResource = {};
    hr = pOutputDuplication->MapDesktopSurface(&mappedResource);
    if (FAILED(hr)) {
        std::cerr << "Failed to map desktop surface to mapped resource" << std::endl;
        pDesktopTexture->Release();
        pOutputDuplication->ReleaseFrame();
        return 1;
    }

    // unmap texture
    pOutputDuplication->UnMapDesktopSurface();
    pDesktopTexture->Release();
    pOutputDuplication->ReleaseFrame();

    // Release resources
    pOutputDuplication->Release();
    DestroyWindow(hWnd);

    return 0;
}
