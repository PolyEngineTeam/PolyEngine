#pragma once

namespace Poly 
{
	class Vector;
	class Quaternion;
	class UniqueID;
	class World;
}	

namespace Invaders
{
	namespace MovementSystem
	{
		void MovementUpdatePhase(Poly::World*);

		void SetLinearVelocity(Poly::World*, const Poly::UniqueID&, const Poly::Vector&);
		const Poly::Vector& GetLinearVelocity(Poly::World*, Poly::UniqueID);
		void SetLinearAcceleration(Poly::World*, const Poly::UniqueID&, const Poly::Vector&);
		const Poly::Vector& GetLinearAcceleration(Poly::World*, Poly::UniqueID);

		void SetAngularVelocity(Poly::World*, const Poly::UniqueID&, const Poly::Quaternion&);
		const Poly::Quaternion& GetAngularVelocity(Poly::World*, Poly::UniqueID);
		void SetAngularAcceleration(Poly::World*, const Poly::UniqueID&, const Poly::Quaternion&);
		const Poly::Quaternion& GetAngularAcceleration(Poly::World*, Poly::UniqueID);
	}
}