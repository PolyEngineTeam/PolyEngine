#include "EnginePCH.hpp"
#include "Rendering/Particles/ParticleEmitter.hpp"

#include <algorithm>

using namespace Poly;

ParticleEmitter::ParticleEmitter(const Settings& settings)
	: settings(settings), ToEmit(settings.InitialSize), ParticlesPool(settings.MaxSize)
{
	Spritesheet = ResourceManager<TextureResource>::Load(
		settings.SpritesheetSettings.SpritePath,
		settings.SpritesheetSettings.Source,
		eTextureUsageType::ALBEDO
	);
	ParticleProxy = gEngine->GetRenderingDevice()->CreateParticle();
}

ParticleEmitter::~ParticleEmitter()
{
	if (Spritesheet)
	{
		ResourceManager<TextureResource>::Release(Spritesheet);
	}
}

void ParticleEmitter::Emit(size_t size)
{
	ToEmit += size;
}