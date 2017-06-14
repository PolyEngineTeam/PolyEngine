#include "EnginePCH.hpp"

#include "OpenALRenderingContext.hpp"

#include <al.h> 
#include <alc.h>

using namespace Poly;

OpenALRenderingContext::OpenALRenderingContext()
{
}

OpenALRenderingContext::~OpenALRenderingContext()
{
	alcCloseDevice(Device);
}

void OpenALRenderingContext::Init()
{
	Device = alcOpenDevice(NULL);
}

void OpenALRenderingContext::EndFrame()
{
}
