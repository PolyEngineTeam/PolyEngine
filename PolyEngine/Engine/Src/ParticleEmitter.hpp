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

		ParticleEmitter(int size);

		const Dynarray<float>& GetInstances() const { return InstancesTransform; }

		void Burst(int quota);
		
		const IParticleDeviceProxy* GetParticleProxy() const { return ParticleProxy.get(); }


		void Update();
		void UpdateDeviceProxy();

		bool HasInstances() const { return InstancesTransform.GetSize() != 0; }

	private:
		Dynarray<float> InstancesTransform;
		std::unique_ptr<IParticleDeviceProxy> ParticleProxy;

		float Random() const;
		float Random(float min, float max) const;
	};
}