#pragma once

#include <Windows.Foundation.h>
#include <windows.devices.lights.h>
#include <windows.devices.lights.effects.h>
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

	static bool FindClassILampArrayEffectPlaylistStatics(
		Microsoft::WRL::ComPtr<ABI::Windows::Devices::Lights::Effects::ILampArrayEffectPlaylistStatics>& lampArrayEffectPlaylistStatics);

	static bool FindClassILampArrayCustomEffectFactory(
		Microsoft::WRL::ComPtr<ABI::Windows::Devices::Lights::Effects::ILampArrayCustomEffectFactory>& lampArrayCustomEffectFactory);

	static bool FindClassIColorHelperStatics(
		Microsoft::WRL::ComPtr<ABI::Windows::UI::IColorHelperStatics>& colorHelperStatics);
};
