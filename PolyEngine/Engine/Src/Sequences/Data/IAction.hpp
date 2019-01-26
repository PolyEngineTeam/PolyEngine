#pragma once

#include <Defines.hpp>
#include <RTTI/RTTI.hpp>

namespace Poly
{
	class Entity;

	class IAction
	{
	public:
		virtual TimeDuration GetTotalTime() const = 0;

		virtual void OnBegin(Entity* entity) {};
		virtual void OnUpdate(TimeDuration dt) = 0;
		virtual void OnAbort() {};
		virtual void OnFinish() {};

	}; // class IAction
} // namespace Poly