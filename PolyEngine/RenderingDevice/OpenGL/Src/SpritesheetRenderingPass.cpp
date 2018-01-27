#include "SpritesheetRenderingPass.hpp"

#include "GLRenderingDevice.hpp"
#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <AABox.hpp>
#include <World.hpp>
#include <TimeSystem.hpp>
#include <CameraComponent.hpp>
#include <MovementSystem.hpp>
#include <SpritesheetComponent.hpp>
#include <PostprocessSettingsComponent.hpp>

using namespace Poly;

SpritesheetRenderingPass::SpritesheetRenderingPass(const PostprocessQuad* quad)
	: RenderingPassBase("Shaders/spritesheetVert.shader", "Shaders/spritesheetFrag.shader"), Quad(quad)
{
	gConsole.LogInfo("SpritesheetRenderingPass::SpritesheetRenderingPass");

	GetProgram().RegisterUniform("float", "uTime");
	GetProgram().RegisterUniform("mat4", "uTransform");
	GetProgram().RegisterUniform("mat4", "uMVPTransform");
}

void SpritesheetRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType /*passType = ePassType::GLOBAL*/ )
{
	gConsole.LogInfo("SpritesheetRenderingPass::OnRun");

	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	const Matrix& mv = camera->GetModelViewMatrix();
	const Matrix& p = camera->GetProjectionMatrix();

	glDisable(GL_CULL_FACE);

	GetProgram().BindProgram();
	GetProgram().SetUniform("uTime", Time);
	GetProgram().SetUniform("uP", p);

	// Render meshes
	for (auto componentsTuple : world->IterateComponents<SpritesheetComponent>())
	{
		gConsole.LogInfo("SpritesheetRenderingPass::OnRun found Spritesheet");

		const SpritesheetComponent* spritesheetCmp = std::get<SpritesheetComponent*>(componentsTuple);
		const EntityTransform& transform = spritesheetCmp->GetTransform();
		const Matrix& objTransform = transform.GetGlobalTransformationMatrix();
		Matrix screenTransform = mv * objTransform;
		GetProgram().SetUniform("uMV", screenTransform);

		const TextureResource* Spritesheet = spritesheetCmp->GetSpritesheet();
		GLuint TextureID = Spritesheet == nullptr
			? FallbackWhiteTexture
			: static_cast<const GLTextureDeviceProxy*>(Spritesheet->GetTextureProxy())->GetTextureID();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		glBindVertexArray(Quad->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	glEnable(GL_CULL_FACE);
}
