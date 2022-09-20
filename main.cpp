// Ref: https://docs.microsoft.com/en-us/cpp/cppcx/wrl/how-to-activate-and-use-a-windows-runtime-component-using-wrl
//
// Win32_Cpp_WinRT.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.Foundation.h>
#include <windows.devices.lights.h>
#include <windows.devices.lights.effects.h>
#include <windows.devices.enumeration.h>
#include <windows.ui.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\client.h>
#include <wrl\event.h>
#include <stdio.h>
#include <ppltasks.h>
#include <map>
#include <winrt\base.h>
#include <chrono>
#include "UtilClassesWinRT.h"

using namespace concurrency;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Numerics;
using namespace ABI::Windows::Devices::Enumeration;
using namespace ABI::Windows::Devices::Lights;
using namespace ABI::Windows::Devices::Lights::Effects;
using namespace ABI::Windows::UI;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace std;
using namespace std::chrono;
using namespace winrt;

#pragma comment(lib, "windowsapp")

map<wstring, wstring> _gIdNameMap;

void GetDeviceInformation(ComPtr<IDeviceInformationStatics>& deviceInformationStatics,
	const HString& id,
	IDeviceInformation** deviceInformation)
{
	IAsyncOperation<DeviceInformation*>* operation;
	deviceInformationStatics->CreateFromIdAsync(id, &operation);
	task<void> task([operation, &deviceInformation]()
		{
			HRESULT hr;
			do
			{
				WaitForSingleObjectEx(::GetCurrentThread(), 30, FALSE); //change to concurrency::wait()
				hr = operation->GetResults(deviceInformation);
			} while (FAILED(hr));
		});
	task.get();
}

void GetLampArray(ComPtr<ILampArrayStatics>& lampArrayStatics,
	const HString& id,
	ILampArray** lampArray)
{
	IAsyncOperation<LampArray*>* operation;
	lampArrayStatics->FromIdAsync(id, &operation);
	task<void> task([operation, &lampArray]()
		{
			HRESULT hr;
			do
			{
				WaitForSingleObjectEx(::GetCurrentThread(), 30, FALSE); //change to concurrency::wait()
				hr = operation->GetResults(lampArray);
			} while (FAILED(hr));
		});
	task.get();
}

