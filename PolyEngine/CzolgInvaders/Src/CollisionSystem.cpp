#include "CollisionSystem.hpp"

#include <World.hpp>
#include <TransformComponent.hpp>

#include "CollisionComponent.hpp"

void Invaders::CollisionSystem::CollisionUpdatePhase(Poly::World* world)
{
	CollisionComponent* collider1;
	CollisionComponent* collider2;

	for (auto tuple1 : world->IterateComponents<CollisionComponent, Poly::TransformComponent>())
		for (auto tuple2 : world->IterateComponents<CollisionComponent, Poly::TransformComponent>())
		{
			collider1 = std::get<CollisionComponent*>(tuple1);
			collider2 = std::get<CollisionComponent*>(tuple2);

			if(collider1 == collider2) continue;

			collider1->Collider.SetPosition(std::get<Poly::TransformComponent*>(tuple1)->GetGlobalTranslation());
			collider2->Collider.SetPosition(std::get<Poly::TransformComponent*>(tuple2)->GetGlobalTranslation());

			// TODO: if there aren't any collisions colliding = false 
			if (collider1->Collider.Intersects(&collider2->Collider)) collider1->Colliding = collider2->Colliding = true;
		}
	
}
