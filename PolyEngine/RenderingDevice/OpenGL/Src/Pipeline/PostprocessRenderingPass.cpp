#include <PolyRenderingDeviceGLPCH.hpp>

#include <Pipeline/PostprocessRenderingPass.hpp>
#include <GLRenderingDevice.hpp>
#include <Proxy/GLMeshDeviceProxy.hpp>
#include <Proxy/GLTextureDeviceProxy.hpp>

using namespace Poly;

PostprocessRenderingPass::PostprocessRenderingPass(const GLRenderingDevice* rdi, const core::storage::String& fragment)
	: RenderingPassBase(rdi, "Shaders/postprocessCommon.vert.glsl", fragment)
{
}

PostprocessRenderingPass::PostprocessRenderingPass(const GLRenderingDevice* rdi, const core::storage::String& geometry, const core::storage::String& fragment)
	: RenderingPassBase(rdi, "Shaders/postprocessCommon.vert.glsl", geometry, fragment)
{
}

void PostprocessRenderingPass::OnRun(Scene* world, const CameraComponent* camera, const core::math::AARect& rect, ePassType /*passType = ePassType::GLOBAL*/)
{
	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	float ResolutionX = rect.GetSize().X * gRenderingDevice->GetScreenSize().Width;
	float ResolutionY = rect.GetSize().Y * gRenderingDevice->GetScreenSize().Height;
	const EntityTransform& cameraTransform = camera->GetTransform();
	core::math::Vector CameraPosition = cameraTransform.GetGlobalTranslation();
	core::math::Matrix CameraRotation = cameraTransform.GetGlobalRotation().ToRotationMatrix();

	GetProgram().BindProgram();

	GetProgram().SetUniform("uTime", Time);
	GetProgram().SetUniform("uResolution", ResolutionX, ResolutionY);

	GetProgram().SetUniform("uCameraPosition", CameraPosition);
	GetProgram().SetUniform("uCameraRotation", CameraRotation);

	const PostprocessSettingsComponent* PostprocessSettings = camera->GetSibling<PostprocessSettingsComponent>();
	if (PostprocessSettings == nullptr)
	{
		core::utils::gConsole.LogInfo("void PostprocessRenderingPass::OnRun: PostprocessSettings is NULL");
	}

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
