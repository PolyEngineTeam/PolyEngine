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

		T *Get() const
		{
			return dynamic_cast<T*>(SafePtrRoot::GetPointer(PointerIdx));
		}

		SafePtr operator=(const SafePtrRoot* pointer)
		{
			return SafePtr(pointer);
		}

		T *operator->() const
		{
			return Get();
		}

		T *operator*() const
		{
			return Get();
		}

		bool operator==(const SafePtr<T>& other) const 
		{
			return this->Get() == other.Get();
		}

		bool operator==(const T& other) const
		{
			return this->Get() == &other;
		}

		bool operator!=(const SafePtr<T>& other) const
		{
			return !(*this == other);
		}

		bool operator!=(const T& other) const
		{
			return !(*this == other);
		}

	private:
		size_t PointerIdx;
	};
}