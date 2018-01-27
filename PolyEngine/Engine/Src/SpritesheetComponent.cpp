#include "EnginePCH.hpp"

#include "CameraComponent.hpp"
#include "SpritesheetComponent.hpp"
#include "ResourceManager.hpp"
#include "TextureResource.hpp"

using namespace Poly;

SpritesheetComponent::SpritesheetComponent(const String& spritesheetPath, eResourceSource source)
{
	Spritesheet = ResourceManager<TextureResource>::Load(spritesheetPath, source);
	// Mesh = ResourceManager<MeshResource>::Load(meshPath, source);
}

SpritesheetComponent::~SpritesheetComponent()
{
	if (Spritesheet)
		ResourceManager<TextureResource>::Release(Spritesheet);
}
