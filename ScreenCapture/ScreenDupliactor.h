#pragma once
#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <iostream>
#include <vector>
#include <QObject>
#include <QImage>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3d11.lib")

using namespace std;
class ScreenDuplicator {
public:
	ScreenDuplicator();
	~ScreenDuplicator();

	HRESULT getFrame(UCHAR ** out_ucharPixelData, UINT& out_pixelDataSize);

private:
	IDXGIFactory2* pFactory = nullptr; 
	DXGI_ADAPTER_DESC1 adapterDesc;
	ID3D11Device* pDevice = nullptr;
	D3D_FEATURE_LEVEL featureLevel;
	IDXGIOutputDuplication* pOutputDuplication;
	DXGI_OUTDUPL_DESC outputDuplicationDesc;
	ID3D11DeviceContext* pDeviceContext = nullptr;

	vector <IDXGIAdapter1*> vAdapters; // available adapters 
	vector <DXGI_ADAPTER_DESC1> vAdapterDesc; // available adapter descriptions
	vector<vector<IDXGIOutput1*>> vvOutputs; // available outputs for each adapter; [adapter index][output index] 

	HRESULT Initialize();
	HRESULT initializeFactory(); 
	HRESULT initializeAdapters();
	HRESULT initializeAdapterDescription();
	HRESULT initualizeOutputs();
	HRESULT initializeD3D11Device();
	HRESULT initializeOutputDuplication();
	HRESULT releaseMemory();
	HRESULT HR;
};

class QScreenDuplicatorWorker : public QObject {
	Q_OBJECT
public slots:
	void getFrame() {
		UCHAR* pPixelData = new UCHAR();
		UINT pixelDataSize = 0;
		screenDuplicator.getFrame(&pPixelData, pixelDataSize);
		emit frameReady(pPixelData);

		QImage *img =  new QImage(pPixelData, 1920, 1080, QImage::Format_RGBA8888);
		QImage *rgbSwappedImg = new QImage(img->rgbSwapped());
		delete img;

		emit imageReady(rgbSwappedImg);
		// TODO:: in the future when I use the  frame ready signal move the deletion of pPixelData there
		delete pPixelData;
	}

signals:
	void frameReady(UCHAR* pPixelData);
	void imageReady(QImage* img);

private:
	ScreenDuplicator screenDuplicator = ScreenDuplicator();
	UCHAR* ppixelData = nullptr;
};
