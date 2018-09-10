#pragma once

#include <Defines.hpp>
#include <ECS/ISystem.hpp>

namespace Poly
{
	class Scene;
	using PhaseUpdateFunction = std::function<void(Scene*)>;

	class LambdaSystem : public ISystem
	{
	public:
		LambdaSystem(PhaseUpdateFunction func) { UpdateFunctionObject = std::move(func); }
		void OnUpdate(Scene* scene) { UpdateFunctionObject(scene); }
	private:
		PhaseUpdateFunction UpdateFunctionObject;
	};
}