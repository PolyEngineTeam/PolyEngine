#pragma once

#include <Core.hpp>

namespace Poly {

	class World;

	class ENGINE_DLLEXPORT SystemBase : public BaseObject<>
	{
	public:
		virtual void Update(World* world) = 0;
	};

}