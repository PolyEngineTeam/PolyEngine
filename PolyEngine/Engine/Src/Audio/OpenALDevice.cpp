#include <EnginePCH.hpp>

#include <Audio/OpenALDevice.hpp>

using namespace Poly;

OpenALDevice::OpenALDevice()
{
	Device = alcOpenDevice(nullptr);

	if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_TRUE)
	{
		const ALCchar* devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

		int i = 0;
		while (true)
		{
			AvailableDevices.PushFront(String(&devices[i]));

			while (devices[i] != 0) ++i;
			if (devices[i] == 0 && devices[i + 1] == 0) break;
			++i;
		}
	}
	else
	{
		AvailableDevices.PushFront(String(alcGetString(Device, ALC_DEVICE_SPECIFIER)));
	}
}

OpenALDevice::~OpenALDevice()
{
	alcCloseDevice(Device);
}
