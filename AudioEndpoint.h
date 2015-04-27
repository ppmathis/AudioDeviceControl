#pragma once

#include <string>
#include <mmdeviceapi.h>
#include <mmreg.h>

class AudioEndpoint
{
public:
	AudioEndpoint(IMMDevice *pDevice);
	~AudioEndpoint();

	const std::wstring& GetUuid() const { return mUuid; };
	const std::wstring& GetFriendlyName() const { return mFriendlyName; };
	DWORD GetSamplesPerSec() const { return mSamplesPerSec; };
	WORD GetBitsPerSample() const { return mBitsPerSample; };

private:
	AudioEndpoint(const AudioEndpoint& other) = delete;
	AudioEndpoint& operator=(AudioEndpoint const&) = delete;

	IMMDevice *mpDevice;
	IPropertyStore *mpProperties;

	std::wstring mUuid;
	std::wstring mFriendlyName;
	DWORD mSamplesPerSec;
	WORD mBitsPerSample;

	void readFriendlyName();
	void readDeviceFormat();
};