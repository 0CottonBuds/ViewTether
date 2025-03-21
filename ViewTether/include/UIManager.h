#pragma once
#include <QObject>

#include "ui_MainWindow.h"
#include "ScreenCapture/DisplayInformation.h"
#include "VIdeoWidget.h"
#include "Windows.h"
#include <shellapi.h>

class UIManager : public QObject 
{
	Q_OBJECT
public:
	UIManager();
	UIManager(const UIManager&);
	~UIManager();

private:
	Ui_MainWidget* m_mainWidget; //from ui files
	VideoWidget* m_videoWidget;
	DisplayInformationManager m_displayInformationManager;

	bool isActive = false;

public slots:
	void initialize();
	void setVideoFrame(std::shared_ptr<unsigned char> pixelData);
	void streamSwitch();

	// only for display
	void setAddress(QString ip, QString port);
	void setConnectionStatus(bool status);
	void setDisplayInformation(DisplayInformationManager displayInformationManager);

signals:
	void activeStatusChanged(bool status);

	void fpsChanged(int newFps);
	void screenChanged(int adapterIndex, int outputIndex);

	void driverInstallClicked();
	void driverUninstallClicked();
	void addVirtualMonitorClicked();
	void removeVirtualMonitorClicked();

private:
	void initializeButtons();
	void initializeVideoWidget();
	void initializeComboBoxes();
};