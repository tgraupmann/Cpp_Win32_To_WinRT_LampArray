#include "StaticClassesWinRT.h"

#include <Windows.Foundation.h>
#include <windows.devices.lights.h>
#include <windows.devices.enumeration.h>
#include <windows.ui.h>
#include <wrl\wrappers\corewrappers.h>
#include <stdio.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Devices::Enumeration;
using namespace ABI::Windows::Devices::Lights;
using namespace ABI::Windows::UI;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;


bool StaticClassesWinRT::FindClassIDeviceInformationStatics(ComPtr<IDeviceInformationStatics>& deviceInformationStatics)
{

#pragma region class IDeviceInformationStatics

	HRESULT hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Devices_Enumeration_DeviceInformation).Get(), &deviceInformationStatics);
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to get WinRT IDeviceInformationStatics class! Line: %d Result: %ld\n", __LINE__, hr);
		return false;
	}
	//wprintf_s(L"WinRT IDeviceInformationStatics class found!\n");
	return true;

#pragma endregion class IDeviceInformationStatics

}

bool StaticClassesWinRT::FindClassILampArrayStatics(ComPtr<ILampArrayStatics>& lampArrayStatics)
{

#pragma region class ILampArrayStatics

	HRESULT hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Devices_Lights_LampArray).Get(), &lampArrayStatics);
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to get WinRT ILampArrayStatics class! Line: %d Result: %ld\n", __LINE__, hr);
		return false;
	}
	//wprintf_s(L"WinRT ILampArrayStatics class found!\n");
	return true;

#pragma endregion class ILampArrayStatics

}

bool StaticClassesWinRT::FindClassIColorHelperStatics(ComPtr<IColorHelperStatics>& colorHelperStatics)
{

#pragma region class IColorHelperStatics

	HRESULT hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_UI_ColorHelper).Get(), &colorHelperStatics);
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to get WinRT IColorHelperStatics class! Line: %d Result: %ld\n", __LINE__, hr);
		return false;
	}
	//wprintf_s(L"WinRT IColorHelperStatics class found!\n");
	return true;

#pragma endregion class IColorHelperStatics

}
