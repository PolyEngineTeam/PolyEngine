#include "EnginePCH.hpp"
#include "ParticleEmitter.hpp"

#include <algorithm>

using namespace Poly;

ParticleEmitter::ParticleEmitter(const Settings& settings)
	: settings(settings), ToEmit(settings.InitialSize), ParticlesPool(settings.MaxSize)
{
	Spritesheet = ResourceManager<TextureResource>::Load(
		settings.SprsheetSettings.SpritePath,
		settings.SprsheetSettings.Source,
		eTextureUsageType::DIFFUSE
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