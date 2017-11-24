#include "PostprocessRenderingPass.hpp"

#include "GLRenderingDevice.hpp"
#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <AARect.hpp>
#include <World.hpp>
#include <TimeSystem.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MovementSystem.hpp>
#include <MeshRenderingComponent.hpp>
#include <PostprocessSettingsComponent.hpp>


using namespace Poly;

PostprocessRenderingPass::PostprocessRenderingPass(const PostprocessQuad* quad, const String& fragment)
	: RenderingPassBase("Shaders/postprocessCommonVert.shader", fragment), Quad(quad)
{
}

PostprocessRenderingPass::PostprocessRenderingPass(const PostprocessQuad* quad, const String& geometry, const String& fragment)
	: RenderingPassBase("Shaders/postprocessCommonVert.shader", geometry, fragment), Quad(quad)
{
}


void PostprocessRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType /*passType = ePassType::GLOBAL*/)
{
	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	float ResolutionX = rect.GetSize().X * gRenderingDevice->GetScreenSize().Width;
	float ResolutionY = rect.GetSize().X * gRenderingDevice->GetScreenSize().Height;
	const TransformComponent* CameraTransform = camera->GetSibling<TransformComponent>();
	Vector CameraPosition = CameraTransform->GetGlobalTranslation();
	Matrix CameraRotation = CameraTransform->GetGlobalRotation().ToRotationMatrix();

	GetProgram().BindProgram();

	GetProgram().SetUniform("uTime", Time);
	GetProgram().SetUniform("uResolution", ResolutionX, ResolutionY);

	GetProgram().SetUniform("uCameraPosition", CameraPosition);
	GetProgram().SetUniform("uCameraRotation", CameraRotation);

	const PostprocessSettingsComponent* PostprocessSettings = camera->GetSibling<PostprocessSettingsComponent>();
	if (PostprocessSettings == nullptr)
	{
		gConsole.LogInfo("void PostprocessRenderingPass::OnRun: PostprocessSettings is NULL");
	}
	else
	{
		GetProgram().SetUniform("uUseCashetes",						PostprocessSettings->UseCashetes);
		GetProgram().SetUniform("uDistortionPower",					PostprocessSettings->Distortion);
		GetProgram().SetUniform("uColorTempValue",					PostprocessSettings->ColorTempValue);
		GetProgram().SetUniform("uColorTempLuminancePreservation",	PostprocessSettings->ColorTempLuminancePreservation);
		GetProgram().SetUniform("uSaturationPower",					PostprocessSettings->Saturation);
		GetProgram().SetUniform("uGrainPower",						PostprocessSettings->Grain);
		GetProgram().SetUniform("uStripesPower",					PostprocessSettings->Stripes);
			
		
		//gConsole.LogInfo("void PostprocessRenderingPass::OnRun: UseCashetes: {}", PostprocessSettings->UseCashetes);
	}


	glBindVertexArray(Quad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
