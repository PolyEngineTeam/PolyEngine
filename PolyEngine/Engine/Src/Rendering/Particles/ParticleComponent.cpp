#include <EnginePCH.hpp>

#include <Rendering/Particles/ParticleComponent.hpp>
#include <Rendering/Camera/CameraComponent.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::ParticleComponent)

ParticleComponent::ParticleComponent(const ParticleEmitter::Settings& emitter)
{
	Emitter = new ParticleEmitter(emitter);
}

Poly::ParticleComponent::~ParticleComponent()
{
	delete Emitter;
}