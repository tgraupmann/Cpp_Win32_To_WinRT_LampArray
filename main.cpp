// Ref: https://docs.microsoft.com/en-us/cpp/cppcx/wrl/how-to-activate-and-use-a-windows-runtime-component-using-wrl
//
// Win32_Cpp_WinRT.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.Foundation.h>
#include <windows.devices.lights.h>
#include <windows.devices.enumeration.h>
#include <windows.ui.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\client.h>
#include <wrl\event.h>
#include <stdio.h>
#include <ppltasks.h>
#include <map>
#include <winrt\base.h>
#include "StaticClassesWinRT.h"

using namespace concurrency;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Numerics;
using namespace ABI::Windows::Devices::Enumeration;
using namespace ABI::Windows::Devices::Lights;
using namespace ABI::Windows::UI;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace std;
using namespace winrt;

#pragma comment(lib, "windowsapp")

map<wstring, wstring> _gIdNameMap;

void SetAllDevicesToColors(
	ComPtr<IDeviceInformationStatics> deviceInformationStatics,
	ComPtr<ILampArrayStatics> lampArrayStatics,
	ComPtr<IColorHelperStatics> colorHelperStatics)
{
	INT32 index = 0;
	for (map<wstring, wstring>::iterator it = _gIdNameMap.begin(); it != _gIdNameMap.end(); ++it)
	{
		wstring wId = it->first;
		wstring wName = it->second;

		wprintf_s(L"Device %d id=%s name=%s...\n", index, wId.c_str(), wName.c_str());
		++index;

		hstring hId{ wId.c_str() };
		HString id;
		HRESULT hrCopy = id.Set(wId.c_str());
		if (FAILED(hrCopy))
		{
			fwprintf_s(stderr, L"Failed to copy device id=%s\n", wId.c_str());
			continue;
		}

		// convert to await task
		IAsyncOperation<DeviceInformation*>* opGetDeviceInformation;
		deviceInformationStatics->CreateFromIdAsync(id, &opGetDeviceInformation);
		IDeviceInformation* deviceInformation = nullptr;
		while (true)
		{
			HRESULT hr = opGetDeviceInformation->GetResults(&deviceInformation);
			if (SUCCEEDED(hr))
			{
				break;
			}
			Sleep(1);
		}

		if (!deviceInformation)
		{
			continue;
		}

		// convert to await task
		IAsyncOperation<LampArray*>* opGetLampArray;
		lampArrayStatics->FromIdAsync(id, &opGetLampArray);
		ILampArray* lampArray = nullptr;
		while (true)
		{
			HRESULT hrGetLamps = opGetLampArray->GetResults(&lampArray);

			if (SUCCEEDED(hrGetLamps))
			{
				break;
			}
			Sleep(1);
		}

		INT32 lampCount;
		lampArray->get_LampCount(&lampCount);

		wprintf_s(L"Added device: name=%s lampCount=%d id=%s\n",
			wName.c_str(),
			lampCount,
			wId.c_str());

		Color colorClear;
		Color colorRed;
		if (FAILED(colorHelperStatics->FromArgb(0, 0, 0, 0, &colorClear)) ||
			FAILED(colorHelperStatics->FromArgb(255, 255, 0, 0, &colorRed)))
		{
			fwprintf_s(stderr, L"Failed to create color!\n");
		}
		else
		{
			HRESULT hrSetColor = lampArray->SetColor(colorClear);
			if (FAILED(hrSetColor))
			{
				fwprintf_s(stderr, L"Failed to set clear color: name=%s lampCount=%d\n", wName.c_str(), lampCount);
			}
			else
			{
				wprintf_s(L"Set clear color: name=%s lampCount=%d\n", wName.c_str(), lampCount);

				for (INT32 lamp = 0; lamp < lampCount; ++lamp)
				{
					ILampInfo* lampInfo;
					HRESULT hrLampInfo = lampArray->GetLampInfo(lamp, &lampInfo);
					if (FAILED(hrLampInfo))
					{
						fwprintf_s(stderr, L"Failed to get lampInfo: name=%s lamp=%d\n", wName.c_str(), lamp);
					}
					else
					{
						Vector3 vector3;
						lampInfo->get_Position(&vector3);
						//wprintf_s(L"Lamp %d: position x=%f y=%f z=%f\n", lamp, vector3.X, vector3.Y, vector3.Z);
					}

					HRESULT hrSetColor = lampArray->SetColorForIndex(lamp, colorRed);
					if (FAILED(hrSetColor))
					{
						fwprintf_s(stderr, L"Failed to set color: name=%s lamp=%d\n", wName.c_str(), lamp);
					}
					else
					{
						wprintf_s(L"Set red color: name=%s lamp=%d\n", wName.c_str(), lamp);
					}
				}
			}
		}
	}
}

