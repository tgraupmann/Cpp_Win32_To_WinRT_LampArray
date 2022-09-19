#pragma once

#include <Windows.Foundation.h>
#include <windows.devices.lights.h>
#include <windows.devices.enumeration.h>
#include <windows.ui.h>
#include <wrl\client.h>

class StaticClassesWinRT
{
public:

	static bool FindClassIDeviceInformationStatics(
		Microsoft::WRL::ComPtr<ABI::Windows::Devices::Enumeration::IDeviceInformationStatics>& deviceInformationStatics);

	static bool FindClassILampArrayStatics(
		Microsoft::WRL::ComPtr<ABI::Windows::Devices::Lights::ILampArrayStatics>& lampArrayStatics);

	static bool FindClassIColorHelperStatics(
		Microsoft::WRL::ComPtr<ABI::Windows::UI::IColorHelperStatics>& colorHelperStatics);
};
