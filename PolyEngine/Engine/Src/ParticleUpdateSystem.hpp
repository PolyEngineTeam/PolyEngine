#pragma once

namespace Poly
{
	class World;
	class EntityTransform;
	class ParticleComponent;
	class ParticleEmitter;

	namespace ParticleUpdateSystem
	{
		void ParticleUpdatePhase(World* world);
		void EmitterEmit(World*, ParticleEmitter*, ParticleComponent*);
		void EmitterUpdate(World*, ParticleEmitter*);
		void EmitterRecreateBuffer(World*, ParticleEmitter*);
	}
}