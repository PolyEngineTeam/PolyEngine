#include "EnginePCH.hpp"

#include "Rendering/Camera/CameraComponent.hpp"
#include "Rendering/SpritesheetComponent.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/TextureResource.hpp"

using namespace Poly;

SpritesheetComponent::SpritesheetComponent(const SpritesheetSettings& settings) : Settings(settings)
{
	Spritesheet = ResourceManager<TextureResource>::Load(settings.SpritePath, settings.Source, eTextureUsageType::ALBEDO);
}

SpritesheetComponent::~SpritesheetComponent()
{
	if (Spritesheet)
	{
		ResourceManager<TextureResource>::Release(Spritesheet);
	}
}
