#pragma once

#include <Core.hpp>

#include "Entity.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT World : public BaseObject<>
	{
	public:
		Entity* SpawnEntity();
		void DestroyEntity(const Entity* ent);

		const Dynarray<Entity>& GetEntities() const { return Entities; }
		
	private:
		Dynarray<Entity> Entities;
	};

}