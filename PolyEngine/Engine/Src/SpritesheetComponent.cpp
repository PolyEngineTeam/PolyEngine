#include "EnginePCH.hpp"

#include "CameraComponent.hpp"
#include "SpritesheetComponent.hpp"
#include "ResourceManager.hpp"
#include "TextureResource.hpp"

using namespace Poly;

SpritesheetComponent::SpritesheetComponent(Settings settings)
{
	Spritesheet = ResourceManager<TextureResource>::Load(settings.spritePath, settings.source, eTextureUsageType::DIFFUSE);
}

SpritesheetComponent::~SpritesheetComponent()
{
	if (Spritesheet)
		ResourceManager<TextureResource>::Release(Spritesheet);
}
