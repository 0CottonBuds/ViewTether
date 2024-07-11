#include "DriverHelper.h"

#include <windows.h>
#include <setupapi.h>
#include <iostream>
#include <string>

#pragma comment(lib, "SetupAPI.lib")

bool DriverHelper::isAmyuniInstalled() {
    std::wstring driverName = L"USB Mobile Monitor Virtual Display";
    // Get a list of all installed devices
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get device list." << std::endl;
        return false;
    }

    // Enumerate through all devices in the list
    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD deviceIndex = 0;

    while (SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData)) {
        deviceIndex++;

        // Get the device description
        wchar_t deviceName[256];
        if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC,
            NULL, (PBYTE)deviceName, sizeof(deviceName), NULL)) {
            if (driverName == deviceName) {
                SetupDiDestroyDeviceInfoList(deviceInfoSet);
				std::cout << "Driver is installed." << std::endl;
                return true;
            }
        }
    }

    // Clean up
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
	std::cout << "Driver is not installed." << std::endl;
    return false;
}

void DriverHelper::installAmyuni() {
    try {
        system("usbmmidd_v2\\deviceinstaller64.exe install usbmmidd_v2\\usbmmidd.inf usbmmidd");
        std::cout << "successfully installed amyuni drivers";
    }
    catch(...){
        std::cerr << "failed to install amyuni";
    }
}

void DriverHelper::uninstallAmyuni()
{
    try {
        system("usbmmidd_v2\\deviceinstaller64.exe stop usbmmidd");
        system("usbmmidd_v2\\deviceinstaller64.exe remove usbmmidd");
        std::cout << "successfully uninstalled amyuni drivers";
    }
    catch(...){
        std::cerr << "failed to install amyuni";
    }
}

void DriverHelper::addVirtualScreen() {
	try {
        system("usbmmidd_v2\\deviceinstaller64.exe enableidd 1");
        std::cout << "successfully added a new virtual monitor";
        emit virtualScreenModified();
    }
    catch(...){
        std::cerr << "failed to install amyuni";
    }
}

void DriverHelper::removeVirtualScreen() {
	try {
        system("usbmmidd_v2\\deviceinstaller64.exe enableidd 0");
        std::cout << "successfully removed a new virtual monitor";
        emit virtualScreenModified();
    }
    catch(...){
        std::cerr << "failed to install amyuni";
    }
}

