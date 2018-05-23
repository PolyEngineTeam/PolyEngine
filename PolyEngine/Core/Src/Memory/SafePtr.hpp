#pragma once

#include "Defines.hpp"
#include "SafePtrRoot.hpp"

namespace Poly
{
	/// <summary>Pointer wrapper, which returns nullptr if target object is not existing anymore</summary>
	template<typename T>
	class SafePtr final : public BaseObjectLiteralType<>
	{
	public:
		SafePtr() : SafePtr(nullptr) {}

		/// <summary>Basic constructor</summary>
		/// <param name="pointer">Raw pointer to target object</param>
		SafePtr(SafePtrRoot* pointer)
		{
			PointerIdx = SafePtrRoot::RegisterPointer(pointer);
		}

		/// <summary>Returns raw pointer to target object or nullptr, if target object do not exist</summary>
		/// <returns>Raw pointer to target object</returns>
		T *Get() const
		{
			return dynamic_cast<T*>(SafePtrRoot::GetPointer(PointerIdx));
		}

		/// <summary>Member access operator</summary>
		/// <returns>Raw pointer to target object</returns>
		T *operator->() { return Get();	}

		/// <summary>Constant member access operator</summary>
		/// <returns>Constant raw pointer to target object</returns>
		const T *operator->() const	{ return Get();	}

		/// <summary>Dereference operator</summary>
		/// <returns>Raw pointer to target object</returns>
		T *operator*() { return Get(); }

		/// <summary>Constant dereference operator</summary>
		/// <returns>Constant raw pointer to target object</returns>
		const T *operator*() const { return Get(); }

		/// <summary>Equality operator</summary>
		/// <param name="other">Another SafePtr instance</param>
		/// <returns>True, if both SafePtr instances points to the same object. False, if not</returns>
		bool operator==(const SafePtr<T>& other) const { return this->Get() == other.Get();	}

		/// <summary>Equality operator</summary>
		/// <param name="other">Raw pointer</param>
		/// <returns>True, if both SafePtr instance and raw pointer points to the same object. False, if not</returns>
		bool operator==(const T* other) const {	return this->Get() == other; }

		/// <summary>Inequality operator</summary>
		/// <param name="other">Another SafePtr instance</param>
		/// <returns>False, if both SafePtr instances points to the same object, True, if not</returns>
		bool operator!=(const SafePtr<T>& other) const { return !(*this == other); }

		/// <summary>Inequality operator</summary>
		/// <param name="other">Raw pointer</param>
		/// <returns>False, if both SafePtr instance and raw pointer points to the same object, True, if not</returns>
		bool operator!=(const T* other) const { return !(*this == other); }

		operator bool() const { return Get() != nullptr; }
		explicit operator T*() { return Get(); }
		explicit operator T*() const { return Get(); }
	private:
		size_t PointerIdx;
	};
}