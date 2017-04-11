#pragma once

#include <Core.hpp>

#include "Entity.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT World : public BaseObject<>
	{
	public:
		//TODO implement world
		//Entity* SpawnEntity();
		//void DestroyEntity(const Entity* ent);
		
	private:
		PoolAllocator<Entity> Entities;
		// TODO implement registered component storage here using IterablePoolAllocators
	};

}