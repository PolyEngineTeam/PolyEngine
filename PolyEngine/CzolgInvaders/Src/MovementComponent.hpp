#pragma once

#include <ComponentBase.hpp>
#include <Vector.hpp>
#include <Quaternion.hpp>

#include "MovementSystem.hpp"

namespace Invaders
{
	namespace MovementSystem
	{
		class MovementComponent : public Poly::ComponentBase
		{
		friend void MovementUpdatePhase(Poly::World*);
		friend void SetLinearVelocity(Poly::World*, const Poly::UniqueID&, const Poly::Vector&);
		friend const Poly::Vector& GetLinearVelocity(Poly::World*, Poly::UniqueID);
		friend void SetLinearAcceleration(Poly::World*, const Poly::UniqueID&, const Poly::Vector&);
		friend const Poly::Vector& GetLinearAcceleration(Poly::World*, Poly::UniqueID);
		friend void SetAngularVelocity(Poly::World*, const Poly::UniqueID&, const Poly::Quaternion&);
		friend const Poly::Quaternion& GetAngularVelocity(Poly::World*, Poly::UniqueID);
		friend void SetAngularAcceleration(Poly::World*, const Poly::UniqueID&, const Poly::Quaternion&);
		friend const Poly::Quaternion& GetAngularAcceleration(Poly::World*, Poly::UniqueID);
		public:
			MovementComponent(Poly::Vector linVel, Poly::Vector linAcc, Poly::Quaternion angVel, Poly::Quaternion angAcc) :
				LinearVelocity(linVel),
				LinearAcceleration(linAcc),
				AngularVelocity(angVel),
				AngularAcceleration(angAcc)
				{}
			MovementComponent() {}
			~MovementComponent() {}

		private:
			Poly::Vector LinearVelocity;
			Poly::Vector LinearAcceleration;
			Poly::Quaternion AngularVelocity;
			Poly::Quaternion AngularAcceleration;
		};
	}
}