#pragma once

#include <Queue.hpp>

namespace Poly
{
	class HeavyTaskBase : public BaseObject<>
	{
	public:
		virtual void Execute(World* w) = 0;
	public:
		virtual String GetDescription() = 0;
	};

	using HeavyTaskQueue = Queue<HeavyTaskBase*>;
}
