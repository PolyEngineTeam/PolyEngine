#pragma once

#include <Collections/Queue.hpp>

namespace Poly
{
	class DeferredTaskBase : public BaseObject<>
	{
	public:
		virtual void Execute(Scene* w) = 0;
		virtual const char* GetDescription() const = 0;
	};
}
