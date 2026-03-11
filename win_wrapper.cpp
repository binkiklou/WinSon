#ifdef _WIN64
#include "win_wrapper.hpp"
#include <iostream>

void win_wrapper::init()
{
	std::cout << "Windows init" << std::endl;

	HRESULT hres;
	hres = CoInitialize(nullptr);

	if (hres != S_OK)
	{
		std::cout << "Com Library Init error" << std::endl;
		return;
	}

	_get_device();
}

void win_wrapper::_get_device()
{
	std::cout << "Fetching device" << std::endl;

	HRESULT hres;
	IMMDeviceEnumerator* immde = nullptr;
	hres = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&immde);
	if (hres != S_OK)
	{
		return;
	}
	hres = immde->GetDefaultAudioEndpoint(eRender, eConsole, &(this->immd));
	if (hres != S_OK) {
		return;
	}


	std::cout << "Succesfully got device!" << std::endl;
}
#endif