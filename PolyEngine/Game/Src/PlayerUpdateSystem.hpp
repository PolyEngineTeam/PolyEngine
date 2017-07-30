#pragma once


namespace Poly
{
	class World;
	class TransformComponent;
	class Vector;
}

namespace SGJ
{
	namespace PlayerUpdateSystem
	{
		void Update(Poly::World* world);

		float ElasticEaseOut(float p);
		void KillPlayer(Poly::World* world);
		void ResetPlayer(Poly::World* world, const Poly::Vector& spawnLocation);
		void TryPlayerJump(Poly::World* world);
		void UpdateInAir(Poly::World* world);
		void ProcessJumpStrech(Poly::World* world);

		void UpdateDeathAction(Poly::World* world);
		void PushPlayer(Poly::World * world, const Poly::Vector& normal, float force);
	}

}