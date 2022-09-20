#include "UtilClassesWinRT.h"

#include <Windows.Foundation.h>
#include <windows.devices.lights.h>
#include <windows.devices.lights.effects.h>
#include <windows.devices.enumeration.h>
#include <windows.ui.h>
#include <wrl\wrappers\corewrappers.h>
#include <stdio.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Devices::Enumeration;
using namespace ABI::Windows::Devices::Lights;
using namespace ABI::Windows::Devices::Lights::Effects;
using namespace ABI::Windows::UI;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;


bool UtilClassesWinRT::FindClassIDeviceInformationStatics(ComPtr<IDeviceInformationStatics>& deviceInformationStatics)
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

bool UtilClassesWinRT::FindClassILampArrayStatics(ComPtr<ILampArrayStatics>& lampArrayStatics)
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

bool UtilClassesWinRT::FindClassILampArrayEffectPlaylistStatics(ComPtr<ILampArrayEffectPlaylistStatics>& lampArrayEffectPlaylistStatics)
{

#pragma region class ILampArrayEffectPlaylistStatics

	HRESULT hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Devices_Lights_Effects_LampArrayEffectPlaylist).Get(), &lampArrayEffectPlaylistStatics);
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to get WinRT ILampArrayEffectPlaylistStatics class! Line: %d Result: %ld\n", __LINE__, hr);
		return false;
	}
	//wprintf_s(L"WinRT ILampArrayEffectPlaylistStatics class found!\n");
	return true;

#pragma endregion class ILampArrayEffectPlaylistStatics

}

bool UtilClassesWinRT::ActivateInstanceILampArrayEffectPlaylist(
	Microsoft::WRL::ComPtr<ABI::Windows::Devices::Lights::Effects::ILampArrayEffectPlaylist>& lampArrayEffectPlaylist)
{
#pragma region class ILampArrayEffectPlaylist

	HRESULT hr = ActivateInstance(HStringReference(RuntimeClass_Windows_Devices_Lights_Effects_LampArrayEffectPlaylist).Get(), &lampArrayEffectPlaylist);
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to instantiate WinRT ILampArrayEffectPlaylist class! Line: %d Result: %ld\n", __LINE__, hr);
		return false;
	}
	//wprintf_s(L"WinRT ILampArrayEffectPlaylist class found!\n");
	return true;

#pragma endregion class ILampArrayEffectPlaylist
}

bool UtilClassesWinRT::FindClassILampArrayCustomEffectFactory(ComPtr<ILampArrayCustomEffectFactory>& lampArrayCustomEffectFactory)
{

#pragma region class ILampArrayCustomEffectFactory

	HRESULT hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Devices_Lights_Effects_LampArrayCustomEffect).Get(), &lampArrayCustomEffectFactory);
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to get WinRT ILampArrayCustomEffectFactory class! Line: %d Result: %ld\n", __LINE__, hr);
		return false;
	}
	//wprintf_s(L"WinRT ILampArrayCustomEffectFactory class found!\n");
	return true;

#pragma endregion class ILampArrayCustomEffectFactory

}

bool UtilClassesWinRT::FindClassIColorHelperStatics(ComPtr<IColorHelperStatics>& colorHelperStatics)
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