void SetAllDevicesToColors(
	ComPtr<IDeviceInformationStatics> deviceInformationStatics,
	ComPtr<ILampArrayStatics> lampArrayStatics,
	ComPtr<ILampArrayCustomEffectFactory> lampArrayCustomEffectFactory,
	ComPtr<ILampArrayEffectPlaylistStatics> lampArrayEffectPlaylistStatics,
	ComPtr<IColorHelperStatics> colorHelperStatics)
{

#pragma region Create colors

	Color colorClear;
	if (FAILED(colorHelperStatics->FromArgb(0, 0, 0, 0, &colorClear)))
	{
		fwprintf_s(stderr, L"Failed to create clear color!\n");
		return;
	}

	Color colorWhite;
	if (FAILED(colorHelperStatics->FromArgb(255, 255, 255, 255, &colorWhite)))
	{
		fwprintf_s(stderr, L"Failed to create white color!\n");
		return;
	}

	Color colorRed;
	if (FAILED(colorHelperStatics->FromArgb(255, 255, 0, 0, &colorRed)))
	{
		fwprintf_s(stderr, L"Failed to create red color!\n");
		return;
	}

#pragma endregion Create colors

	// loop through all the connected lighting devices

	INT32 lampArrayIndex = 0;
	for (map<wstring, wstring>::iterator it = _gIdNameMap.begin(); it != _gIdNameMap.end(); ++it, ++lampArrayIndex)
	{
		wstring wId = it->first;
		wstring wName = it->second;

		wprintf_s(L"Device %d id=%s name=%s...\n", lampArrayIndex, wId.c_str(), wName.c_str());

		hstring hId{ wId.c_str() };
		HString id;
		HRESULT hrCopy = id.Set(wId.c_str());
		if (FAILED(hrCopy))
		{
			fwprintf_s(stderr, L"Failed to copy device id=%s\n", wId.c_str());
			continue;
		}

#pragma region Get DeviceInformation

		IDeviceInformation* deviceInformation = nullptr;
		GetDeviceInformation(deviceInformationStatics,
			id,
			&deviceInformation);

		if (!deviceInformation)
		{
			continue;
		}

#pragma endregion Get DeviceInformation


#pragma region Get LampArray

		ILampArray* lampArray = nullptr;
		GetLampArray(lampArrayStatics,
			id,
			&lampArray);

		if (!lampArray)
		{
			continue;
		}

#pragma endregion Get LampArray		

		INT32 lampCount;
		lampArray->get_LampCount(&lampCount);

		wprintf_s(L"Added device: name=%s lampCount=%d id=%s\n",
			wName.c_str(),
			lampCount,
			wId.c_str());

#pragma region Get lamp info positions

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
		}

#pragma endregion Get lamp info positions

#pragma region Prepare lamp indices

		vector<INT32> lampIndices;
		for (INT32 lamp = 0; lamp < lampCount; ++lamp)
		{
			lampIndices.push_back(lamp);
		}

#pragma endregion Prepare lamp indices

#pragma region Show color on the device

		if (FAILED(lampArray->SetColor(colorWhite)))
		{
			fwprintf_s(stderr, L"Failed to set color: name=%s lampCount=%d\n", wName.c_str(), lampCount);
		}

		if (FAILED(lampArray->SetSingleColorForIndices(colorWhite, lampCount, &lampIndices[0])))
		{
			fwprintf_s(stderr, L"Failed to set color for indices: name=%s lampCount=%d\n", wName.c_str(), lampCount);
		}

#pragma endregion Show color on the device

#pragma region Create Playlist

		ComPtr<ILampArrayEffectPlaylist> lampArrayEffectPlaylist;
		if (!UtilClassesWinRT::ActivateInstanceILampArrayEffectPlaylist(lampArrayEffectPlaylist))
		{
			fwprintf_s(stderr, L"Failed to create playlist: name=%s lampCount=%d\n", wName.c_str(), lampCount);
			continue;
		}

#pragma endregion Create Playlist

#pragma region Set Playlist start mode

		HRESULT hrStartMode = lampArrayEffectPlaylist->put_EffectStartMode(LampArrayEffectStartMode::LampArrayEffectStartMode_Simultaneous);
		if (FAILED(hrStartMode))
		{
			fwprintf_s(stderr, L"Failed to set playlist start mode: name=%s lampCount=%d\n", wName.c_str(), lampCount);
			continue;
		}

#pragma endregion Set Playlist start mode

#pragma region Create Custom Effect

		// Create custom effect
		ILampArrayCustomEffect* customEffect;
		HRESULT hrCustomEffect = lampArrayCustomEffectFactory->CreateInstance(lampArray, lampCount, &lampIndices[0], &customEffect);
		if (FAILED(hrCustomEffect))
		{
			fwprintf_s(stderr, L"Failed to create custom effect name=%s!\n", wName.c_str());
			continue;
		}

#pragma endregion Create Custom Effect

#pragma region Set Effect Duration

		TimeSpan effectDuration;
		effectDuration.Duration = INT64_MAX; // Same as TimeSpan.MaxValue
		if (FAILED(customEffect->put_Duration(effectDuration)))
		{
			fwprintf_s(stderr, L"Failed to set custom effect duration!\n");
			continue;
		}

#pragma endregion Set Effect Duration

#pragma region Set Effect Update Interval

		TimeSpan updateInterval;
		// duration: A time period expressed in 100-nanosecond units.
		updateInterval.Duration = std::chrono::milliseconds{ 1000 }.count() * 10000; // multiply by 10000 to get nanoseconds.
		if (FAILED(customEffect->put_UpdateInterval(updateInterval)))
		{
			fwprintf_s(stderr, L"Failed to set custom effect update interval !\n");
			continue;
		}

#pragma endregion Set Effect Update Interval

#pragma region Assign UpdateRequested event

		EventRegistrationToken updatedToken;

		// Type define the event handler types to make the code more readable.
		typedef ITypedEventHandler<LampArrayCustomEffect*, LampArrayUpdateRequestedEventArgs*> UpdateHandler;

		HRESULT hrAddUpdate = customEffect->add_UpdateRequested(Callback<UpdateHandler>([lampArrayIndex, lampArrayEffectPlaylist, customEffect, lampCount, colorClear, colorRed, wName](ILampArrayCustomEffect* customEffect, ILampArrayUpdateRequestedEventArgs* args) -> HRESULT
			{
				if (FAILED(args->SetColor(colorClear)))
				{
					fwprintf_s(stderr, L"Failed to set clear color: name=%s lampCount=%d\n", wName.c_str(), lampCount);
				}
				//wprintf_s(L"Set clear color: name=%s lampCount=%d\n", wName.c_str(), lampCount);

				for (INT32 lamp = 0; lamp < lampCount; ++lamp)
				{
					HRESULT hrSetColor = args->SetColorForIndex(lamp, colorRed);
					if (FAILED(hrSetColor))
					{
						fwprintf_s(stderr, L"Failed to set color: name=%s lamp=%d\n", wName.c_str(), lamp);
					}
					if (lamp == 0) // log just the first element per device
					{
						wprintf_s(L"Device %d: set red color: name=%s lamp=%d of %d\n", lampArrayIndex, wName.c_str(), lamp, lampCount);
					}
				}

				return S_OK;

			}).Get(), &updatedToken);

		if (FAILED(hrAddUpdate)) {
			fwprintf_s(stderr, L"Failed to set UpdateRequested event: name=%s\n", wName.c_str());
			continue;
		}

#pragma endregion Assign UpdateRequested event

#pragma region Append to playlist

		HRESULT hrAppend = lampArrayEffectPlaylist->Append((ILampArrayEffect*)customEffect);
		if (FAILED(hrAppend))
		{
			fwprintf_s(stderr, L"Failed to append to playlist: name=%s\n", wName.c_str());
			continue;
		}
		wprintf_s(L"Appended to playlist: name=%s\n", wName.c_str());

#pragma endregion Append to playlist

#pragma region Start playlist

		HRESULT hrStart = lampArrayEffectPlaylist->Start();
		if (FAILED(hrStart))
		{
			fwprintf_s(stderr, L"Failed to start playlist: name=%s\n", wName.c_str());
			continue;
		}
		wprintf_s(L"Started playlist: name=%s\n", wName.c_str());

#pragma endregion Start playlist

	}
}

