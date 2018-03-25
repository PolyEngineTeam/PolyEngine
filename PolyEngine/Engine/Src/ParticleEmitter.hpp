#pragma once

#include <IterablePoolAllocator.hpp>
#include <BaseObject.hpp>
#include <IRenderingDevice.hpp>
#include <Dynarray.hpp>
#include <SpritesheetComponent.hpp>
#include "ParticleUpdateSystem.hpp"

typedef unsigned int GLuint;

namespace Poly
{

	class ParticleComponent;

	class ENGINE_DLLEXPORT ParticleEmitter : public BaseObject<>
	{
		friend void ParticleUpdateSystem::EmitterEmit(World*, ParticleEmitter*, ParticleComponent*);
		friend void ParticleUpdateSystem::EmitterUpdate(World*, ParticleEmitter*);
		friend void ParticleUpdateSystem::EmitterRecreateBuffer(World*, ParticleEmitter*);
	public:

		enum class eSimulationSpace
		{
			LOCAL_SPACE,
			WORLD_SPACE,
			_COUNT
		};

		struct ENGINE_DLLEXPORT Particle
		{
			Vector Position;
			Vector Scale;
			Vector Velocity;
			Vector Acceleration;
			float Age;
			float LifeTime;
		};

		struct ENGINE_DLLEXPORT Settings
		{
			int MaxSize = 1000;
			int InitialSize = 10;
			float BurstTimeMin = 1.0f;
			float BurstTimeMax = 2.0f;
			int BurstSizeMin = 1;
			int BurstSizeMax = 5;
			eSimulationSpace SimulationSpace = eSimulationSpace::WORLD_SPACE;
			// TODO: move to material
			SpritesheetSettings SpritesheetSettings;
			float Speed = 1.0f;
			Color Color = Color::WHITE;
			// TODO end
			std::function<void(Particle*)> ParticleInitFunc		= [](ParticleEmitter::Particle* p) {};
			std::function<void(Particle*)> ParticleUpdateFunc	= [](ParticleEmitter::Particle* p) {};
		};

		ParticleEmitter(const Settings& settings);
		~ParticleEmitter();

		const Settings& GetSettings() const { return settings; }
		const TextureResource* GetSpritesheet() const { return Spritesheet; }
		const IParticleDeviceProxy* GetParticleProxy() const { return ParticleProxy.get(); }
		bool GetIsBurstEnabled() { return IsBurstEnabled; }
		void SetBurstEnabled(bool value) { IsBurstEnabled = value; }
		const IterablePoolAllocator<Particle>& GetParticlesPool() const { return ParticlesPool; }
		bool HasInstances() const { return ParticlesPool.GetSize() != 0; }
		size_t GetInstancesCount() const { return ParticlesPool.GetSize(); }
		
		void Emit(size_t quota);

	private:
		Settings settings;
		TextureResource* Spritesheet;
		std::unique_ptr<IParticleDeviceProxy> ParticleProxy;
		bool IsBurstEnabled = true;
		float NextBurstTime = -1.0;
		size_t ToEmit = 0;
		IterablePoolAllocator<Particle> ParticlesPool;
	};
}