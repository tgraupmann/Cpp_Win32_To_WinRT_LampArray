// Ref: https://docs.microsoft.com/en-us/cpp/cppcx/wrl/how-to-activate-and-use-a-windows-runtime-component-using-wrl
//
// Win32_Cpp_WinRT.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.Foundation.h>
#include <windows.devices.lights.h>
#include <windows.devices.enumeration.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\client.h>
#include <wrl\event.h>
#include <stdio.h>
#include <ppltasks.h>
#include <map>

using namespace concurrency;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Devices::Enumeration;
using namespace ABI::Windows::Devices::Lights;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace std;

#pragma comment(lib, "windowsapp")


map<wstring, int> _gIdMap;
int _gIndex = 0;

int main()
{

#pragma region Initialize WinRT

	// Initialize the Windows Runtime.
	RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	if (FAILED(initialize))
	{
		fwprintf_s(stderr, L"Failed to initialize WinRT! Line: %d Result: %ld\n", __LINE__, (HRESULT)initialize);
		return -1;
	}
	wprintf_s(L"WinRT initialzed!\n");

#pragma endregion Initialize WinRT

#pragma region class ILampArrayStatics

	ComPtr<ILampArrayStatics> lampArrayStatics;
	HRESULT hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Devices_Lights_LampArray).Get(), &lampArrayStatics);
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to get WinRT ILampArrayStatics Class! Line: %d Result: %ld\n", __LINE__, hr);
		return -1;
	}
	wprintf_s(L"WinRT ILampArrayStatics Class found!\n");

#pragma endregion class ILampArrayStatics

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

#pragma region class IDeviceInformationStatics

	ComPtr<IDeviceInformationStatics> deviceInformationStatics;
	hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Devices_Enumeration_DeviceInformation).Get(), &deviceInformationStatics);
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to get WinRT IDeviceInformationStatics Class! Line: %d Result: %ld\n", __LINE__, hr);
		return -1;
	}
	wprintf_s(L"WinRT IDeviceInformationStatics Class found!\n");

#pragma endregion class IDeviceInformationStatics

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
	typedef __FITypedEventHandler_2_Windows__CDevices__CEnumeration__CDeviceWatcher_Windows__CDevices__CEnumeration__CDeviceInformation AddedHandler;

	hr = deviceWatcher->add_Added(Callback<AddedHandler>([lampArrayStatics](IDeviceWatcher* watcher, IDeviceInformation* deviceInformation) -> HRESULT
		{
			HString id;
			HRESULT hrGetId = deviceInformation->get_Id(id.GetAddressOf());

			HString name;
			HRESULT hrGetName = deviceInformation->get_Name(name.GetAddressOf());

			if (SUCCEEDED(hrGetId) && SUCCEEDED(hrGetName))
			{
				wstring wId = id.GetRawBuffer(nullptr);

				int index = 0;
				if (_gIdMap.find(wId) == _gIdMap.end())
				{
					index = _gIndex;
					++_gIndex;
					_gIdMap[wId] = index;
				}
				else
				{
					index = _gIdMap[wId];
				}

				IAsyncOperation<LampArray*>* operation;

				lampArrayStatics->FromIdAsync(id, &operation);

				ILampArray* lampArray;

				// convert to await task
				while (true)
				{
					HRESULT hrGetLamps = operation->GetResults(&lampArray);

					if (SUCCEEDED(hrGetLamps))
					{
						break;
					}
					Sleep(1);
				}

				INT32 lampCount;
				lampArray->get_LampCount(&lampCount);

				ILampInfo* lampInfo;
				HRESULT hrLampInfo = lampArray->GetLampInfo(index, &lampInfo);

				INT32 lampIndex;
				lampInfo->get_Index(&lampIndex);

				INT32 redCount;
				lampInfo->get_BlueLevelCount(&redCount);

				INT32 greenCount;
				lampInfo->get_BlueLevelCount(&greenCount);

				INT32 blueCount;
				lampInfo->get_BlueLevelCount(&blueCount);

				INT32 gainCount;
				lampInfo->get_GainLevelCount(&gainCount);

				wprintf_s(L"Added device: name=%s id=%s Index=%i lampIndex=%d lampCount=%d redCount=%d greenCount=%d blueCount=%d gainCount=%d\n",
					name.GetRawBuffer(nullptr),
					id.GetRawBuffer(nullptr),
					index,
					lampIndex,
					lampCount,
					redCount,
					greenCount,
					blueCount,
					gainCount);
			}

			return S_OK;

		}).Get(), &addedToken);

#pragma endregion Assign Device Watcher Added Event

#pragma region Assign Device Watcher Removed Event

	EventRegistrationToken removedToken;

	// Type define the event handler types to make the code more readable.
	typedef __FITypedEventHandler_2_Windows__CDevices__CEnumeration__CDeviceWatcher_Windows__CDevices__CEnumeration__CDeviceInformationUpdate RemovedHandler;

	hr = deviceWatcher->add_Removed(Callback<RemovedHandler>([](IDeviceWatcher* watcher, IDeviceInformationUpdate* deviceInformation) -> HRESULT
		{
			HString id;
			HRESULT hr = deviceInformation->get_Id(id.GetAddressOf());
			if (SUCCEEDED(hr))
			{
				wprintf_s(L"Removed device: %s\n", id.GetRawBuffer(nullptr));
			}
			return S_OK;

		}).Get(), &removedToken);

#pragma endregion Assign Device Watcher Removed Event

#pragma region Start Device Watcher

	hr = deviceWatcher->Start();
	if (FAILED(hr))
	{
		fwprintf_s(stderr, L"Failed to Start DeviceWatcher! Line: %d Result: %ld\n", __LINE__, hr);
		return -1;
	}
	wprintf_s(L"WinRT Started DeviceWatcher!\n");

#pragma endregion Start Device Watcher

	Sleep(5000); // wait for events before exiting

	return 0;
}
