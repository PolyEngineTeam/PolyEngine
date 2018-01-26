#pragma once

#include <BaseObject.hpp>
#include <Dynarray.hpp>

namespace Poly
{
	class ENGINE_DLLEXPORT ParticleEmitter : public BaseObject<>
	{
	public:
		const Dynarray<float>& GetInstances() const { return Instances; }

		bool HasInstances() const { return Instances.GetSize() != 0; }

	private:
		Dynarray<float> Instances;
	};
}