#include "EnginePCH.hpp"

#include "CameraComponent.hpp"
#include "SpritesheetComponent.hpp"
#include "ResourceManager.hpp"
#include "TextureResource.hpp"

using namespace Poly;

SpritesheetComponent::SpritesheetComponent(const SpritesheetSettings& settings) : Settings(settings)
{
	Spritesheet = ResourceManager<TextureResource>::Load(settings.SpritePath, settings.Source, eTextureUsageType::DIFFUSE);
}

SpritesheetComponent::~SpritesheetComponent()
{
	if (Spritesheet)
	{
		ResourceManager<TextureResource>::Release(Spritesheet);
	}
}
