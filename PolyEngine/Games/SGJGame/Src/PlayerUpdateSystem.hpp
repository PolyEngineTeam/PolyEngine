#pragma once


namespace Poly
{
	class World;
	class TransformComponent;
	class RigidBody2DComponent;
	class Vector;
}

namespace SGJ
{
	class PlayerControllerComponent;
	enum class ePowerup;

	namespace PlayerUpdateSystem
	{
		void Update(Poly::World* world);

		void PlayerAlive(Poly::World * world, SGJ::PlayerControllerComponent * playerCmp, double deltaTime, Poly::RigidBody2DComponent * rigidbodyCmp, double time);

		void PlayerDead(Poly::World * world, double time, SGJ::PlayerControllerComponent * playerCmp);

		float ElasticEaseOut(float p);

		void KillPlayer(Poly::World* world);
		void ResetPlayer(Poly::World* world, const Poly::Vector& spawnLocation);
		void TryPlayerJump(Poly::World* world);
		void UpdateInAir(Poly::World* world);
		void ProcessJumpStrech(Poly::World* world);
		void PickupPowerup(Poly::World* world, ePowerup powerup);
		void UpdateDeathAction(Poly::World* world);
		void PushPlayer(Poly::World * world, const Poly::Vector& normal, float force);
	}

}