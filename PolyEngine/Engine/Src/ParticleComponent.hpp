#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "ParticleEmitter.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT ParticleComponent : public ComponentBase
	{
	public:
		ParticleComponent(ParticleEmitter::Settings emitter, const String& spritePath, eResourceSource source);
		ParticleComponent(ParticleEmitter::Settings emitter);
		virtual ~ParticleComponent();
		
		ParticleEmitter* GetEmitter() { return Emitter; }
		const TextureResource* GetSpritesheet() const { return Spritesheet; }

	private:
		ParticleEmitter* Emitter;
		TextureResource* Spritesheet;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ParticleComponent)
}