#include "stdafx.h"
#include "AudioEnumerator.h"
#include "AudioEndpoint.h"

#include <iostream>
#include <typeinfo>
#include <string>
#include <sstream>

static int modeEnumerate();
static int modeChangeFormat(std::wstring endpointUuid, WORD bitsPerSample, DWORD samplesPerSec);

template <typename T> void ParseArgument(wchar_t* argv[], int position, T *destination)
{
	std::basic_istringstream<wchar_t> argumentStream(argv[position]);
	argumentStream >> *destination;
	if (!argumentStream) {
		std::wcerr << L"FATAL ERROR: Bad argument #" << position << " - expected " << typeid(T).name() << std::endl;
		exit(2);
	}
}

void RequireArguments(int providedArgs, int requiredArgs, const std::wstring& mode)
{
	if ((providedArgs - 1) != requiredArgs) {
		std::wcerr << L"FATAL ERROR: The mode '" << mode << "' expects " << requiredArgs << " argument(s). Please consult documentation for further information!" << std::endl;
		exit(4);
	}
}

int wmain(int argc, wchar_t* argv[])
{
	std::wstring mode;
	std::wstring endpointUuid;
	WORD bitsPerSample;
	DWORD samplesPerSec;

	// Check if atleast one argument was provided
	if (argc < 2) {
		std::wcerr << L"FATAL ERROR: No arguments specified. Please consult documentation for further information!" << std::endl;
		return 1;
	}

	// Check which mode was choosen by user
	ParseArgument<std::wstring>(argv, 1, &mode);
	if (mode == L"enumerate") {
		RequireArguments(argc, 1, mode);

		return modeEnumerate();
	} else if (mode == L"changeformat") {
		RequireArguments(argc, 4, mode);
		ParseArgument<std::wstring>(argv, 2, &endpointUuid);
		ParseArgument<WORD>(argv, 3, &bitsPerSample);
		ParseArgument<DWORD>(argv, 4, &samplesPerSec);

		return modeChangeFormat(endpointUuid, bitsPerSample, samplesPerSec);
	} else {
		std::wcerr << L"FATAL ERROR: Unknown mode specified! Supported modes are: enumerate, changeformat" << std::endl;
		return 3;
	}
}

static int modeEnumerate()
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

	return 0;
}

static int modeChangeFormat(std::wstring endpointUuid, WORD bitsPerSample, DWORD samplesPerSec)
{
	// Check if all arguments are valid
	if (endpointUuid.length() < 1 || !bitsPerSample || !samplesPerSec) {
		std::wcerr << L"FATAL ERROR: One or more arguments were invalid. Please consult documentation for further information!" << std::endl;
		return 5;
	}

	// Search for the given endpoint UUID
	AudioEnumerator enumerator{};
	std::vector<AudioEndpoint*> endpoints = enumerator.GetEndpoints();

	for (std::vector<AudioEndpoint*>::iterator iEndpoint = endpoints.begin(); iEndpoint != endpoints.end(); ++iEndpoint) {
		AudioEndpoint *endpoint = *iEndpoint;
		if (endpoint->GetUuid() == endpointUuid) {
			// TODO: Implement method to change device format
			return 0;
		}
	}

	// It seems like no matching endpoint was found - fail!
	std::wcerr << L"FATAL ERROR: No endpoint exists with UUID <" << endpointUuid << ">. Please use 'enumerate' to see all possible endpoints!" << std::endl;
	return 6;
}