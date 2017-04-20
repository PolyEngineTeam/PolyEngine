#pragma once

#include "InputQueue.hpp"

namespace Poly
{
	InputQueue* InputEventsQueue;

	class World;

	namespace InputSystem
	{
		void InputPhase(World*);
	}
}