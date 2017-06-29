#pragma once

#include <Queue.hpp>

namespace Poly
{
	class DeferredTaskBase : public BaseObject<>
	{
	public:
		virtual void Execute(World* w) = 0;
		virtual const char* GetDescription() const = 0;
	};
}
