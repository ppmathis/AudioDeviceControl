#include "stdafx.h"
#include "AudioEndpoint.h"
#include "ComHelpers.h"
#include "Formatter.h"

#include <stdexcept>
#include <functiondiscoverykeys_devpkey.h>

AudioEndpoint::AudioEndpoint(IMMDevice *pDevice) : mpDevice(pDevice)
{
	HRESULT hr;

	// Get the UUID of the endpoint
	LPWSTR deviceId;
	hr = mpDevice->GetId(&deviceId);

	if (SUCCEEDED(hr)) {
		mUuid = std::wstring(deviceId);
	} else {
		throw std::runtime_error(Formatter() << L"Unable to get UUID of endpoint: " << std::hex << hr);
	}

	// Try to open the property store of the endpoint
	hr = mpDevice->OpenPropertyStore(STGM_READ, &mpProperties);
	if (FAILED(hr)) {
		throw std::runtime_error(Formatter() << L"Unable to open property store of endpoint <" << mUuid << ">: " << std::hex << hr);
	}

	// Read various data from the endpoint
	readFriendlyName();
	readDeviceFormat();
}

AudioEndpoint::~AudioEndpoint()
{
	printf("RELEASE DAT SHIT\n");
	SafeRelease(&mpProperties);
	SafeRelease(&mpDevice);
}

void AudioEndpoint::readFriendlyName()
{
	HRESULT hr;
	PROPVARIANT friendlyName;

	PropVariantInit(&friendlyName);
	hr = mpProperties->GetValue(PKEY_Device_FriendlyName, &friendlyName);
	if (FAILED(hr)) {
		throw std::runtime_error(Formatter() << L"Unable to get friendly name of endpoint <" << mUuid << ">: " << std::hex << hr);
	}

	mFriendlyName = std::wstring(friendlyName.pwszVal);
	PropVariantClear(&friendlyName);
}

void AudioEndpoint::readDeviceFormat()
{
	HRESULT hr;
	PROPVARIANT deviceFormat;

	// Get the device format from the property store
	PropVariantInit(&deviceFormat);
	hr = mpProperties->GetValue(PKEY_AudioEngine_DeviceFormat, &deviceFormat);
	if (FAILED(hr)) {
		throw std::runtime_error(Formatter() << L"Failed to get device format of endpoint <" << mUuid << ">: " << std::hex << hr);
	}

	// The returned PROPVARIANT always contains a WAVEFORMATEX structure, which we have to parse
	WAVEFORMATEX *pWaveFormatEx = reinterpret_cast<WAVEFORMATEX*>(deviceFormat.blob.pBlobData);
	WORD formatTag = pWaveFormatEx->wFormatTag;
	mSamplesPerSec = pWaveFormatEx->nSamplesPerSec;

	// To get the actual bits per sample, we have to parse another structure according to FormatTag
	switch (formatTag) {
		case WAVE_FORMAT_EXTENSIBLE: {
			WAVEFORMATEXTENSIBLE *pWaveFormatExtensible = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pWaveFormatEx);
			mBitsPerSample = pWaveFormatExtensible->Samples.wValidBitsPerSample;
		}; break;

		default: {
			throw std::runtime_error(Formatter() << L"Unsupported format tag of endpoint <" << mUuid << ">: " << std::hex << formatTag);
		}; break;
	}

	PropVariantClear(&deviceFormat);
}