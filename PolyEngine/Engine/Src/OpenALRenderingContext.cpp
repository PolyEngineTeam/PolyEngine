#include "EnginePCH.hpp"

#include "OpenALRenderingContext.hpp"

#include <al.h> 
#include <alc.h>

using namespace Poly;

OpenALRenderingContext::OpenALRenderingContext()
{
	Device = alcOpenDevice(NULL);
}

OpenALRenderingContext::~OpenALRenderingContext()
{
	alcCloseDevice(Device);
}
