#include "ParticlesRenderingPass.hpp"

#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <World.hpp>
#include <TimeSystem.hpp>
#include <CameraComponent.hpp>
#include <ParticleComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <MovementSystem.hpp>

using namespace Poly;

ParticlesRenderingPass::ParticlesRenderingPass()
	: RenderingPassBase("Shaders/instancedVert.shader", "Shaders/instancedFrag.shader")
{
	// gConsole.LogInfo("ParticlesRenderingPass::ParticlesRenderingPass");

	GetProgram().RegisterUniform("float", "uTime");
	GetProgram().RegisterUniform("mat4", "uMV");
	GetProgram().RegisterUniform("mat4", "uP");
}

void ParticlesRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType passType = ePassType::GLOBAL)
{
	// gConsole.LogInfo("ParticlesRenderingPass::OnRun");

	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	const Matrix& mvp = camera->GetMVP();
	const Matrix& mv = camera->GetModelViewMatrix();
	const Matrix& p = camera->GetProjectionMatrix();

	glDisable(GL_CULL_FACE);

	GetProgram().BindProgram();
	GetProgram().SetUniform("uTime", Time);
	GetProgram().SetUniform("uP", p);

	// Render meshes
	for (auto componentsTuple : world->IterateComponents<ParticleComponent>())
	{
		const ParticleComponent* particleCmp = std::get<ParticleComponent*>(componentsTuple);
		const EntityTransform& transform = particleCmp->GetTransform();
		const Matrix& objTransform = transform.GetGlobalTransformationMatrix();
		Matrix screenTransform = mv * objTransform;
		GetProgram().SetUniform("uMV", screenTransform);

		int partileLen = particleCmp->Emitter->GetInstances().GetSize() / 16;
		const GLParticleDeviceProxy* particleProxy = static_cast<const GLParticleDeviceProxy*>(particleCmp->Emitter->GetParticleProxy());
		GLuint particleVAO = particleProxy->GetVAO();

		// gConsole.LogInfo("ParticlesRenderingPass::OnRun VAO: {}, found particles: {}",
		// 	particleVAO, partileLen);

		// const TextureResource* DiffuseTexture = particleCmp->Emitter.GetDiffTexture();
		// GLuint TextureID = DiffuseTexture == nullptr
		// 	? FallbackWhiteTexture
		// 	: static_cast<const GLTextureDeviceProxy*>(DiffuseTexture->GetTextureProxy())->GetTextureID();
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, TextureID);

		glBindVertexArray(particleVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, partileLen);
		glBindVertexArray(0);
		CHECK_GL_ERR();
	}

	glEnable(GL_CULL_FACE);
}
