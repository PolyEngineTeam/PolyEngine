#pragma once

#include <Core.hpp>
#include <Queue.hpp>

namespace Poly
{
	class HeavyTaskBase : public BaseObject<>
	{
	public:
		virtual void Execute(World* w) {}
	};

	using HeavyTaskQueue = Queue<HeavyTaskBase*>;
}