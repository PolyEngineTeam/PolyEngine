#pragma once

#include <Core.hpp>

namespace Poly {

	class World;

	//TODO implement system reqistrtation

	class ENGINE_DLLEXPORT SystemBase : public BaseObject<>
	{
	public:
		virtual void Update(World* world) = 0;
	};

}