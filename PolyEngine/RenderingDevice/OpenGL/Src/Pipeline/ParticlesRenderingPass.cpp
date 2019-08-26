#include <PolyRenderingDeviceGLPCH.hpp>

#include <Pipeline/ParticlesRenderingPass.hpp>
#include <Proxy/GLMeshDeviceProxy.hpp>
#include <Proxy/GLTextureDeviceProxy.hpp>
#include <Proxy/GLParticleDeviceProxy.hpp>
#include <GLRenderingDevice.hpp>

using namespace Poly;

ParticlesRenderingPass::ParticlesRenderingPass(const GLRenderingDevice* rdi)
	: RenderingPassBase(rdi, "Shaders/instanced.vert.glsl", "Shaders/instanced.frag.glsl")
{
	GetProgram().RegisterUniform("float", "uTime");
	GetProgram().RegisterUniform("mat4", "uScreenFromView");
	GetProgram().RegisterUniform("mat4", "uViewFromWorld");
	GetProgram().RegisterUniform("mat4", "uWorldFromModel");
	GetProgram().RegisterUniform("vec4", "uColor");
}

void ParticlesRenderingPass::OnRun(Scene* world, const CameraComponent* camera, const core::math::AARect& /*rect*/, ePassType passType = ePassType::GLOBAL)
{
	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	const core::math::Matrix& ViewFromWorld = camera->GetViewFromWorld();
	const core::math::Matrix& ScreenFromView = camera->GetClipFromView();

	glDisable(GL_CULL_FACE);

	GetProgram().BindProgram();
	GetProgram().SetUniform("uTime", Time);
	GetProgram().SetUniform("uScreenFromView", ScreenFromView);

	for (auto componentsTuple : world->IterateComponents<ParticleComponent>())
	{
		const ParticleComponent* particleCmp = std::get<ParticleComponent*>(componentsTuple);
		const EntityTransform& transform = particleCmp->GetTransform();
		const core::math::Matrix& WorldFromModel = particleCmp->GetEmitter()->GetSettings().SimulationSpace == ParticleEmitter::eSimulationSpace::LOCAL_SPACE
			? transform.GetWorldFromModel()
			: core::math::Matrix();
		
		GetProgram().SetUniform("uViewFromWorld", ViewFromWorld);
		GetProgram().SetUniform("uWorldFromModel", WorldFromModel);
		
		ParticleEmitter::Settings emitterSettings = particleCmp->GetEmitter()->GetSettings();
		GetProgram().SetUniform("uEmitterColor", emitterSettings.Albedo);
		
		SpritesheetSettings spriteSettings = emitterSettings.Spritesheet;
		GetProgram().SetUniform("uSpriteColor", spriteSettings.SpriteColor);
		float startFrame = spriteSettings.IsRandomStartFrame ? core::math::RandomRange(0.0f, spriteSettings.SubImages.X * spriteSettings.SubImages.Y) : spriteSettings.StartFrame;
		GetProgram().SetUniform("uSpriteStartFrame", startFrame);
		GetProgram().SetUniform("uSpriteSpeed", spriteSettings.Speed);
		GetProgram().SetUniform("uSpriteSubImages", spriteSettings.SubImages.X, spriteSettings.SubImages.Y);

		GLsizei partileLen = (GLsizei)(particleCmp->GetEmitter()->GetInstancesCount());
		const TextureResource* Texture = particleCmp->GetEmitter()->GetSpritesheet();
		const GLParticleDeviceProxy* particleProxy = static_cast<const GLParticleDeviceProxy*>(particleCmp->GetEmitter()->GetParticleProxy());
		GLuint particleVAO = particleProxy->GetVAO();

		GLuint TextureID = Texture == nullptr
			? RDI->FallbackWhiteTexture
			: static_cast<const GLTextureDeviceProxy*>(Texture->GetTextureProxy())->GetTextureID();

		GetProgram().SetUniform("uHasSprite", Texture == nullptr ? 0.0f : 1.0f );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		glBindVertexArray(particleVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, partileLen);
		glBindVertexArray(0);
		CHECK_GL_ERR();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glEnable(GL_CULL_FACE);
}
