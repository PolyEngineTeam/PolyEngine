#pragma once


namespace Poly
{
	class World;
	class TransformComponent;
}

namespace SGJ
{
	namespace PlayerUpdateSystem
	{
		void ProcessInput(Poly::World* world);
	}

}