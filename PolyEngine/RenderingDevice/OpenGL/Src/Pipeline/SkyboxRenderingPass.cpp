#include "Pipeline/SkyboxRenderingPass.hpp"

#include "GLRenderingDevice.hpp"
#include "Proxy/GLCubemapDeviceProxy.hpp"

#include <Math/AABox.hpp>
#include <Math/AARect.hpp>
#include <ECS/World.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/SkyboxWorldComponent.hpp>

using namespace Poly;

SkyboxRenderingPass::SkyboxRenderingPass(const GLRenderingDevice* rdi)
	: RenderingPassBase(rdi, "Shaders/skyboxVert.shader", "Shaders/skyboxFrag.shader")
{
	GetProgram().RegisterUniform("mat4", "uClipFromWorld");
}

void SkyboxRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType /*passType = ePassType::GLOBAL*/ )
{
	const SkyboxWorldComponent* SkyboxWorldCmp = world->GetWorldComponent<SkyboxWorldComponent>();
	if (SkyboxWorldCmp != nullptr)
		RenderSkybox(camera, SkyboxWorldCmp);
}

void SkyboxRenderingPass::RenderSkybox(const CameraComponent* camera, const SkyboxWorldComponent* SkyboxWorldCmp)
{
	const Matrix projection = camera->GetClipFromView();
	Matrix modelView = Matrix(camera->GetViewFromWorld());
	// center cube in view space by setting translation to 0 for x, y and z. SetTranslation resets Matrix to identity
	modelView.Data[3] = 0.0f;
	modelView.Data[7] = 0.0f;
	modelView.Data[11] = 0.0f;

	Matrix mvp = projection * modelView;

	GetProgram().BindProgram();
	GetProgram().SetUniform("uClipFromWorld", mvp);

	GLuint CubemapID = static_cast<const GLCubemapDeviceProxy*>(SkyboxWorldCmp->GetCubemap().GetTextureProxy())->GetTextureID();

	glDepthMask(GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapID);

	glBindVertexArray(RDI->PrimitivesCube->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}
