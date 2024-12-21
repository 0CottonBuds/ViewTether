#pragma once

#include <iostream>

#include <QObject>
#include <d3d11.h>
#include "ScreenCapture/DisplayInformation.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class ScreenCapture : public QObject {
	Q_OBJECT

public slots:
	virtual HRESULT getFrame() = 0;
	virtual HRESULT Initialize() = 0;

signals:
	void frameReady(std::shared_ptr<UCHAR> pPixelData);
	void hwframeReady(ComPtr<ID3D11Texture2D> desktopTexture);
	void initializationFinished();

public:
	virtual DisplayInformationManager getDisplayInformationManager() = 0;
	virtual HRESULT changeDisplay(int providerIndex = 0, int displayIndex = 0) = 0;
	void setActive(bool state) { isActive = state; };
	int frameCount = 0;

protected: 
	DisplayInformationManager informationManager;
	bool isActive;
};