int main()
{
	// Initialize the Windows Runtime.
	RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	if (FAILED(initialize))
	{
		fwprintf_s(stderr, L"Failed to initialize WinRT! Line: %d Result: %ld\n", __LINE__, (HRESULT)initialize);
		return -1;
	}
	wprintf_s(L"WinRT initialzed!\n");

	ComPtr<IDeviceInformationStatics> deviceInformationStatics;
	if (!StaticClassesWinRT::FindClassIDeviceInformationStatics(deviceInformationStatics))
	{
		return -1;
	}

	ComPtr<ILampArrayStatics> lampArrayStatics;
	if (!StaticClassesWinRT::FindClassILampArrayStatics(lampArrayStatics))
	{
		return -1;
	}

	ComPtr<IColorHelperStatics> colorHelperStatics;
	if (!StaticClassesWinRT::FindClassIColorHelperStatics(colorHelperStatics))
	{
		return -1;
	}

#pragma region Get Device Selector

	HString deviceSelector;
	HRESULT hr = lampArrayStatics->GetDeviceSelector(deviceSelector.GetAddressOf());
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to get WinRT LampArray DeviceSelector! Line: %d Result: %ld\n", __LINE__, hr);
		return -1;
	}
	wprintf_s(L"WinRT LampArray DeviceSelector:%s!\n", deviceSelector.GetRawBuffer(nullptr));

#pragma endregion Get Device Selector

#pragma region Create Device Watcher

	ComPtr<IDeviceWatcher> deviceWatcher;
	hr = deviceInformationStatics->CreateWatcherAqsFilter(deviceSelector, &deviceWatcher);
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to CreateWatcher! Line: %d Result: %ld\n", __LINE__, hr);
		return -1;
	}
	wprintf_s(L"WinRT Created DeviceWatcher!\n");

#pragma endregion Create Device Watcher

	// Ref: WRL Events - https://learn.microsoft.com/en-us/cpp/cppcx/wrl/how-to-handle-events-using-wrl
	
#pragma region Assign Device Watcher Added Event

	EventRegistrationToken addedToken;

	// Type define the event handler types to make the code more readable.
	typedef ITypedEventHandler<DeviceWatcher*, DeviceInformation*> AddedHandler;

	hr = deviceWatcher->add_Added(Callback<AddedHandler>([](IDeviceWatcher* watcher, IDeviceInformation* deviceInformation) -> HRESULT
		{
			HString id;
			HRESULT hrGetId = deviceInformation->get_Id(id.GetAddressOf());

			if (FAILED(hrGetId))
			{
				fwprintf_s(stderr, L"Failed to get device id!");
			}
			else
			{
				wstring wId = id.GetRawBuffer(nullptr);

				HString name;
				wstring wName;
				HRESULT hrGetName = deviceInformation->get_Name(name.GetAddressOf());
				if (SUCCEEDED(hrGetName))
				{
					wName = name.GetRawBuffer(nullptr);
				}

				_gIdNameMap[wId] = wName;
				wprintf_s(L"Added device: id=%s name=%s\n", wId.c_str(), wName.c_str());
			}

			return S_OK;

		}).Get(), &addedToken);

#pragma endregion Assign Device Watcher Added Event

#pragma region Assign Device Watcher Removed Event

	EventRegistrationToken removedToken;

	// Type define the event handler types to make the code more readable.
	typedef ITypedEventHandler<DeviceWatcher*, DeviceInformationUpdate*> RemovedHandler;

	hr = deviceWatcher->add_Removed(Callback<RemovedHandler>([](IDeviceWatcher* watcher, IDeviceInformationUpdate* deviceInformation) -> HRESULT
		{
			HString id;
			HRESULT hr = deviceInformation->get_Id(id.GetAddressOf());
			if (SUCCEEDED(hr))
			{
				wstring wId = id.GetRawBuffer(nullptr);
				wprintf_s(L"Removed device: %s\n", wId.c_str());
				if (_gIdNameMap.find(wId) != _gIdNameMap.end())
				{
					_gIdNameMap.erase(wId);
				}
			}
			return S_OK;

		}).Get(), &removedToken);

#pragma endregion Assign Device Watcher Removed Event

	wprintf(L"Detect connected devices...\n");

#pragma region Start Device Watcher

	hr = deviceWatcher->Start();
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to Start DeviceWatcher! Line: %d Result: %ld\n", __LINE__, hr);
		return -1;
	}
	wprintf_s(L"WinRT Started DeviceWatcher!\n");

#pragma endregion Start Device Watcher

	Sleep(100); // wait to set colors
	wprintf(L"Waited for devices.\n");

	SetAllDevicesToColors(
		deviceInformationStatics,
		lampArrayStatics,
		colorHelperStatics);

	Sleep(5000); // wait for events before exiting

	return 0;
}
