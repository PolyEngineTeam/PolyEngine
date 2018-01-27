#include "EnginePCH.hpp"

#include "ParticleComponent.hpp"
#include "CameraComponent.hpp"

using namespace Poly;

ParticleComponent::ParticleComponent(ParticleEmitter::Settings settings)
{
	Emitter = new ParticleEmitter(settings);
}

Poly::ParticleComponent::~ParticleComponent()
{
	delete Emitter;
}