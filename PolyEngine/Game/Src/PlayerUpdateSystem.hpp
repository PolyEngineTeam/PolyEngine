#pragma once


namespace Poly
{
	class World;
	class TransformComponent;
	class RigidBody2DComponent;
}

namespace SGJ
{
	class PlayerControllerComponent;

	namespace PlayerUpdateSystem
	{
		void Update(Poly::World* world);

		void PlayerAlive(Poly::World * world, SGJ::PlayerControllerComponent * playerCmp, double deltaTime, Poly::RigidBody2DComponent * rigidbodyCmp, double time);

		void PlayerDead(Poly::World * world, double time, SGJ::PlayerControllerComponent * playerCmp);

		float ElasticEaseOut(float p);

		void KillPlayer(Poly::World* world);

		void PlayerJump(Poly::World* world);
	}

}