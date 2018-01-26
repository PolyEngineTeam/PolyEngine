#include "EnginePCH.hpp"

#include "ParticlesComponent.hpp"
#include "CameraComponent.hpp"

using namespace Poly;

ParticlesComponent::ParticlesComponent()
{
	emitter = new ParticleEmitter();
}

Poly::ParticlesComponent::~ParticlesComponent()
{
}