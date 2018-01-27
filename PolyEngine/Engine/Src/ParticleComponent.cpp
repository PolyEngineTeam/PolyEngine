#include "EnginePCH.hpp"

#include "ParticleComponent.hpp"
#include "CameraComponent.hpp"

using namespace Poly;

ParticleComponent::ParticleComponent()
{
	emitter = new ParticleEmitter();
}

Poly::ParticleComponent::~ParticleComponent()
{
}