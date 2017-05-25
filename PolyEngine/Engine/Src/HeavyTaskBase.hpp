#pragma once

#include <Queue.hpp>

namespace Poly
{
	class HeavyTaskBase : public BaseObject<>
	{
	public:
		virtual void Execute(World* w) = 0;
		virtual String GetDescription() const = 0;
	};

	using HeavyTaskQueue = Queue<HeavyTaskBase*>;
}
