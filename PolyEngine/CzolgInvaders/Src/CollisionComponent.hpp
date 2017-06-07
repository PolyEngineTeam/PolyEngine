#pragma once

#include <ComponentBase.hpp>
#include <AABox.hpp>

namespace Invaders
{
	namespace CollisionSystem
	{
		class CollisionComponent : public Poly::ComponentBase
		{
		friend void CollisionUpdatePhase(Poly::World*);
		public:
			CollisionComponent(const Poly::Vector& position, const Poly::Vector& size) : Collider(position, size) {};
			~CollisionComponent() {};

			bool IsColliding() { return Colliding; }

		private:
			Poly::AABox Collider;
			// TODO: implement collection of colliding entities' ID's
			bool Colliding;
		};
	}
}