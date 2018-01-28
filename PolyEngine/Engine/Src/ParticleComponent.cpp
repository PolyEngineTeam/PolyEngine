#include "EnginePCH.hpp"

#include "ParticleComponent.hpp"
#include "CameraComponent.hpp"

using namespace Poly;

ParticleComponent::ParticleComponent(ParticleEmitter::Settings emitter, const String& spritePath, eResourceSource source)
{
	Spritesheet = ResourceManager<TextureResource>::Load(spritePath, source, eTextureUsageType::DIFFUSE);
	Emitter = new ParticleEmitter(emitter);
}

ParticleComponent::ParticleComponent(ParticleEmitter::Settings emitter)
{
	Emitter = new ParticleEmitter(emitter);
}

Poly::ParticleComponent::~ParticleComponent()
{
	delete Emitter;

	if (Spritesheet)
		ResourceManager<TextureResource>::Release(Spritesheet);
}