int main()
{

#pragma region Initialize WinRT

	RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	if (FAILED(initialize))
	{
		fwprintf_s(stderr, L"Failed to initialize WinRT! Line: %d Result: %ld\n", __LINE__, (HRESULT)initialize);
		return -1;
	}
	wprintf_s(L"WinRT initialzed!\n");

#pragma endregion Initialize WinRT

#pragma region Get WinRT classes

	ComPtr<IDeviceInformationStatics> deviceInformationStatics;
	if (!UtilClassesWinRT::FindClassIDeviceInformationStatics(deviceInformationStatics))
	{
		return -1;
	}

	ComPtr<ILampArrayStatics> lampArrayStatics;
	if (!UtilClassesWinRT::FindClassILampArrayStatics(lampArrayStatics))
	{
		return -1;
	}

	ComPtr<ILampArrayCustomEffectFactory> lampArrayCustomEffectFactory;
	if (!UtilClassesWinRT::FindClassILampArrayCustomEffectFactory(lampArrayCustomEffectFactory))
	{
		return -1;
	}

	ComPtr<ILampArrayEffectPlaylistStatics> lampArrayEffectPlaylistStatics;
	if (!UtilClassesWinRT::FindClassILampArrayEffectPlaylistStatics(lampArrayEffectPlaylistStatics))
	{
		return -1;
	}

	ComPtr<IColorHelperStatics> colorHelperStatics;
	if (!UtilClassesWinRT::FindClassIColorHelperStatics(colorHelperStatics))
	{
		return -1;
	}

#pragma endregion Get WinRT classes

	HRESULT hr;

#pragma region Get Device Selector

	HString deviceSelector;
	hr = lampArrayStatics->GetDeviceSelector(deviceSelector.GetAddressOf());
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

	// wait to detect connected devices
	Sleep(100);
	wprintf(L"Waited for devices.\n");

	// Set all detected lighting devices to a color
	SetAllDevicesToColors(
		deviceInformationStatics,
		lampArrayStatics,
		lampArrayCustomEffectFactory,
		lampArrayEffectPlaylistStatics,
		colorHelperStatics);

	// wait for events before exiting
	Sleep(5000);

	return 0;
}
