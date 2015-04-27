#include "stdafx.h"
#include "AudioEnumerator.h"
#include "AudioEndpoint.h"

#include <iostream>
#include <string>
#include <tclap\CmdLine.h>

int wmain(int argc, wchar_t* argv[])
{
	AudioEnumerator enumerator{};
	std::vector<AudioEndpoint*> endpoints = enumerator.GetEndpoints();

	for (std::vector<AudioEndpoint*>::iterator iEndpoint = endpoints.begin(); iEndpoint != endpoints.end(); ++iEndpoint) {
		AudioEndpoint *endpoint = *iEndpoint;
		std::wcout << L"Endpoint #" << (iEndpoint - endpoints.begin()) << std::endl;
		std::wcout << L"> UUID: " << endpoint->GetUuid() << std::endl;
		std::wcout << L"> Friendly Name: " << endpoint->GetFriendlyName() << std::endl;
		std::wcout << L"> Samples per Second: " << endpoint->GetSamplesPerSec() << std::endl;
		std::wcout << L"> Bits per Sample: " << endpoint->GetBitsPerSample() << std::endl;
		std::wcout << std::endl;
	}

	std::cin.ignore();
	return 0;
}

