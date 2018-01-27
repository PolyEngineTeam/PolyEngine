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
		const IParticleDeviceProxy* GetParticleProxy() const { return ParticleProxy.get(); }

		void Emit(int quota);

		void Update();

		bool HasInstances() const { return InstancesTransform.GetSize() != 0; }

	private:
		Dynarray<float> InstancesTransform;
		std::unique_ptr<IParticleDeviceProxy> ParticleProxy;

		void UpdateDeviceProxy();

		float Random() const;
		float Random(float min, float max) const;
	};
}