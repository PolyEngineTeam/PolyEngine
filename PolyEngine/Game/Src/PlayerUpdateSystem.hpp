#pragma once


namespace Poly
{
	class World;
	class TransformComponent;
}

namespace sgj
{
	namespace PlayerUpdateSystem
	{
		void ProcessInput(Poly::World* world);
	}

}