#include "EnginePCH.hpp"

#include "ParticleComponent.hpp"
#include "CameraComponent.hpp"

using namespace Poly;

ParticleComponent::ParticleComponent(int size)
{
	Emitter = new ParticleEmitter(size);
}

Poly::ParticleComponent::~ParticleComponent()
{
}