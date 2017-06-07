#pragma once

#include <ComponentBase.hpp>
#include <UniqueID.hpp>

namespace Invaders
{
	class TankComponent : public Poly::ComponentBase
	{
	public:
		TankComponent(Poly::UniqueID t) : Turret(t) {}
		~TankComponent() {}

		Poly::UniqueID Turret;
	};
}