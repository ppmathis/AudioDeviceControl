#pragma once

#include "AudioEndpoint.h"

#include <vector>
#include <mmdeviceapi.h>

class AudioEnumerator
{
public:
	AudioEnumerator();
	~AudioEnumerator();

	const std::vector<AudioEndpoint*>& GetEndpoints() const { return mEndpoints; };

private:
	AudioEnumerator(const AudioEnumerator& other) = delete;
	AudioEnumerator& operator=(AudioEnumerator const&) = delete;

	IMMDeviceEnumerator *mpEnumerator;
	IMMDeviceCollection *mpDevices;

	std::vector<AudioEndpoint*> mEndpoints;
};