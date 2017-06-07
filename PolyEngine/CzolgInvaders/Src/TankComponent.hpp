#pragma once

#include <ComponentBase.hpp>
#include <UniqueID.hpp>

namespace Invaders
{
	class TankComponent : public Poly::ComponentBase
	{
	public:
		TankComponent(Poly::UniqueID t, Poly::Angle deg, float time = 0.0f) : Turret(t), NextRotTime(time), Degree(deg)  {}
		~TankComponent() {}

		Poly::UniqueID Turret;
		float NextRotTime = 0.0f;
		Poly::Angle Degree;
		float MovedDistance = 0;
	};
}