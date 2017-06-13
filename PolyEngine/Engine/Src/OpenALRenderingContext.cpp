#include "EnginePCH.hpp"

#include "OpenALRenderingContext.hpp"

using namespace Poly;

namespace Poly {
	IRenderingContext* CreateRenderingContext() { return new OpenGLRenderingContext; }
}

bool OpenALRenderingContext::Init()
{
	return false;
}

void OpenALRenderingContext::Deinit()
{
}

void OpenALRenderingContext::EndFrame()
{
}
