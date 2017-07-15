#include "RenderingPassBase.hpp"

#include "GLTextureDeviceProxy.hpp"

#include <Engine.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
void RenderingPassBase::BindOutput(const String& outputName, RenderingTargetBase* target)
{
	if (target)
	{
		ASSERTE(Outputs.find(outputName) == Outputs.end(), "There is a target already bound!");
		Outputs[outputName] = target;
	}
	else
	{
		ASSERTE(Outputs.find(outputName) != Outputs.end(), "There is no target bound!");
		Outputs.erase(outputName);
	}
}

//------------------------------------------------------------------------------
void RenderingPassBase::BindInput(const String& inputName, RenderingTargetBase* target)
{
	if (target)
	{
		ASSERTE(Inputs.find(inputName) == Inputs.end(), "There is a target already bound!");
		Inputs[inputName] = target;
	}
	else
	{
		ASSERTE(Inputs.find(inputName) != Inputs.end(), "There is no target bound!");
		Inputs.erase(inputName);
	}
}

//------------------------------------------------------------------------------
RenderingTargetBase* RenderingPassBase::GetInputTarget(const String& name)
{
	auto it = Inputs.find(name);
	if (it != Inputs.end())
		return it->second;
	return nullptr;
}

//------------------------------------------------------------------------------
RenderingTargetBase* RenderingPassBase::GetOutputTarget(const String& name)
{
	auto it = Outputs.find(name);
	if (it != Outputs.end())
		return it->second;
	return nullptr;
}


void ScreenBufferRenderingTarget::Bind()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

Poly::TextureRenderingTarget::TextureRenderingTarget(GLuint format)
	: Format(format)
{
	ScreenSize size = gEngine->GetRenderingDevice()->GetScreenSize();
	Texture = std::make_unique<GLTextureDeviceProxy>(size.Width, size.Height, format);
}

void Poly::TextureRenderingTarget::Resize(size_t width, size_t height)
{
	Texture = std::make_unique<GLTextureDeviceProxy>(width, height, Format);
}

GLuint Poly::TextureRenderingTarget::GetTextureID()
{
	return Texture->GetTextureID();
}

Poly::DepthRenderingTarget::DepthRenderingTarget()
	: TextureRenderingTarget(GL_DEPTH_COMPONENT32F)
{
}
