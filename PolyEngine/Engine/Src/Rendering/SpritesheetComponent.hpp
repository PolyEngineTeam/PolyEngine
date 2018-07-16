#pragma once

#include "ECS/ComponentBase.hpp"
#include "RenderingSystem.hpp"

#include <Math/Vector2f.hpp>
#include <Resources/ResourceManager.hpp>

namespace Poly {

	struct ENGINE_DLLEXPORT SpritesheetSettings
	{
		String SpritePath;
		eResourceSource Source = eResourceSource::GAME;
		Vector2f SubImages = Vector2f(4.0, 4.0);
		Color SpriteColor = Color::WHITE;
		float StartFrame = 0.0f;
		float Speed = 1.0f;
		bool IsRandomStartFrame = false;
	};

	class ENGINE_DLLEXPORT SpritesheetComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(SpritesheetComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY()
		}

	public:
		friend void RenderingSystem::RenderingPhase(Scene*);
	public:
		SpritesheetComponent(const SpritesheetSettings& settings);
		virtual ~SpritesheetComponent();

		const TextureResource* GetSpritesheet() const { return Spritesheet; }
		const SpritesheetSettings& GetSettings() const { return Settings; }

	private:
		SpritesheetSettings Settings;
		TextureResource* Spritesheet; 
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SpritesheetComponent)
}