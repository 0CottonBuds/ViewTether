#pragma once

#include <iostream>

#include <QObject>
#include <d3d11.h>
#include "ScreenCapture/DisplayInformation.h"

class ScreenCapture : public QObject {
	Q_OBJECT

public slots:
	virtual HRESULT getFrame() = 0;
	virtual HRESULT Initialize() = 0;

signals:
	void frameReady(std::shared_ptr<UCHAR> pPixelData);
	void initializationFinished();

public:
	virtual DisplayInformationManager getInformationManager() = 0;
	virtual HRESULT changeDisplay(int providerIndex = 0, int displayIndex = 0) = 0;
	void setActive(bool state) { isActive = state; };
	int frameCount = 0;

protected: 
	DisplayInformationManager informationManager;
	bool isActive;
};
