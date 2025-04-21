#pragma once

#include <iostream>

#include <QObject>
#include <d3d11.h>
#include "ScreenCapture/DisplayInformation.h"
#include <wrl/client.h>
#include <QTimer>

using Microsoft::WRL::ComPtr;

class ScreenCapture : public QObject {
	Q_OBJECT

public slots:
	virtual HRESULT getFrame() = 0;
	virtual HRESULT Initialize() = 0;

	void setFrameRate(int newFps) { fps = newFps; frameTimer->setInterval(1000 / fps); };
	void setActive(bool state) { 
		isActive = state; 
		if (isActive) frameTimer->start();
		else frameTimer->stop();
	};
	void setupFrameTimer() { 
		frameTimer = new QTimer(this); 
		frameTimer->setInterval(1000 / fps); 
		connect(frameTimer, &QTimer::timeout, this, &ScreenCapture::getFrame); 
		frameTimer->stop(); 
	};

signals:
	void frameReady(std::shared_ptr<UCHAR> pPixelData);
	void hwframeReady(ComPtr<ID3D11Texture2D> desktopTexture);
	void initializationFinished();
	void displayInformationReady(DisplayInformationManager manager);

public:
	virtual DisplayInformationManager getDisplayInformationManager() = 0;
	virtual HRESULT changeScreen(int providerIndex = 0, int displayIndex = 0) = 0;

	QTimer* frameTimer;
	int fps = 15;
	int frameCount = 0;

protected: 
	DisplayInformationManager informationManager;
	bool isActive;
};
