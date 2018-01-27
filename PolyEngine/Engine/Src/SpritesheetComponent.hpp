#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT SpritesheetComponent : public ComponentBase
	{
		friend void RenderingSystem::RenderingPhase(World*);
	public:
		SpritesheetComponent(const String& spritePath, eResourceSource source);
		virtual ~SpritesheetComponent();

		const TextureResource* GetSpritesheet() const { return Spritesheet; }

	private:
		TextureResource* Spritesheet; 
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SpritesheetComponent)
}