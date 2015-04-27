#include "stdafx.h"
#include "AudioEnumerator.h"
#include "Formatter.h"
#include "ComHelpers.h"

#include <stdexcept>
#include <mmdeviceapi.h>

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

AudioEnumerator::AudioEnumerator()
{
	HRESULT hr;

	// Initialize COM interface
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		throw std::runtime_error(Formatter() << "Unable to initialize COM: " << std::hex << hr);
	}

	// Initialize the device enumerator
	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&mpEnumerator));
	if (FAILED(hr)) {
		throw std::runtime_error(Formatter() << "Unable to instantiate device enumerator: " << std::hex << hr);
	}

	// Enumerate all active endpoints which are renderers
	hr = mpEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &mpDevices);
	if (FAILED(hr)) {
		throw std::runtime_error(Formatter() << "Unable to enumerate audio endpoints: " << std::hex << hr);
	}

	// Get endpoint collection length
	UINT deviceCount;
	hr = mpDevices->GetCount(&deviceCount);
	if (FAILED(hr)) {
		throw std::runtime_error(Formatter() << "Unable to get endpoint collection length: " << std::hex << hr);
	}

	// Create AudioEndpoint instances for every item within the collection
	for (UINT iDevice = 0; iDevice < deviceCount; iDevice++) {
		IMMDevice *pDevice = NULL;
		hr = mpDevices->Item(iDevice, &pDevice);
		if (FAILED(hr)) {
			throw std::runtime_error(Formatter() << "Unable to retrieve device " << iDevice << ": " << std::hex << hr);
		}

		mEndpoints.push_back(new AudioEndpoint{ pDevice });
	}
}

AudioEnumerator::~AudioEnumerator()
{
	mEndpoints.erase(mEndpoints.begin(), mEndpoints.end());
	SafeRelease(&mpDevices);
	SafeRelease(&mpEnumerator);
	CoUninitialize();
}