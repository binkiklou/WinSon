#pragma once
#ifdef  _WIN64

#include <Windows.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>

#include "son.hpp"

class win_wrapper : public son::impl_wrapper
{
public:
	virtual void init() override;
private:
	void _get_device();

	IMMDevice* immd = nullptr;
};
#endif