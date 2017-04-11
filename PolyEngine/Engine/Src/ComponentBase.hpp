#pragma once

#include <Core.hpp>

namespace Poly {

	class Entity;

	//TODO implement component reqistrtation

	constexpr unsigned int MAX_COMPONENTS_COUNT = 64;

	class ENGINE_DLLEXPORT ComponentBase : public BaseObject<>
	{
	public:

	private:
		Entity* Owner = nullptr;
	};
}