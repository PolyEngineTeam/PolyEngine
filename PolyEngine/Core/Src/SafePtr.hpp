#pragma once

#include "Defines.hpp"
#include "SafePtrRoot.hpp"

namespace Poly
{
	template<typename T>
	class SafePtr : public BaseObject<>
	{
	public:
		SafePtr(SafePtrRoot* pointer)
		{
			PointerIdx = SafePtrRoot::RegisterPointer(pointer);
		}

		SafePtrRoot *operator->()
		{
			return SafePtrRoot::GetPointer(PointerIdx);
		}

		SafePtrRoot *operator*()
		{
			return SafePtrRoot::GetPointer(PointerIdx);
		}
	private:
		size_t PointerIdx;
	};
}