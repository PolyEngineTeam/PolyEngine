#include "SkyboxRenderingPass.hpp"

#include "GLRenderingDevice.hpp"
#include "GLCubemapDeviceProxy.hpp"

#include <AABox.hpp>
#include <AARect.hpp>
#include <World.hpp>
#include <CameraComponent.hpp>
#include <SkyboxWorldComponent.hpp>

using namespace Poly;

SkyboxRenderingPass::SkyboxRenderingPass(const PrimitiveCube* cube)
	: RenderingPassBase("Shaders/skyboxVert.shader", "Shaders/skyboxFrag.shader"), Cube(cube)
{
	GetProgram().RegisterUniform("mat4", "uMVP");
}

void SkyboxRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType /*passType = ePassType::GLOBAL*/ )
{
	const SkyboxWorldComponent* SkyboxWorldCmp = world->GetWorldComponent<SkyboxWorldComponent>();
	if (SkyboxWorldCmp == nullptr)
	{
		gConsole.LogInfo("SkyboxRenderingPass::OnRun: SkyboxWorldCmp is NULL");
	}
	else
	{
		RenderSkybox(camera, SkyboxWorldCmp);
	}
}

void SkyboxRenderingPass::RenderSkybox(const CameraComponent* camera, const SkyboxWorldComponent* SkyboxWorldCmp)
{
	const Matrix projection = camera->GetProjectionMatrix();
	Matrix modelView = Matrix(camera->GetModelViewMatrix());
	// center cube in view space by setting translation to 0 for x, y and z. SetTranslation resets Matrix to identity
	modelView.Data[3] = 0.0f;
	modelView.Data[7] = 0.0f;
	modelView.Data[11] = 0.0f;

	Matrix mvp = projection * modelView;

	GetProgram().BindProgram();
	GetProgram().SetUniform("uMVP", mvp);

	GLuint CubemapID = static_cast<const GLCubemapDeviceProxy*>(SkyboxWorldCmp->GetCubemap().GetTextureProxy())->GetTextureID();

	glDepthMask(GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapID);

	glBindVertexArray(Cube->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}
