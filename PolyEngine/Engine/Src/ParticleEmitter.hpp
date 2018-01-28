#pragma once

#include <BaseObject.hpp>
#include <IRenderingDevice.hpp>
#include <Dynarray.hpp>

typedef unsigned int GLuint;

namespace Poly
{

	class ENGINE_DLLEXPORT ParticleEmitter : public BaseObject<>
	{
	public:

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
			int InitialSize;
			float BurstTimeMin;
			float BurstTimeMax;
			int BurstSizeMin;
			int BurstSizeMax;
			Color Color;
			std::function<void(Particle*)> ParticleInitFunc;
			std::function<void(Particle*)> ParticleUpdateFunc;
		};

		ParticleEmitter(Settings settings);

		const Dynarray<float>& GetInstances() const { return InstancesTransform; }
		const IParticleDeviceProxy* GetParticleProxy() const { return ParticleProxy.get(); }

		void Emit(size_t quota);

		bool GetIsBurstEnabled() { return IsBurstEnabled; }
		
		Settings GetSettings() { return settings; }

		void SetBurstEnabled(bool value) { IsBurstEnabled = value; }

		void Update(World* world);

		void RecreateBufferForProxy();

		bool HasInstances() const { return InstancesTransform.GetSize() != 0; }

	private:
		Settings settings;

		IterablePoolAllocator<Particle> ParticlesPool;

		Dynarray<float> InstancesTransform;
		std::unique_ptr<IParticleDeviceProxy> ParticleProxy;

		void UpdateDeviceProxy();

		float NextBurstTime = -1.0;

		bool IsBurstEnabled = true;

		float Random();
		float Random(float min, float max);
	};
}