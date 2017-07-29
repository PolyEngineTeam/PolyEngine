#pragma once

#include "ComponentBase.hpp"
#include "Physics2DSystem.hpp"

namespace Poly
{
	class World;

	enum class eRigidBody2DType
	{
		STATIC,
		KINEMATIC,
		DYNAMIC,
		_COUNT
	};

	struct RigidBody2DData;

	class ENGINE_DLLEXPORT RigidBody2DComponent : public ComponentBase
	{
		friend void Physics2DSystem::Physics2DUpdatePhase(World* world);
	public:
		RigidBody2DComponent(World* world, eRigidBody2DType type, float density = 1.0f, float friction = 0.3f);
		~RigidBody2DComponent();

		void DebugPrintInfo() const;
	private:
		void FinishInit();

		eRigidBody2DType BodyType;
		std::unique_ptr<RigidBody2DData> ImplData;
	};
}