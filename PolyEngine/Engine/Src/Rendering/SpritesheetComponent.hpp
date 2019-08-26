#pragma once

#include <pe/Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Rendering/RenderingSystem.hpp>
#include <Configs/ConfigBase.hpp>

namespace Poly {

	struct ENGINE_DLLEXPORT SpritesheetSettings
	{
		::pe::core::storage::String SpritePath;
		eResourceSource Source = eResourceSource::GAME;
		::pe::core::math::Vector2f SubImages = ::pe::core::math::Vector2f(4.0, 4.0);
		::pe::core::math::Color SpriteColor = ::pe::core::math::Color::WHITE;
		float SpriteDepthFade = 0.0f;
		float StartFrame = 0.0f;
		float Speed = 1.0f;
		bool IsRandomStartFrame = false;
	};

	class ENGINE_DLLEXPORT SpritesheetComponent : public ComponentBase
	{
		RTTI_DECLARE_COMPONENT(::Poly::SpritesheetComponent) { NO_RTTI_PROPERTY(); }
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