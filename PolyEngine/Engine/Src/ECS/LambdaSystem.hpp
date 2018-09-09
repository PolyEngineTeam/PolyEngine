#pragma once

#include <Defines.hpp>
#include "ECS/ISystem.hpp"

namespace Poly
{
	class Scene;
	typedef std::function<void(Scene*)> PhaseUpdateFunction;

	class LambdaSystem : public ISystem
	{
	public:
		LambdaSystem(PhaseUpdateFunction func) { Payload = func; }
		void OnUpdate(Scene* world) { Payload(world); }
	private:
		PhaseUpdateFunction Payload;
	};
}