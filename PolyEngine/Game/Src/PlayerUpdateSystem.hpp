#pragma once


namespace Poly
{
	class World;
	class TransformComponent;
}

namespace SGJ
{
	namespace PlayerUpdateSystem
	{
		void Update(Poly::World* world);

		float ElasticEaseOut(float p);
		void KillPlayer(Poly::World* world);
		void PlayerJump(Poly::World* world);
	}

}