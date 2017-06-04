#pragma once

#include <Queue.hpp>

namespace Poly
{
	class DeferredTaskBase : public BaseObject<>
	{
	public:
		virtual void Execute(World* w) = 0;
		virtual String GetDescription() const = 0;
	};

	using DeferredTaskQueue = Queue<DeferredTaskBase*>;
}
