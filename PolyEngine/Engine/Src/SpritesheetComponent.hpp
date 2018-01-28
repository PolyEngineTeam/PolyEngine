#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT SpritesheetComponent : public ComponentBase
	{
	public:

		struct ENGINE_DLLEXPORT Settings
		{
			eResourceSource source;
			String spritePath;
			int SizeX;
			int SizeY;
			float StartFrame;
			float Speed;
			float SpeedExp;
			float UseRnd;
		};

		friend void RenderingSystem::RenderingPhase(World*);
	public:
		SpritesheetComponent(Settings settings);
		virtual ~SpritesheetComponent();

		const TextureResource* GetSpritesheet() const { return Spritesheet; }
		const Settings GetSettings() const { return settings; }

	private:
		Settings settings;
		TextureResource* Spritesheet; 
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SpritesheetComponent)
}