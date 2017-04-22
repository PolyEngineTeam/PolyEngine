#pragma once

#include "InputQueue.hpp"

namespace Poly
{
	extern InputQueue* InputEventsQueue;

	class World;

	namespace InputSystem
	{
		void InputPhase(World*);
	}
}