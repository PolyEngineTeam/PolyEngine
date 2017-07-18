#include "RenderingPassBase.hpp"

#include "GLTextureDeviceProxy.hpp"

#include "GLRenderingDevice.hpp"

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
Poly::RenderingPassBase::RenderingPassBase(const String& vertex, const String& fragment)
	: Program(vertex, fragment)
{
}

//------------------------------------------------------------------------------
Poly::RenderingPassBase::RenderingPassBase(const String& vertex, const String& geometry, const String& fragment)
	: Program(vertex, geometry, fragment)
{
}

//------------------------------------------------------------------------------
void RenderingPassBase::Run(World* world, const CameraComponent* camera, const AABox& rect)
{
	// Bind inputs
	Program.BindProgram();
	size_t samplerCount = 0;
	for (auto& kv : GetInputs())
	{
		const String& name = kv.first;
		RenderingTargetBase* target = kv.second;

		switch (target->GetType())
		{
		case eRenderingTargetType::TEXTURE_2D:
		{
			GLuint textureID = static_cast<Texture2DRenderingTarget*>(target)->GetTextureID();
			size_t idx = samplerCount++;
			glActiveTexture(GL_TEXTURE0 + idx);
			glBindTexture(GL_TEXTURE_2D, textureID);
			Program.SetUniform(name, (int)idx);
			break;
		}
		case eRenderingTargetType::DEPTH:
		case eRenderingTargetType::SCREENBUFFER:
		default:
			ASSERTE(false, "Unhandled target type!");
		}
	}
	glActiveTexture(GL_TEXTURE0); // reset texture binding

	// bind outputs (by binding fbo)
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	// call run implementation
	OnRun(world, camera, rect);
}

//------------------------------------------------------------------------------
void RenderingPassBase::Finalize()
{
	ASSERTE(GetOutputs().size() > 0, "No outputs from rendering pass!");
	if (GetOutputs().size() == 1)
		return; // we want the default FBO == 0, which is the screen buffer
	
	ASSERTE(FBO == 0, "Calling finalize twice!");
	glGenFramebuffers(1, &FBO);
	ASSERTE(FBO > 0, "Failed to create FBO!");
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	bool foundDepth = false;
	
	Dynarray<GLenum> colorAttachements;
	for (auto& kv : GetOutputs())
	{
		const String& name = kv.first;
		RenderingTargetBase* target = kv.second;

		switch (target->GetType())
		{
		case eRenderingTargetType::TEXTURE_2D:
		{
			GLuint textureID = static_cast<Texture2DRenderingTarget*>(target)->GetTextureID();
			size_t idx = Program.GetOutputsInfo().at(name).Index;
			GLenum attachementIdx = GL_COLOR_ATTACHMENT0 + idx;
			glBindTexture(GL_TEXTURE_2D, textureID);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachementIdx, GL_TEXTURE_2D, textureID, 0);
			colorAttachements.PushBack(attachementIdx);
			CHECK_FBO_STATUS();
			break;
		}
		case eRenderingTargetType::DEPTH:
		{
			GLuint textureID = static_cast<DepthRenderingTarget*>(target)->GetTextureID();
			glBindTexture(GL_TEXTURE_2D, textureID);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
			foundDepth = true;
			CHECK_FBO_STATUS();
			break;
		}
		case eRenderingTargetType::SCREENBUFFER:
		default:
			ASSERTE(false, "Unhandled target type!");
		}
	}
	ASSERTE(foundDepth, "Depth buffer not present when constructing FBO!");
	CHECK_GL_ERR();
	glDrawBuffers(colorAttachements.GetSize(), colorAttachements.GetData());
	CHECK_GL_ERR();
	CHECK_FBO_STATUS();

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
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

Poly::Texture2DRenderingTarget::Texture2DRenderingTarget(GLuint format)
	: Texture2DRenderingTarget(format, eInternalTextureUsageType::COLOR_ATTACHEMENT)
{
}

Poly::Texture2DRenderingTarget::Texture2DRenderingTarget(GLuint format, eInternalTextureUsageType internalUsage)
	: Format(format), InternalUsage(internalUsage)
{
	ScreenSize size = gRenderingDevice->GetScreenSize();
	Texture = std::make_unique<GLTextureDeviceProxy>(size.Width, size.Height, InternalUsage, format);
}

void Poly::Texture2DRenderingTarget::Resize(size_t width, size_t height)
{
	Texture = std::make_unique<GLTextureDeviceProxy>(width, height, InternalUsage, Format);
}

GLuint Poly::Texture2DRenderingTarget::GetTextureID()
{
	return Texture->GetTextureID();
}

Poly::DepthRenderingTarget::DepthRenderingTarget()
	: Texture2DRenderingTarget(GL_DEPTH_COMPONENT32F, eInternalTextureUsageType::DEPTH_ATTACHEMENT)
{
}