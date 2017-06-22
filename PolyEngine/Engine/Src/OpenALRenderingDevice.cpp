#include "EnginePCH.hpp"

#include "OpenALRenderingDevice.hpp"

#include <alc.h>

using namespace Poly;

OpenALRenderingDevice::OpenALRenderingDevice()
{
	Device = alcOpenDevice(NULL);
}

OpenALRenderingDevice::~OpenALRenderingDevice()
{
	alcCloseDevice(Device);
}
