#include <EnginePCH.hpp>

#include <Rendering/Particles/ParticleEmitter.hpp>
#include <Resources/ResourceManager.hpp>

using namespace Poly;

ParticleEmitter::ParticleEmitter(const Settings& settings)
	: settings(settings), ToEmit(settings.InitialSize), ParticlesPool(settings.MaxSize)
{
	Spritesheet = ResourceManager<TextureResource>::Load(
		settings.Spritesheet.SpritePath,
		settings.Spritesheet.Source,
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