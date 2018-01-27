#include "EnginePCH.hpp"

#include "ParticleComponent.hpp"
#include "CameraComponent.hpp"

using namespace Poly;

ParticleComponent::ParticleComponent()
{
	Emitter = new ParticleEmitter();
}

Poly::ParticleComponent::~ParticleComponent()
{
}